#include "ClusterConfig.hpp"
#include "Timer.hpp"
#include <unistd.h>
#include <regex>
#include <algorithm>

#define SPACE	"\\s\\s*"
#define FLOAT	"([+-]?[0-9]*[.]?[0-9]+f?)"
#define F1		FLOAT
#define F2		F1 SPACE F1
#define F3		F2 SPACE F1
#define F4		F3 SPACE F1

std::vector< ImacConfig >					ClusterConfig::_clusterConfig;
std::map< int, std::list< std::string > >	ClusterConfig::_groupConfig;
std::map< int, RenderLoop >					ClusterConfig::_renderLoops;

void		ClusterConfig::LoadRenderLoop(std::ifstream & configFile, const int groupId, int & nLines)
{
	std::string		line;
	std::smatch		matches;
	std::string		syncRegex("Sync\\s\\s*(None|Linear\\s\\s*(\\d\\d*))");
	std::string		uniformParameters("(" F4 "|" F3 "|" F2 "|" F1 ")");
	std::regex		commentLine("\\s*#.*");
	std::regex		openingBraceLine("\\s*\\{\\s*");
	std::regex		closingBraceLine("\\s*\\}\\s*");
	std::regex		focusLine("\\s*Focus\\s\\s*(\\d\\d*)\\s\\s*" + syncRegex);
	std::regex		uniformLine("\\s*Uniform([1-4]f|1i)\\s\\s*(\\w+)\\s\\s*" + uniformParameters + SPACE + syncRegex);
	std::regex		waitLine("\\s*Wait\\s\\s*(\\d\\d*)");
	bool			openbrace = false;
	size_t			currentIteration = -1;

	std::cout << "\\s*Uniform([1-4]f|1i)\\s\\s*(\\w+)\\s\\s*" + uniformParameters + SPACE + syncRegex << std::endl;
	while (std::getline(configFile, line))
	{
		line = std::regex_replace(line, commentLine, "");
		if (line.empty())
			continue ;
		if (std::regex_match(line, matches, focusLine))
		{
			RenderIteration	r;
			int				programIndex = std::stoi(matches[1]);
			std::string		syncOffset = matches[2];

			currentIteration++;
			r.waitTime = 0;
			r.programIndex = programIndex;
			if (syncOffset.find("Linear") != std::string::npos)
			{
				int delaySecs = std::stoi(matches[3]);
				r.syncOffset = SyncOffset::CreateLinearSyncOffset(delaySecs, 0);
			}
			else if (syncOffset.find("None") != std::string::npos)
				r.syncOffset = SyncOffset::CreateNoneSyncOffset();
			else
				std::cout << "parse error at line: " << nLines << ": " << line << std::endl, exit(-1);
			_renderLoops[groupId].push_back(r);
		}
		else if (std::regex_match(line, matches, waitLine))
		{
			if (currentIteration >= _renderLoops[groupId].size())
				std::cout << "parse error: Wait without program !", exit(-1);
			int		waitTime = std::stoi(matches[1]);
			_renderLoops[groupId][currentIteration].waitTime = waitTime;
		}
		else if (std::regex_match(line, matches, uniformLine))
		{
			std::string		syncOffset = matches[14];
			std::string		uniformType = matches[1];
			std::string		uniformName = matches[2];

			//TODO: get uniform parameters, store it and add it to executer
		}
		else if (std::regex_match(line, matches, openingBraceLine))
		{
			if (openbrace)
				std::cout << "parse error at line: " << nLines << ": " << line << std::endl, exit(-1);
			openbrace = true;
		}
		else if (std::regex_match(line, matches, closingBraceLine))
		{
			if (!openbrace)
				std::cout << "parse error at line: " << nLines << ": " << line << std::endl, exit(-1);
			openbrace = false;
		}
		else
			std::cout << "parse error at line: " << nLines << ": " << line << std::endl, exit(-1);
		nLines++;
	}
	if (openbrace)
		std::cout << "parse error in RenderLoop block near line " << nLines << std::endl, exit(-1);
	if (_renderLoops[groupId].size() == 0)
		std::cout << "parse error: empty render loop !" << std::endl, exit(-1);
}

