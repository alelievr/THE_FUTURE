#ifndef NETWORKGUI_HPP
# define NETWORKGUI_HPP
# include "NetworkManager.hpp"
# include "SFML/Window.hpp"
# include "SFML/Graphics.hpp"
# include <iostream>
# include <string>
# include <map>
# include <list>

# define WINDOW_WIDTH	1920
# define WINDOW_HEIGHT	1310

class		NetworkGUI
{
	struct		GUIClient
	{
		sf::Color		color;
		ClientStatus	status;
	};

	private:
		sf::RenderWindow	*_win;
		sf::Font			*_font;
		sf::Vector2< int >	_mousePosition;
		sf::Vector2< unsigned >	_windowSize;
		NetworkManager		*_netManager;
		int					_selectedGroup;
		std::map< int, std::map< int, GUIClient > >	_GUIClients;
		std::list< sf::Color > _groupColors;

		void		DrawCluster(const bool clicked);
		void		DrawPlace(const int x, const int y, const bool client);
		void		DrawGroupOptions(const bool clicked);
		bool		DrawButton(const int x, const int y, const int width, const int height, const bool clicked, const std::string & text, const sf::Color & color) const;
		void		DrawText(const int x, const int y, const std::string & text) const;
		void		FillColorList(void);

	public:
		NetworkGUI(void) = delete;
		NetworkGUI(NetworkManager *nm);
		NetworkGUI(const NetworkGUI &) = delete;
		virtual ~NetworkGUI(void);

		NetworkGUI &	operator=(NetworkGUI const & src) = delete;

		void	RenderLoop(void);
};

#endif
