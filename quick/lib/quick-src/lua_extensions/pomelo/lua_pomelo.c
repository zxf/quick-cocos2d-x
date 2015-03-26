#include <string.h>

#include <lauxlib.h>
#include <lua.h>
#include <pomelo.h>


static int ext_lua_checkbool(lua_State* L, int idx) {
	if(lua_isboolean(L, idx)){
		return lua_toboolean(L, idx);
	} else if(lua_isnumber(L, idx)) {
		return lua_tointeger(L, idx);
	} else if(lua_isnoneornil(L, idx)) {
		return 0;
	} else if(lua_isstring(L, idx)){
		if(strlen(lua_tostring(L, idx)) > 0){
			return 1;
		}
		return 0;
	} else {
		return 1;
	}
}

static lua_State* ext_lua_func_state(lua_State* L, int idx) {
	lua_State* lua_st;
	int ref;
	if(!lua_isfunction(L, idx) && !lua_iscfunction(L, idx)) {
		luaL_error(L, "type error: %s is not a function.", lua_typename(L, lua_type(L, idx)));
	} else {
		lua_pushvalue(L, idx);
		ref = luaL_ref(L, LUA_REGISTRYINDEX);
		lua_st = lua_newthread(L);
		lua_rawgeti(lua_st, LUA_REGISTRYINDEX, ref);
		luaL_unref(L, LUA_REGISTRYINDEX, ref);
		lua_pop(L, 1);
	}

	return lua_st;
}

static int ext_lua_reg_state(lua_State* L, void* group, lua_State* LL) {
	lua_pushlightuserdata(L, group);
	lua_gettable(L, LUA_REGISTRYINDEX);
	if(lua_isnoneornil(L, -1)){
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushlightuserdata(L, group);
		lua_pushvalue(L, -2);
		lua_settable(L, LUA_REGISTRYINDEX);
	}
	lua_pushlightuserdata(L, (void *)&LL);
	lua_pushlightuserdata(L, LL);
	lua_settable(L, -3);
	lua_pop(L, 1);
	return 0;
}

static int ext_lua_unreg_state(lua_State* L, void* group) {
	lua_pushlightuserdata(L, group);
	lua_pushnil(L);
	lua_settable(L, LUA_REGISTRYINDEX);
	return 0;
}

static void default_destructor(void* ex_data) {
    
}

static int local_storage_cb(pc_local_storage_op_t op, char* data, size_t* len, void* ex_data) {
	int ret = -1;
    char* res = NULL;
	const char* error;
	lua_State* L = ext_lua_func_state((lua_State*)ex_data, -1);	
	if (op == PC_LOCAL_STORAGE_OP_WRITE) {
		lua_pushinteger(L, op);
		lua_pushlstring(L, data, *len);
		if (lua_pcall(L, 3, 1, 0) != 0) {
			error = lua_tostring(L, -1);
			luaL_error(L, error);
		} else if (lua_isnone(L, -1) ) {
			luaL_error(L, "error...");
		} else {
			ret = luaL_checklong(L, -1);
		}
		return ret;
	} else {
		lua_pushinteger(L, op);
		if (lua_pcall(L, 1, 1, 0) != 0) {
			error = lua_tostring(L, -1);
			luaL_error(L, error);
		} else if (lua_isnone(L, -1)) {
			luaL_error(L, "error...");
		} else {
			if (!lua_isnil(L, -1)) {
				res = (char *)lua_tostring(L, -1);
			}
		}
		if (res) {
            *len = strlen(res);
            if (*len == 0) {
                return -1;
            }
            if (data) {
                strncpy(data, res, *len);
            }
            return 0;
        } else {
            return -1;
        }
	}
}

