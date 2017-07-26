#include "ShaderApplication.hpp"

static ShaderRender		*renderShader;

ShaderApplication::ShaderApplication(bool fullScreen, bool headless)
{
	loadingShaders = true;
	_programToFocus = -1;
	_headless = headless;

	if (!headless)
	{
		glfwSetErrorCallback(
			[](int code, const char *description)
			{
				(void)code;
				std::cout << "GLFW error: " << description << std::endl;
				exit(-1);
			}
		);

		framebuffer_size.x = WIN_W;
		framebuffer_size.y = WIN_H;

		//GLFW and shaderRender initializations
		if (!glfwInit())
			printf("glfwInit error !\n"), exit(-1);
		glfwWindowHint (GLFW_SAMPLES, 4);
		glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		GLFWmonitor *	monit = (fullScreen) ? glfwGetPrimaryMonitor() : NULL;
		int				win_w = WIN_W;
		int				win_h = WIN_H;
		const GLFWvidmode * mode = (fullScreen) ? glfwGetVideoMode(monit) : NULL;
		if (fullScreen)
		{
			win_w = mode->width;
			win_h = mode->height;
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		}
		if (!(window = glfwCreateWindow(win_w, win_h, "ShaderApp", monit, NULL)))
			printf("failed to create window !\n"), exit(-1);
		if (fullScreen)
			glfwSetWindowMonitor(window, monit, 0, 0, mode->width, mode->height, mode->refreshRate);

		glfwMakeContextCurrent (window);
		glfwSwapInterval(0);
	}

	shaderRender = new ShaderRender(headless);
	renderShader = shaderRender;

	if (!headless)
	{
		glfwSetFramebufferSizeCallback(window,
			[](GLFWwindow *, int width, int height) {
				renderShader->framebufferSizeCallback(width, height);
			}
		);

		glfwSetKeyCallback(window,
			[](GLFWwindow *win, int key, int scancode, int action, int mods) {
				(void)scancode;
				(void)action;
				(void)mods;
				if (key == GLFW_KEY_ESCAPE)
					glfwSetWindowShouldClose(win, true);
			}
		);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		int		fw, fh;
		glfwGetFramebufferSize(window, &fw, &fh);
		framebuffer_size.x = fw;
		framebuffer_size.y = fh;
	}
	std::cout << "framebuffser size: " << framebuffer_size.x << "/" << framebuffer_size.y << std::endl;
}

bool	ShaderApplication::LoadShader(const std::string & shaderFile)
{
	return shaderRender->attachShader(shaderFile);
}

void	ShaderApplication::FocusShader(const int programIndex, const int transitionIndex)
{
	if (loadingShaders)
	{
		std::cout << "stored shader to focus: " << programIndex << "\n";
		_programToFocus = programIndex;
		return ;
	}

	std::cout << "shader switched to display " << programIndex << std::endl;
	shaderRender->SetCurrentRenderedShader(programIndex, transitionIndex);
}

void	ShaderApplication::AddShader(const int programIndex)
{
	shaderRender->AddCurrentRenderedShader(programIndex);
}

void	ShaderApplication::UpdateLocalParam(const int programIndex, const std::string & uniformName, const UniformParameter & param)
{
	shaderRender->UpdateLocalParam(programIndex, uniformName, param);
}

void	ShaderApplication::OnLoadingShaderFinished()
{
	if (_programToFocus != -1)
		FocusShader(_programToFocus, -1);
}

void	ShaderApplication::RenderLoop(void)
{
	int		fps = 0;
	float	lastTime = 0;

	if (_headless)
		while (42)
			shaderRender->Render();
	else
	{
		while (!glfwWindowShouldClose(window))
		{
			shaderRender->Render();
			glfwSwapBuffers(window);
			glfwPollEvents();
			fps++;
			if (glfwGetTime() - lastTime > 1)
			{
				glfwSetWindowTitle(window, std::to_string(fps).c_str());
				fps = 0;
				lastTime = glfwGetTime();
			}
		}
		glfwTerminate();
	}
}

ShaderApplication::~ShaderApplication()
{
	delete shaderRender;
}
