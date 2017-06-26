/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderProgram.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/07 20:35:27 by alelievr          #+#    #+#             */
/*                                                                            */
/* ************************************************************************** */

#include "ShaderProgram.hpp"

//#define DEBUG

const char * FRAGMENT_SHADER_HEADER =
"#version 330\n"
"in vec4 outColor;\n"
"out vec4 fragColor;\n"
"\n"
"uniform vec2           iResolution;\n"
"uniform float          iGlobalTime;\n"
"uniform float          iValue1;\n"
"uniform float          iValue2;\n"
"uniform float          iValue3;\n"
"uniform float          iValue4;\n"
"uniform sampler2D      iChannel0;\n"
"uniform sampler2D      iChannel1;\n"
"uniform sampler2D      iChannel2;\n"
"uniform sampler2D      iChannel3;\n"
"uniform sampler2D      iChannel4;\n"
"uniform sampler2D      iChannel5;\n"
"uniform sampler2D      iChannel6;\n"
"uniform sampler2D      iChannel7;\n"
"\n"
"void mainImage(vec2 f);\n"
"\n"
"vec4 texture2D(sampler2D s, vec2 coord, float f)\n"
"{\n"
"       return texture(s, coord, f);\n"
"}\n"
"\n"
"vec4 texture2D(sampler2D s, vec2 coord)\n"
"{\n"
"       return texture(s, coord);\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"       mainImage(gl_FragCoord.xy);\n"
"}\n"
"#line 1\n";

const char * VERTEX_SHADER_DEFAULT =
"#version 330\n"
"in vec2                fragPosition;\n"
"out vec4               outColor;\n"
"void main()\n"
"{\n"
"       gl_Position = vec4(fragPosition, 0.0, 1.0);\n"
"}\n";


#define VERTICES(x1, y1, x2, y2) {x1, y1, x1, y2, x2, y2, x2, y2, x2, y1, x1, y1}

static float _renderverticeMap[5][12] = {
	VERTICES(-1, -1, 1, 1),
	VERTICES(-1, -.7, 1, .7),
	VERTICES(-.42, -1, .42, 1),
	VERTICES(-.6, -1, .6, 1),
	VERTICES(-.5, -.5, .5, .5),
};

static	GLuint _renderCount = 6;

ShaderProgram::ShaderProgram(void)
{
	this->_id = 0;
	this->_framebufferId = 0;
	this->_renderId = -1;
	this->_vbo = -1;
	this->_vao = -1;
	this->_loaded = false;
	this->_firstUse = true;
	this->_viewport = ProgramViewport::Normal;

	this->_channels = new ShaderChannel[MAX_CHANNEL_COUNT];
}

ShaderProgram::~ShaderProgram(void) {
	delete [] this->_channels;
}

#define CHECK_ACTIVE_FLAG(x, y) if (strstr(piece, x)) mode |= y;
const std::string		ShaderProgram::_LoadSourceFile(const std::string & filePath)
{
	int				mode;
	std::string		fileSource = "";
	std::string		line;
	std::smatch		match;

	std::ifstream	file(filePath);
	std::regex		channelPragma("^\\s*#\\s*pragma\\s{1,}iChannel(\\d)\\s{1,}([^\\s]*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*).*");
	std::regex		vertexPragma("^\\s*#\\s*pragma\\s+vert\\s+(.*)");
	std::regex		viewportPragma("^\\s*#\\s*pragma\\s+viewport\\s+(normal|cinema|portrait|square|perfplz)$");

	static std::map< std::string, ProgramViewport > viewports = {
		{"normal", ProgramViewport::Normal},
		{"cinema", ProgramViewport::Cinema},
		{"portrait", ProgramViewport::Portrait},
		{"square", ProgramViewport::Square},
		{"perfplz", ProgramViewport::PerfPlz},
	};

	while (std::getline(file, line)) {
		if (std::regex_match(line, match, channelPragma))
		{
			std::string channelFile = match[2];
			int			channelIndex = std::stoi(match[1]);

			mode = 0;
			for (size_t i = 2; i < match.size(); ++i) {
				std::ssub_match sub_match = match[i];
				const char * piece = sub_match.str().c_str();
				CHECK_ACTIVE_FLAG("linear", CHAN_LINEAR);
				CHECK_ACTIVE_FLAG("nearest", CHAN_NEAREST);
				CHECK_ACTIVE_FLAG("mipmap", CHAN_MIPMAP);
				CHECK_ACTIVE_FLAG("v-flip", CHAN_VFLIP);
				CHECK_ACTIVE_FLAG("clamp", CHAN_CLAMP);
			}
			_channels[channelIndex].updateChannel(channelFile, mode);
		}
		else if (std::regex_match(line, match, vertexPragma))
		{
			//TODO
		}
		else if (std::regex_match(line, match, viewportPragma))
		{
			std::string viewportMode = match[1];

			if (viewports.find(viewportMode) != viewports.end())
				_viewport = viewports[viewportMode];
		}
		else
			fileSource += line + "\n";
	}
	return fileSource;
}

