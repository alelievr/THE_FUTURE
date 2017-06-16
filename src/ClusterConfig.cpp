#include "ClusterConfig.hpp"
#include <fstream>
#include <regex>

void		ClusterConfig::LoadConfigFile(const std::string & fName)
{
	std::string fileName = fName;
	if (fileName.empty())
		fileName = DEFAULT_CONFIG_FILE;

	std::ifstream	infile(fileName);
	std::string		line;
	std::regex		iMacGroupLine("e[1-3]r(1[0-3]|[1-9])p(2[0-3]|1[0-9]|[1-9])\\s\\s*\\d");
	std::regex		groupShaderLine("(\\d)\\s\\s*(.*)");
	std::regex		commentLine("\\s*#.*");
	std::smatch		matches;
	int				n = 0;

	while (std::getline(infile, line))
	{
		line = std::regex_replace(line, commentLine, "");
		if (line.empty())
			continue ;
		if (std::regex_match(line, matches, iMacGroupLine))
		{
			
		}
		else if (std::regex_match(line, matches, groupShaderLine))
		{

		}
		else
			std::cout << "syntax error at line " << n << ": \"" << line << "\"\n";
		n++;
	}
}

void		ClusterConfig::SaveConfigFile(const NetworkGUI & gui)
{
	(void)gui;
}


ClusterConfig &	ClusterConfig::operator=(ClusterConfig const & src)
{
	std::cout << "Assignment operator called" << std::endl;

	if (this != &src) {
	}
	return (*this);
}

std::ostream &	operator<<(std::ostream & o, ClusterConfig const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
