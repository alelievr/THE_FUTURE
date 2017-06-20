/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderRender.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/07 21:28:59 by alelievr          #+#    #+#             */
/*   Updated: 2017/06/20 15:36:32 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include "shaderpixel.h"
# include <vector>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdbool.h>
# include <sys/stat.h>
# include <string.h>
# include <time.h>
# include <math.h>

#include "ShaderProgram.hpp"
#include "KernelProgram.hpp"
#include "ShaderChannel.hpp"
#include "NetworkManager.hpp"
#include "LuaGL.hpp"

//#define UNIFORM_DEBUG 1

class ShaderRender
{
	private:
		std::vector< ICGProgram * >		_programs;
		std::vector< std::size_t >		_currentRenderedPrograms;
		std::vector< ICGProgram * >		_transitionPrograms;

		vec2			angleAmount;
		int				cursor_mode;
		float			lastPausedTime;
		bool			programLoaded;

		void			displayWindowFps(void);

	public:
		ShaderRender(void);
		ShaderRender(const ShaderRender & rhs) = delete;
		ShaderRender &		operator=(ShaderRender & ths) = delete;

		virtual ~ShaderRender(void);

		void				Render(void);
		bool				attachShader(const std::string file);
		void				SetCurrentRenderedShader(const size_t programIndex, const int transitionIndex);
		void				AddCurrentRenderedShader(const size_t programIndex);
		void				DeleteCurrentRenderedShader(const size_t programIndex);
		void				ClearCurrentRenderedShader(void);
		void				UpdateLocalParam(const int programIndex, const std::string & uniformName, const UniformParameter & param);

		void				framebufferSizeCallback(int width, int height);
};
