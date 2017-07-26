/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderApplication.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/26 03:37:10 by jpirsch           #+#    #+#             */
/*   Updated: 2017/07/26 08:29:13 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADER_APPLICATION
# define SHADER_APPLICATION
# include "shaderpixel.h"
# include "ShaderRender.hpp"
# include "NetworkManager.hpp"

class ShaderApplication
{
	public:
		bool			loadingShaders;
		bool			_headless;

		ShaderApplication(bool fullscreen = false, bool headless = false);
		virtual ~ShaderApplication(void);

		bool	LoadShader(const std::string & shaderFile);
		void	FocusShader(const int programIndex, const int transitionIndex);
		void	AddShader(const int programIndex);
		void	RenderLoop(void);
		void	OnLoadingShaderFinished(void);
		void    UpdateLocalParam(const int programIndex, const std::string & uniformName, const UniformParameter & param);

	private:
		int					_programToFocus;

		ShaderRender		*shaderRender;
		GLFWwindow			*window;
};

#endif
