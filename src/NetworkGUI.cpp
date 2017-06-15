#include "NetworkGUI.hpp"

NetworkGUI::NetworkGUI(NetworkManager *nm)
{
	_netManager = nm;
	_win = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "THE FUTURE");
	_win->setFramerateLimit(60);

	sf::Texture placeTexture;
	placeTexture.loadFromFile("textures/place.gif");

	_placeSprite = new sf::Sprite();
	_placeSprite->setTexture(placeTexture);
	sf::RectangleShape rectangle(sf::Vector2f(120, 50));
}

NetworkGUI::~NetworkGUI(void)
{
	delete _win;
	delete _placeSprite;
}

void		NetworkGUI::DrawPlace(const int row, const int seat)
{

}

void		NetworkGUI::DrawCluster(bool clicked)
{

}

void		NetworkGUI::RenderLoop(void)
{
	bool		clicked = false;

	while (_win->isOpen())
    {
        sf::Event event;
        while (_win->pollEvent(event))
        {
			switch (event.type)
			{
				case sf::Event::Closed:
                	_win->close();
					break ;
				case sf::Event::MouseButtonPressed:
					clicked = true;
					break ;
				case sf::Event::MouseMoved:
					_mousePosition.x = event.mouseMove.x;
					_mousePosition.y = event.mouseMove.y;
					break ;
				default:
					break ;
			}
        }

		_win->clear(sf::Color::Black);

		DrawCluster(clicked);

		_win->display();
    }
}
