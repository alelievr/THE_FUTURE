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

#define DEBUG		0

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

static std::mutex		_mutex;
#if DEBUG >= 1
# define LOCK	std::cout << "locking " << __FUNCTION__ << ":" << __LINE__ << std::endl; _mutex.lock();
# define UNLOCK	std::cout << "unlocking " << __FUNCTION__ << ":" << __LINE__ << std::endl; _mutex.unlock();
#else
# define LOCK	_mutex.lock();
# define UNLOCK	_mutex.unlock();
#endif

int	NetworkManager::_localGroupId = 1;

NetworkManager::NetworkManager(bool server, bool co)
{
	struct sockaddr_in		connection;
	int						yes = 1;

	this->_connection = co;
	this->_isServer = server;
	this->_connectedToServer = false;
	this->_localClientIndex = 0;
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

Client &		NetworkManager::_FindClient(const int groupId, const size_t ip)
{
	static Client	defaultClient;

	LOCK;
	if (groupId < 0 || _clients.find(groupId) == _clients.end())
	{
		UNLOCK;
		return DEBUG("FindClient failed, groupId out of bounds: %i\n", groupId), defaultClient;
	}

	for (auto & cKP : _clients[groupId])
	{
		auto & c = cKP.second;
		if (c.ip == ip)
		{
			UNLOCK;
			return c;
		}
	}
	UNLOCK;
	DEBUG("Client %zu not found in group: %i\n", ip, groupId);
	return defaultClient;
}

void				NetworkManager::OnClientResourcesLoaded(Client & c)
{
	const auto & cLocalConfig = ClusterConfig::GetLocalParamsForClient(c.row, c.seat);

	for (const LocalParam & lp : cLocalConfig)
	{
		UpdateLocalParamOnClient(Timer::Now(), c.row, c.seat, lp.programIndex, lp.localParamName, lp.value);
	}
}

NetworkStatus		NetworkManager::_SendPacketToAllClients(const Packet & packet) const
{
	struct sockaddr_in		connection;
	bool					error = false;

	bzero(&connection, sizeof(connection));
	connection.sin_family = AF_INET;
	connection.sin_port = htons(CLIENT_PORT);

	LOCK;
	for (auto & clientListKP : _clients)
	{
		auto & clientList = clientListKP.second;
		for (const auto & cKP : clientList)
		{
			const Client & c = cKP.second;
			connection.sin_addr.s_addr = c.ip;
			if (sendto(_clientSocket, &packet, sizeof(packet), 0, reinterpret_cast< struct sockaddr * >(&connection), sizeof(connection)) < 0)
			{
				error = true;
				DEBUG2("[To all] sendto: %s at ip %s\n", strerror(errno), inet_ntoa(connection.sin_addr));
			}
		}
	}
	UNLOCK;
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

	for (const auto & cKP : clientGroupList)
	{
		const Client & c = cKP.second;
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

NetworkManager::Packet		NetworkManager::_CreateShaderFocusPacket(const int groupId, const Timeval *tv, const int programIndex, const int transitionIndex) const
{
	Packet	p;

	p.type = PacketType::ShaderFocus;
	p.groupId = groupId;
	p.timing = *tv;
	p.programIndex = programIndex;
	p.transitionIndex = transitionIndex;

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

NetworkManager::Packet		NetworkManager::_CreateTimeoutCheckPacket(void) const
{
	Packet	p;

	p.type = PacketType::TimeoutCheck;
	return p;
}

NetworkManager::Packet		NetworkManager::_CreateUpdateLocalParamPacket(const int groupId, const Timeval *tv, const int programIndex, const std::string & uniformName, const UniformParameter & uniformParam) const
{
	Packet	p;

	p.type = PacketType::LocalParamUpdate;
	p.groupId = groupId;
	p.programIndex = programIndex;
	p.timing = *tv;
	strcpy(p.uniformName, uniformName.c_str());
	p.uniformParam = uniformParam;
	return p;
}

NetworkManager::Packet		NetworkManager::_CreateLoadAudioFilePacket(const int groupId, const std::string & fileName, const bool last) const
{
	Packet	p;

	p.type = PacketType::LoadAudioFile;
	p.groupId = groupId;
	p.lastAudioFile = last;
	strcpy(p.audioFile, fileName.c_str());
	return p;
}

NetworkManager::Packet		NetworkManager::_CreateAudioUpdatePacket(const int groupId, const Timeval *timeout, const AudioUpdateType type, const int audioIndex, const float volume) const
{
	Packet	p;

	p.type = PacketType::AudioUpdate;
	p.timing = *timeout;
	p.groupId = groupId;
	p.audioUpdateType = type;
	p.audioIndex = audioIndex;
	p.audioVolume = volume;
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

NetworkManager::Packet		NetworkManager::_CreateShaderFocusResponsePacket(const bool success) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);
	
	Packet	p;

	p.type = PacketType::ShaderFocusResponse;
	p.seat = _me->seat;
	p.row = _me->row;
	p.success = success;
	p.groupId = _me->groupId;
	return p;
}

NetworkManager::Packet		NetworkManager::_CreateChangeGroupResponsePacket(const bool success) const
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

NetworkManager::Packet		NetworkManager::_CreateHelloPacket(void) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);

	Packet	p;

	p.type = PacketType::HelloServer;
	p.seat = _me->seat;
	p.row = _me->row;
	p.ip = _me->ip;
	p.groupId = _me->groupId;
	return p;
}

NetworkManager::Packet		NetworkManager::_CreateShaderLoadErrorPacket(void) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);

	Packet	p;

	p.type = PacketType::ShaderLoadResponse;
	p.seat = _me->seat;
	p.row = _me->row;
	p.success = false;
	p.groupId = _me->groupId;
	return p;
}

