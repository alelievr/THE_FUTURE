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
#include "Timer.hpp"
#include "LuaGL.hpp"
#include <functional>
#include <algorithm>

#define DEBUG

vec2		framebuffer_size = {0, 0};
std::list< const std::string > transitionShaders = {
	"shaders/transitions/blockyTrasition.glsl",
//	"shaders/transitions/burnTransition.glsl",
	"shaders/transitions/fadeTransition.glsl",
//	"shaders/transitions/flashTransition.glsl",
//	"shaders/transitions/perlinTransition.glsl",
};

ShaderRender::ShaderRender(void)
{
	_programLoaded = false;
	_currentTransitionIndex = -1;

	//Load transition shaders:
	for (const std::string & shader : transitionShaders)
	{
		ICGProgram	*prog = new ShaderProgram();
		prog->LoadSourceFile(shader);

		if (!prog->CompileAndLink())
		{
			delete prog;
			std::cout << "transition shader " << shader << " failed to compile !\n";
		}

		_transitionPrograms.push_back(prog);
	}

	//init_LuaGL(this);
}

void		ShaderRender::Render(void)
{
	if (!_programLoaded)
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

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	static int frames = 0;

//	load_run_script(getL(NULL), "lua/draw.lua");

	for (const std::size_t pIndex : _currentRenderedPrograms)
		if (pIndex < _programs.size())
		{
			_programs[pIndex]->Use();

			_programs[pIndex]->UpdateUniforms(framebuffer_size);

			_programs[pIndex]->Draw();
		}

	if (_currentTransitionIndex != -1)
	{
		auto & p = _transitionPrograms[_currentTransitionIndex];
		p->Use();
		p->UpdateUniforms(framebuffer_size);
		p->Draw();
	}

	frames++;
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool		ShaderRender::attachShader(const std::string file)
{
	ICGProgram	*newProgram;

	if (CheckFileExtension(file.c_str(), (const char *[]){"cl", NULL}))
		newProgram = new KernelProgram();
	else
		newProgram = new ShaderProgram();

	std::cout << "loading shader file: " << file << std::endl;

	newProgram->LoadSourceFile(file);

	if (!newProgram->CompileAndLink())
	{
		delete newProgram;
		return std::cout << "shader " << file << " failed to compile !\n", false;
	}
	else
		_programLoaded = true;

	_programs.push_back(newProgram);

	return true;
}

void		ShaderRender::SetCurrentRenderedShader(const size_t programIndex, const int transitionIndex)
{
	if (transitionIndex >= 0 && static_cast< size_t >(transitionIndex) < _transitionPrograms.size())
	{
		_currentTransitionIndex = transitionIndex;
		_transitionPrograms[_currentTransitionIndex]->UpdateLocalParam("localStartTime", 0, true);
		Timer::Timeout(Timer::TimeoutInMilliSeconds(500),
			[this, programIndex](void)
			{
				_currentRenderedPrograms.clear();
				_currentRenderedPrograms.push_back(programIndex);
			}
		);
		Timer::Timeout(Timer::TimeoutInMilliSeconds(1000),
			[this](void)
			{
				_currentTransitionIndex = -1;
			}
		);
	}
	else //no transition
	{
		_currentRenderedPrograms.clear();
		_currentRenderedPrograms.push_back(programIndex);
	}
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

void		ShaderRender::UpdateLocalParam(const int programIndex, const std::string & localParamName, const UniformParameter & param)
{
	if (programIndex < 0 || static_cast< size_t >(programIndex) >= _programs.size())
	{
		std::cout << "programIndex out of bounds";
		return ;
	}
	std::cout << "updating localParam of program " << programIndex << ": " << localParamName << " -> " << param.f1 << " (reset: " << param.reset << ")" << std::endl;
	_programs[programIndex]->UpdateLocalParam(localParamName, param.f1, param.reset);
}

void		ShaderRender::framebufferSizeCallback(int width, int height)
{
	framebuffer_size.x = width;
	framebuffer_size.y = height;

	for (auto program : _programs)
		program->UpdateFramebufferSize(vec2{static_cast< float >(width), static_cast< float >(height)});
}

ShaderProgram	*ShaderRender::GetProgram(int id)
{
	ShaderProgram	*sp;
	sp = (ShaderProgram*)(this->_programs[id]);
	return sp;
}

ShaderRender::~ShaderRender()
{
	for (auto program : _programs)
		delete program;
	//lua_close(getL(NULL));		// Cya, Lua
}
