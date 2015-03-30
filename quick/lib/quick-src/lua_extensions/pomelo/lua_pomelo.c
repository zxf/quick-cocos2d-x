#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <lauxlib.h>
#include <lua.h>
#include <pomelo.h>

struct pomelo_client_s {
    pc_client_t* client;
    lua_State* state;
};
typedef struct pomelo_client_s pomelo_client_t;

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

static lua_State* ext_lua_reg_state(lua_State* L){
	lua_State* lua_st;
	char* state_key = "__state";
	int ref;
	lua_newtable(L);
	lua_pushstring(L, state_key);
	lua_st = lua_newthread(L);
	lua_settable(L, -3);
	ref = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_pushinteger(lua_st, ref);
	return lua_st;
}

static int ext_lua_unreg_state(lua_State* L){
	int ref;
	ref = lua_tointeger(L, 1);
	luaL_unref(L, LUA_REGISTRYINDEX, ref);
	return 0;
}

static int ext_lua_reg_function_state(lua_State* destL, lua_State* srcL, int idx, const char* func_key){
	int ref;
	if(!lua_isfunction(srcL, idx) && !lua_iscfunction(srcL, idx)) {
		luaL_error(srcL, "type error: %s is not a function.", lua_typename(srcL, lua_type(srcL, idx)));
	} else {
		ref = lua_tointeger(destL, 1);
		lua_pushvalue(srcL, idx);
		lua_rawgeti(srcL, LUA_REGISTRYINDEX, ref);
		lua_pushstring(srcL, func_key);
		lua_pushvalue(srcL, -3);
		lua_settable(srcL, -3);
		lua_pop(srcL, 2);
	}
	return 0;
}

static int ext_lua_unreg_function_state(lua_State* L, const char* func_key){
    int ref;
    lua_State* lua_st;
    
    ref = lua_tointeger(L, 1);
    lua_st = lua_newthread(L);
    lua_pop(L, 1);
    lua_rawgeti(lua_st, LUA_REGISTRYINDEX, ref);
    lua_pushstring(lua_st, func_key);
    lua_pushnil(lua_st);
    lua_settable(L, -3);
    return 0;
}

static lua_State* ext_lua_get_function_state(lua_State* L, const char* func_key) {
	int ref;
	lua_State* lua_st;

	ref = lua_tointeger(L, 1);
	lua_st = lua_newthread(L);
	lua_pop(L, 1);
	lua_rawgeti(lua_st, LUA_REGISTRYINDEX, ref);
	lua_pushstring(lua_st, func_key);
	lua_gettable(lua_st, -2);
	if(!lua_isfunction(lua_st, -1) && !lua_iscfunction(lua_st, -1)) {
		luaL_error(lua_st, "type error: %s is not a function.", lua_typename(lua_st, lua_type(lua_st, -1)));
	} else {
		lua_remove(lua_st, 1);
	}

	return lua_st;
}



static void default_destructor(void* ex_data) {
    ext_lua_unreg_function_state((lua_State*)ex_data, "event_cb");
}

static void default_event_cb(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2)
{
    const char* error;
    int enable_poll;
    lua_State* L;
    L = ext_lua_get_function_state((lua_State*)ex_data, "local_storage_cb");
    
    assert(client && L);
    enable_poll = pc_client_config(client)->enable_polling;
    
    lua_pushinteger(L, ev_type);
    lua_pushstring(L, arg1);
    lua_pushstring(L, arg2);
    if (lua_pcall(L, 4, 1, 0) != 0) {
        error = lua_tostring(L, -1);
        luaL_error(L, error);
    } else if (lua_isnone(L, -1) ) {
        luaL_error(L, "error...");
    }
}


