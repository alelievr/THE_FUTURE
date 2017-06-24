#include "ClusterConfig.hpp"
#include "Timer.hpp"
#include <unistd.h>
#include <algorithm>

#define IMAC	"e[1-3]r(1[0-3]|[1-9])p(2[0-3]|1[0-9]|[1-9])"
#define SPACE	"\\s\\s*"
#define FLOAT	"([+-]?[0-9]*[.]?[0-9]+f?|RESET)"
#define F1		FLOAT
#define F2		F1 SPACE F1
#define F3		F2 SPACE F1
#define F4		F3 SPACE F1

std::vector< ImacConfig >			ClusterConfig::_clusterConfig;
std::map< int, GroupConfig >		ClusterConfig::_groupConfigs;
std::map< int, RenderLoop >			ClusterConfig::_renderLoops;
std::map< int, std::map< int, std::vector< LocalParam > > >	ClusterConfig::_localParams;

UniformType	ClusterConfig::_UniformTypeStringToType(const std::string & uniType)
{
	static std::map< std::string, UniformType > uniformTypeMap = {
		{"f1", UniformType::Float1},
		{"f2", UniformType::Float2},
		{"f3", UniformType::Float3},
		{"f4", UniformType::Float4},
		{"i1", UniformType::Int1},
	};

	return uniformTypeMap[uniType];
}

SyncOffset	ClusterConfig::_ParseSyncOffset(const std::smatch & matches, const int typeIndex, const int paramIndex, const int nLines, const std::string & line)
{
	std::string		soType = matches[typeIndex];
	SyncOffset		sOffset;

	if (soType.find("Linear") != std::string::npos)
	{
		long delayMilliSecs = std::stol(matches[paramIndex]);
		int	delaySecs = delayMilliSecs / 1000;
		delayMilliSecs %= 1000;
		sOffset = SyncOffset::CreateLinearSyncOffset(delaySecs, delayMilliSecs);
	}
	else if (soType.find("None") != std::string::npos)
		sOffset = SyncOffset::CreateNoneSyncOffset();
	else
		std::cout << "parse error at line: " << nLines << ": " << line << std::endl, exit(-1);

	return sOffset;
}

