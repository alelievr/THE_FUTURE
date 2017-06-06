/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NetworkManager.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/02 17:40:05 by alelievr          #+#    #+#             */
/*   Updated: 2017/06/07 01:46:32 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "NetworkManager.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <functional>
#include <sstream>
#include <vector>
#include <locale>

int	NetworkManager::_localGroupId = 1;

NetworkManager::NetworkManager(bool server)
{
	struct sockaddr_in		connection;
	int						yes = 1;

	this->_isServer = server;
	this->_connectedToServer = false;
	bzero(&connection, sizeof(connection));

	FD_ZERO(&_serverFdSet);

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

		_FillLocalInfos();
		
		std::cout << "client bound udp port " << CLIENT_PORT << " !" << std::endl;
		FD_SET(_serverSocket, &_serverFdSet);
	}

	if ((_clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		perror("socket"), exit(-1);
}

NetworkManager::~NetworkManager(void)
{
}

// Private functions

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
				error = true;
		}
	}
	return (error) ? NetworkStatus::Error : NetworkStatus::Success;
}

NetworkStatus		NetworkManager::_SendPacketToGroup(const int groupId, const Packet & packet)
{
	struct sockaddr_in		connection;
	bool					error = false;

	bzero(&connection, sizeof(connection));
	connection.sin_family = AF_INET;
	connection.sin_port = htons(CLIENT_PORT);

	for (const Client & c : _clients[groupId])
	{
		connection.sin_addr.s_addr = c.ip;
		if (sendto(_clientSocket, &packet, sizeof(packet), 0, reinterpret_cast< struct sockaddr * >(&connection), sizeof(connection)) < 0)

			error = true, perror("[To group] sendto");
	}
	return (error) ? NetworkStatus::Error : NetworkStatus::Success;
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

	if (sendto(_clientSocket, &packet, sizeof(packet), 0, reinterpret_cast< struct sockaddr * >(&connection), sizeof(connection)) < 0)

		error = true, perror("[To server] sendto");
	return (error) ? NetworkStatus::Error : NetworkStatus::Success;
}

void	NetworkManager::_InitPacketHeader(Packet *p, const Client & client, const PacketType type) const
{
	bzero(p, sizeof(Packet));

	p->type = type;
	p->groupId = client.groupId;
	p->ip = client.ip;
}

NetworkManager::Packet	NetworkManager::_CreatePokeStatusPacket(void) const
{
	if (!_isServer)
		std::cout << "Attempted to create poke packet in client mode !\n", exit(-1);

	static Packet		p;

	p.type = PacketType::Status;

	return p;
}

NetworkManager::Packet	NetworkManager::_CreatePokeStatusResponsePacket(const Client & client, const Packet & oldPacket) const
{
	if (_isServer)
		std::cout << "Attempted to create poke response packet in server mode !\n", exit(-1);

	Packet p;

	//TODO: integrate client status code

	_InitPacketHeader(&p, client, PacketType::Status);
	p.status = ClientStatus::WaitingForCommand;
	p.seat = _me->seat;
	p.row = _me->row;
	p.cluster = _me->cluster;
	p.ip = oldPacket.ip;

	return p;
}

//public functions

NetworkStatus		NetworkManager::ConnectCluster(int clusterNumber)
{
	if (!_isServer)
		return (std::cout << "not in server mode !" << std::endl, NetworkStatus::ServerReservedCommand);
	if (clusterNumber <= 0 || clusterNumber > 3)
		return (std::cout << "bad cluster number !" << std::endl, NetworkStatus::Error);
	for (int r = 0; r < CLUSTER_MAX_ROWS; r++)
		for (int s = 0; s < CLUSTER_MAX_ROW_SEATS; s++)
		{
			std::string	ip = "10.1" + std::to_string(clusterNumber) + "." + std::to_string(r) + "." + std::to_string(s);

			Client		c(r, s, clusterNumber, ip.c_str());

			_clients[0].push_back(c);
		}

	_SendPacketToAllClients(_CreatePokeStatusPacket());
	return NetworkStatus::Success;
}

NetworkStatus		NetworkManager::CloseAllConnections(void)
{
	Packet		closePacket;
	_SendPacketToAllClients(closePacket);
	return NetworkStatus::Success;
}

void				NetworkManager::GetSyncOffsets(void)
{
}

void				NetworkManager::_ClientSocketEvent(const struct sockaddr_in & connection, const Packet & packet)
{
	switch (packet.type)
	{
		case PacketType::Status:
			_connectedToServer = true;
			strcpy(_serverIp, inet_ntoa(connection.sin_addr));
			_SendPacketToServer(_CreatePokeStatusResponsePacket(*_me, packet));
			break ;
		case PacketType::UniformUpdate:
			break ;
		default:
			break ;
	}
	std::cout << "client received message !\n";
}

void				NetworkManager::_ServerSocketEvent(void)
{
	Packet					packet;
	struct sockaddr_in      connection;
	socklen_t               connectionLen;
	long					r;

	connectionLen = sizeof(connection);
	if ((r = recvfrom(_serverSocket, &packet, sizeof(packet), 0, reinterpret_cast< struct sockaddr * >(&connection), &connectionLen)) > 0)
	{
		if (_isServer)
		{
			std::cout << "server received message !\n";
			switch (packet.type)
			{
				case PacketType::Status:
					std::cout << "server received status message from iMac e" << packet.cluster << "r" << packet.row << "p" << packet.seat << std::endl;
					break ;
				case PacketType::UniformUpdate:
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
	while (_messageQueue.size() != 0)
	{
		Message m = _messageQueue.front();
		_messageQueue.pop();

		_SendPacketToGroup(m.groupId, m.packet);
	}

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

// Public static fuctions

int		NetworkManager::CreateNewGroup(void)
{
	return _localGroupId++;
}

NetworkStatus		NetworkManager::AddIMacToGroup(int row, int seat, int floor)
{
	(void)row;
	(void)seat;
	(void)floor;
	return NetworkStatus::Success;
}

NetworkStatus		NetworkManager::RunShaderOnGroup(int groupId, const std::string & shaderName)
{
	SwitchShaderOnGroup(groupId, shaderName);
	return NetworkStatus::Success;
}

NetworkStatus		NetworkManager::UpdateUniformOnGroup(int groupId, const std::string uniformName, ...)
{
	(void)groupId;
	(void)uniformName;
	return NetworkStatus::Success;
}

NetworkStatus		NetworkManager::SwitchShaderOnGroup(int groupId, const std::string & shaderName)
{
	(void)groupId;
	(void)shaderName;
	return NetworkStatus::Success;
}

//Client callbacks:
void	NetworkManager::SetShaderSwitchCallback(std::function< void (Timeval *timing, const std::string & shaderName) > callback)
{
	_shaderSwitchCallback = callback;
}

//Utils

bool		NetworkManager::IsServer(void) const { return (this->_isServer); }

std::ostream &	operator<<(std::ostream & o, NetworkManager const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}