static void default_request_cb(const pc_request_t* req, int rc, const char* resp)
{
    int enable_poll;
    const char* error;
    pc_client_t* client = pc_request_client(req);
    lua_State* L = ext_lua_get_function_state((lua_State* )pc_request_ex_data(req), "request_cb");
    
    assert(client && L);
    enable_poll = pc_client_config(client)->enable_polling;
    
    lua_pushinteger(L, rc);
    lua_pushstring(L, resp);
    if (lua_pcall(L, 3, 1, 0) != 0) {
        error = lua_tostring(L, -1);
        luaL_error(L, error);
    } else if (lua_isnone(L, -1) ) {
        luaL_error(L, "error...");
    }
}

static void default_notify_cb(const pc_notify_t* notify, int rc)
{
    int enable_poll;
     const char* error;
    pc_client_t* client = pc_notify_client(notify);
    lua_State* L = ext_lua_get_function_state((lua_State* )pc_notify_ex_data(notify), "notify_cb");
    
    assert(client && L);
    enable_poll = pc_client_config(client)->enable_polling;
    
    lua_pushinteger(L, rc);
    if (lua_pcall(L, 2, 1, 0) != 0) {
        error = lua_tostring(L, -1);
        luaL_error(L, error);
    } else if (lua_isnone(L, -1) ) {
        luaL_error(L, "error...");
    }
}


