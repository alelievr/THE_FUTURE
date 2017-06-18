#include "NetworkGUI.hpp"
#include "SyncOffset.hpp"
#include "Timer.hpp"
#include <sstream>

#define MAX_COLORS		150

static std::map< ClientStatus, sf::Color >	clientStatusColor = {
	{ClientStatus::Unknown,					sf::Color(150, 150, 150)},		//grey
	{ClientStatus::Disconnected,			sf::Color(91, 91, 91)},			//dark grey
	{ClientStatus::WaitingForCommand,		sf::Color(55, 228, 63)},		//green
	{ClientStatus::ShaderLoaded,			sf::Color(0, 255, 212)},		//cyan
	{ClientStatus::Running,					sf::Color(40, 194, 248)},		//light blue
	{ClientStatus::Error,					sf::Color(255, 0, 0)},			//red
	{ClientStatus::Timeout,					sf::Color(255, 232, 0)},		//yellow
};

static const bool clusterMap[13][25] = {
  // 1  2  3  4  5  6  7     8  9 10 11 12 13 14 15 16    17 18 19 20 21 22 23
	{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1}, //r13
	{1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1}, //r12
	{1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1}, //r11
	{1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1}, //r10
	{1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1}, //r9
	{1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1}, //r8
	{1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1}, //r7
	{1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1}, //r6
	{1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1}, //r5
	{1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1}, //r4
	{1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1}, //r3
	{1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1}, //r2
	{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1}, //r1
};

static const char *clusterIPMap[13][25] = {
	{"13.01", "13.02", "13.03", "13.04", "13.05", "13.06", "13.07", "--.--", "--.--", "--.--", "--.--", "--.--", "--.--", "--.--", "--.--", "--.--", "--.--", "--.--", "13.08", "13.09", "13.10", "13.11", "13.12", "13.13", "13.14"}, //r13
	{"12.01", "12.02", "12.03", "12.04", "12.05", "12.06", "12.07", "--.--", "12.08", "12.09", "12.10", "12.11", "12.12", "12.13", "12.14", "12.15", "12.16", "--.--", "12.17", "12.18", "12.19", "12.20", "12.21", "12.22", "12.23"}, //r12
	{"11.01", "11.02", "11.03", "11.04", "11.05", "11.06", "11.07", "--.--", "11.08", "11.09", "11.10", "11.11", "11.12", "11.13", "11.14", "11.15", "11.16", "--.--", "11.17", "11.18", "11.19", "11.20", "11.21", "11.22", "11.23"}, //r11
	{"10.01", "10.02", "10.03", "10.04", "10.05", "10.06", "10.07", "--.--", "10.08", "10.09", "10.10", "10.11", "10.12", "10.13", "10.14", "10.15", "10.16", "--.--", "10.17", "10.18", "10.19", "10.20", "10.21", "10.22", "10.23"}, //r10
	{"09.01", "09.02", "09.03", "09.04", "09.05", "09.06", "--.--", "--.--", "--.--", "09.07", "09.08", "09.09", "09.10", "09.11", "09.12", "09.13", "--.--", "--.--", "09.14", "09.15", "09.16", "09.17", "09.18", "09.19", "09.20"}, //r9
	{"08.01", "08.02", "08.03", "08.04", "08.05", "08.06", "--.--", "--.--", "--.--", "08.07", "08.08", "08.09", "08.10", "08.11", "08.12", "08.13", "08.14", "--.--", "08.15", "08.16", "08.17", "08.18", "08.19", "08.20", "08.21"}, //r8
	{"07.01", "07.02", "07.03", "07.04", "07.05", "07.06", "07.07", "--.--", "07.08", "07.09", "07.10", "07.11", "07.12", "07.13", "07.14", "07.15", "07.16", "--.--", "07.17", "07.18", "07.19", "07.20", "07.21", "07.22", "07.23"}, //r7
	{"06.01", "06.02", "06.03", "06.04", "06.05", "06.06", "06.07", "--.--", "--.--", "06.08", "06.09", "06.10", "06.11", "06.12", "06.13", "06.14", "--.--", "--.--", "--.--", "06.15", "06.16", "06.17", "06.18", "06.19", "06.20"}, //r6
	{"05.01", "05.02", "05.03", "05.04", "05.05", "05.06", "05.07", "--.--", "05.08", "05.09", "05.10", "05.11", "05.12", "05.13", "05.14", "05.15", "05.16", "--.--", "--.--", "05.17", "05.18", "05.19", "05.20", "05.21", "05.22"}, //r5
	{"04.01", "04.02", "04.03", "04.04", "04.05", "04.06", "04.07", "--.--", "04.08", "04.09", "04.10", "04.11", "04.12", "04.13", "04.14", "04.15", "04.16", "--.--", "04.17", "04.18", "04.19", "04.20", "04.21", "04.22", "04.23"}, //r4
	{"03.01", "03.02", "03.03", "03.04", "03.05", "03.06", "03.07", "--.--", "--.--", "03.08", "03.09", "03.10", "03.11", "03.12", "03.13", "03.14", "--.--", "--.--", "03.15", "03.16", "03.17", "03.18", "03.19", "03.20", "03.21"}, //r3
	{"02.01", "02.02", "02.03", "02.04", "02.05", "02.06", "02.07", "--.--", "02.08", "02.09", "02.10", "02.11", "02.12", "02.13", "02.14", "02.15", "02.16", "--.--", "02.17", "02.18", "02.19", "02.20", "02.21", "02.22", "02.23"}, //r2
	{"01.01", "01.02", "01.03", "01.04", "01.05", "01.06", "01.07", "--.--", "--.--", "--.--", "--.--", "--.--", "--.--", "--.--", "--.--", "--.--", "--.--", "--.--", "01.08", "01.09", "01.10", "01.11", "01.12", "01.13", "01.14"}, //r1
};

