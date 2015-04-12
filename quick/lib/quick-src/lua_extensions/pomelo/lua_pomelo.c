#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <pomelo.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lpomelo.h"


/*
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
    if (lua_pcall(L, 3, 1, 0) != 0) {
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
    pc_client_t* client;
    lua_State* L;
	int ret;

	client = pc_request_client(req);
	L = ext_lua_get_function_state((lua_State* )pc_request_ex_data(req), "request_cb");

    assert(client && L);
    enable_poll = pc_client_config(client)->enable_polling;
    lua_pushinteger(L, rc);
    lua_pushstring(L, resp);
    ret = lua_pcall(L, 2, 0, 0);
    if (ret != 0) {
        error = lua_tostring(L, -1);
        luaL_error(L, error);
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
    if (lua_pcall(L, 1, 1, 0) != 0) {
        error = lua_tostring(L, -1);
        luaL_error(L, error);
    } else if (lua_isnone(L, -1) ) {
        luaL_error(L, "error...");
    }
}
*/


static int lib_init(lua_State* L) {
    lua_pushvalue(L, -3);
	lua_pushvalue(L, -3);
	lua_pushvalue(L, -3);
	lpomelo_lua_initlib(L);
	return 0;
}

static int lib_cleanup(lua_State* L) {
    pc_lib_cleanup();
    return 0;
}

static int ev_to_str(lua_State* L) {
    lua_pushstring(L, lpomelo_lua_toevstr(L, 1));
    return 1;
}

static int rc_to_str(lua_State* L) {
    lua_pushstring(L, lpomelo_lua_torcstr(L, 1));
    return 1;
}

static int state_to_str(lua_State* L) {
    lua_pushstring(L, lpomelo_lua_tostatestr(L, 1));
    return 1;
}

static int create(lua_State* L) {
	lpomelo_client_t* client;
    client = lpomelo_lua_newclient(L);
	if (!client) {
		return 0;
	}
	lua_pushvalue(L, -4);
	lua_pushvalue(L, -4);
	lua_pushvalue(L, -4);
	if(lpomelo_lua_init(L, -4)){
		lpomelo_lua_popclient(L);
		return 0;
	}
    return 1;
}

static int connect(lua_State* L) {
	lua_pushvalue(L, -3);
	lua_pushvalue(L, -3);
	lpomelo_lua_connect(L, -3);
    return 1;
}

static int state(lua_State* L) {
    lpomelo_lua_state(L, -1);
    return 1;
}

static int receive(lua_State* L) {
	//lpomelo_lua_receive(L, -1);
    return 1;
} 

static int add_ev_handler(lua_State* L) {
	/*
    lpomelo_client_t* pclient;
    pc_client_t* client;
    int ret;
	lua_State* ls_ex_data;
	
    pclient = (lpomelo_client_t* )lua_touserdata(L, 1);
	assert(pclient);
	client = pclient->client;
	ls_ex_data = pclient->state;
	assert(client && ls_ex_data);
	
    ext_lua_reg_function_state(ls_ex_data, L, -1, "event_cb");
    ret = pc_client_add_ev_handler(client, default_event_cb, ls_ex_data, default_destructor);

	if (ret != PC_EV_INVALID_HANDLER_ID) {
        ext_lua_unreg_function_state(ls_ex_data, "event_cb");
	}
	lua_pushinteger(L, ret);
    return 1;
	*/
}


static int rm_ev_handler(lua_State* L) {
	/*
    lpomelo_client_t* pclient;
    pc_client_t* client;
    int handler_id;
    int ret;

    pclient = (lpomelo_client_t* )lua_touserdata(L, 1);
	assert(pclient);
    handler_id = lua_tointeger(L, 2);
    client = pclient->client;
	assert(client);
    ret = pc_client_rm_ev_handler(client, handler_id);
    lua_pushinteger(L, ret);
    return 1;
	*/
}

static int request(lua_State* L) {
	/*
    lpomelo_client_t* pclient;
    pc_client_t* client;
    lua_State* ls_ex_data;
    char* route = NULL;
    char* msg = NULL;
    int timeout = 0;
    int ret;
    
    pclient = (lpomelo_client_t* )lua_touserdata(L, 1);
	assert(pclient);
    client = pclient->client;
	ls_ex_data = pclient->state;
	assert(client && ls_ex_data);
    route = (char*)lua_tostring(L, 2);
    msg = (char*)lua_tostring(L, 3);
    timeout = lua_tointeger(L, 4);
    
    ext_lua_reg_function_state(ls_ex_data, L, -1, "request_cb");
    
    ret = pc_request_with_timeout(client, route, msg,
                                  ls_ex_data, timeout, default_request_cb);
    lua_pushinteger(L, ret);
    return 1;
	*/
}

static int notify(lua_State* L) {
	/*
    lpomelo_client_t* pclient;
    pc_client_t* client;
    lua_State* ls_ex_data;
    char* route = NULL;
    char* msg = NULL;
    int timeout = 0;
    int ret;
    
    pclient = (lpomelo_client_t* )lua_touserdata(L, 1);
	assert(pclient);
    client = pclient->client;
	ls_ex_data = pclient->state;
	assert(client && ls_ex_data);
    route = (char*)lua_tostring(L, 2);
    msg = (char*)lua_tostring(L, 3);
    timeout = lua_tointeger(L, 4);
    
    ext_lua_reg_function_state(ls_ex_data, L, -1, "notify_cb");

    ret = pc_notify_with_timeout(client, route, msg,
                                 ls_ex_data, timeout, default_notify_cb);

    lua_pushinteger(L, ret);
    return 1;
	*/
}

static int poll(lua_State* L) {
	/*
    lpomelo_client_t* pclient;
    pc_client_t* client;
    int ret;
    
    pclient = (lpomelo_client_t* )lua_touserdata(L, 1);
	assert(pclient);
    client = pclient->client;
	assert(client);
    ret = pc_client_poll(client);
    
    lua_pushinteger(L, ret);
    return 1;*/
}

static int quality(lua_State* L) {
	/*
    lpomelo_client_t* pclient;
    pc_client_t* client;
    int ret;
    
    pclient = (lpomelo_client_t* )lua_touserdata(L, 1);
	assert(pclient);
    client = pclient->client;
	assert(client);
    ret = pc_client_conn_quality(client);

    lua_pushinteger(L, ret);
    return 1;*/
}

static int disconnect(lua_State* L) {
	/*
    lpomelo_client_t* pclient;
    pc_client_t* client;
    int ret;
    
    pclient = (lpomelo_client_t* )lua_touserdata(L, 1);
	assert(pclient);
    client = pclient->client;
	assert(client);
    ret = pc_client_disconnect(client);
    
    lua_pushinteger(L, ret);
    return 1;*/
}

static int destroy(lua_State* L) {
	/*
    lpomelo_client_t* pclient;
    pc_client_t* client;
    int ret;
    
	
    pclient = (lpomelo_client_t* )lua_touserdata(L, 1);
	assert(pclient);
    client = pclient->client;
    assert(client);
    ret = pc_client_cleanup(client);
    if (ret == PC_RC_OK) {
		if(pclient->state) {
			ext_lua_unreg_state(pclient->state);
		}
		pclient->state = NULL;
		pclient->client = NULL;
        free(client);
    }
    
    lua_pushinteger(L, ret);
    return 1;*/
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