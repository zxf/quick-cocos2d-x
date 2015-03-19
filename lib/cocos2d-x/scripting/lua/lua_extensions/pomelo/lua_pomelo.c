#include <lauxlib.h>
#include <lua.h>

int lua_pomelo_lib_init(lua_State* L) {
    int level = luaL_optinteger(L, 1, 2);
    lua_pushinteger(L, level);
    return 1;
}

static const luaL_reg pomelo_functions[] = {
    {"lib_init",    lua_pomelo_lib_init},
    {NULL,  NULL}
};

int luaopen_pomelo(lua_State* tolua_S) {
    luaL_register(L, "pomelo", pomelo_functions);
    return 1;
}