NetworkManager::Packet		NetworkManager::_CreateShaderLoadedPacket(void) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);

	Packet	p;

	p.type = PacketType::ShaderLoadResponse;
	p.seat = _me->seat;
	p.row = _me->row;
	p.success = true;
	p.groupId = _me->groupId;
	return p;
}

NetworkManager::Packet		NetworkManager::_CreateTimeoutCheckResponsePacket(void) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);

	Packet	p;
	p.ip = _me->ip;
	p.type = PacketType::TimeoutCheckResponse;
	p.groupId = _me->groupId;
	p.seat = _me->seat;
	p.row = _me->row;
	return p;
}

NetworkManager::Packet		NetworkManager::_CreateClientQuitPacket(const bool crash) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);

	Packet	p;

	p.type = PacketType::ClientQuit;
	p.crashed = crash;
	p.seat = _me->seat;
	p.row = _me->row;
	p.groupId = _me->groupId;
	return p;
}

NetworkManager::Packet		NetworkManager::_CreateLoadAudioFileResponsePacket(const bool success, const bool last) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);

	Packet	p;

	p.type = PacketType::LoadAudioFileResponse;
	p.success = success;
	p.ip = _me->ip;
	p.groupId = _me->groupId;
	p.seat = _me->seat;
	p.row = _me->row;
	p.lastAudioFile = last;
	return p;
}

NetworkManager::Packet		NetworkManager::_CreateAudioUpdateResponsePacket(const AudioUpdateType type, const bool success) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);

	Packet	p;

	p.type = PacketType::AudioUpdateResponse;
	p.audioUpdateType = type;
	p.success = success;
	p.ip = _me->ip;
	p.groupId = _me->groupId;
	p.seat = _me->seat;
	p.row = _me->row;
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

	LOCK;
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

			_clients[0][_localClientIndex++] = c;
		}
	UNLOCK;

	_SendPacketToAllClients(_CreatePokeStatusPacket());
	first = false;

	Timer::Timeout(Timer::TimeoutInSeconds(CLIENT_TIMEOUT),
		[this](void)
		{
			Timer::Interval(
				[this](void)
				{
					CheckClusterTimeout();
				},
				CLIENT_TIMEOUT * 1000
			);
		}
	);

	return NetworkStatus::Success;
}