bool		ShaderProgram::LoadSourceFile(const std::string & file)
{
	try {
		if (CheckFileExtension(file.c_str(), (const char * []){"vert", "vs", "vsh", NULL}))
			_vertexFileSources.push_back(ShaderFile(file, _LoadSourceFile(file)));
		else
			_fragmentFileSources.push_back(ShaderFile(file, _LoadSourceFile(file)));
		return true;
	} catch (const std::exception & e) {
		return false;
	}
}

bool		ShaderProgram::CompileAndLink(void)
{
	if (_fragmentFileSources.size() == 0)
		return false;

	GLuint		fragmentShaderId;
	const char	*fragmentSources[] = {FRAGMENT_SHADER_HEADER, _fragmentFileSources[0].source.c_str()};
	const char	*vertexSources[] = {((_vertexFileSources.size() == 0) ? VERTEX_SHADER_DEFAULT : _vertexFileSources[0].source.c_str())};
   
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 2, fragmentSources, NULL);
	glCompileShader(fragmentShaderId);
	if (!CheckCompilation(fragmentShaderId))
		return false;

	GLuint		vertexShaderId;

	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, vertexSources, NULL);
	glCompileShader(vertexShaderId);
	if (!CheckCompilation(vertexShaderId))
		return false;

	if (_id != 0)
		glDeleteProgram(_id);
	_id = glCreateProgram();
	glAttachShader(_id, vertexShaderId);
	glAttachShader(_id, fragmentShaderId);
	glLinkProgram(_id);
	if (!CheckLink(_id))
		return false;

	CreateVAO();

	LoadUniformLocations();

	//inizialize fragPosition
	GLint       fragPos;

	if ((fragPos = glGetAttribLocation(_id, "fragPosition")) < 0)
		return false;
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glEnableVertexAttribArray(fragPos);
	glVertexAttribPointer(fragPos, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*) 0);

	_loaded = true;

	return true;
}

void		ShaderProgram::LoadUniformLocations(void)
{
	_uniforms["iGlobalTime"] = glGetUniformLocation(_id, "iGlobalTime");
	_uniforms["iResolution"] = glGetUniformLocation(_id, "iResolution");

	_uniforms["iValue1"] = glGetUniformLocation(_id, "iValue1");
	_uniforms["iValue2"] = glGetUniformLocation(_id, "iValue2");
	_uniforms["iValue3"] = glGetUniformLocation(_id, "iValue3");
	_uniforms["iValue4"] = glGetUniformLocation(_id, "iValue4");

	_uniforms["iChannel0"] = glGetUniformLocation(_id, "iChannel0");
	_uniforms["iChannel1"] = glGetUniformLocation(_id, "iChannel1");
	_uniforms["iChannel2"] = glGetUniformLocation(_id, "iChannel2");
	_uniforms["iChannel3"] = glGetUniformLocation(_id, "iChannel3");
	_uniforms["iChannel4"] = glGetUniformLocation(_id, "iChannel4");
	_uniforms["iChannel5"] = glGetUniformLocation(_id, "iChannel5");
	_uniforms["iChannel6"] = glGetUniformLocation(_id, "iChannel6");
	_uniforms["iChannel7"] = glGetUniformLocation(_id, "iChannel7");
}

void		ShaderProgram::Use(void)
{
#ifdef DEBUG
	std::cout << "use " << _id << std::endl;
#endif

	if (_firstUse)
		__localParams->at("localStartTime") = glfwGetTime(), _firstUse = false;

	glUseProgram(_id);
}

void		ShaderProgram::Draw(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);
	if (_renderId != -1)
	{
		GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, buffers);
	}
#ifdef DEBUG
	std::cout << "drawing program: " << _id << " to " << _vao << "\n";
#endif
	glBindVertexArray(_vao);
	glUseProgram(_id);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, _renderCount);
}

