#include "NetworkManager.hpp"
#include "Timer.hpp"
#include "ClusterConfig.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <functional>
#include <sstream>
#include <vector>
#include <locale>
#include <sys/time.h>

#define DEBUG		1

#ifdef DEBUG
# if DEBUG == 1
#  undef DEBUG
#  define DEBUG(...) printf(__VA_ARGS__)
#  define DEBUG2(...)
# elif DEBUG == 2
#  undef DEBUG
#  define DEBUG(...) printf(__VA_ARGS__)
#  define DEBUG2(...) {printf("\033[38;5;42m");printf(__VA_ARGS__);printf("\033[0m");}
# else
#  define DEBUG2(...) (void)0
#  undef DEBUG
#  define DEBUG(...) (void)0
# endif
#else // DEBUG
# define DEBUG2(...) (void)0
# define DEBUG(...) (void)0
#endif

int	NetworkManager::_localGroupId = 1;

NetworkManager::NetworkManager(bool server, bool co)
{
	struct sockaddr_in		connection;
	int						yes = 1;

	this->_connection = co;
	this->_isServer = server;
	this->_connectedToServer = false;
	bzero(&connection, sizeof(connection));

	FD_ZERO(&_serverFdSet);

	_FillLocalInfos();

	if ((_clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		perror("socket"), exit(-1);

	if (server)
	{
		//here _serverSocket is used to read messages from clients and _clientSocket to write messages to clients
		if ((_serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
			perror("socket"), exit(-1);
		connection.sin_family = AF_INET;
		connection.sin_port = htons(SERVER_PORT);
		connection.sin_addr.s_addr = htonl(INADDR_ANY);

		if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
			perror("setsockopt"), exit(-1);

		if (bind(_serverSocket, reinterpret_cast< struct sockaddr* >(&connection), sizeof(connection)) == -1)
			perror("bind"), exit(-1);

		FD_SET(_serverSocket, &_serverFdSet);

		std::cout << "server started !" << std::endl;
	}
	else
	{
		//here _serverSocket is the client socket which listen to the server and _clientSocket is used to write to the server
		if ((_serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
			perror("socket"), exit(-1);
		connection.sin_family = AF_INET;
		connection.sin_port = htons(CLIENT_PORT);
		if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(int)) == -1)
			perror("setsockopt"), exit(-1);
		connection.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(_serverSocket, reinterpret_cast< struct sockaddr* >(&connection), sizeof(connection)) == -1)
			perror("bind"), exit(-1);
		
		std::cout << "client bound udp port " << CLIENT_PORT << " !" << std::endl;
		FD_SET(_serverSocket, &_serverFdSet);

		_SendHelloPacket();
	}
}

NetworkManager::~NetworkManager(void)
{
}

// Private functions

void		NetworkManager::_SendHelloPacket()
{
	Packet packet = _CreateHelloPacket();
	for (int r = 1; r < CLUSTER_MAX_ROWS; r++)
		for (int p = 1; p < CLUSTER_MAX_ROW_SEATS; p++)
		{
			std::string ip = "10.1" + std::to_string(_me->cluster) + "." + std::to_string(r) + "." + std::to_string(p);
			struct sockaddr_in		connection;

			bzero(&connection, sizeof(connection));
			connection.sin_family = AF_INET;
			connection.sin_port = htons(SERVER_PORT);
			if (inet_aton(ip.c_str(), &connection.sin_addr) == 0)
				perror("inet_aton");

			if (sendto(_clientSocket, &packet, sizeof(packet), 0, reinterpret_cast< struct sockaddr * >(&connection), sizeof(connection)) < 0)
				perror("sendto");

		}
}

struct ipReader: std::ctype< char >
{
	ipReader(): std::ctype< char >(get_table()) {}
	static std::ctype_base::mask const* get_table() {
		static std::vector< std::ctype_base::mask > rc(table_size, std::ctype_base::mask());

		rc['.'] = std::ctype_base::space;
		return &rc[0];
	}
}; 

void	NetworkManager::_FillLocalInfos(void)
{
	struct ifaddrs  	*addrs;
	struct ifaddrs  	*tmp;
	std::stringstream	ss;
	char				*ip;
	int					n, seat, row, cluster;

	getifaddrs(&addrs);
	tmp = addrs;
	while (tmp)
	{
		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET && !strcmp(tmp->ifa_name, "en0"))
			ip = inet_ntoa(((struct sockaddr_in *)tmp->ifa_addr)->sin_addr);
		tmp = tmp->ifa_next;
	}

	ss.str(ip);
	ss.imbue(std::locale(std::locale(), new ipReader()));
	ss >> n;
	ss >> cluster;
	cluster -= 10;
	ss >> row;
	ss >> seat;

	freeifaddrs(addrs);

	_me = new Client(row, seat, cluster, ip);
}

bool				NetworkManager::_ImacExists(const int row, const int seat) const
{
	static const struct { int row; int seat; } inexistantPositions[] = {
		{1, 15}, {1, 16}, {1, 17}, {1, 18}, {1, 19}, {1, 20}, {1, 21}, {1, 22}, {1, 23},
		{3, 22}, {3, 23},
		{5, 23},
		{6, 21}, {6, 22}, {6, 23},
		{8, 22}, {8, 23},
		{9, 21}, {9, 22}, {9, 23},
		{13, 15}, {13, 16}, {13, 17}, {13, 18}, {13, 19}, {13, 20}, {13, 21}, {13, 22}, {13, 23},
	};

	for (const auto & pos : inexistantPositions)
		if (row == pos.row && seat == pos.seat)
			return false;
	return true;
}

NetworkStatus		NetworkManager::_FindClient(const int groupId, const size_t ip, std::function< void(Client &) > callback)
{
	bool		found = false;

	if (groupId < 0 || _clients.find(groupId) == _clients.end())
		return DEBUG("Findclient failed, groupId out of bounds: %i\n", groupId), NetworkStatus::OutOfBound;

	auto & clientList = _clients[groupId];
	for (auto & c : clientList)
		if (c.ip == ip)
			found = true, callback(c);
	if (!found)
		DEBUG("Client %zu not found in group: %i\n", ip, groupId);
	return (found) ? NetworkStatus::Success : NetworkStatus::Error;
}

NetworkStatus		NetworkManager::_SendPacketToAllClients(const Packet & packet) const
{
	struct sockaddr_in		connection;
	bool					error = false;

	bzero(&connection, sizeof(connection));
	connection.sin_family = AF_INET;
	connection.sin_port = htons(CLIENT_PORT);

	for (auto clientList : _clients)
	{
		for (const Client & c : clientList.second)
		{
			connection.sin_addr.s_addr = c.ip;
			if (sendto(_clientSocket, &packet, sizeof(packet), 0, reinterpret_cast< struct sockaddr * >(&connection), sizeof(connection)) < 0)
			{
				error = true;
				DEBUG2("[To all] sendto: %s at ip %s\n", strerror(errno), inet_ntoa(connection.sin_addr));
			}
		}
	}
	return (error) ? NetworkStatus::MissingClients : NetworkStatus::Success;
}

NetworkStatus		NetworkManager::_SendPacketToGroup(const int groupId, Packet packet, const SyncOffset & sync) const
{
	struct sockaddr_in		connection;
	bool					error = false;
	Timeval					defaultTiming(packet.timing);
	Timeval					additionalTiming;
	int						i = 0;

	bzero(&connection, sizeof(connection));
	connection.sin_family = AF_INET;
	connection.sin_port = htons(CLIENT_PORT);

	const auto clientGroupKP = _clients.find(groupId);
	if (clientGroupKP == _clients.end())
	{
		DEBUG("groupId out of bouds: %i\n", groupId);
		return NetworkStatus::OutOfBound;
	}
	const auto clientGroupList = clientGroupKP->second;
	DEBUG("sending %zu packets to Imacs in group %i\n", clientGroupList.size(), groupId);

	//TODO: check the sync order and reverse it if needed + custom + random

	for (const Client & c : clientGroupList)
	{
		if (sync.type == SyncOffsetType::Linear)
		{
			packet.timing = defaultTiming + additionalTiming;
			additionalTiming += sync.linearDelay;
		}

		connection.sin_addr.s_addr = c.ip;
		DEBUG2("sending packet to: %s\n", inet_ntoa(connection.sin_addr));
		if (sendto(_clientSocket, &packet, sizeof(packet), 0, reinterpret_cast< struct sockaddr * >(&connection), sizeof(connection)) < 0)
		{
			error = true;
			DEBUG("[To group] sendto: %s For ip: %s\n", strerror(errno), inet_ntoa(connection.sin_addr));
		}
		i++;
	}
	return (error) ? NetworkStatus::MissingClients : NetworkStatus::Success;
}

NetworkStatus		NetworkManager::_SendPacketToServer(const Packet & packet) const
{
	bool			error = false;

	if (!_connectedToServer)
		return NetworkStatus::NotConnectedToServer;
	if (_isServer)
		return NetworkStatus::ClientReservedCommand;

	struct sockaddr_in		connection;

	bzero(&connection, sizeof(connection));
	connection.sin_family = AF_INET;
	connection.sin_port = htons(SERVER_PORT);
	if (inet_aton(_serverIp, &connection.sin_addr) == 0)
		perror("inet_aton");

	DEBUG("sending packet to server(%s) on port %i\n", inet_ntoa(connection.sin_addr), SERVER_PORT);
	if (sendto(_clientSocket, &packet, sizeof(packet), 0, reinterpret_cast< struct sockaddr * >(&connection), sizeof(connection)) < 0)

		error = true, perror("[To server] sendto");
	return (error) ? NetworkStatus::Error : NetworkStatus::Success;
}

NetworkStatus		NetworkManager::_SendPacketToClient(const int ip, const Packet & packet) const
{
	bool			error = false;

	if (!_isServer)
		return NetworkStatus::ServerReservedCommand;

	struct sockaddr_in		connection;

	bzero(&connection, sizeof(connection));
	connection.sin_family = AF_INET;
	connection.sin_port = htons(CLIENT_PORT);
	connection.sin_addr.s_addr = ip;

	if (sendto(_clientSocket, &packet, sizeof(packet), 0, reinterpret_cast< struct sockaddr * >(&connection), sizeof(connection)) < 0)

		error = true, perror("[To server] sendto");
	return (error) ? NetworkStatus::Error : NetworkStatus::Success;
}

void						NetworkManager::_InitPacketHeader(Packet *p, const Client & client, const PacketType type) const
{
	bzero(p, sizeof(Packet));

	p->type = type;
	p->groupId = client.groupId;
	p->ip = client.ip;
}

//server packet creation functions

NetworkManager::Packet		NetworkManager::_CreatePokeStatusPacket(void) const
{
	if (!_isServer)
		std::cout << "Attempted to create poke packet in client mode !\n", exit(-1);

	static Packet		p;

	p.type = PacketType::Status;

	return p;
}

NetworkManager::Packet		NetworkManager::_CreateShaderFocusPacket(const int groupId, const Timeval *tv, const int programIndex) const
{
	Packet	p;

	p.type = PacketType::ShaderFocus;
	p.groupId = groupId;
	p.timing = *tv;
	p.programIndex = programIndex;

	return p;
}

NetworkManager::Packet		NetworkManager::_CreateShaderLoadPacket(const int groupId, const std::string & shaderName, bool last) const
{
	Packet	p;

	p.type = PacketType::ShaderLoad;
	p.groupId = groupId;
	p.lastShader = last;
	strncpy(p.shaderName, shaderName.c_str(), MAX_SHADER_NAME - 1);
	return p;
}

NetworkManager::Packet		NetworkManager::_CreateChangeGroupPacket(const int groupId) const
{
	Packet	p;

	p.type = PacketType::ChangeGroup;
	p.groupId = groupId;
	p.newGroupId = groupId;
	return p;
}

//Client Packet creation functions

NetworkManager::Packet		NetworkManager::_CreatePokeStatusResponsePacket(void) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);

	Packet p;

	//TODO: integrate client status code

	_InitPacketHeader(&p, *_me, PacketType::Status);
	p.status = ClientStatus::WaitingForCommand;
	p.seat = _me->seat;
	p.row = _me->row;
	p.cluster = _me->cluster;
	p.ip = _me->ip;
	p.groupId = _me->groupId;
	gettimeofday(&p.clientTime, NULL);

	return p;
}

