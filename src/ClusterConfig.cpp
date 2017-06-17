#include "ClusterConfig.hpp"
#include <fstream>
#include <regex>
#include <algorithm>

std::vector< ImacConfig >					ClusterConfig::_clusterConfig;
std::map< int, std::list< std::string > >	ClusterConfig::_groupConfig;

void		ClusterConfig::LoadConfigFile(const std::string & fName)
{
	std::string fileName = fName;
	if (fName.empty())
		fileName = DEFAULT_CONFIG_FILE;

	std::ifstream	infile(fileName);
	std::string		line;
	std::regex		iMacGroupLine("e[1-3]r(1[0-3]|[1-9])p(2[0-3]|1[0-9]|[1-9])\\s\\s*(\\d)");
	std::regex		groupShaderLine("(\\d)\\s\\s*(.*)");
	std::regex		commentLine("\\s*#.*");
	std::smatch		matches;
	int				n = 0;

	if (!infile.is_open())
	{
		std::cout << "Can't open config file: " << fName << std::endl;
		return ;
	}
	else
		std::cout << "Loading config file: " << fileName << std::endl;

	while (std::getline(infile, line))
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
