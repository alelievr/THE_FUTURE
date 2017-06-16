#include "ShaderApplication.hpp"
#include "NetworkManager.hpp"
#include "Timer.hpp"
#include "SyncOffset.hpp"
#include "Timeval.hpp"
#include "NetworkGUI.hpp"
#include "ClusterConfig.hpp"
#include <iostream>
#include <string>
#include <list>
#include <getopt.h>

#define CLUSTER_SCAN_INTERVAL	15 //secs
#define NETWORK_UPDATE_TIMEOUT	50 //ms

static bool		networkMustQuit = false;
static bool		server = false;
static bool		noNetwork = false;
static bool		connection = false;
static bool		threadHasExited = false;
static bool		serverGUINotInitialized = true;
static char *	shaderToLoad = NULL;
static bool		serverSentAllShadersToLoad;
static std::list< const std::string >	shadersToLoad;

static struct option longopts[] = {
	{ "server",     optional_argument,      NULL,           1},
	{ "connection", no_argument,			NULL, 			'c'},
	{ "nonetwork",  required_argument,      NULL,           'n'},
	{ NULL,         0,                      NULL,           0}
};

static void	usage(char *prog) __attribute__((noreturn));
static void	usage(char *prog)
{
	std::cout << "usage: " << prog << " <shader file>" << std::endl;
	exit(0);
}

static void options(int *ac, char ***av)
{
	int bflag, ch;

    bflag = 0;
    while ((ch = getopt_long(*ac, *av, "cn:", longopts, NULL)) != -1)
        switch (ch) {
            case 1:
                server = true;
				if (optarg != NULL)
					ClusterConfig::LoadConfigFile(optarg);
				else
					ClusterConfig::LoadConfigFile("");
                break;
			case 'c':
				connection = true;
				break ;
			case 'n':
				noNetwork = true;
				shaderToLoad = optarg;
				break ;
            default:
                usage(*av[0]);
     	}
    *ac -= optind;
    *av += optind;
}

static void NetworkThread(NetworkManager *nm, ShaderApplication *app)
{
	if (!server)
	{
		nm->SetShaderFocusCallback(
			[app](const Timeval *timing, const int programIndex)
			{
				if (app == NULL)
				{
					std::cout << "Can't focus a shader, shaderApplication not initialized" << std::endl;
					return ;
				}
				Timer::Timeout(timing,
					[programIndex, app](void)
					{
						std::cout << "focusing shader: " << programIndex << std::endl;
						app->FocusShader(programIndex);
					}
				);
			}
		);

		nm->SetShaderLoadCallback(
			[app](const std::string & shaderName, const bool last)
			{
				shadersToLoad.push_back(shaderName);
			
				if (last)
					serverSentAllShadersToLoad = true;
			}
		);

		while (!networkMustQuit)
		{
			if (nm->Update() == NetworkStatus::Error)
				break ;
			usleep(NETWORK_UPDATE_TIMEOUT * 1000);
		}
	}
	else
	{
		while (serverGUINotInitialized)
			usleep(10000);

		Timer::Interval(
			[&]()
			{
				nm->ConnectCluster(nm->GetLocalCluster());
			},
			CLUSTER_SCAN_INTERVAL * 1000,
			1 //function will block until first scan is complete
		);

		int		nGroups = ClusterConfig::GetGroupNumber();

		for (int i = 0; i < nGroups; i++)
			nm->CreateNewGroup();

		const auto & clusterConfig = ClusterConfig::GetClusterConfig();
		for (auto iMacConfig : clusterConfig)
			if (iMacConfig.groupId != 0)
				nm->MoveIMacToGroup(iMacConfig.groupId, iMacConfig.row, iMacConfig.seat);

		for (int i = 0; i < nGroups; i++)
		{
			const auto shaderList = ClusterConfig::GetShadersInGroup(i);
			for (const std::string & shader : shaderList)
				nm->LoadShaderOnGroup(i, shader, (&shader == &shaderList.back()));
		}

/*		int		group = nm->CreateNewGroup();

		nm->MoveIMacToGroup(group, nm->GetLocalRow(), nm->GetLocalSeat(), nm->GetLocalCluster());

		nm->LoadShaderOnGroup(group, "shaders/fractal/kifs.glsl");
		nm->LoadShaderOnGroup(group, "shaders/fractal/mandelbrot-orbit.glsl", true);

		std::cout << "focus shader 0 send !" << std::endl;
		nm->FocusShaderOnGroup(Timer::Now(), group, 0, SyncOffset::CreateLinearSyncOffset(1, 0));
		nm->FocusShaderOnGroup(Timer::TimeoutInSeconds(10), group, 1, SyncOffset::CreateNoneSyncOffset());*/

		while (!networkMustQuit)
		{
			if (nm->Update() == NetworkStatus::Error)
				break ;
			usleep(NETWORK_UPDATE_TIMEOUT * 1000);
		}
		nm->CloseAllConnections();
	}
	threadHasExited = true;
}

int		main(int ac, char **av)
{
	options(&ac, &av);

	if (server)
	{
		NetworkManager		nm(server, connection);
		std::thread			serverThread(NetworkThread, &nm, (ShaderApplication *)NULL);

		NetworkGUI			gui(&nm);
		serverGUINotInitialized = false;

		gui.RenderLoop();

		networkMustQuit = true;
		if (!threadHasExited)
			serverThread.join();
	}
	else
	{
		ShaderApplication		app(server);

		if (noNetwork)
		{
			app.LoadShader(shaderToLoad);
			app.loadingShaders = false;
			app.OnLoadingShaderFinished();
			app.FocusShader(0);

			app.RenderLoop();
		}
		else
		{
			NetworkManager		nm(server, connection);
			std::thread			clientThread(NetworkThread, &nm, &app);

			while (!serverSentAllShadersToLoad)
				usleep(16000);

			for (const std::string & shaderName : shadersToLoad)
				app.LoadShader(shaderName);
			app.loadingShaders = false;
			app.OnLoadingShaderFinished();

			app.RenderLoop();

			networkMustQuit = true;
			if (!threadHasExited)
				clientThread.join();
		}
	}

	return 0;
}
