#include <lauxlib.h>
#include <lua.h>
#include <pomelo.h>

static int lib_init(lua_State* L) {
    int log_level;
    char* ca_file = NULL;
    char* ca_path = NULL;
    log_level = luaL_checkinteger(L, 1);
    if(lua_gettop(L) > 0){
        if(lua_isnil(L, 1)){
            lua_remove(L, 1)
        } else {
            ca_file = luaL_checkstring(L, 1);
        }
    }
    if(lua_gettop(L) > 0){
        if(lua_isnil(L, 1)){
            lua_remove(L, 1)
        } else {
            ca_path = luaL_checkstring(L, 1);
        }
    }
#if !defined(PC_NO_UV_TLS_TRANS)
    if (ca_file || ca_path) {
        tr_uv_tls_set_ca_file(ca_file, ca_path);
    }
#endif
	pc_lib_set_default_log_level(log_level); 
    pc_lib_init(NULL, NULL, NULL, "Lua Client");
    lua_pushnil(L);
    return 1;
}

static int lib_cleanup(lua_State* L) {
    pc_lib_cleanup();
    lua_pushnil(L);
    return 1;
}

static int ev_to_str(lua_State* L) {
    int ev = luaL_checkinteger(L, 1);
    lua_pushstring(L, pc_client_ev_str(ev));
    return 1;
}

static int rc_to_str(lua_State* L) {
    int rc = luaL_checkinteger(L, 1);
    lua_pushstring(L, pc_client_rc_str(ev));
    return 1;
}

static int state_to_str(lua_State* L) {
    int state = luaL_checkinteger(L, 1);
    lua_pushstring(L, pc_client_state_str(ev));
    return 1;
}

static int create(lua_State* L) {
    return 1
}

static const luaL_reg pomelo_functions[] = {
    {"lib_init",    lib_init},
    {NULL,  NULL}
};

int luaopen_pomelo(lua_State* L) {

    luaL_register(L, "pomelo", pomelo_functions);
    return 1;
}