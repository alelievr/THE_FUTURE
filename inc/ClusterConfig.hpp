#ifndef CLUSTERCONFIG_HPP
# define CLUSTERCONFIG_HPP
# include "NetworkGUI.hpp"
# include "SyncOffset.hpp"
# include "NetworkManager.hpp"
# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <regex>
# include <map>

#define DEFAULT_CONFIG_FILE		".config"

struct LocalParam
{
	int			seat;
	int			row;
	std::string	localParamName;
	int			programId;
	float		value;
};

struct ImacConfig
{
	int		row;
	int		seat;
	int		groupId;

	std::vector< LocalParam >	localParams;
};

enum class RenderLoopCommandType
{
	Focus,
	Wait,
	Uniform,
	LocalParam,
	None,
};

struct	RenderLoopCommand
{
	RenderLoopCommandType	type;

	//focus
	int					programIndex;
	int					transitionIndex;
	SyncOffset			syncOffset;

	//wait
	int					waitTime;

	//uniform
	std::string			uniformName;
	UniformParameter	uniformParam;	

	//localParam
	int					row;
	int					seat;
	std::string			localParamName;
	UniformParameter	localParamValue;

	//Wait constructor
	RenderLoopCommand(const int wTime)
	{
		this->type = RenderLoopCommandType::Wait;
		this->waitTime = wTime;
	}

	//Focus constructor
	RenderLoopCommand(const int pIndex, const int tIndex, const SyncOffset & sOffset)
	{
		this->type = RenderLoopCommandType::Focus;
		this->programIndex = pIndex;
		this->syncOffset = sOffset;
		this->transitionIndex = tIndex;
	}

	//Uniform update constructor
	RenderLoopCommand(const int pIndex, const std::string & uName, const UniformParameter & value, const SyncOffset & sOffset)
	{
		this->type = RenderLoopCommandType::Uniform;
		this->programIndex = pIndex;
		this->uniformName = uName;
		this->syncOffset = sOffset;
		this->uniformParam = value;
	}

	//LocalParam constructor
	RenderLoopCommand(const int pIndex, const int row, const int seat, const std::string & name, const UniformParameter & value)
	{
		this->type = RenderLoopCommandType::LocalParam;
		this->programIndex = pIndex;
		this->localParamName = name;
		this->row = row;
		this->seat = seat;
		this->localParamValue = value;
	}
};

typedef std::vector< RenderLoopCommand >	RenderLoop;

struct		GroupConfig
{
	std::list< std::string >	shaders;
	std::list< std::string >	audioFiles;
};

class		ClusterConfig
{
	private:
		static std::vector< ImacConfig >					_clusterConfig;
		static std::map< int, GroupConfig >					_groupConfigs;
		static std::map< int, RenderLoop >					_renderLoops;

		static void			LoadRenderLoop(std::ifstream & condigFile, const int groupId, int & nLines);
		static UniformType	_UniformTypeStringToType(const std::string & uniType);
		static SyncOffset	_ParseSyncOffset(const std::smatch & matches, const int typeIndex, const int paramIndex, const int nLines, const std::string & line);

	public:
		ClusterConfig(void) = delete;
		ClusterConfig(const ClusterConfig &) = delete;
		virtual ~ClusterConfig(void) = delete;

		ClusterConfig &	operator=(ClusterConfig const & src) = delete;

		static void	LoadConfigFile(const std::string & fileName = "");
		//static void	SaveConfigFile(const NetworkGUI & gui);
		static GroupConfig &				GetShadersInGroup(const int groupId);
		static std::vector< ImacConfig > &	GetClusterConfig(void);
		static int							GetGroupForImac(const int row, const int seat);
		static void							StartAllRenderLoops(NetworkManager *netManager);
		static int							GetGroupNumber(void);
		static size_t						GetImacIndexInGroup(const int groupId, const int row, const int seat);
};

std::ostream &	operator<<(std::ostream & o, ClusterConfig const & r);

#endif
