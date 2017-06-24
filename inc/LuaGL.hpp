/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LuaGL.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/26 05:39:09 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/24 03:12:48 by jpirsch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LUAGL
# define LUAGL
# include "lua_utils.hpp"

class ShaderRender;

/*                                   Init                                     */
int				init_LuaGL(ShaderRender* ShadRen);
/*                                 Functions                                  */
int				get_prog(lua_State *L);
int				use_prog(lua_State *L);
int				update_vao(lua_State *L);
int				get_time(lua_State *L);
/*int				create_element(lua_State *L);
int				draw_element(lua_State *L);
int				delete_element(lua_State *L);*/

#endif
