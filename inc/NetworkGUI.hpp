#ifndef NETWORKGUI_HPP
# define NETWORKGUI_HPP
# include "SFGUI/SFGUI.hpp"
# include "SFGUI/Widgets.hpp"
# include "NetworkManager.hpp"
# include "SFML/Window.hpp"
# include "SFML/Graphics.hpp"
# include "Timeval.hpp"
# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <list>

# define WINDOW_WIDTH	2220
# define WINDOW_HEIGHT	1310

#define TIMEOUT_BEFORE_SENDING_CHANGES	50 //ms

const std::string	localParams[] = {"iValue1", "iValue2", "iValue3", "iValue4"};

class		NetworkGUI
{
	struct		GUIClient
	{
		int				groupId;
		ClientStatus	status;
	};

	struct		DelayChange
	{
		int				programIndex;
		std::string		localParamName;
		float			value;
		Timeval			timeout;
	};

	private:
		sf::RenderWindow *		_win;
		sf::Font *				_font;
		sf::Vector2< int >		_mousePosition;
		sf::Vector2< unsigned >	_windowSize;
		sfg::SFGUI				_sfgui;
		sfg::Desktop			_desktop;
		sfg::Box::Ptr			_groupBox;
		sfg::Window::Ptr		_groupWindow;
		NetworkManager *		_netManager;
		std::vector< DelayChange > _toDelayChanges;
		int						_selectedGroup;
		int						_oldGroupCount;
		std::map< int, std::map< int, GUIClient > >	_GUIClients;
		std::vector< sf::Color > _groupColors;

		void		DrawCluster(const bool clicked);
		void		DrawPlace(const int x, const int y, const bool client);
		void		UpdateGroupList(void);
		void		InitContainers(void);
		void		FillColorList(void);
		void		AddToDelayChanges(const int programIndex, const std::string & localParamName, const float value);
		void		SendDelayChanges(void);
		GUIClient &	FindGUIClient(const int row, const int seat);

	public:
		NetworkGUI(void) = delete;
		NetworkGUI(NetworkManager *nm);
		NetworkGUI(const NetworkGUI &) = delete;
		virtual ~NetworkGUI(void);

		NetworkGUI &	operator=(NetworkGUI const & src) = delete;

		void	RenderLoop(void);
};

#endif