void		ClusterConfig::LoadRenderLoop(std::ifstream & configFile, const int groupId, int & nLines)
{
	std::string		line;
	std::smatch		matches;
	std::string		syncRegex("Sync\\s\\s*(None|Linear\\s\\s*(\\d\\d*))");
	std::string		uniformParameters("(" F4 "|" F3 "|" F2 "|" F1 ")");
	std::regex		commentLine("\\s*#.*");
	std::regex		openingBraceLine("\\s*\\{\\s*");
	std::regex		closingBraceLine("\\s*\\}\\s*");
	std::regex		focusLine("\\s*Focus\\s\\s*(\\d\\d*)\\s\\s*(-?\\d\\d*)\\s\\s*" + syncRegex);
	std::regex		uniformLine("\\s*Uniform([1-4]f|1i)\\s\\s*(\\w+)\\s\\s*" + uniformParameters + SPACE + syncRegex);
	std::regex		localParamLine("\\s*LocalParam\\s+" IMAC "\\s+(\\w+)\\s+" + std::string(FLOAT) + SPACE + syncRegex);
	std::regex		waitLine("\\s*Wait\\s\\s*(\\d\\d*)");
	std::regex		audioLine("\\s*Audio(Play|Pause|Volume\\s+" FLOAT ")\\s+(\\d+)\\s+" + syncRegex);
	bool			openbrace = false;
	int				currentProgramIndex = -1;

	std::cout << "\\s*LocalParam\\s+" IMAC "\\s+(\\w+)\\s+" FLOAT <<  std::endl;
	while (std::getline(configFile, line))
	{
		line = std::regex_replace(line, commentLine, "");
		if (line.empty())
			continue ;
		if (std::regex_match(line, matches, focusLine))
		{
			int				programIndex = std::stoi(matches[1]);
			int				transitionIndex = std::stoi(matches[2]);
			SyncOffset		sOffset;

			sOffset = _ParseSyncOffset(matches, 3, 4, nLines, line);
			currentProgramIndex = programIndex;
			_renderLoops[groupId].push_back(RenderLoopCommand(programIndex, transitionIndex, sOffset));
		}
		else if (std::regex_match(line, matches, waitLine))
		{
			int		waitTime = std::stoi(matches[1]);
			_renderLoops[groupId].push_back(RenderLoopCommand(waitTime));
		}
		else if (std::regex_match(line, matches, uniformLine))
		{
			if (currentProgramIndex == -1)
				std::cout << "parse error at line: " << nLines << ": " << line << std::endl, exit(-1);
			std::string			uniformType = matches[1];
			std::string			uniformName = matches[2];
			UniformParameter	param;
			SyncOffset			sOffset;

			param.type = _UniformTypeStringToType(uniformType);
			//TODO: functionize this

			if (matches[4].length()) //4 params: 4 5 6 7
			{
				param.f4.x = std::stof(matches[4]);
				param.f4.y = std::stof(matches[5]);
				param.f4.z = std::stof(matches[6]);
				param.f4.w = std::stof(matches[7]);
			}
			else if (matches[8].length()) //3 params: 8 9 10
			{
				param.f3.x = std::stof(matches[8]);
				param.f3.y = std::stof(matches[9]);
				param.f3.z = std::stof(matches[10]);
			}
			else if (matches[11].length()) //2 params: 11 12
			{
				param.f2.x = std::stof(matches[11]);
				param.f2.y = std::stof(matches[12]);
			}
			else //1 param
			{
				if (std::string(matches[13]).find("RESET") != std::string::npos)
					param.reset = true;
				else
					param.f1 = std::stof(matches[13]);
			}

			if (matches.size() == 14)
				sOffset = _ParseSyncOffset(matches, 14, 15, nLines, line);
			_renderLoops[groupId].push_back(RenderLoopCommand(currentProgramIndex, uniformName, param, sOffset));
		}
		else if (std::regex_match(line, matches, localParamLine))
		{
			int					row = std::stoi(matches[1]);
			int					seat = std::stoi(matches[2]);
			std::string			name = matches[3];
			UniformParameter	param;
			SyncOffset			sync;

			sync = _ParseSyncOffset(matches, 5, 6, nLines, line);

			param.type = UniformType::Float1;
			param.reset = false;
			if (std::string(matches[4]).find("RESET") != std::string::npos)
				param.reset = true;
			else
				param.f1 = std::stof(matches[4]);

			_renderLoops[groupId].push_back(RenderLoopCommand(currentProgramIndex, row, seat, name, param, sync));
		}
		else if (std::regex_match(line, matches, audioLine))
		{
			std::string		audioUpdateType = matches[1];
			std::cout << "match3: " << matches[3] << std::endl;
			int				audioIndex = std::stoi(matches[3]);
			float			audioVolume = 0;
			AudioUpdateType	type;
			SyncOffset		sync;

			if (audioUpdateType.find("Play") != std::string::npos)
				type = AudioUpdateType::Play;
			else if (audioUpdateType.find("Pause") != std::string::npos)
				type = AudioUpdateType::Pause;
			else
			{
				type = AudioUpdateType::Volume;
				audioVolume = std::stof(matches[2]);
			}

			sync = _ParseSyncOffset(matches, 4, 5, nLines, line);

			_renderLoops[groupId].push_back(RenderLoopCommand(currentProgramIndex, audioIndex, type, audioVolume, sync));
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
	std::regex		iMacGroupLine("\\s*" IMAC "\\s\\s*(\\d\\d*)");
	std::regex		groupFileLine("\\s*(\\d+)\\s\\s*(.*)");
	std::regex		renderLoopLine("RenderLoop\\s\\s*(\\d\\d*)");
	std::regex		localParamLine("\\s*LocalParam\\s+" IMAC "\\s+(\\d+)\\s+(\\w+)\\s+" FLOAT);
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
			_clusterConfig.push_back(ImacConfig{row, seat, groupId, {}});
		}
		else if (std::regex_match(line, matches, groupFileLine))
		{
			int groupId = std::stoi(matches[1]);
			std::string file = matches[2];
			if (CheckFileExtension(file.c_str(), (const char *[]){"glsl", "frag", "sf", "cl", "ocl", "kern", NULL}))
				_groupConfigs[groupId].shaders.push_back(file);
			else if (CheckFileExtension(file.c_str(), (const char *[]){"ogg", "oga", "wav", "flac", NULL}))
				_groupConfigs[groupId].audioFiles.push_back(file);
		}
		else if (std::regex_match(line, matches, renderLoopLine))
		{
			int	groupId = std::stoi(matches[1]);
			LoadRenderLoop(configFile, groupId, n);
		}
		else if (std::regex_match(line, matches, localParamLine))
		{
			int					row = std::stoi(matches[1]);
			int					seat = std::stoi(matches[2]);
			int					programIndex = std::stoi(matches[3]);
			std::string			localParamName = matches[4];
			UniformParameter	param;
			LocalParam			lp;

			param.type = UniformType::Float1;
			param.reset = false;
			if (std::string(matches[5]).find("RESET") != std::string::npos)
				param.reset = true;
			else
				param.f1 = std::stof(matches[5]);

			lp.programIndex = programIndex;
			lp.localParamName = localParamName;
			lp.value = param;
			_localParams[row][seat].push_back(lp);
		}
		else
			std::cout << "syntax error at line " << n << ": \"" << line << "\"\n";
		n++;
	}
}