NetworkManager::Packet	NetworkManager::_CreateShaderFocusResponsePacket(const bool success) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);
	
	Packet	p;

	p.type = PacketType::ShaderFocusResponse;
	p.seat = _me->seat;
	p.row = _me->row;
	p.success = success;
	return p;
}

NetworkManager::Packet	NetworkManager::_CreateChangeGroupResponsePacket(const bool success) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);

	Packet	p;

	p.type = PacketType::GroupChangeResponse;
	p.seat = _me->seat;
	p.row = _me->row;
	p.groupId = _me->groupId;
	p.success = success;
	return p;
}

NetworkManager::Packet	NetworkManager::_CreateHelloPacket(void) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);

	Packet	p;

	p.type = PacketType::HelloServer;
	p.seat = _me->seat;
	p.row = _me->row;
	p.ip = _me->ip;
	return p;
}

NetworkManager::Packet	NetworkManager::_CreateShaderLoadErrorPacket(void) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);

	Packet	p;

	p.type = PacketType::ShaderLoadResponse;
	p.seat = _me->seat;
	p.row = _me->row;
	p.success = false;
	return p;
}

NetworkManager::Packet	NetworkManager::_CreateShaderLoadedPacket(void) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);

	Packet	p;

	p.type = PacketType::ShaderLoadResponse;
	p.seat = _me->seat;
	p.row = _me->row;
	p.success = true;
	return p;
}

