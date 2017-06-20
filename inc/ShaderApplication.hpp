/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderApplication.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/26 03:37:10 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/20 15:22:58 by alelievr         ###   ########.fr       */
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
		void	RenderLoop(void);
		void	OnLoadingShaderFinished(void);
		void    UpdateUniform(const int programIndex, const std::string & uniformName, const UniformParameter & param);

	private:
		int					_programToFocus;

		ShaderRender		*shaderRender;
		GLFWwindow			*window;
};

#endif