void		ClusterConfig::LoadConfigFile(const std::string & fName)
{
	std::string fileName = fName;
	if (fName.empty())
		fileName = DEFAULT_CONFIG_FILE;

	std::ifstream	configFile(fileName);
	std::string		line;
	std::regex		iMacGroupLine("e[1-3]r(1[0-3]|[1-9])p(2[0-3]|1[0-9]|[1-9])\\s\\s*(\\d\\d*)");
	std::regex		groupShaderLine("(\\d\\d*)\\s\\s*(.*)");
	std::regex		renderLoopLine("RenderLoop\\s\\s*(\\d\\d*)");
	std::regex		commentLine("\\s*#.*");
	std::smatch		matches;
	int				n = 1;

	if (!configFile.is_open())
	{
		std::cout << "Can't open config file: " << fName << std::endl;
		return ;
	}
	else
		std::cout << "Loading config file: " << fileName << std::endl;

	while (std::getline(configFile, line))
	{
		line = std::regex_replace(line, commentLine, "");
		if (line.empty())
			continue ;
		if (std::regex_match(line, matches, iMacGroupLine))
		{
			int row = std::stoi(matches[1]);
			int seat = std::stoi(matches[2]);
			int	groupId = std::stoi(matches[3]);
			_clusterConfig.push_back(ImacConfig{row, seat, groupId});
		}
		else if (std::regex_match(line, matches, groupShaderLine))
		{
			int groupId = std::stoi(matches[1]);
			std::string shaderFile = matches[2];
			_groupConfig[groupId].push_back(shaderFile);
		}
		else if (std::regex_match(line, matches, renderLoopLine))
		{
			int	groupId = std::stoi(matches[1]);
			LoadRenderLoop(configFile, groupId, n);
		}
		else
			std::cout << "syntax error at line " << n << ": \"" << line << "\"\n";
		n++;
	}
}

std::vector< ImacConfig > &	ClusterConfig::GetClusterConfig(void)
{
	return _clusterConfig;
}

std::list< std::string > &	ClusterConfig::GetShadersInGroup(const int groupId)
{
	return _groupConfig[groupId];
}

int							ClusterConfig::GetGroupForImac(const int row, const int seat)
{
	static std::list< std::string >	emptyList;

	auto imac = std::find_if(_clusterConfig.begin(), _clusterConfig.end(),
		[&](const ImacConfig & imac)
		{
			return imac.row == row && imac.seat == seat;
		}
	);

	if (imac != _clusterConfig.end())
		return imac->groupId;

	return -1;
}

void		ClusterConfig::StartAllRenderLoops(NetworkManager *netManager)
{
	for (auto & renderLoopKP : _renderLoops)
	{
		int		groupId = renderLoopKP.first;
		Timer::Async(
			[netManager, groupId](void)
			{
				size_t		renderIndex = 0;
				auto &		renderIterations = _renderLoops[groupId];

				while (42)
				{
					auto renderIteration = renderIterations[renderIndex];

					netManager->FocusShaderOnGroup(Timer::TimeoutInSeconds(1), groupId, renderIteration.programIndex, renderIteration.syncOffset);
					usleep(renderIteration.waitTime * 1000 * 1000);

					renderIndex++;
					if (renderIndex == renderIterations.size())
						renderIndex = 0;
				}
			}
		);
	}
}

size_t			ClusterConfig::GetImacIndexInGroup(const int groupId, const int row, const int seat)
{
	size_t		pos = 0;

	for (const auto & imacConfig : _clusterConfig)
	{
		if (imacConfig.row == row && imacConfig.seat == seat)
			break ;
		if (imacConfig.groupId == groupId)
			pos++;
	}
	return pos;
}

int			ClusterConfig::GetGroupNumber(void)
{
	return _groupConfig.size();
}

/*void		ClusterConfig::SaveConfigFile(const NetworkGUI & gui)
{
	std::cout << "Save config file: TODO !\n" << std::endl;
	(void)gui;
}*/

std::ostream &	operator<<(std::ostream & o, ClusterConfig const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