//public functions

#include <netdb.h>
NetworkStatus				NetworkManager::ConnectCluster(int clusterNumber)
{
	static bool			first = true;

	if (!_isServer)
		return (std::cout << "not in server mode !" << std::endl, NetworkStatus::ServerReservedCommand);
	if (clusterNumber <= 0 || clusterNumber > 3)
		return (std::cout << "bad cluster number !" << std::endl, NetworkStatus::Error);

	_clients.clear();

	for (int r = 1; r <= CLUSTER_MAX_ROWS; r++)
		for (int s = 1; s <= CLUSTER_MAX_ROW_SEATS; s++)
		{
			if (!_ImacExists(r, s))
				continue ;

			std::string	ip = "10.1" + std::to_string(clusterNumber) + "." + std::to_string(r) + "." + std::to_string(s);

			if (_connection && first)
			{
				//TODO: check if ip address is responding
			}

			DEBUG2("added iMac ip %s to the client list\n", ip.c_str());

			Client		c(r, s, clusterNumber, ip.c_str());

			_clients[0].push_back(c);
		}

	_SendPacketToAllClients(_CreatePokeStatusPacket());
	first = false;
	return NetworkStatus::Success;
}

NetworkStatus		NetworkManager::CloseAllConnections(void)
{
	Packet		closePacket;
	_SendPacketToAllClients(closePacket);
	return NetworkStatus::Success;
}

