#ifndef CLUSTERCONFIG_HPP
# define CLUSTERCONFIG_HPP
# include "NetworkGUI.hpp"
# include <iostream>
# include <string>

#define DEFAULT_CONFIG_FILE		".config"

class		ClusterConfig
{
	private:

	public:
		ClusterConfig(void) = delete;
		ClusterConfig(const ClusterConfig &) = delete;
		virtual ~ClusterConfig(void) = delete;

		ClusterConfig &	operator=(ClusterConfig const & src);

		static void	LoadConfigFile(const std::string & fileName = "");

		static void	SaveConfigFile(const NetworkGUI & gui);
};

std::ostream &	operator<<(std::ostream & o, ClusterConfig const & r);

#endif
