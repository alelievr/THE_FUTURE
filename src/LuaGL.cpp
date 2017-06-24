/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LuaGL.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/26 05:35:14 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/24 05:45:49 by jpirsch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LuaGL.hpp"

static const luaL_Reg Functions[] =
{
	{"get_prog", get_prog},
	{"use_prog", use_prog},
	{"update_vao", update_vao},
	{"get_time", get_time},
/*	{"create_element", create_element},
	{"draw_element", draw_element},
	{"delete_element", delete_element},*/
	{NULL, NULL}
};


/* ************************************************************************** */
/*                                                                            */
/*                                   Init                                     */
/*                                 ~20 lines                                  */
/* ************************************************************************** */

int	init_LuaGL(ShaderRender *sr)
{
	lua_State	*L;

	getSR(sr);
	L = luaL_newstate();    // All Lua contexts
	getL(L);
	
    // Into the lua "gol" table
	luaL_register(L, "gol", Functions);
	luaL_openlibs(L);       // Load Lua libraries

	load_run_script(L, "lua/init_loop.lua");
	lua_getfield(L, LUA_GLOBALSINDEX, "init_oo");
	lua_call(L, 0, 0);
//	load_script(L, "lua/draw.lua");
}

/* ************************************************************************** */
/*                                                                            */
/*                                 Functions                                  */
/*                                 ~30 lines                                  */
/* ************************************************************************** */

int	get_prog(lua_State *L)
{
	ShaderProgram	*prog;

	int	ind = luaL_checkinteger(L, 1);
	prog = getSR(NULL)->GetProgram(ind);
	lua_pushinteger(L, lua_Integer(prog));
	return (1);
}

int	use_prog(lua_State *L)
{
	ShaderProgram	*prog;

	prog = (ShaderProgram*)(luaL_checkinteger(L, 1));
	prog->Use();
	return (0);
}

int	update_vao(lua_State *L)
{
	ShaderProgram *prog;

	prog = (ShaderProgram*)(luaL_checkinteger(L, 1));
    size_t count; // vertices
    float* vert = pop_float(L, 2, &count);

	prog->UpdateVAO(vert, count);
	return (0);
}

int	get_time(lua_State *L)
{
	double	time = glfwGetTime();
	lua_pushnumber(L, lua_Number(time));
	return (1);
}

/*int	create_element(lua_State *L)
{
    size_t ind_nb; // indices
    unsigned short* ind = pop_ushort( L, 1, &ind_nb );
    
    size_t pos_nb; // positions
    vec3* pos = pop_vec3( L, 2, &pos_nb );
    
    size_t uv_nb; // texture coordinates
    vec2* uv = pop_vec2( L, 3, &uv_nb );
    
	Element	*elem = new Element(ind, ind_nb, pos, pos_nb, uv, uv_nb);
	lua_pushinteger(L, lua_Integer(elem));
	return (1);
}

int	draw_element(lua_State *L)
{
	Element *elem = (Element*)(luaL_checkinteger(L, 1));
	GLuint vert_loc = (GLuint)(luaL_checkinteger(L, 2));
	GLuint uv_loc = (GLuint)(luaL_checkinteger(L, 3));
	
	elem->draw(vert_loc, uv_loc);
	return (0);
}

int				delete_element(lua_State *L)
{
	Element	*elem;

	elem = (Element*)(luaL_checkinteger(L, 1));
	delete elem;
	return (0);
}*/