const std::vector< ImacConfig > &	ClusterConfig::GetClusterConfig(void)
{
	return _clusterConfig;
}

GroupConfig &				ClusterConfig::GetConfigForGroup(const int groupId)
{
	return _groupConfigs[groupId];
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

const std::vector< LocalParam > &  ClusterConfig::GetLocalParamsForClient(const int row, const int seat)
{
	static const std::vector< LocalParam > defaultValue = {};
	const auto	mapRow = _localParams.find(row);
	if (mapRow == _localParams.end())
		return defaultValue;
	const auto	mapSeat = mapRow->second.find(seat);
	if (mapSeat == mapRow->second.end())
		return defaultValue;
	return mapSeat->second;
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
				const int	networkActionDelay = 1; //secs
				auto &		renderLoopCommands = _renderLoops[groupId];

				while (42)
				{
					auto command = renderLoopCommands[renderIndex];

					switch (command.type)
					{
						case RenderLoopCommandType::Focus:
							netManager->FocusShaderOnGroup(Timer::TimeoutInSeconds(networkActionDelay), groupId, command.programIndex, command.transitionIndex, command.syncOffset);
							break ;
						case RenderLoopCommandType::Wait:
							usleep(command.waitTime * 1000 * 1000);
							break ;
						case RenderLoopCommandType::Uniform:
							netManager->UpdateLocalParamOnGroup(Timer::TimeoutInSeconds(networkActionDelay), groupId, command.programIndex, command.uniformName, command.uniformParam, command.syncOffset);
							break ;
						case RenderLoopCommandType::LocalParam:
							netManager->UpdateLocalParamOnClient(Timer::TimeoutInSeconds(networkActionDelay), command.row, command.seat, command.programIndex, command.localParamName, command.localParamValue);
						case RenderLoopCommandType::AudioUpdate:
							netManager->UpdateAudioOnGroup(Timer::TimeoutInSeconds(networkActionDelay), groupId, command.audioUpdateType, command.audioIndex, command.audioVolume, command.syncOffset);
							break ;
						default:
							break ;
					}

					renderIndex++;
					if (renderIndex == renderLoopCommands.size())
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
	return _groupConfigs.size();
}

std::ostream &	operator<<(std::ostream & o, ClusterConfig const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