void						NetworkManager::GetSyncOffsets(void)
{
}

int							NetworkManager::GetLocalRow(void) const
{
	return _me->row;
}

int							NetworkManager::GetLocalSeat(void) const
{
	return _me->seat;
}

int							NetworkManager::GetLocalCluster(void) const
{
	return _me->cluster;
}

int							NetworkManager::GetGroupCount(void) const
{
	return _localGroupId;
}

void						NetworkManager::_ClientSocketEvent(const struct sockaddr_in & connection, const Packet & packet)
{
	Timeval		packetTiming = packet.timing;

	_me->groupId = packet.groupId;
	std::cout << "client received message !\n";
	switch (packet.type)
	{
		case PacketType::Status:
			_connectedToServer = true;
			strcpy(_serverIp, inet_ntoa(connection.sin_addr));
			_SendPacketToServer(_CreatePokeStatusResponsePacket());
			printf("responding to status\n");
			break ;
		case PacketType::UniformUpdate:
			//_SendPacketToServer(_CreateShaderUniformResponseCallback(false));
			break ;
		case PacketType::ShaderFocus:
			DEBUG("received focus program %i, timeout: %s\n", packet.programIndex, Timer::ReadableTime(packetTiming));
			if (_shaderFocusCallback != NULL)
			{
				bool success = _shaderFocusCallback(&packetTiming, packet.programIndex);
				_SendPacketToServer(_CreateShaderFocusResponsePacket(success));
			}
			else
				_SendPacketToServer(_CreateShaderFocusResponsePacket(false));
			break ;
		case PacketType::ShaderLoad:
			if (_shaderLoadCallback != NULL)
				_shaderLoadCallback(std::string(packet.shaderName), packet.lastShader);
			break ;
		case PacketType::ChangeGroup:
			_me->groupId = packet.newGroupId;
			_SendPacketToServer(_CreateChangeGroupResponsePacket(true));
			break ;
		default:
			break ;
	}
}

