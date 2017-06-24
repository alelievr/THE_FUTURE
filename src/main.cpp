#include "ShaderApplication.hpp"
#include "NetworkManager.hpp"
#include "Timer.hpp"
#include "SyncOffset.hpp"
#include "Timeval.hpp"
#include "NetworkGUI.hpp"
#include "ClusterConfig.hpp"
#include "AudioManager.hpp"
#include <iostream>
#include <string>
#include <list>
#include <getopt.h>

#define NETWORK_UPDATE_TIMEOUT	50 //ms

static bool		networkMustQuit = false;
static bool		server = false;
static bool		noNetwork = false;
static bool		connection = false;
static bool		threadHasExited = false;
static bool		fullScreen = false;
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
    while ((ch = getopt_long(*ac, *av, "fcn:", longopts, NULL)) != -1)
        switch (ch) {
            case 1:
                server = true;
				if (optarg != NULL)
					ClusterConfig::LoadConfigFile(optarg);
				else
					ClusterConfig::LoadConfigFile("");
                break;
			case 'f':
				fullScreen = true;
				break ;
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
		AudioManager		am;

		nm->SetShaderFocusCallback(
			[app](const Timeval *timing, const int programIndex, const int transitionIndex)
			{
				if (app == NULL)
				{
					std::cout << "Can't focus a shader, shaderApplication not initialized" << std::endl;
					return false;
				}
				Timer::Timeout(timing,
					[programIndex, app, transitionIndex](void)
					{
						std::cout << "focusing shader: " << programIndex << std::endl;
						return app->FocusShader(programIndex, transitionIndex);
					}
				);
				return true;
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

		nm->SetShaderLocalParamCallback(
			[app](const Timeval *timing, const int programIndex, const std::string uniformName, const UniformParameter & param)
			{
				Timer::Timeout(timing,
					[programIndex, uniformName, param, app](void)
					{
						return app->UpdateLocalParam(programIndex, uniformName, param);
					}
				);
			}
		);

		nm->SetLoadAudioFileCallback(
			[&am](const std::string & fileName)
			{
				return am.LoadAudioFile(fileName);
			}
		);

		nm->SetAudioUpdateCallback(
			[app, &am](const Timeval *timing, const AudioUpdateType type, const size_t audioIndex, const float audioVolume)
			{
				switch (type)
				{
					case AudioUpdateType::Play:
						Timer::Timeout(timing,
							[&am, audioIndex](void)
							{
								return am.Play(audioIndex);
							}
						);
						break ;
					case AudioUpdateType::Pause:
						Timer::Timeout(timing,
							[&am](void)
							{
								return am.Pause();
							}
						);
						break ;
					case AudioUpdateType::Volume:
						Timer::Timeout(timing,
							[&am, audioVolume](void)
							{
								return am.Volume(audioVolume);
							}
						);
						break ;
					default:
						break ;
				}
				return false;
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

		nm->ConnectCluster(nm->GetLocalCluster());

		int		nGroups = ClusterConfig::GetGroupNumber();

		for (int i = 0; i < nGroups; i++)
			nm->CreateNewGroup();

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
		ShaderApplication		app(fullScreen);

		if (noNetwork)
		{
			app.LoadShader(shaderToLoad);
			app.loadingShaders = false;
			app.OnLoadingShaderFinished();
			app.FocusShader(0, -1);
			app.LoadShader("shaders/test/texture-test.glsl");
			app.AddShader(1);
			app.LoadShader("shaders/noise/simplex.glsl");
			app.AddShader(2);

			app.RenderLoop();
		}
		else
		{
			NetworkManager		nm(server, connection);
			std::thread			clientThread(NetworkThread, &nm, &app);

			while (!serverSentAllShadersToLoad)
				usleep(16000);

			for (const std::string & shaderName : shadersToLoad)
				if (!app.LoadShader(shaderName))
					nm.SendShaderLoadError();
			app.loadingShaders = false;
			nm.SendShaderLoaded();
			app.OnLoadingShaderFinished();

			app.RenderLoop();

			nm.SendQuit(false);

			networkMustQuit = true;
			if (!threadHasExited)
				clientThread.join();
		}
	}
	Timer::ExitAllThreads();

	return 0;
}