NetworkGUI::NetworkGUI(NetworkManager *nm)
{
	_netManager = nm;
	_win = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "THE FUTURE");
	_win->setFramerateLimit(60);
	_windowSize = _win->getSize();
	_selectedGroup = -1;

	_font = new sf::Font();
	_font->loadFromFile("fonts/ParkTech.ttf");

	_syncOffset = SyncOffset::CreateLinearSyncOffset(1, 0);

	FillColorList();

	for (int x = 0; x < CLUSTER_MAX_ROW_SEATS + 2; x++) // +2 for cluster lanes
		for (int y = 0; y < CLUSTER_MAX_ROWS; y++)
			_GUIClients[y][x] = GUIClient{0, ClientStatus::Unknown};

	_netManager->SetClientStatusUpdateCallback(
		[this](const int row, const int seat, const ClientStatus status)
		{
			GUIClient & guic = FindGUIClient(row, seat);
			guic.status = status;
		}
	);

	_netManager->SetClientGroupChangeCallback(
		[this](const int row, const int seat, const int newGroup)
		{
			GUIClient & guic = FindGUIClient(row, seat);
			guic.groupId = newGroup;
		}
	);

	_netManager->SetClientShaderLoadCallback(
		[this](const int row, const int seat, const bool success)
		{
			GUIClient & guic = FindGUIClient(row, seat);
			if (success)
				guic.status = ClientStatus::ShaderLoaded;
			else
			{
				std::cout << "client at r" << row << "p" << seat << "failed to load shaders !" << std::endl;
				guic.status = ClientStatus::Error;
			}
		}
	);

	_netManager->SetClientShaderFocusCallback(
		[this](const int row, const int seat, const bool success)
		{
			GUIClient & guic = FindGUIClient(row, seat);
			if (success)
				guic.status = ClientStatus::Running;
			else
			{
				std::cout << "client at r" << row << "p" << seat << "failed to focus a shader !" << std::endl;
				guic.status = ClientStatus::Error;
			}
		}
	);
}

NetworkGUI::~NetworkGUI(void)
{
	delete _win;
	delete _font;
}

#define OFFSET_X		900
#define OFFSET_Y		50
#define PLACE_WIDTH		40
#define PLACE_HEIGHT	70
#define PLACE_PADDING_X	10
#define PLACE_PADDING_Y	20

