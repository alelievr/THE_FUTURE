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

struct ImacConfig
{
	int		row;
	int		seat;
	int		groupId;
};

enum class RenderLoopCommandType
{
	Focus,
	Wait,
	Uniform,
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
	RenderLoopCommand(const int pIndex, const std::string & uName, const SyncOffset & sOffset)
	{
		this->type = RenderLoopCommandType::Uniform;
		this->programIndex = pIndex;
		this->uniformName = uName;
		this->syncOffset = sOffset;
	}
};

typedef std::vector< RenderLoopCommand >	RenderLoop;

class		ClusterConfig
{
	private:
		static std::vector< ImacConfig >					_clusterConfig;
		static std::map< int, std::list< std::string > >	_groupConfig;
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
		static std::list< std::string > &	GetShadersInGroup(const int groupId);
		static std::vector< ImacConfig > &	GetClusterConfig(void);
		static int							GetGroupForImac(const int row, const int seat);
		static void							StartAllRenderLoops(NetworkManager *netManager);
		static int							GetGroupNumber(void);
		static size_t						GetImacIndexInGroup(const int groupId, const int row, const int seat);
};

std::ostream &	operator<<(std::ostream & o, ClusterConfig const & r);

#endif
