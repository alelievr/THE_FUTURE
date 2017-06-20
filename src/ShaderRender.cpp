/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderRender.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created  2016/07/11 18:11:03 by alelievr          #+#    #+#             */
/*   Updated  2016/07/25 18:53:04 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ShaderRender.hpp"
#include "LuaGL.hpp"
#include <functional>
#include <algorithm>

#define DEBUG

vec2		framebuffer_size = {0, 0};
vec2		window = {WIN_W, WIN_H};
float		pausedTime = 0;

ShaderRender::ShaderRender(void)
{
	angleAmount = vec2{0, 0};
	cursor_mode = 0;
	lastPausedTime = 0;
	programLoaded = false;

	//init_LuaGL(this);
}

void		ShaderRender::Render(void)
{
	if (!programLoaded)
		return ;

	//process render buffers if used:
/*	foreachShaderChannels([&](ShaderProgram *prog, ShaderChannel *channel) {
			(void)prog;

			if (channel->getType() == ShaderChannelType::CHANNEL_PROGRAM)
			{
				auto fboProgram = channel->getProgram();

#ifdef DEBUG
				std::cout << "bound framebuffer: " << fboProgram->getFramebufferId() << std::endl;
#endif
				glBindFramebuffer(GL_FRAMEBUFFER, fboProgram->getFramebufferId());

				glViewport(0, 0, framebuffer_size.x, framebuffer_size.y);

				glClearColor(0, 1, 0, 1);
				glClear(GL_COLOR_BUFFER_BIT);

				fboProgram->use();

				updateUniforms(fboProgram);

				fboProgram->draw();

				glBindTexture(GL_TEXTURE_2D, 0);

				uint8_t *data = (uint8_t *)malloc((int)framebuffer_size.x * (int)framebuffer_size.y * 4);
				typedef struct
				{
					int width;
					int height;
					uint8_t *data;
					size_t size;
				}	ppm_image;

				glReadBuffer(GL_COLOR_ATTACHMENT0);
				glReadPixels(0, 0, (int)framebuffer_size.x, (int)framebuffer_size.y, GL_RGB, GL_UNSIGNED_BYTE, data);

				int fd = open("f.ppm", O_WRONLY | O_CREAT, 0644);
				ppm_image img = (ppm_image){(int)framebuffer_size.x, (int)framebuffer_size.y, data, static_cast< size_t >((int)window.x * (int)window.y * 3)};

				dprintf(fd, "P6\n# THIS IS A COMMENT\n%d %d\n%d\n", 
						img.width, img.height, 0xFF);
				write(fd, img.data, img.width * img.height * 3);
			}
		}
	);*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, framebuffer_size.x, framebuffer_size.y);

	glClearColor(.75, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	load_run_script(getL(NULL), "lua/draw.lua");

	static int frames = 0;

	for (const std::size_t pIndex : _currentRenderedPrograms)
		if (pIndex < _programs.size())
		{
			_programs[pIndex]->Use();

			_programs[pIndex]->UpdateUniforms(framebuffer_size);

			_programs[pIndex]->Draw();
		}

	frames++;
	glBindTexture(GL_TEXTURE_2D, 0);
}

void		ShaderRender::displayWindowFps(void)
{
	static int		frames = 0;
	static double	last_time = 0;
	double			current_time = glfwGetTime();

	frames++;
	if (current_time - last_time >= 1.0)
	{
		printf("%sfps:%.3f%s", "\x1b\x37", 1.0 / (1000.0 / (float)frames) * 1000.0, "\x1b\x38");
		frames = 0;
		fflush(stdout);
		last_time++;
	}
}

bool		ShaderRender::attachShader(const std::string file)
{
	ICGProgram	*newProgram;

	if (CheckFileExtension(file.c_str(), (const char *[]){"cl"}))
		newProgram = new KernelProgram();
	else
		newProgram = new ShaderProgram();

	std::cout << "loading shader file: " << file << std::endl;

	newProgram->LoadSourceFile(file);

	if (!newProgram->CompileAndLink())
		return std::cout << "shader " << file << " failed to compile !\n", false;
	else
		programLoaded = true;

	_programs.push_back(newProgram);

	return true;
}

void		ShaderRender::SetCurrentRenderedShader(const size_t programIndex)
{
	_currentRenderedPrograms.clear();
	_currentRenderedPrograms.push_back(programIndex);
}

void		ShaderRender::AddCurrentRenderedShader(const size_t programIndex)
{
	_currentRenderedPrograms.push_back(programIndex);
}

void		ShaderRender::DeleteCurrentRenderedShader(const size_t programIndex)
{
	_currentRenderedPrograms.erase(std::remove(_currentRenderedPrograms.begin(), _currentRenderedPrograms.end(), programIndex), _currentRenderedPrograms.end());

}

void		ShaderRender::ClearCurrentRenderedShader()
{
	_currentRenderedPrograms.clear();
}

void		ShaderRender::UpdateUniform(const int programIndex, const std::string & uniformName, const UniformParameter & param)
{
	(void)programIndex;
	(void)uniformName;
	(void)param;
	//TODO: update uniform for program !
}

void		ShaderRender::framebufferSizeCallback(int width, int height)
{
	framebuffer_size.x = width;
	framebuffer_size.y = height;

	for (auto program : _programs)
		program->UpdateFramebufferSize(vec2{static_cast< float >(width), static_cast< float >(height)});
}

ShaderRender::~ShaderRender()
{
	for (auto program : _programs)
		delete program;
//	lua_close(getL(NULL));		// Cya, Lua
}