static int local_storage_cb(pc_local_storage_op_t op, char* data, size_t* len, void* ex_data) {
	int ret = -1;
    char* res = NULL;
	const char* error;
    lua_State* L;
    L = ext_lua_get_function_state((lua_State*)ex_data, "local_storage_cb");
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
    if(!lua_isnil(L, 2)){
        ca_file = luaL_checkstring(L, 2);
    }
    if(!lua_isnil(L, 3)){
        ca_path = luaL_checkstring(L, 3);
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
    pomelo_client_t* pclient = NULL;
    pc_client_t* client = NULL;
	int ret;
	lua_State* ls_ex_data;
    pc_client_config_t config = PC_CLIENT_CONFIG_DEFAULT;
	tls = ext_lua_checkbool(L, 1);
	polling = ext_lua_checkbool(L, 2);
    
	if (tls) {
        config.transport_name = PC_TR_NAME_UV_TLS;
    }
    if (polling) {
        config.enable_polling = 1;
    }
	

	client = (pc_client_t* )malloc(pc_client_size());
	if(!client){
		return 0;
	}
    
	ls_ex_data = ext_lua_reg_state(L);
	ext_lua_reg_function_state(ls_ex_data, L, -1, "local_storage_cb");
    
	config.local_storage_cb = local_storage_cb;
	config.ls_ex_data = ls_ex_data;
    ret = pc_client_init(client, NULL, &config);
    if (ret != PC_RC_OK) {
		ext_lua_unreg_state(ls_ex_data);
        lua_pushnil(L);
		return 0;
    }
    
    lua_newuserdata(L, sizeof(pomelo_client_t));
    pclient = (pomelo_client_t*)lua_touserdata(L, -1);
    pclient->client = client;
    pclient->state = ls_ex_data;
    return 0;
}

static int connect(lua_State* L) {
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
    pc_client_t* client;
    int state;

    client = (pc_client_t* )lua_touserdata(L, 1);
    
    state = pc_client_state(client);

    lua_pushinteger(L, state);
    return 0;
}


static int add_ev_handler(lua_State* L) {
    pomelo_client_t* pclient;
    pc_client_t* client;
    int ret;
	lua_State* ls_ex_data;

    pclient = (pomelo_client_t* )lua_touserdata(L, 1);

	ls_ex_data = pclient->state;
    client = pclient->client;
    
    ext_lua_reg_function_state(ls_ex_data, L, -1, "event_cb");
    ret = pc_client_add_ev_handler(client, default_event_cb, ls_ex_data, default_destructor);
	if (ret != PC_EV_INVALID_HANDLER_ID) {
        ext_lua_unreg_function_state(ls_ex_data, "event_cb");
	}
	lua_pushinteger(L, ret);
    return 0;
}


static int rm_ev_handler(lua_State* L) {
    pomelo_client_t* pclient;
    pc_client_t* client;
    int handler_id;
    int ret;

    pclient = (pomelo_client_t* )lua_touserdata(L, 1);
    handler_id = lua_tointeger(L, 2);
    client = pclient->client;

    ret = pc_client_rm_ev_handler(client, handler_id);

    lua_pushinteger(L, ret);
    return 0;
}

static int request(lua_State* L) {
    pomelo_client_t* pclient;
    pc_client_t* client;
    lua_State* ls_ex_data;
    char* route = NULL;
    char* msg = NULL;
    int timeout = 0;
    int ret;
    
    pclient = (pomelo_client_t* )lua_touserdata(L, 1);
    client = pclient->client;
    
    route = (char*)lua_tostring(L, 2);
    msg = (char*)lua_tostring(L, 3);
    timeout = lua_tointeger(L, 4);
    ls_ex_data = pclient->state;
    ext_lua_reg_function_state(ls_ex_data, L, -1, "request_cb");
    
    ret = pc_request_with_timeout(client, route, msg,
                                  L, timeout, default_request_cb);
    lua_pushinteger(L, ret);
    return 0;
}

static int notify(lua_State* L) {
    pomelo_client_t* pclient;
    pc_client_t* client;
    lua_State* ls_ex_data;
    char* route = NULL;
    char* msg = NULL;
    int timeout = 0;
    int ret;
    
    pclient = (pomelo_client_t* )lua_touserdata(L, 1);
    client = pclient->client;
    
    route = (char*)lua_tostring(L, 2);
    msg = (char*)lua_tostring(L, 3);
    timeout = lua_tointeger(L, 4);
    
    ls_ex_data = pclient->state;
    ext_lua_reg_function_state(ls_ex_data, L, -1, "notify_cb");

    ret = pc_notify_with_timeout(client, route, msg,
                                 L, timeout, default_notify_cb);

    lua_pushinteger(L, ret);
    return 0;
}

static int poll(lua_State* L) {
    pomelo_client_t* pclient;
    pc_client_t* client;
    int ret;
    
    pclient = (pomelo_client_t* )lua_touserdata(L, 1);
    client = pclient->client;
    ret = pc_client_poll(client);
    
    lua_pushinteger(L, ret);
    return 0;
}

static int quality(lua_State* L) {
    pomelo_client_t* pclient;
    pc_client_t* client;
    int ret;
    
    pclient = (pomelo_client_t* )lua_touserdata(L, 1);
    client = pclient->client;
    ret = pc_client_conn_quality(client);

    lua_pushinteger(L, ret);
    return 0;
}

static int disconnect(lua_State* L) {
    pomelo_client_t* pclient;
    pc_client_t* client;
    int ret;
    
    pclient = (pomelo_client_t* )lua_touserdata(L, 1);
    client = pclient->client;
    ret = pc_client_disconnect(client);
    
    lua_pushinteger(L, ret);
    return 0;
}

static int destroy(lua_State* L) {
    pomelo_client_t* pclient;
    pc_client_t* client;
    int ret;
    
    pclient = (pomelo_client_t* )lua_touserdata(L, 1);
    client = pclient->client;
    assert(pclient && client);

    ret = pc_client_cleanup(client);
    if (ret == PC_RC_OK) {
        ext_lua_unreg_state(pclient->state);
        free(client);
    }
    
    lua_pushinteger(L, ret);
    return 0;
}

static const luaL_reg pomelo_functions[] = {
    {"lib_init",    lib_init},
	{"lib_cleanup", lib_cleanup},
	{"ev_to_str", ev_to_str},
	{"rc_to_str", rc_to_str},
	{"state_to_str", state_to_str},
	{"create", create},
	{"connect", connect},
	{"state", state},
	{"add_ev_handler", add_ev_handler},
    {"rm_ev_handler", rm_ev_handler},
    {"request", request},
    {"notify", notify},
    {"poll", poll},
    {"quality", quality},
    {"disconnect", disconnect},
    {"destroy", destroy},
    {NULL,  NULL}
};

int luaopen_pomelo(lua_State* L) {
    luaL_register(L, "luapomelo", pomelo_functions);
    return 0;
}