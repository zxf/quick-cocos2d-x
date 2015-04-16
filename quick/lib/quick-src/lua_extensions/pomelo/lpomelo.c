#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pomelo.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lpomelo.h"

static void strcopy(char* dest, const char* src) {
    if(src) {
        dest = (char*)malloc(sizeof(char)*(strlen(src) + 1));
        strcpy(dest, src);
    } else {
        dest = NULL;
    }
}

lpomelo_event_recv_t* lpomelo_event_recv_create(int ev, const char* arg1, const char* arg2) {
    lpomelo_event_recv_t* recv;
    recv->ev = ev;
    strcopy(recv->arg1, arg1);
    strcopy(recv->arg2, arg2);
    return recv;
}

void lpomelo_event_recv_destroy(lpomelo_event_recv_t* recv){
    if(!recv) {
        return;
    }
    if(recv->arg1) {
        free(recv->arg1);
    }
    if(recv->arg2) {
        free(recv->arg2);
    }
    free(recv);
}

lpomelo_request_recv_t* lpomelo_request_recv_create(int ret, int rc, const char* resp) {
    lpomelo_request_recv_t* recv;
    recv = (lpomelo_request_recv_t* )malloc(sizeof(lpomelo_request_recv_t));
    recv->ret = ret;
    recv->rc = rc;
    strcopy(recv->resp, resp);
    return recv;
}

void lpomelo_request_recv_destroy(lpomelo_request_recv_t* recv){
    if(!recv) {
        return;
    }
    if(recv->resp) {
        free(recv->resp);
    }
    free(recv);
}

lpomelo_notify_recv_t* lpomelo_notify_recv_create(int ret, int rc) {
    lpomelo_notify_recv_t* recv;
    recv = (lpomelo_notify_recv_t* )malloc(sizeof(lpomelo_notify_recv_t));
    recv->ret = ret;
    recv->rc = rc;
    return recv;
}

void lpomelo_notify_recv_destroy(lpomelo_notify_recv_t* recv){
    if(!recv) {
        return;
    }
    free(recv);
}

lpomelo_buffer_t* lpomelo_buffer_create(int size, int flag) {
    lpomelo_buffer_t* buffer;
    buffer = (lpomelo_buffer_t* )malloc(sizeof(lpomelo_buffer_t));
    assert(buffer);
    buffer->size = size;
    buffer->free = 0;
    buffer->flag = flag;
    return buffer;
}

int lpomelo_buffer_push(lpomelo_buffer_t* buffer, void* data) {
    lpomelo_buffer_entity_t* entity;
    if (buffer->free > 0){
        entity = (lpomelo_buffer_entity_t* )malloc(sizeof(lpomelo_buffer_entity_t));
        assert(entity);
        entity->data = data;
        buffer->last->next = entity;
        buffer->last = entity;
        buffer->free --;
        return 0;
    }
    return -1;
}

void* lpomelo_buffer_pop(lpomelo_buffer_t* buffer) {
    lpomelo_buffer_entity_t* entity;
    void* data;
    if (buffer->free < buffer->size){
        entity = buffer->first;
        buffer->first = entity->next;
        buffer->free ++;
        data = entity->data;
        free(entity);
    }
    return data;
}

void lpomelo_buffer_destroy(lpomelo_buffer_t* buffer) {
    if (!buffer){
        return;
    }
    free(buffer);
}

void lpomelo_event_buffer_destory(lpomelo_buffer_t* buffer) {
    if (!buffer){
        return;
    }
    while (buffer->free < buffer->size) {
         lpomelo_event_recv_destroy(lpomelo_buffer_pop(buffer));
    }
    lpomelo_buffer_destroy(buffer);
}



