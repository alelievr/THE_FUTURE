#ifndef CLUSTERCONFIG_HPP
# define CLUSTERCONFIG_HPP
# include "NetworkGUI.hpp"
# include "SyncOffset.hpp"
# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <map>

#define DEFAULT_CONFIG_FILE		".config"

struct ImacConfig
{
	int		row;
	int		seat;
	int		groupId;
};

struct RenderIteration
{
	int			programIndex;
	SyncOffset	syncOffset;
	int			waitTime; //seconds before passing to another program
};

typedef std::vector< RenderIteration >	RenderLoop;

class		ClusterConfig
{
	private:
		static std::vector< ImacConfig >					_clusterConfig;
		static std::map< int, std::list< std::string > >	_groupConfig;
		static std::map< int, RenderLoop >					_renderLoops;

		static void	LoadRenderLoop(std::ifstream & condigFile, const int groupId, int & nLines);

	public:
		ClusterConfig(void) = delete;
		ClusterConfig(const ClusterConfig &) = delete;
		virtual ~ClusterConfig(void) = delete;

		ClusterConfig &	operator=(ClusterConfig const & src) = delete;

		static void	LoadConfigFile(const std::string & fileName = "");
		static void	SaveConfigFile(const NetworkGUI & gui);
		static std::list< std::string > &	GetShadersInGroup(const int groupId);
		static std::vector< ImacConfig > &	GetClusterConfig(void);
		static int							GetGroupForImac(const int row, const int seat);
		static const std::map< int, RenderLoop > & GetRenderLoops(void);
		static int	GetGroupNumber(void);
};

std::ostream &	operator<<(std::ostream & o, ClusterConfig const & r);

#endif
