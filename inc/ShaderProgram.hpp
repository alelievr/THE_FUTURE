/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderProgram.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/07 20:35:23 by alelievr          #+#    #+#             */
/*   Updated: 2017/06/20 01:53:17 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADERPROGRAM_HPP
# define SHADERPROGRAM_HPP
# include <iostream>
# include <string>
# include <vector>
# include <fstream>
# include <streambuf>
# include <regex>
# include <map>
# include <exception>
# include "sys/stat.h"

# include "shaderpixel.h"
# include "ShaderChannel.hpp"
# include "ICGProgram.hpp"

class		ShaderProgram : public ICGProgram
{
	private:
		struct ShaderFile {
			std::string		file;
			std::string		source;

			ShaderFile(std::string file, std::string source)
			{
				this->file = file;
				this->source = source;
			}
		};

		std::vector< ShaderFile >		_fragmentFileSources;
		std::vector< ShaderFile >		_vertexFileSources;
		int								_framebufferId;
		int								_renderId;
		ShaderChannel *					_channels;
		std::map< std::string, GLuint >	_uniforms;
		std::map< std::string, float >	_localParams;
		bool							_loaded;
		bool							_firstUse;
		GLuint							_id; //opengl program id

		GLuint							_vao;
		GLuint							_vbo;

		const std::string			_LoadSourceFile(const std::string & file);
		bool						CheckCompilation(GLuint shader);
		bool						CheckLink(GLuint program);
		void						CreateVAO(void);
		void						LoadUniformLocations(void);
		void						UpdateLocalParam(const std::string & name, const float value);

	public:
		ShaderProgram(void);
		ShaderProgram(const ShaderProgram&) = delete;
		virtual ~ShaderProgram(void);

		ShaderProgram &	operator=(ShaderProgram const & src) = delete;

		bool    LoadSourceFile(const std::string & fileName);

		bool    CompileAndLink(void);

		void    Use(void);
		void    UpdateUniforms(const vec2 winSize, bool pass = false);
		void    UpdateFramebufferSize(const vec2 fbSize);
		void    Draw(void);

		void	SetFramebufferId(const GLuint fbo);
		GLuint	GetFramebufferId(void) const;
		void	SetRenderId(const GLuint renderTexture);
		GLuint	GetRenderId(void) const;
};

std::ostream &	operator<<(std::ostream & o, ShaderProgram const & r);

#define FRAGMENT_SHADER_HEADER \
"#version 330\n" \
"in vec4 outColor;\n" \
"out vec4 fragColor;\n" \
"\n" \
"uniform vec2           iResolution;\n" \
"uniform float          iGlobalTime;\n" \
"uniform int            iFrame;\n" \
"uniform vec4           iMouse;\n" \
"uniform vec2           iScrollAmount;\n" \
"uniform vec4           iMoveAmount;\n" \
"uniform vec3           iForward;\n" \
"uniform vec4           iFractalWindow;\n" \
"uniform sampler2D      iChannel0;\n" \
"uniform sampler2D      iChannel1;\n" \
"uniform sampler2D      iChannel2;\n" \
"uniform sampler2D      iChannel3;\n" \
"uniform sampler2D      iChannel4;\n" \
"uniform sampler2D      iChannel5;\n" \
"uniform sampler2D      iChannel6;\n" \
"uniform sampler2D      iChannel7;\n" \
"\n" \
"void mainImage(vec2 f);\n" \
"\n" \
"vec4 texture2D(sampler2D s, vec2 coord, float f)\n" \
"{\n" \
"       return texture(s, coord, f);\n" \
"}\n" \
"\n" \
"vec4 texture2D(sampler2D s, vec2 coord)\n" \
"{\n" \
"       return texture(s, coord);\n" \
"}\n" \
"\n" \
"void main()\n" \
"{\n" \
"       mainImage(gl_FragCoord.xy);\n" \
"}\n" \
"#line 1\n"

#define VERTEX_SHADER_DEFAULT \
"#version 330\n" \
"in vec2                fragPosition;\n" \
"out vec4               outColor;\n" \
"void main()\n" \
"{\n" \
"       gl_Position = vec4(fragPosition, 0.0, 1.0);\n" \
"}\n"

#endif
