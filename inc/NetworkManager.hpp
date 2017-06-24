/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NetworkManager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/02 17:39:53 by alelievr          #+#    #+#             */
/*   Updated: 2017/06/23 13:11:16 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NETWORKSERVER_HPP
# define NETWORKSERVER_HPP
# include "shaderpixel.h"
# include "SyncOffset.hpp"
# include "Timeval.hpp"
# include <iostream>
# include <string>
# include <list>
# include <map>
# include <queue>
# include <sys/select.h>
# include <netdb.h>
# include <sys/socket.h>
# include <stdio.h>
# include <arpa/inet.h>
# include <functional>

# define CLIENT_PORT			5446
# define SERVER_PORT			5447

# define MAX_UNIFORM_LENGTH		64
# define MAX_SHADER_NAME		128
# define MAX_UNIFORM_DATAS		4
# define MAX_MESSAGE_LENGTH		256
# define MAX_FILE_LENGTH		256

# define NOW					-1

# define CLIENT_TIMEOUT			10 //secs

# define E1						1
# define E2						2
# define E3						3

# define IP_LENGHT				sizeof("127.127.127.127")

enum class	NetworkStatus
{
	Success,						// obvious
	Error,							// a studid/strange error occured
	ServerReservedCommand,			// you try to run a server command as a client.
	ClientReservedCommand,			// you try to run a client command as a client.
	AnotherCommandIsRunning,		// obvious
	NotConnectedToServer,			// obvious
	MissingClients,					// the command was not executed on all selected clients
	OutOfBound,						//out of bounds, mostly for groupIds
};

enum class		ClientStatus
{
	Unknown,					//unknow client status
	Disconnected,				//the client is disconnected
	WaitingForCommand,			//nothing is running, client wait for first commands
	ShaderLoaded,				//a shader is running
	Running,					//the render loop is running
	Error,						//a command failed on the client
	Timeout,					//the client is not responding
};

enum class      UniformType
{
	Float1,
	Float2,
	Float3,
	Float4,
	Int1,
};

enum class		AudioUpdateType
{
	Play,
	Pause,
	Volume,
};

enum class		AudioStatus
{
	OK,
	FailedToLoad,
	Loaded,
	Playing,
	Paused,
	Unknown,
};

struct  UniformParameter
{
	UniformType type;
	bool		reset;
	union
	{
		int     i1;
		float   f1;
		vec2    f2;
		vec3    f3;
		vec4    f4;
	};
};


struct			Client
{
	int				seat;
	int				row;
	int				cluster;
	int				groupId;
	ClientStatus	status;
	AudioStatus		audioStatus;
	unsigned int	ip;
	Timeval			averageTimeDelta;
	bool			willTimeout;
	bool			audioLoaded;
	bool			shaderLoaded;

	Client(int row, int seat, int cluster, const char *cip)
	{
		struct sockaddr_in	connection;

		this->audioStatus = AudioStatus::Unknown;
		this->seat = seat;
		this->row = row;
		this->cluster = cluster;
		this->groupId = 0;
		this->audioLoaded = false;
		this->shaderLoaded = false;
		status = ClientStatus::Unknown;
		inet_aton(cip, &connection.sin_addr);
		ip = connection.sin_addr.s_addr;
	}

	Client & 	operator=(const Client & oth)
	{
		if (this != &oth)
		{
			this->audioLoaded = oth.audioLoaded;
			this->audioStatus = oth.audioStatus;
			this->shaderLoaded = oth.shaderLoaded;
			this->seat = oth.seat;
			this->cluster = oth.cluster;
			this->row = oth.row;
			this->groupId = oth.groupId;
			this->status = oth.status;
			this->ip = oth.ip;
			this->averageTimeDelta = oth.averageTimeDelta;
		}
		return *this;
	}

	Client() {}

	friend std::ostream &	operator<<(std::ostream & o, Client const & r)
	{
		return o << "e" << r.cluster << "r" << r.row << "p" << r.seat;
	}
};

//Client callbacks:
typedef std::function< bool (const Timeval *timing, const int programIndex, const int transitionIndex) > ShaderFocusCallback;
typedef std::function< void (const std::string & shaderName, const bool last) >				ShaderLoadCallback;
typedef std::function< Timeval & (const int seat, const int row, const int index) >			CustomSyncOffsetCallback;
typedef std::function< void (const Timeval *timing, const int programIndex, const std::string uniformName, const UniformParameter & param) > ShaderLocalParamCallback;
typedef std::function< bool (const Timeval *timing, const AudioUpdateType type, const size_t audioIndex, const float audioVolume) > AudioUpdateCallback;
typedef std::function< bool (const std::string & fileName )>								LoadAudioFileCallback;

//Server callbacks:
//                              row       saet
typedef std::function< void (const int, const int, const ClientStatus status) >		ClientStatusUpdateCallback;
typedef std::function< void (const int, const int, const int newGroup) >			ClientGroupChangeCallback;
typedef std::function< void (const int, const int, const bool success) >			ClientShaderLoadCallback;
typedef std::function< void (const int, const int, const bool success) >			ClientShaderFocusCallback;
typedef std::function< void (const int, const int, const bool success) >			ClientShaderLocalParamCallback;
typedef std::function< void (const int, const int) >								ClientTimeoutCallback;
typedef std::function< void (const int, const int) >								ClientQuitCallback;
typedef std::function< void (const int, const int, const AudioStatus status) >		ClientAudioStatusCallback;
typedef std::function< void (const int, const int, bool success) >					ClientLoadAudioFileCallback;

class		NetworkManager
{
	private:
		enum class		PacketType
		{
			Status,
			ShaderFocus,
			ShaderLoad,
			LocalParamUpdate,
			ChangeGroup,
			HelloServer,
			TimeoutCheck,
			ClientQuit,
			LoadAudioFile,
			AudioUpdate,
			ShaderLoadResponse,
			GroupChangeResponse,
			ShaderFocusResponse,
			ShaderLocalParamResponse,
			TimeoutCheckResponse,
			AudioUpdateResponse,
			LoadAudioFileResponse,
		};

		typedef struct timeval		NetworkTimeval;

		struct			Packet
		{
			PacketType		type;
			int				groupId;
			int				ip;
			NetworkTimeval	timing; //time to wait to execute datas
			union
			{
				struct //query client status
				{
					ClientStatus	status;
					int				seat;
					int				row;
					int				cluster;
					NetworkTimeval	clientTime;
				};
				struct //String packet (for errors/warning/debug)
				{
					char			message[MAX_MESSAGE_LENGTH];
				};
				struct //update client uniform
				{
					GLuint				:32; //programIndex;
					char				uniformName[MAX_UNIFORM_LENGTH];
					UniformParameter	uniformParam;
				};
				struct //Load shader
				{
					char	shaderName[MAX_SHADER_NAME];
					bool	lastShader;
				};
				struct //Shader focus
				{
					GLuint	programIndex;
					int		transitionIndex;
				};
				struct //Move imac to new group
				{
					int		newGroupId;
				};
				struct //Shader load/group change response AND audio status responses
				{
					bool	success;
				};
				struct //application quit packet
				{
					bool	crashed;
				};
				struct //audio update
				{
					char			:8; //success padding
					AudioUpdateType	audioUpdateType;
					size_t			audioIndex;
					float			audioVolume;
					bool			lastAudioFile;
				};
				struct //audio file load
				{
					char			audioFile[MAX_FILE_LENGTH];
				};
			};
		};

		std::map< int, std::map< int, Client > >	_clients;
		bool					_isServer;
		int						_serverSocket;
		int						_clientSocket;
		bool					_connectedToServer;
		bool					_connection;
		Client *				_me;
		char					_serverIp[IP_LENGHT];
		fd_set					_serverFdSet;
		static int				_localGroupId;
		int						_localClientIndex;

		ShaderFocusCallback		_shaderFocusCallback = NULL;
		ShaderLocalParamCallback _shaderLocalParamCallback = NULL;
		ShaderLoadCallback		_shaderLoadCallback = NULL;
		AudioUpdateCallback		_audioUpdateCallback = NULL;
		LoadAudioFileCallback	_loadAudioFileCallback = NULL;

		ClientStatusUpdateCallback	_clientStatusUpdateCallback = NULL;
		ClientGroupChangeCallback	_clientGroupChangeCallback = NULL;
		ClientShaderLoadCallback	_clientShaderLoadCallback = NULL;
		ClientShaderFocusCallback	_clientShaderFocusCallback = NULL;
		ClientShaderLocalParamCallback	_clientShaderLocalParamCallback = NULL;
		ClientTimeoutCallback		_clientTimeoutCallback = NULL;
		ClientQuitCallback			_clientQuitCallback = NULL;
		ClientAudioStatusCallback	_clientAudioStatusCallback = NULL;
		ClientLoadAudioFileCallback	_clientLoadAudioFileCallback = NULL;

		NetworkStatus			_SendPacketToAllClients(const Packet & packet) const;
		NetworkStatus			_SendPacketToGroup(const int groupId, Packet packet, const SyncOffset & sync) const;
		NetworkStatus			_SendPacketToClient(const int ip, const Packet & packet) const;
		NetworkStatus			_SendPacketToServer(const Packet & packet) const;

		void					_ServerSocketEvent(void);
		void                	_ClientSocketEvent(const struct sockaddr_in & connection, const Packet & packet);

		void					_FillLocalInfos(void);
		void					_SendHelloPacket(void);
		bool					_ImacExists(const int row, const int seat) const;
		Client &				_FindClient(const int groupId, const size_t ip);
		void					OnClientResourcesLoaded(Client & c);

		//Create packet functions:
		void					_InitPacketHeader(Packet *p, const Client & client, const PacketType type) const;
		Packet					_CreatePokeStatusPacket(void) const;
		Packet					_CreatePokeStatusResponsePacket(void) const;
		Packet					_CreateShaderFocusPacket(const int groupId, const Timeval *tv, const int programIndex, const int transitionIndex) const;
		Packet					_CreateShaderFocusResponsePacket(const bool success) const;
		Packet					_CreateShaderLoadPacket(const int groupId, const std::string & shaderName, bool last) const;
		Packet					_CreateShaderLoadResponsePacket(const bool success) const;
		Packet					_CreateChangeGroupPacket(const int groupId) const;
		Packet					_CreateChangeGroupResponsePacket(const bool success) const;
		Packet					_CreateUpdateLocalParamPacket(const int groupId, const Timeval *tv, const int programIndex, const std::string & uniformName, const UniformParameter & uniformParam) const;
		Packet					_CreateHelloPacket(void) const;
		Packet					_CreateShaderLoadErrorPacket(void) const;
		Packet					_CreateShaderLoadedPacket(void) const;
		Packet					_CreateTimeoutCheckPacket(void) const;
		Packet					_CreateTimeoutCheckResponsePacket(void) const;
		Packet					_CreateClientQuitPacket(const bool crash) const;
		Packet					_CreateLoadAudioFilePacket(const int groupId, const std::string & fileName, const bool last) const;
		Packet					_CreateLoadAudioFileResponsePacket(const bool success, const bool last) const;
		Packet					_CreateAudioUpdatePacket(const int groupId, const Timeval *timeout, const AudioUpdateType type, const int audioIndex, const float volume) const;
		Packet					_CreateAudioUpdateResponsePacket(const AudioUpdateType type, const bool success) const;

	public:
		NetworkManager(bool server = false, bool connection = false);
		NetworkManager(const NetworkManager&) = delete;
		virtual ~NetworkManager(void);

		NetworkManager &	operator=(NetworkManager const & src) = delete;

		NetworkStatus		ConnectCluster(int clusterNumber);
		NetworkStatus		CheckClusterTimeout(void);
		NetworkStatus		CloseAllConnections(void);
		void				GetSyncOffsets(void);
		NetworkStatus		Update(void);

		//Server callbacks:
		void			SetClientStatusUpdateCallback(ClientStatusUpdateCallback callback);
		void			SetClientGroupChangeCallback(ClientGroupChangeCallback callback);
		void			SetClientShaderLoadCallback(ClientShaderLoadCallback callback);
		void			SetClientShaderFocusCallback(ClientShaderFocusCallback callback);
		void			SetClientShaderLocalParamCallback(ClientShaderLocalParamCallback callback);
		void			SetClientTimeoutCallback(ClientTimeoutCallback callback);
		void			SetClientQuitCallback(ClientQuitCallback callback);
		void			SetClientAudioStatusCallback(ClientAudioStatusCallback callback);
		void			SetClientLoadAudioFileCallback(ClientLoadAudioFileCallback callback);

		//Client callbacks:
		void			SetShaderFocusCallback(ShaderFocusCallback callback);
		void			SetShaderLoadCallback(ShaderLoadCallback callback);
		void			SetShaderLocalParamCallback(ShaderLocalParamCallback callback);
		void			SetAudioUpdateCallback(AudioUpdateCallback callback);
		void			SetLoadAudioFileCallback(LoadAudioFileCallback callback);

		//server control functions:
		NetworkStatus	FocusShaderOnGroup(const Timeval *timeout, const int groupId, const int programIndex, const int transitionIndex, const SyncOffset & syncOffset) const;
		NetworkStatus	UpdateLocalParamOnGroup(const Timeval *timeout, const int groupId, const int programIndex, const std::string & uniformName, const UniformParameter & uniformParam, const SyncOffset & syncOffset) const;
		NetworkStatus	UpdateLocalParamOnClient(const Timeval *timeout, const int row, const int seat, const int programIndex, const std::string name, const UniformParameter & value);
		NetworkStatus	LoadShaderOnGroup(const int groupId, const std::string & shaderName, bool last = false) const;
		NetworkStatus	LoadAudioFileOnGroup(const int groupId, const std::string & fileName, const bool last) const;
		NetworkStatus	UpdateAudioOnGroup(const Timeval *timeout, const int groupId, const AudioUpdateType type, const int audioIndex, const float audioVolume, const SyncOffset & syncOffset) const;
		int				CreateNewGroup(void);
		NetworkStatus	MoveIMacToGroup(const int groupId, const int row, const int seat);

		//client control functions:
		void			SendShaderLoadError(void);
		void			SendShaderLoaded(void);
		void			SendQuit(const bool crash);

		int				GetLocalRow(void) const;
		int				GetLocalSeat(void) const;
		int				GetLocalCluster(void) const;

		int				GetGroupCount(void) const;

		bool			IsServer(void) const;
};

std::ostream &	operator<<(std::ostream & o, NetworkManager const & r);

#endif