void		NetworkGUI::DrawPlace(const int x, const int y, const bool clicked)
{
	int			posX = PLACE_WIDTH * x + PLACE_PADDING_X * x + OFFSET_X;
	int			posY = PLACE_HEIGHT * y + PLACE_PADDING_Y * y + OFFSET_Y;
	GUIClient &	client = _GUIClients[y][x];

	sf::RectangleShape rectangle(sf::Vector2f(30, 50));

	sf::Color c = _groupColors[0];
	if (static_cast< size_t>(client.groupId) < _groupColors.size())
		c = _groupColors[client.groupId];
	rectangle.setFillColor(c);

	rectangle.setPosition(posX, posY + ((!(x % 2)) ? 25 : 0));
	rectangle.setOutlineThickness(3);
	rectangle.setOutlineColor(clientStatusColor[client.status]);

	float mx = (float)_mousePosition.x / ((float)_windowSize.x / (float)WINDOW_WIDTH);
	float my = (float)_mousePosition.y / ((float)_windowSize.y / (float)WINDOW_HEIGHT);
	if (clicked
			&& mx > posX && mx <= posX + PLACE_WIDTH
			&& my > posY && my <= posY + PLACE_HEIGHT)
	{
		std::string ip = std::string("10.11.") + clusterIPMap[y][x];
		std::cout << "clicked on IP " << ip << std::endl;
		if (_selectedGroup != -1)
		{
			std::stringstream ss;
			ss.str(clusterIPMap[y][x]);
			std::string n;
			std::getline(ss, n, '.');
			int row = std::stoi(n);
			std::getline(ss, n, '.');
			int seat = std::stoi(n);
			_netManager->MoveIMacToGroup(_selectedGroup, row, seat);
		}
	}

	_win->draw(rectangle);
}

void		NetworkGUI::DrawCluster(const bool clicked)
{
	for (int x = 0; x < CLUSTER_MAX_ROW_SEATS + 2; x++) // +2 for cluster lanes
		for (int y = 0; y < CLUSTER_MAX_ROWS; y++)
			if (clusterMap[y][x])
				DrawPlace(x, y, clicked);
}

#define BUTTON_TEXT_PADDING 6
#define BUTTON_HEIGHT		35

bool		NetworkGUI::DrawButton(const int x, const int y, const int width, const int height, const bool clicked, const std::string & text, const sf::Color & color) const
{
	sf::RectangleShape	buttonBackgroud(sf::Vector2f(width, height));
	sf::Text			buttonText;

	buttonBackgroud.setPosition(x, y);
	buttonBackgroud.setFillColor(color);

	buttonText.setFont(*_font);
	buttonText.setString(text);
	buttonText.setCharacterSize(16);
	buttonText.setFillColor(sf::Color::White);
	buttonText.setPosition(x + BUTTON_TEXT_PADDING, y + BUTTON_TEXT_PADDING);

	_win->draw(buttonBackgroud);
	_win->draw(buttonText);

	if (clicked)
	{
		float mx = (float)_mousePosition.x / ((float)_windowSize.x / (float)WINDOW_WIDTH);
		float my = (float)_mousePosition.y / ((float)_windowSize.y / (float)WINDOW_HEIGHT);
		if (mx > x && mx <= x + width && my > y && my <= y + height)
			return true;
	}
	return false;
}

void		NetworkGUI::DrawText(const int x, const int y, const std::string & text) const
{
	sf::Text			buttonText;

	buttonText.setFont(*_font);
	buttonText.setString(text);
	buttonText.setCharacterSize(16);
	buttonText.setFillColor(sf::Color::White);
	buttonText.setPosition(x, y);

	_win->draw(buttonText);
}

#define GROUP_OFFSET_Y		50
#define GROUP_OFFSET_X		30
#define GROUP_PADDING_Y		20
#define GROUP_PADDING_X		10
#define GROUP_TEXT_WITH		100
#define GROUP_BUTTON_WIDTH	150

void		NetworkGUI::DrawGroupOptions(const bool clicked)
{
	size_t	groupCount = _netManager->GetGroupCount();

	if (DrawButton(GROUP_OFFSET_X, GROUP_OFFSET_Y, 220, BUTTON_HEIGHT, clicked, "ADD NEW GROUP", sf::Color(30, 120, 140)))
	{
		_netManager->CreateNewGroup();
		std::cout << "added a new group\n";
	}

	int	textX = GROUP_OFFSET_X;
	int	textY = GROUP_OFFSET_Y + BUTTON_HEIGHT + GROUP_PADDING_Y + BUTTON_TEXT_PADDING;
	int	buttonX = GROUP_OFFSET_X + GROUP_TEXT_WITH + GROUP_PADDING_X;
	int	buttonY = GROUP_OFFSET_Y + BUTTON_HEIGHT + GROUP_PADDING_Y;
	for (size_t i = 0; i < groupCount; i++)
	{
		sf::Color	c = sf::Color(200, 0, 255);
		if (i < _groupColors.size())
			c = _groupColors[i];
		DrawText(textX, textY, std::string("group ") + std::to_string(i));
		if (DrawButton(buttonX, buttonY, GROUP_BUTTON_WIDTH, BUTTON_HEIGHT, clicked, "", c))
			_selectedGroup = i;
		buttonY += GROUP_PADDING_Y + BUTTON_HEIGHT;
		textY += GROUP_PADDING_Y + BUTTON_HEIGHT;
	}
}

