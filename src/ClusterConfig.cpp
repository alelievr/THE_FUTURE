#include "ClusterConfig.hpp"
#include <regex>
#include <algorithm>

std::vector< ImacConfig >					ClusterConfig::_clusterConfig;
std::map< int, std::list< std::string > >	ClusterConfig::_groupConfig;
std::map< int, RenderLoop >					ClusterConfig::_renderLoops;

void		ClusterConfig::LoadRenderLoop(std::ifstream & configFile, const int groupId, int & nLines)
{
	std::string		line;
	std::smatch		matches;
	std::regex		commentLine("\\s*#.*");
	std::regex		openingBraceLine("\\s*\\{\\s*");
	std::regex		closingBraceLine("\\s*\\}\\s*");
	std::regex		focusLine("\\s*Focus\\s\\s*(\\d\\d*)\\s\\s*Sync\\s\\s*(None|Linear\\s\\s*(\\d\\d*))");
	std::regex		waitLine("\\s*Wait\\s\\s*(\\d\\d*)");
	bool			openbrace = false;

	while (std::getline(configFile, line))
	{
		line = std::regex_replace(line, commentLine, "");
		if (line.empty())
			continue ;
		if (std::regex_match(line, matches, focusLine))
		{
			int				programIndex = std::stoi(matches[1]);
			std::string		syncOffset = matches[2];
			_renderLoops[groupId].programIndex = programIndex;
			if (syncOffset.find("Linear") != std::string::npos)
			{
				int delaySecs = std::stoi(matches[3]);
				_renderLoops[groupId].syncOffset = SyncOffset::CreateLinearSyncOffset(delaySecs, 0);
			}
			else if (syncOffset.find("None") != std::string::npos)
				_renderLoops[groupId].syncOffset = SyncOffset::CreateNoneSyncOffset();
			else
				std::cout << "parse error at line: " << nLines << ": " << line << std::endl, exit(-1);
		}
		if (std::regex_match(line, matches, openingBraceLine))
		{
			if (openbrace)
				std::cout << "parse error at line: " << nLines << ": " << line << std::endl, exit(-1);
			openbrace = true;
		}
		if (std::regex_match(line, matches, closingBraceLine))
		{
			if (!openbrace)
				std::cout << "parse error at line: " << nLines << ": " << line << std::endl, exit(-1);
			openbrace = false;
		}
		nLines++;
	}
	if (openbrace)
		std::cout << "parse error in RenderLoop block near line " << nLines << std::endl, exit(-1);
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

int			ClusterConfig::GetGroupNumber(void)
{
	return _groupConfig.size();
}

void		ClusterConfig::SaveConfigFile(const NetworkGUI & gui)
{
	(void)gui;
}

std::ostream &	operator<<(std::ostream & o, ClusterConfig const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