static lua_State* lpomelo_lua_newstate(lua_State* L){
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

static int lpomelo_lua_rmstate(lua_State* L){
    int ref;
    ref = lua_tointeger(L, 1);
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    return 0;
}

static int lpomelo_lua_regfunc(lua_State* L, lua_State* S){
    int ref;
    lua_State* src_L;
    if (!S) {
        src_L = S;
    } else {
        src_L = L;
    }
    if(!lua_isfunction(L, -1) && !lua_iscfunction(L, -1)) {
        luaL_error(L, "type error: %s is not a function.", lua_typename(L, lua_type(L, -1)));
    } else {
        ref = lua_tointeger(L, 1);
        lua_rawgeti(src_L, LUA_REGISTRYINDEX, ref);
        lua_pushvalue(src_L, -3);
        lua_pushvalue(src_L, -3);
        lua_rawset(src_L, -3);
        lua_pop(src_L, 3);
    }
    return 0;
}

static int lpomelo_lua_unregfunc(lua_State* L){
    int ref;
    ref = lua_tointeger(L, 1);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    lua_pushvalue(L, -2);
    lua_pushnil(L);
    lua_rawset(L, -3);
    lua_pop(L, 2);
    return 0;
}

static lua_State* lpomelo_lua_getfunc(lua_State* L) {
    int ref;
    ref = lua_tointeger(L, 1);
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    lua_pushvalue(L, -2);
    lua_rawget(L, -2);
    if(!lua_isfunction(L, -1) && !lua_iscfunction(L, -1)) {
        luaL_error(L, "type error: %s is not a function.", lua_typename(L, lua_type(L, -1)));
    } else {
        lua_remove(L, -2);
        lua_remove(L, -2);
    }
    return L;
}

int lpomelo_lua_tobool(lua_State* L, int idx) {
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

lpomelo_request_recv_t* lpomelo_torequestrecv(lua_State* L, int idx) {
    return (lpomelo_request_recv_t*)lua_touserdata(L, idx);
}

lpomelo_notify_recv_t* lpomelo_tonotifyrecv(lua_State* L, int idx) {
    return (lpomelo_notify_recv_t*)lua_touserdata(L, idx);
}

lpomelo_buffer_t* lpomelo_tobuffer(lua_State* L, int idx) {
    return (lpomelo_buffer_t*)lua_touserdata(L, idx);
}

lpomelo_client_t* lpomelo_toclient(lua_State* L, int idx) {
    return (lpomelo_client_t*)lua_touserdata(L, idx);
}

const char* lpomelo_toevstr(lua_State* L, int idx) {
	int ev = luaL_checkinteger(L, idx);
	return pc_client_ev_str(ev);
}

const char* lpomelo_torcstr(lua_State* L, int idx) {
	int rc = luaL_checkinteger(L, idx);
	return pc_client_rc_str(rc);
}

const char* lpomelo_tostatestr(lua_State* L, int idx) {
	int state = luaL_checkinteger(L, idx);
	return pc_client_state_str(state);
}


static int local_storage_cb(pc_local_storage_op_t op, char* data, size_t* len, void* ex_data) {
    int ret = -1;
    char* res = NULL;
    const char* error;
    lua_State* L = (lua_State*)ex_data;
    lua_pushstring(L, "storage_db");
    lpomelo_lua_getfunc(L);
    if (op == PC_LOCAL_STORAGE_OP_WRITE) {
        lua_pushinteger(L, op);
        lua_pushlstring(L, data, *len);
        if (lua_pcall(L, 2, 1, 0) != 0) {
            error = lua_tostring(L, -1);
            luaL_error(L, error);
        } else if (lua_isnone(L, -1) ) {
            luaL_error(L, "error...");
        } else {
            ret = luaL_checklong(L, -1);
            lua_pop(L, 1);
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
                lua_pop(L, 1);
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

static void default_destructor(void* ex_data) {
    lpomelo_event_buffer_destory((lpomelo_buffer_t*)ex_data);
}

static void default_event_cb(pc_client_t* client, int ev_type, void* ex_data, const char* arg1, const char* arg2)
{
    int enable_poll;
    lpomelo_buffer_t* buffer;
    lpomelo_event_recv_t* recv;
    buffer = (lpomelo_buffer_t*)ex_data;
    
    assert(client && buffer);
    enable_poll = pc_client_config(client)->enable_polling;
    if(buffer->flag && buffer->flag != ev_type) {
        return;
    }
    recv = lpomelo_event_recv_create(ev_type, arg1, arg2);
    if(lpomelo_buffer_push(buffer, recv)){
        lpomelo_event_recv_destroy(recv);
        fprintf(stderr, "Sub buffer is full.");
    }
}

static void default_request_cb(const pc_request_t* req, int rc, const char* resp)
{
    int enable_poll;
    pc_client_t* client;
    lpomelo_request_recv_t* recv;
    
    client = pc_request_client(req);
    recv = (lpomelo_request_recv_t*)pc_request_ex_data(req);
    
    if(!client || !recv){
        return;
    }
    enable_poll = pc_client_config(client)->enable_polling;
    recv->rc = rc;
    recv->rec = 1;
    strcopy(recv->resp, resp);
}


static void default_notify_cb(const pc_notify_t* notify, int rc)
{
    int enable_poll;
    pc_client_t* client;
    lpomelo_notify_recv_t* recv;
    
    client = pc_notify_client(notify);
    recv = (lpomelo_notify_recv_t*)pc_notify_ex_data(notify);
    
    if(!client || !recv){
        return;
    }
    enable_poll = pc_client_config(client)->enable_polling;
    
    recv->rc = rc;
    recv->rec = 1;
}

int lpomelo_initlib(lua_State* L) {
    int log_level;
    const char* ca_file = NULL;
    const char* ca_path = NULL;
    log_level = luaL_checkinteger(L, -3);
    if(!lua_isnil(L, -2)){
        ca_file = luaL_checkstring(L, -2);
    }
    if(!lua_isnil(L, -1)){
        ca_path = luaL_checkstring(L, -1);
    }
#if !defined(PC_NO_UV_TLS_TRANS)
    if (ca_file || ca_path) {
        tr_uv_tls_set_ca_file(ca_file, ca_path);
    }
#endif
	pc_lib_set_default_log_level(log_level); 
    pc_lib_init(NULL, NULL, NULL, "Lua Client");
	lua_pop(L, 3);
    return 0;
}

lpomelo_client_t* lpomelo_newclient(lua_State* L) {
	lpomelo_client_t* client;
	pc_client_t* pomelo_client;
	pomelo_client = (pc_client_t* )malloc(pc_client_size());
	if(pomelo_client){
		lua_newuserdata(L, sizeof(lpomelo_client_t));
		client = lpomelo_toclient(L, -1);
		client->client = pomelo_client;
		client->state = lpomelo_lua_newstate(L);
	}
	return client;
}

void lpomelo_cleanclient(lua_State*L, int idx) {
    lpomelo_client_t* client;
    client = lpomelo_toclient(L, idx);
    if(client->client){
        lpomelo_lua_rmstate(client->state);
        free(client->client);
        client->state = NULL;
    }
}

int lpomelo_popclient(lua_State* L) {
    lpomelo_cleanclient(L, -1);
	lua_pop(L, 1);
    return 0;
}

int lpomelo_init(lua_State* L, int idx) {
	int ret;
	int tls = 0;
    int polling = 0;
    lpomelo_client_t* client = NULL;
    pc_client_config_t config = PC_CLIENT_CONFIG_DEFAULT;
	tls = lpomelo_lua_tobool(L, -3);
	polling = lpomelo_lua_tobool(L, -2);
    
	if (tls) {
        config.transport_name = PC_TR_NAME_UV_TLS;
    }
    if (polling) {
        config.enable_polling = 1;
    }
	
	client = lpomelo_toclient(L, idx);
    
	config.local_storage_cb = local_storage_cb;
	config.ls_ex_data = client->state;
    
    lua_pushstring(L, "storage_cb");
    lua_pushvalue(L, -2);
    lpomelo_lua_regfunc(client->state, L);
    
    ret = pc_client_init(client->client, NULL, &config);
	lua_pop(L, 3);
    if (ret != PC_RC_OK) {
        lua_pushstring(L, "client init error");
		return 1;
    }
    return 0;
}

int lpomelo_connect(lua_State* L, int idx) {
	lpomelo_client_t* client;
	char* host = NULL;
    int port = 0;
    int ret;

	client = lpomelo_toclient(L, idx);
	assert(client && client->client);
	host = (char*)luaL_checkstring(L, -2);
	port = luaL_checkinteger(L, -1);

    ret = pc_client_connect(client->client, host, port, NULL);
	lua_pop(L, 2);
	lua_pushinteger(L, ret);
    return 1;
}

int lpomelo_state(lua_State* L, int idx) {
	lpomelo_client_t* client;
	int state;
	client = lpomelo_toclient(L, idx);
	assert(client && client->client);
    state = pc_client_state(client->client);
    lua_pushinteger(L, state);
    return 1;
}

int lpomelo_sub(lua_State* L, int idx) {
    int size = 1024;
    int ev_type;
    int ret;
    lpomelo_buffer_t* buffer;
    lpomelo_client_t* client;
    client = lpomelo_toclient(L, idx);
    assert(client && client->client);
    if(!lua_isnil(L, -2)) {
        ev_type = lua_tointeger(L, -2);
    }
    if(!lua_isnil(L, -1)) {
        size = lua_tointeger(L, -1);
    }
    lua_pop(L, 2);
    buffer = lpomelo_buffer_create(size, ev_type);
    ret = pc_client_add_ev_handler(client->client, default_event_cb, buffer, default_destructor);
    if (ret != PC_EV_INVALID_HANDLER_ID) {
        lpomelo_event_buffer_destory(buffer);
        return 0;
    }
    buffer->hid = ret;
    lua_pushlightuserdata(L, buffer);
    return 1;
}

int lpomelo_sub_close(lua_State* L, int idx) {
    int ret;
    lpomelo_buffer_t* buffer;
    lpomelo_client_t* client;
    client = lpomelo_toclient(L, idx);
    buffer = lpomelo_tobuffer(L, -1);
    assert(client && client->client && buffer);
    ret = pc_client_rm_ev_handler(client->client, buffer->hid);
    lpomelo_event_buffer_destory(buffer);
    lua_pop(L, 1);
    lua_pushinteger(L, ret);
    return 1;
}

int lpomelo_sub_result(lua_State* L, int idx) {
    lpomelo_event_recv_t* recv;
    lpomelo_buffer_t* buffer;
    buffer = lpomelo_tobuffer(L, idx);
    
    recv = (lpomelo_event_recv_t*)lpomelo_buffer_pop(buffer);
    if(recv) {
        lua_pushinteger(L, recv->ev);
        lua_pushstring(L, recv->arg1);
        lua_pushstring(L, recv->arg2);
        lpomelo_event_recv_destroy(recv);
        return 3;
    }
    return 0;
}

int lpomelo_request(lua_State* L, int idx) {
    lpomelo_client_t* client;
    lpomelo_request_recv_t* recv;
    char* route = NULL;
    char* msg = NULL;
    int timeout = 0;
    int ret;
    
    client = lpomelo_toclient(L, 1);
    assert(client && client->client);
    route = (char*)lua_tostring(L, -3);
    msg = (char*)lua_tostring(L, -2);
    timeout = lua_tointeger(L, -1);
    lua_pop(L, 3);
    recv = lpomelo_request_recv_create(NULL, NULL, NULL);
    ret = pc_request_with_timeout(client->client, route, msg,
                                  recv, timeout, default_request_cb);
    recv->ret = ret;
    lua_pushlightuserdata(L, recv);
    return 1;
}

int lpomelo_request_close(lua_State* L, int idx) {
    lpomelo_request_recv_t* recv;
    recv = lpomelo_torequestrecv(L, idx);
    lpomelo_request_recv_destroy(recv);
    return 0;
}

int lpomelo_request_result(lua_State* L, int idx) {
    lpomelo_request_recv_t* recv;
    recv = lpomelo_torequestrecv(L, idx);
    if(recv->rec) {
        lua_pushinteger(L, recv->rc);
        lua_pushinteger(L, recv->resp);
        return 2;
    }
    return 0;
}

int lpomelo_notify(lua_State* L, int idx) {
    lpomelo_client_t* client;
    lpomelo_notify_recv_t* recv;
    char* route = NULL;
    char* msg = NULL;
    int timeout = 0;
    int ret;
    
    client = lpomelo_toclient(L, 1);
    assert(client && client->client);
    route = (char*)lua_tostring(L, -3);
    msg = (char*)lua_tostring(L, -2);
    timeout = lua_tointeger(L, -1);
    lua_pop(L, 3);
    recv = lpomelo_notify_recv_create(NULL, NULL);
    ret = pc_notify_with_timeout(client->client, route, msg,
                                  recv, timeout, default_notify_cb);
    recv->ret = ret;
    lua_pushlightuserdata(L, recv);
    return 1;
}

int lpomelo_notify_close(lua_State* L, int idx) {
    lpomelo_notify_recv_t* recv;
    recv = lpomelo_tonotifyrecv(L, idx);
    lpomelo_notify_recv_destroy(recv);
    return 0;
}

int lpomelo_notify_result(lua_State* L, int idx) {
    lpomelo_notify_recv_t* recv;
    recv = lpomelo_tonotifyrecv(L, idx);
    if(recv->rec) {
        lua_pushinteger(L, recv->rc);
        return 1;
    }
    return 0;
}

int lpomelo_poll(lua_State* L, int idx) {
    lpomelo_client_t* client;
    int ret;
    
    client = lpomelo_toclient(L, idx);
    assert(client && client->client);
    ret = pc_client_poll(client->client);
    lua_pushinteger(L, ret);
    return 1;
}

int lpomelo_quality(lua_State* L, int idx) {
    lpomelo_client_t* client;
    int ret;
     
    client = lpomelo_toclient(L, idx);
    assert(client && client->client);
    ret = pc_client_conn_quality(client->client);
    lua_pushinteger(L, ret);
    return 1;
}

int lpomelo_disconnect(lua_State* L, int idx) {
     lpomelo_client_t* client;
     int ret;
     
     client = lpomelo_toclient(L, idx);
     assert(client && client->client);
     ret = pc_client_disconnect(client->client);
     lua_pushinteger(L, ret);
     return 1;
}

int lpomelo_destroy(lua_State* L, int idx) {
     lpomelo_client_t* client;
     int ret;
    
     client = lpomelo_toclient(L, idx);
     assert(client && client->client);
     ret = pc_client_cleanup(client->client);
     if (ret == PC_RC_OK) {
         lpomelo_cleanclient(L, idx);
     }
     lua_pushinteger(L, ret);
     return 1;
}