#define SELECTED_GROUP_OFFSET_X			(GROUP_TEXT_WITH + GROUP_BUTTON_WIDTH + GROUP_PADDING_X * 2)
#define SELECTED_GROUP_OFFSET_Y			50
#define SELECTED_GROUP_BUTTON_PADDING_X	20

void		NetworkGUI::DrawSelectedGroup(const bool clicked)
{
	int		x = SELECTED_GROUP_OFFSET_X + GROUP_OFFSET_X;
	int		y = SELECTED_GROUP_OFFSET_Y;

	if (_selectedGroup == -1)
		return ;

	sf::Color c = sf::Color::Blue;
	if (static_cast< unsigned >(_selectedGroup) < _groupColors.size())
		c = _groupColors[_selectedGroup];
	if (DrawButton(x, y, 100, BUTTON_HEIGHT, clicked, "START", c))
		_netManager->FocusShaderOnGroup(Timer::TimeoutInSeconds(1), _selectedGroup, 0, _syncOffset);
	std::string syncOffsetType = (_selectedSyncOffsetType == SyncOffsetType::None) ? "Sync: None" : "Sync: Linear";
	if (DrawButton(x + 100 + SELECTED_GROUP_BUTTON_PADDING_X, SELECTED_GROUP_OFFSET_Y, 180, BUTTON_HEIGHT, clicked, syncOffsetType, sf::Color(93, 0, 20)))
	{
		if (_selectedSyncOffsetType == SyncOffsetType::None)
		{
			_selectedSyncOffsetType = SyncOffsetType::Linear;
			_syncOffset = SyncOffset::CreateLinearSyncOffset(1, 0);
		}
		else
		{
			_selectedSyncOffsetType = SyncOffsetType::None;
			_syncOffset = SyncOffset::CreateNoneSyncOffset();
		}
	}
}

void		NetworkGUI::RenderLoop(void)
{
	bool		clicked;

	while (_win->isOpen())
    {
		clicked = false;
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
				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::Escape)
						_win->close();
					break ;
				case sf::Event::MouseMoved:
					_mousePosition.x = event.mouseMove.x;
					_mousePosition.y = event.mouseMove.y;
					break ;
				case sf::Event::Resized:
					_windowSize.x = event.size.width;
					_windowSize.y = event.size.height;
					break ;
				default:
					break ;
			}
        }

		_win->clear(sf::Color::Black);

		DrawGroupOptions(clicked);
		DrawSelectedGroup(clicked);
		DrawCluster(clicked);

		_win->display();
    }
}

void		NetworkGUI::FillColorList(void)
{
	srand(time(NULL) + clock());
	_groupColors.push_back(sf::Color(230, 230, 230));
	for (int i = 0; i < MAX_COLORS; i++)
	{
		char	r = 107 + rand() % 127;
		char	g = 107 + rand() % 127;
		char	b = 107 + rand() % 127;
		_groupColors.push_back(sf::Color(r, g, b));
	}
}

NetworkGUI::GUIClient &	NetworkGUI::FindGUIClient(const int row, const int seat)
{
	static GUIClient		defaultRet;

	for (int x = 0; x < CLUSTER_MAX_ROW_SEATS + 2; x++)
		for (int y = 0; y < CLUSTER_MAX_ROWS; y++)
		{
			std::string ip = std::string(((row < 10) ? "0" : "") + std::to_string(row) + "." + ((seat < 10) ? "0" : "") + std::to_string(seat));

			if (clusterIPMap[y][x] == ip)
				return _GUIClients[y][x];
		}
	return defaultRet;
}