NetworkStatus		NetworkManager::CheckClusterTimeout(void)
{
	if (!_isServer)
		return (std::cout << "not in server mode !" << std::endl, NetworkStatus::ServerReservedCommand);

	LOCK;
	const Packet p = _CreateTimeoutCheckPacket();
	for (auto & clientsKP : _clients)
	{
		for (auto & clientKP : clientsKP.second)
		{
			auto & client = clientKP.second;
			if (client.status != ClientStatus::Unknown && client.status != ClientStatus::Error && client.status != ClientStatus::Disconnected)
			{
				if (client.willTimeout)
				{
					client.status = ClientStatus::Timeout;
					if (_clientTimeoutCallback != NULL)
						_clientTimeoutCallback(client.row, client.seat);
				}
				client.willTimeout = true;
				_SendPacketToClient(client.ip, p);
			}
		}
	}
	UNLOCK;
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
	bool		success = false;

	switch (packet.type)
	{
		case PacketType::Status:
			_connectedToServer = true;
			strcpy(_serverIp, inet_ntoa(connection.sin_addr));
			_SendPacketToServer(_CreatePokeStatusResponsePacket());
			printf("responding to status\n");
			break ;
		case PacketType::LocalParamUpdate:
			if (_shaderLocalParamCallback != NULL)
				_shaderLocalParamCallback(&packetTiming, packet.programIndex, packet.uniformName, packet.uniformParam);
			break ;
		case PacketType::ShaderFocus:
			DEBUG("received focus program %i, timeout: %s\n", packet.programIndex, Timer::ReadableTime(packetTiming));
			if (_shaderFocusCallback != NULL)
			{
				bool success = _shaderFocusCallback(&packetTiming, packet.programIndex, packet.transitionIndex);
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
			std::cout << "group changed for: " << packet.newGroupId << std::endl;
			_me->groupId = packet.newGroupId;
			_SendPacketToServer(_CreateChangeGroupResponsePacket(true));
			break ;
		case PacketType::TimeoutCheck:
			std::cout << "responding to timeout !\n";
			_SendPacketToServer(_CreateTimeoutCheckResponsePacket());
			break ;
		case PacketType::LoadAudioFile:
			success = false;
			if (_loadAudioFileCallback != NULL)
				success = _loadAudioFileCallback(packet.audioFile);
			_SendPacketToServer(_CreateLoadAudioFileResponsePacket(success, packet.lastAudioFile));
			break ;
//		case PacketType::AudioUpdate:
//			success = false;
//			if (_audioUpdateCallback != NULL)
//				success = _audioUpdateCallback(&packetTiming, packet.audioUpdateType, packet.audioIndex, packet.audioVolume);
//			//_SendPacketToServer(_CreateAudioUpdateResponsePacket(packet.audioUpdateType, success));
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
					{
						auto & c = _FindClient(packet.groupId, packet.ip);

						c.status = packet.status;
						if (c.status == ClientStatus::WaitingForCommand)
						{
							int		clientGroupId = ClusterConfig::GetGroupForImac(packet.row, packet.seat);
							
							if (clientGroupId != -1)
								MoveIMacToGroup(clientGroupId, packet.row, packet.seat);

							const auto & clientConfig = ClusterConfig::GetConfigForGroup(clientGroupId);
							for (const std::string & shader : clientConfig.shaders)
								LoadShaderOnGroup(clientGroupId, shader, (&shader == &clientConfig.shaders.back()));
							for (const std::string & audioFile : clientConfig.audioFiles)
								LoadAudioFileOnGroup(clientGroupId, audioFile, (&audioFile == &clientConfig.audioFiles.back()));
						}
					}

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
					//WARNING: this event is received only when ALL the shaders are successfuly loaded
					{
						auto & c = _FindClient(packet.groupId, packet.ip);
						if (!packet.success)
							c.status = ClientStatus::Error;
						else
							c.status = ClientStatus::ShaderLoaded;

						if (_clientShaderLoadCallback != NULL)
							_clientShaderLoadCallback(packet.row, packet.seat, packet.success);
						const auto & cConfig = ClusterConfig::GetConfigForGroup(packet.groupId);
						c.shaderLoaded = true;
						if (cConfig.audioFiles.size() == 0 || (cConfig.audioFiles.size() != 0 && c.audioLoaded))
								OnClientResourcesLoaded(c);
					}
					break ;
				case PacketType::GroupChangeResponse:
					if (_clientGroupChangeCallback != NULL)
						_clientGroupChangeCallback(packet.row, packet.seat, packet.groupId);
					break ;
				case PacketType::ShaderFocusResponse:
					{
						auto & c = _FindClient(packet.groupId, packet.ip);
						if (!packet.success)
							c.status = ClientStatus::Error;
						else
							c.status = ClientStatus::Running;
					}
					if (_clientShaderFocusCallback != NULL)
						_clientShaderFocusCallback(packet.row, packet.seat, packet.success);
					break ;
				case PacketType::ShaderLocalParamResponse:
					//WARNING: not fired ...
					{
						auto & c = _FindClient(packet.groupId, packet.ip);
						if (!packet.success)
							c.status = ClientStatus::Error;
						else
							c.status = ClientStatus::Running;
					}
					if (_clientShaderLocalParamCallback != NULL)
						_clientShaderLocalParamCallback(packet.row, packet.seat, packet.success);
					break ;
				case PacketType::TimeoutCheckResponse:
					{
						auto & c = _FindClient(packet.groupId, packet.ip);
						c.willTimeout = false;
					}
					break ;
				case PacketType::ClientQuit:
					std::cout << "received quit packet from client !\n";
					{
						auto & c = _FindClient(packet.groupId, packet.ip);
						c.status = ClientStatus::Disconnected;
					}

					if (_clientQuitCallback != NULL)
						_clientQuitCallback(packet.row, packet.seat);
					break ;
				case PacketType::AudioUpdateResponse:
					//WARNING: this is event is disabled cause there are no reason for it to fail !
					switch (packet.audioUpdateType)
					{
						case AudioUpdateType::Play:
							break ;
						case AudioUpdateType::Pause:
							break ;
						case AudioUpdateType::Volume:
							break ;
					}
					std::cout << "olol1\n";
					break ;
				case PacketType::LoadAudioFileResponse:
					{
						auto & c = _FindClient(packet.groupId, packet.ip);
						if (packet.success)
							c.audioStatus = AudioStatus::Loaded;
						else
							c.audioStatus = AudioStatus::FailedToLoad;
						std::cout << "olol2\n";

						if (packet.lastAudioFile)
							c.audioLoaded = true;
						if (packet.lastAudioFile && c.shaderLoaded)
							OnClientResourcesLoaded(c);
					}
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
	_clients[_localGroupId] = std::map< int, Client >();
	return _localGroupId++;
}
#include <iterator>

NetworkStatus		NetworkManager::MoveIMacToGroup(const int groupId, const int row, const int seat)
{
	std::map< int, std::map< int, Client > >::iterator	group;
	int												nRemoved = 0;
	Client											moved;
	int												oldGroup = 0;

	LOCK;
	if ((group = _clients.find(groupId)) != _clients.end())
	{
		for (auto & clientKP : _clients)
		{
			for (auto it = clientKP.second.begin(); it != clientKP.second.end(); ++it)
			{
				auto & c = it->second;
				if (c.row == row && c.seat == seat)
				{
					moved = c;
					nRemoved++;
					oldGroup = c.groupId;
					clientKP.second.erase(it);
					goto found;
				}
			}
		}
	}
	else
	{
		std::cout << "out of bounds of groupId in MoveImacToGroup !" << std::endl;
		UNLOCK;
		return NetworkStatus::OutOfBound;
	}

	found:

	if (nRemoved == 1)
	{
		_clients[groupId][_localClientIndex++] = moved;

		//Sort to be in the same order than the config file:
		const auto clientList = _clients[groupId];
		for (const auto & clientKP : clientList)
		{
			auto & m = _clients[groupId];
			auto & client = clientKP.second;

			size_t	index = ClusterConfig::GetImacIndexInGroup(groupId, client.row, client.seat);

			m.erase(clientKP.first);
			m[index] = clientKP.second;
			std::cout << "moved client " << client << " to index " << index << std::endl;
		}

		std::cout << "sorted imac list for group " << groupId << ":" << std::endl;
		for (const auto cKP : _clients[groupId])
			std::cout << cKP.second << ", ";
		std::cout << std::endl;

		_SendPacketToClient(moved.ip, _CreateChangeGroupPacket(groupId));
		UNLOCK;
		return NetworkStatus::Success;
	}
	else
	{
		DEBUG("client not found in cluster !\n");
		UNLOCK;
		return NetworkStatus::Error;
	}
}

NetworkStatus		NetworkManager::FocusShaderOnGroup(const Timeval *timeout, const int groupId, const int programIndex, const int transitionIndex, const SyncOffset & syncOffset) const
{
	return _SendPacketToGroup(groupId, _CreateShaderFocusPacket(groupId, timeout, programIndex, transitionIndex), syncOffset);
}

NetworkStatus		NetworkManager::UpdateLocalParamOnGroup(const Timeval *timeout, const int groupId, const int programIndex, const std::string & uniformName, const UniformParameter & uniformParam, const SyncOffset & syncOffset) const
{
	return _SendPacketToGroup(groupId, _CreateUpdateLocalParamPacket(groupId, timeout, programIndex, uniformName, uniformParam), syncOffset);
}

NetworkStatus		NetworkManager::UpdateLocalParamOnClient(const Timeval *timeout, const int row, const int seat, const int programIndex, const std::string name, const UniformParameter & value)
{
	std::string sip = "10.11." + std::to_string(row) + "." + std::to_string(seat);
	struct in_addr	addr;
	inet_aton(sip.c_str(), &addr);

	return _SendPacketToClient(addr.s_addr, _CreateUpdateLocalParamPacket(-1, timeout, programIndex, name, value));
}

NetworkStatus		NetworkManager::LoadShaderOnGroup(const int groupId, const std::string & shaderName, bool last) const
{
	return _SendPacketToGroup(groupId, _CreateShaderLoadPacket(groupId, shaderName, last), SyncOffset::CreateNoneSyncOffset());
}

NetworkStatus		NetworkManager::LoadAudioFileOnGroup(const int groupId, const std::string & fileName, const bool last) const
{
	return _SendPacketToGroup(groupId, _CreateLoadAudioFilePacket(groupId, fileName, last), SyncOffset::CreateNoneSyncOffset());
}

NetworkStatus		NetworkManager::UpdateAudioOnGroup(const Timeval *timeout, const int groupId, const AudioUpdateType type, const int audioIndex, const float audioVolume, const SyncOffset & syncOffset) const
{
	return _SendPacketToGroup(groupId, _CreateAudioUpdatePacket(groupId, timeout, type, audioIndex, audioVolume), syncOffset);
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

void	NetworkManager::SendQuit(const bool crash)
{
	_SendPacketToServer(_CreateClientQuitPacket(crash));
}

//Client callbacks:
void	NetworkManager::SetShaderFocusCallback(ShaderFocusCallback callback) { _shaderFocusCallback = callback; }
void	NetworkManager::SetShaderLocalParamCallback(ShaderLocalParamCallback callback) { _shaderLocalParamCallback = callback; }
void	NetworkManager::SetShaderLoadCallback(ShaderLoadCallback callback) { _shaderLoadCallback = callback; }
void	NetworkManager::SetAudioUpdateCallback(AudioUpdateCallback callback) { _audioUpdateCallback = callback; }
void	NetworkManager::SetLoadAudioFileCallback(LoadAudioFileCallback callback) { _loadAudioFileCallback = callback; }

//server callbacks:

void	NetworkManager::SetClientStatusUpdateCallback(ClientStatusUpdateCallback callback) { _clientStatusUpdateCallback = callback; }
void	NetworkManager::SetClientGroupChangeCallback(ClientGroupChangeCallback callback) { _clientGroupChangeCallback = callback; }
void	NetworkManager::SetClientShaderLoadCallback(ClientShaderLoadCallback callback) { _clientShaderLoadCallback = callback; }
void	NetworkManager::SetClientShaderFocusCallback(ClientShaderFocusCallback callback) { _clientShaderFocusCallback = callback; }
void	NetworkManager::SetClientShaderLocalParamCallback(ClientShaderLocalParamCallback callback) { _clientShaderLocalParamCallback = callback; }
void	NetworkManager::SetClientTimeoutCallback(ClientTimeoutCallback callback) { _clientTimeoutCallback = callback; }
void	NetworkManager::SetClientQuitCallback(ClientQuitCallback callback) { _clientQuitCallback = callback; }
void	NetworkManager::SetClientAudioStatusCallback(ClientAudioStatusCallback callback) { _clientAudioStatusCallback = callback; }
void	NetworkManager::SetClientLoadAudioFileCallback(ClientLoadAudioFileCallback callback) { _clientLoadAudioFileCallback = callback; }

//Utils

bool		NetworkManager::IsServer(void) const { return (this->_isServer); }

std::ostream &	operator<<(std::ostream & o, NetworkManager const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
