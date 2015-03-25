#include <string.h>
//#include <stdlib.h>
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

static int local_storage_cb(pc_local_storage_op_t op, char* data, size_t* len, void* ex_data) {
	lua_State* L = (lua_State*)ex_data;
	int ret = -1;
	int res_len = 0;
    char* res = NULL;
	const char* error;

	if (op == PC_LOCAL_STORAGE_OP_WRITE) {
		lua_pushvalue(L, -1);
		lua_pushinteger(L, op);
		lua_pushlstring(L, data, *len);
		if (lua_pcall(L, 3, 1, 0) != 0) {
			error = lua_tostring(L, -1);
			lua_pop(L, 1);
			luaL_error(L, error);
		} else if (lua_isnone(L, -1) ) {
			lua_pop(L, 1);
			luaL_error(L, "error...");
		} else {
			ret = luaL_checklong(L, -1);
			lua_pop(L, 1);
		}
		return ret;
	} else {
		lua_pushvalue(L, -1);
		lua_pushinteger(L, op);
		if (lua_pcall(L, 1, 1, 0) != 0) {
			error = lua_tostring(L, -1);
			lua_pop(L, 1);
			luaL_error(L, error);
		} else if (lua_isnone(L, -1)) {
			lua_pop(L, 1);
			luaL_error(L, "error...");
		} else {
			if (!lua_isnil(L, -1)) {
				res = (char*)lua_tostring(L, -1);
			}
			lua_pop(L, 1);
		}
		if (res) {
            res_len = strlen(res);
            if (res_len == 0) {
                return -1;
            }
			*len = res_len + 1;

            if (data) {
                strcpy(data, res);
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
	lua_pop(L, 1);
	//client = (pc_client_t* )malloc(pc_client_size());
	if(!client){
		lua_pushnil(L);
		return 0;
	}

	config.local_storage_cb = local_storage_cb;
    config.ls_ex_data = L;
	ret = pc_client_init(client, NULL, &config);
    if (ret != PC_RC_OK) {
        lua_pushnil(L);
		return 0;
    }
	lua_pushinteger(L, (unsigned long)client);
    return 0;
}

static const luaL_reg pomelo_functions[] = {
    {"lib_init",    lib_init},
	{"lib_cleanup", lib_cleanup},
	{"ev_to_str", ev_to_str},
	{"rc_to_str", rc_to_str},
	{"state_to_str", state_to_str},
	{"create", create},
    {NULL,  NULL}
};

int luaopen_pomelo(lua_State* L) {
    luaL_register(L, "luapomelo", pomelo_functions);
    return 1;
}