void						NetworkManager::_ServerSocketEvent(void)
{
	Packet					packet;
	struct sockaddr_in      connection;
	socklen_t               connectionLen;
	long					r;
	Timeval					now;

	connectionLen = sizeof(connection);
	if ((r = recvfrom(_serverSocket, &packet, sizeof(packet), 0, reinterpret_cast< struct sockaddr * >(&connection), &connectionLen)) > 0)
	{
		if (_isServer)
		{
			switch (packet.type)
			{
				case PacketType::Status:
					std::cout << "server received status message from iMac e" << packet.cluster << "r" << packet.row << "p" << packet.seat << ", status: " << (int)packet.status << std::endl;
					gettimeofday(&now, NULL);

					//update the client status:
					_FindClient(packet.groupId, packet.ip, 
						[&](Client & c)
						{
							c.status = packet.status;

							if (c.status == ClientStatus::WaitingForCommand)
							{
								int		clientGroupId = ClusterConfig::GetGroupForImac(packet.row, packet.seat);
								
								if (clientGroupId != -1)
									MoveIMacToGroup(clientGroupId, packet.row, packet.seat);

								const auto & clientShaders = ClusterConfig::GetShadersInGroup(clientGroupId);
								for (const std::string & shader : clientShaders)
									LoadShaderOnGroup(clientGroupId, shader, (&shader == &clientShaders.back()));
							}
						}
					);

					if (_clientStatusUpdateCallback != NULL)
						_clientStatusUpdateCallback(packet.row, packet.seat, packet.status);

					break ;
				case PacketType::HelloServer:
					struct in_addr i;
					i.s_addr = packet.ip;
					DEBUG("Sending packet poke to %s\n", inet_ntoa(i));
					_SendPacketToClient(packet.ip, _CreatePokeStatusPacket());
					break ;
				case PacketType::ShaderLoadResponse:
					_FindClient(packet.groupId, packet.ip, 
						[&](Client & c)
						{
							if (!packet.success)
								c.status = ClientStatus::Error;
							else
								c.status = ClientStatus::ShaderLoaded;
						}
					);
					if (_clientShaderLoadCallback != NULL)
						_clientShaderLoadCallback(packet.row, packet.seat, packet.success);
					break ;
				case PacketType::GroupChangeResponse:
					if (_clientGroupChangeCallback != NULL)
						_clientGroupChangeCallback(packet.row, packet.seat, packet.groupId);
					break ;
				case PacketType::ShaderFocusResponse:
					_FindClient(packet.groupId, packet.ip, 
						[&](Client & c)
						{
							if (!packet.success)
								c.status = ClientStatus::Error;
							else
								c.status = ClientStatus::Running;
						}
					);
					if (_clientShaderFocusCallback != NULL)
						_clientShaderFocusCallback(packet.row, packet.seat, packet.success);
					break ;
				case PacketType::ShaderUniformResponse:
					_FindClient(packet.groupId, packet.ip, 
						[&](Client & c)
						{
							if (!packet.success)
								c.status = ClientStatus::Error;
							else
								c.status = ClientStatus::Running;
						}
					);
					if (_clientShaderUniformCallback != NULL)
						_clientShaderUniformCallback(packet.row, packet.seat, packet.success);
					break ;
				default:
					break ;
			}
		}
		else
			_ClientSocketEvent(connection, packet);
	}
	else if (r == 0)
		FD_CLR(_serverSocket, &_serverFdSet);
	else
		perror("recvfrom");
}

NetworkStatus		NetworkManager::Update(void)
{
	struct timeval		timeout;
	fd_set				eventSet;

	bzero(&timeout, sizeof(timeout));

	eventSet = _serverFdSet;
	if (select(FD_SETSIZE, &eventSet, NULL, NULL, &timeout) < 0)
		perror("select"), exit(-1);
	for (int i = 0; i < FD_SETSIZE; ++i)
		if (FD_ISSET(i, &eventSet))
		{
			if (i == _serverSocket)
				_ServerSocketEvent();
			else
			{
				std::cout << "received event on socket " << i << std::endl;
				//void the socket
				read(i, (char[0xF000]){0}, 0xF000);
			}
		}

	return NetworkStatus::Success;
}

