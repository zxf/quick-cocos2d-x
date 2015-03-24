#include <lauxlib.h>
#include <lua.h>
#include <pomelo.h>

int lua_pomelo_lib_init(lua_State* L) {
    int log_level = 1;
    char* ca_file = NULL;
    char* ca_path = NULL;
	pc_lib_set_default_log_level(log_level); 
    pc_lib_init(NULL, NULL, NULL, "Lua Client");
    lua_pushinteger(L, 100);
    return 1;
}

static const luaL_reg pomelo_functions[] = {
    {"lib_init",    lua_pomelo_lib_init},
    {NULL,  NULL}
};

int luaopen_pomelo(lua_State* L) {
    luaL_register(L, "pomelo", pomelo_functions);
    return 1;
}