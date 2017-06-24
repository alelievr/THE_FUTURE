/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderApplication.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/26 03:37:10 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/23 21:06:11 by jpirsch          ###   ########.fr       */
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

		ShaderApplication(bool fullscreen = false);
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