static int lib_init(lua_State* L) {
    int log_level;
    const char* ca_file = NULL;
    const char* ca_path = NULL;
    log_level = luaL_checkinteger(L, 1);
    if(lua_gettop(L) > 0){
        if(lua_isnil(L, 1)){
            lua_remove(L, 1);
        } else {
            ca_file = luaL_checkstring(L, 1);
        }
    }
    if(lua_gettop(L) > 0){
        if(lua_isnil(L, 1)){
            lua_remove(L, 1);
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
    return 0;
}

static int lib_cleanup(lua_State* L) {
    pc_lib_cleanup();
    lua_pushnil(L);
    return 0;
}

static int ev_to_str(lua_State* L) {
    int ev = luaL_checkinteger(L, 1);
    lua_pushstring(L, pc_client_ev_str(ev));
    return 0;
}

static int rc_to_str(lua_State* L) {
    int rc = luaL_checkinteger(L, 1);
    lua_pushstring(L, pc_client_rc_str(rc));
    return 0;
}

static int state_to_str(lua_State* L) {
    int state = luaL_checkinteger(L, 1);
    lua_pushstring(L, pc_client_state_str(state));
    return 0;
}

static int create(lua_State* L) {
	int tls = 0;
    int polling = 0;
    pc_client_t* client = NULL;
	int ret;
	lua_State* ls_ex_data;
	pc_client_config_t config = PC_CLIENT_CONFIG_DEFAULT;
	tls = ext_lua_checkbool(L, 1);
	polling = ext_lua_checkbool(L, 1);

	if (tls) {
        config.transport_name = PC_TR_NAME_UV_TLS;
    }
    if (polling) {
        config.enable_polling = 1;
    }
	
	lua_newuserdata(L, pc_client_size());
	client = (pc_client_t*)lua_touserdata(L, -1);
	if(!client){
		lua_pushnil(L);
		return 0;
	}
	
	ls_ex_data = ext_lua_func_state(L, -2);
	config.local_storage_cb = local_storage_cb;
	config.ls_ex_data = ls_ex_data;
	ret = pc_client_init(client, NULL, &config);
    if (ret != PC_RC_OK) {
		lua_pop(L, 1);
        lua_pushnil(L);
		return 0;
    }
	ext_lua_reg_state(L, client, ls_ex_data);
    return 0;
}

static int connect(lua_State* L) {
    unsigned long addr;
    pc_client_t* client;
    char* host = NULL;
    int port = 0;
    int ret;

	client = (pc_client_t* )lua_touserdata(L, 1);
	host = (char*)luaL_checkstring(L, 2);
	port = luaL_checkinteger(L, 3);

    ret = pc_client_connect(client, host, port, NULL);

	lua_pushinteger(L, ret);
    return 0;
}

static int state(lua_State* L) {
    unsigned long addr;
    pc_client_t* client;
    int state;

    client = (pc_client_t* )lua_touserdata(L, 1);
    
    state = pc_client_state(client);

    lua_pushinteger(L, state);
    return 0;
}

/*
static int add_ev_handler(lua_State* L) {
    unsigned long addr;
    pc_client_t* client;
    int ret;
	lua_State* ls_ex_data;

    client = (pc_client_t* )lua_touserdata(L, 1);

	ls_ex_data = ext_lua_state(L, -1);
    ret = pc_client_add_ev_handler(client, default_event_cb, ls_ex_data, default_destructor);
	if (ret != PC_EV_INVALID_HANDLER_ID) {
		ext_lua_reg_state(L, client, ls_ex_data);
	}
	lua_pushinteger(L, ret);
    return 0;
}
*/

static const luaL_reg pomelo_functions[] = {
    {"lib_init",    lib_init},
	{"lib_cleanup", lib_cleanup},
	{"ev_to_str", ev_to_str},
	{"rc_to_str", rc_to_str},
	{"state_to_str", state_to_str},
	{"create", create},
	{"connect", connect},
	{"state", state},
	//{"add_ev_handler", add_ev_handler},
    {NULL,  NULL}
};

int luaopen_pomelo(lua_State* L) {
    luaL_register(L, "luapomelo", pomelo_functions);
    return 0;
}