// Public server fuctions

int		NetworkManager::CreateNewGroup(void)
{
	_clients[_localGroupId] = std::list< Client >();
	return _localGroupId++;
}

NetworkStatus		NetworkManager::MoveIMacToGroup(const int groupId, const int row, const int seat)
{
	std::map< int, std::list< Client > >::iterator	group;
	int												nRemoved = 0;
	Client											moved;

	if ((group = _clients.find(groupId)) != _clients.end())
	{
		for (auto clientKP : _clients)
			clientKP.second.remove_if(
				[&](const Client & c) mutable
				{
					if (c.row == row && c.seat == seat)
					{
						moved = c;
						nRemoved++;
						return true;
					}
					return false;
				}
			);
	}
	else
	{
		DEBUG("out of bounds of groupId in MoveImacToGroup !\n");
		return NetworkStatus::OutOfBound;
	}

	if (nRemoved == 1)
	{
		_clients[groupId].push_back(moved);
		_SendPacketToClient(moved.ip, _CreateChangeGroupPacket(groupId));
		std::cout << "moved imac " << moved << " to group " << groupId << std::endl;
		return NetworkStatus::Success;
	}
	else
	{
		DEBUG("client not found in cluster !\n");
		return NetworkStatus::Error;
	}
}

NetworkStatus		NetworkManager::FocusShaderOnGroup(const Timeval *timeout, const int groupId, const int programIndex, const SyncOffset & syncOffset) const
{
	return _SendPacketToGroup(groupId, _CreateShaderFocusPacket(groupId, timeout, programIndex), syncOffset);
}

NetworkStatus		NetworkManager::UpdateUniformOnGroup(const Timeval *timeout, const int groupId, const std::string uniformName, ...) const
{
	(void)timeout;
	(void)groupId;
	(void)uniformName;
	return NetworkStatus::Success;
}

NetworkStatus		NetworkManager::LoadShaderOnGroup(const int groupId, const std::string & shaderName, bool last) const
{
	return _SendPacketToGroup(groupId, _CreateShaderLoadPacket(groupId, shaderName, last), SyncOffset::CreateNoneSyncOffset());
}

//public client functions:
void	NetworkManager::SendShaderLoadError(void)
{
	if (_isServer)
		return ;
	if (!_connectedToServer)
		return ;

	_SendPacketToServer(_CreateShaderLoadErrorPacket());
}

void	NetworkManager::SendShaderLoaded(void)
{
	if (_isServer)
		return ;
	if (!_connectedToServer)
		return ;

	_SendPacketToServer(_CreateShaderLoadedPacket());
}

//Client callbacks:
void	NetworkManager::SetShaderFocusCallback(ShaderFocusCallback callback)
{
	_shaderFocusCallback = callback;
}

void	NetworkManager::SetShaderUniformCallback(ShaderUniformCallback callback)
{
	_shaderUniformCallback = callback;
}

void	NetworkManager::SetShaderLoadCallback(ShaderLoadCallback callback)
{
	_shaderLoadCallback = callback;
}

//server callbacks:

void	NetworkManager::SetClientStatusUpdateCallback(ClientStatusUpdateCallback callback) { _clientStatusUpdateCallback = callback; }
void	NetworkManager::SetClientGroupChangeCallback(ClientGroupChangeCallback callback) { _clientGroupChangeCallback = callback; }
void	NetworkManager::SetClientShaderLoadCallback(ClientShaderLoadCallback callback) { _clientShaderLoadCallback = callback; }
void	NetworkManager::SetClientShaderFocusCallback(ClientShaderFocusCallback callback) { _clientShaderFocusCallback = callback; }
void	NetworkManager::SetClientShaderUniformCallback(ClientShaderUniformCallback callback) { _clientShaderUniformCallback = callback; }

//Utils

bool		NetworkManager::IsServer(void) const { return (this->_isServer); }

std::ostream &	operator<<(std::ostream & o, NetworkManager const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
