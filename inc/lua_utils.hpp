/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lua_utils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/04 06:31:22 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/24 02:34:40 by jpirsch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LUA_UTILS
# define LUA_UTILS
# include <shaderpixel.h>
# include <ShaderRender.hpp>
# include <lua.h>
# include <lualib.h>
# include <lauxlib.h>

/*                      Getters of vector tables from lua                     */
unsigned short*	pop_ushort(lua_State* state, int narg, size_t* outLength);
float*			pop_float(lua_State* state, int narg, size_t* outLength);
vec2*			pop_vec2(lua_State *state, int narg, size_t *outLength);
vec3*			pop_vec3(lua_State* state, int narg, size_t* outLength);
/*                               Singleton utils                              */
ShaderRender	*getSR(ShaderRender *shadren);
lua_State		*getL(lua_State *L);
/*                               Script utils                                 */
int				load_script(lua_State *L, char *script);
int				run_script(lua_State *L);
int				load_run_script(lua_State *L, char *script);

#endif
