/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderProgram.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/07 20:35:23 by alelievr          #+#    #+#             */
/*   Updated: 2017/06/24 02:36:19 by jpirsch          ###   ########.fr       */
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

	public:
		ShaderProgram(void);
		ShaderProgram(const ShaderProgram&) = delete;
		virtual ~ShaderProgram(void);

		ShaderProgram &	operator=(ShaderProgram const & src) = delete;

		bool    LoadSourceFile(const std::string & fileName);

		bool    CompileAndLink(void);

		void		UpdateVAO(float *vert, GLuint count);
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

#endif
