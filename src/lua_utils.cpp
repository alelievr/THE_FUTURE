#include "lua_utils.hpp"

/* ************************************************************************** */
/*                                                                            */
/*                      Getters of vector tables from lua                     */
/*                                 ~80 lines                                  */
/* ************************************************************************** */

unsigned short* pop_ushort( lua_State* state, int narg,
												size_t* outLength )
{
	int nb = (int)lua_objlen( state, narg );
	*outLength = nb ;
	unsigned short* indices = (ushort*)(malloc( nb * sizeof(unsigned short) ));
	unsigned short* pt = indices;
	for( int i = 1; i <= nb; ++i )
	{
		lua_pushinteger( state, i ); // key
		lua_gettable( state, narg ); // Value at key (lua_gettable pops the key)
		*pt = (unsigned short)luaL_checkinteger( state, -1 );
		lua_pop(state, 1);
		++pt;
	}
	return indices;
}

float* pop_float( lua_State* state, int narg, size_t* outLength )
{
	int nb = (int)lua_objlen( state, narg );
	*outLength = nb ;
	float* vert = (float*)(malloc( nb * sizeof(float) ));
	float* pt = vert;
	for( int i = 1; i <= nb; ++i )
	{
		lua_pushinteger( state, i ); // key
		lua_gettable( state, narg ); // Value at key (lua_gettable pops the key)
		*pt = (float)luaL_checknumber( state, -1 );
		lua_pop(state, 1);
		++pt;
	}
	return vert;
}

#define READ_FIELD( dst, i )				\
{											\
	lua_rawgeti( state, -1, i );			\
	(dst) = luaL_checknumber(state, -1);	\
	lua_pop(state, 1);						\
}

vec2* pop_vec2(lua_State *state, int narg, size_t *outLength)
{
    int nb = (int)lua_objlen( state, narg );
    *outLength = nb;
    vec2* vectors = (vec2*)(malloc( nb * sizeof(vec2) ));
    vec2* pt = vectors;
    for( int i = 1; i <= nb; ++i )
    {
        // vectors[vectorIndex]
        lua_pushinteger( state, i ); // key
        lua_gettable( state, -2 ); // Value at key (lua_gettable pops the key)
        if( lua_istable( state, -1) )
        {
			READ_FIELD( pt->x, 1 );
			READ_FIELD( pt->y, 2 );
            ++pt;
        }
        else
        {
            std::cout << "The table contains something else than a table" << std::endl;
            lua_error(state);
            return 0;
        }
        lua_pop(state, 1); // Remove the subtable
    }
    return vectors;
}

vec3* pop_vec3(lua_State* state, int narg, size_t* outLength)
{
	int count = (int)lua_objlen( state, narg );
	*outLength = count;
	vec3* vectors = (vec3*)(malloc( *outLength * sizeof(vec3) ));
	vec3* pt = vectors;
	for( int i = 0; i < count; ++i )
	{
		// vectors[vectorIndex]
		lua_pushinteger( state, i+1 ); // key
		lua_gettable( state, narg ); // Value at key (lua_gettable pops the key)
		if( lua_istable( state, -1 ) )
		{
			READ_FIELD( pt->x, 1 );
			READ_FIELD( pt->y, 2 );
			READ_FIELD( pt->z, 3 );
			++pt;
		}
		else
		{
            std::cout << "The table contains something else than a table" << std::endl;
			lua_error(state);
			return 0;
		}
		lua_pop(state, 1); // Remove the subtable
	}
	return vectors;
}

/* ************************************************************************** */
/*                                                                            */
/*                               Push table util                              */
/*                                 ~20 lines                                  */
/* ************************************************************************** */

/*void	push_table(std::vector< ICGProgram * >	progs)
{
		
	lua_newtable(L);			// We will pass a table
	for (i = 1; i <= 5; i++)
	{
		lua_pushnumber(L, i);	// Push the table index
		lua_pushnumber(L, i*2);	// Push the cell value
		lua_rawset(L, -3);		// Stores pair in table
	}

	lua_setglobal(L, "foo");	// Name the script has to reference our table?
}*/

/* ************************************************************************** */
/*                                                                            */
/*                               Singleton utils                              */
/*                                 ~20 lines                                  */
/* ************************************************************************** */

ShaderRender  *getSR(ShaderRender *shadren)
{
	static ShaderRender *sr;

	if (shadren == NULL)
		return (sr);
	else if (shadren)
		sr = shadren;
	return (NULL);
}

lua_State	*getL(lua_State *l)
{
	static lua_State	*L;

	if (l == NULL)
		return (L);
	else if (l)
		L = l;
	return (NULL);
}

/* ************************************************************************** */
/*                                                                            */
/*                               Script utils                                 */
/*                                 ~20 lines                                  */
/* ************************************************************************** */

int	load_script(lua_State *L, char *script)
{
	int	status;

	status = luaL_loadfile(L, script);
	if (status)
	{
		fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
		exit(1);
	}
	return (0);
}

int	run_script(lua_State *L)
{
	int	result;

	result = lua_pcall(L, 0, LUA_MULTRET, 0); // Lua run script
	if (result)
	{
		fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
		exit(1);
	}
	return (0);
}

int	load_run_script(lua_State *L, char *script)
{
	int	status, result;

	status = luaL_loadfile(L, script);
	if (status)
	{
		fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
		exit(1);
	}
	result = lua_pcall(L, 0, LUA_MULTRET, 0); // Lua run script
	if (result)
	{
		fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
		exit(1);
	}
	return (0);
}