bool		ShaderProgram::CheckLink(GLuint program)
{
	GLint isLinked = 0;

	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		char		buff[maxLength];
		glGetProgramInfoLog(program, maxLength, &maxLength, buff);
		printf("%s\n", buff);

		glDeleteProgram(program);
		return false;
	}
	return true;
}

bool		ShaderProgram::CheckCompilation(GLuint shader)
{
	GLint isCompiled = 0;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		char		buff[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, buff);
		printf("%s\n", buff);

		glDeleteShader(shader);
		return false;
	}
	return true;
}

void			ShaderProgram::UpdateVAO(float *vert, GLuint count)
{
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//TODO: Vector3 management
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count, vert, GL_STATIC_DRAW);
}

void			ShaderProgram::CreateVAO(void)
{
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//TODO: Vector3 management
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _renderCount * 2, _renderverticeMap[static_cast< int >(_viewport)], GL_STATIC_DRAW);

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
}

#include <fcntl.h>
#include <unistd.h>
void			ShaderProgram::UpdateUniforms(const vec2 winSize, bool pass)
{
	if (!_loaded)
		return ;

	glUniform2f(_uniforms["iResolution"], winSize.x, winSize.y);
	glUniform1f(_uniforms["iGlobalTime"], glfwGetTime() - __localParams->at("localStartTime"));

	int j = 0;
	for (int i = 0; i < MAX_CHANNEL_COUNT; i++)
	{
		ShaderChannel * channel = &_channels[i];
		ShaderProgram *p = channel->getProgram();

		switch (_channels[i].getType())
		{
			case ShaderChannelType::CHANNEL_IMAGE:
				glActiveTexture(GL_TEXTURE1 + j);
				glBindTexture(GL_TEXTURE_2D, channel->getTextureId());
				glUniform1i(_uniforms["iChannel" + std::to_string(j++)], channel->getTextureId());
				break ;
			case ShaderChannelType::CHANNEL_SOUND:
				/*soundTexId = get_sound_texture(channel->getTextureId());
				glActiveTexture(GL_TEXTURE1 + j);
				glBindTexture(GL_TEXTURE_2D, soundTexId);
				updateUniform1("iChannel" + std::to_string(j++), soundTexId);*/
				break ;
			case ShaderChannelType::CHANNEL_PROGRAM:
				if (p == this && pass)
					break ;
				if (p == this && !pass)
					pass = true;
				p->Use();
				p->UpdateUniforms(winSize, pass);
				p->Draw();

				{
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
				ppm_image img = (ppm_image){(int)framebuffer_size.x, (int)framebuffer_size.y, data, static_cast< size_t >((int)framebuffer_size.x * (int)framebuffer_size.y * 3)};

				dprintf(fd, "P6\n# THIS IS A COMMENT\n%d %d\n%d\n", 
						img.width, img.height, 0xFF);
				write(fd, img.data, img.width * img.height * 3);
				}

				Use();

				glActiveTexture(GL_TEXTURE1 + j);
				glBindTexture(GL_TEXTURE_2D, channel->getTextureId());
				glUniform1i(_uniforms["iChannel" + std::to_string(j++)], channel->getTextureId());
			default:
				break ;
		}
	}

	for (const auto & param : *__localParams)
	{
		if (_uniforms.find(param.first) != _uniforms.end())
			glUniform1f(_uniforms[param.first], param.second);
		else
			glUniform1f(glGetUniformLocation(_id, param.first.c_str()), param.second);
	}
}

void	ShaderProgram::UpdateFramebufferSize(const vec2 fbSize)
{
	for (int i = 0; i < MAX_CHANNEL_COUNT; i++)
		if (_channels[i].getType() == ShaderChannelType::CHANNEL_PROGRAM)
		{
			auto program = _channels[i].getProgram();
			glTexImage2D(program->GetRenderId(), 0, GL_RGB, framebuffer_size.x, framebuffer_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			if (program != this)
				program->UpdateFramebufferSize(fbSize);
		}
}

void	ShaderProgram::SetFramebufferId(const GLuint fbo) { _framebufferId = fbo; }
GLuint	ShaderProgram::GetFramebufferId(void) const { return _framebufferId; }
void	ShaderProgram::SetRenderId(const GLuint renderTexture) { _renderId = renderTexture; }
GLuint	ShaderProgram::GetRenderId(void) const { return _renderId; }
ProgramViewport ShaderProgram::GetViewport(void) const { return _viewport; }
