#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pomelo.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lpomelo.h"

static int local_storage_cb(pc_local_storage_op_t op, char* data, size_t* len, void* ex_data) {
	int ret = -1;
    char* res = NULL;
	const char* error;
    lua_State* L = (lua_State*)ex_data;
	if (op == PC_LOCAL_STORAGE_OP_WRITE) {
		lua_pushvalue(L, -1);
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
		lua_pushvalue(L, -1);
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

lpomelo_client_t* lpomelo_lua_toclient(lua_State* L, int idx) {
    return (lpomelo_client_t*)lua_touserdata(L, idx);
}

const char* lpomelo_lua_toevstr(lua_State* L, int idx) {
	int ev = luaL_checkinteger(L, idx);
	return pc_client_ev_str(ev);
}

const char* lpomelo_lua_torcstr(lua_State* L, int idx) {
	int rc = luaL_checkinteger(L, idx);
	return pc_client_rc_str(rc);
}

const char* lpomelo_lua_tostatestr(lua_State* L, int idx) {
	int state = luaL_checkinteger(L, idx);
	return pc_client_state_str(state);
}

void lpomelo_lua_initlib(lua_State* L) {
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
}

lpomelo_client_t* lpomelo_lua_newclient(lua_State* L) {
	lpomelo_client_t* client;
	pc_client_t* pomelo_client;
	pomelo_client = (pc_client_t* )malloc(pc_client_size());
	if(pomelo_client){
		lua_newuserdata(L, sizeof(lpomelo_client_t));
		client = lpomelo_lua_toclient(L, -1);
		client->client = pomelo_client;
		client->state = L;
	}
	return client;
}

void lpomelo_lua_popclient(lua_State* L) {
	lpomelo_client_t* client;
	client = lpomelo_lua_toclient(L, -1);
	if(client->client){
		free(client->client);
		client->state = NULL;
	}
	lua_pop(L, 1);
}

int lpomelo_lua_init(lua_State* L, int idx) {
	int ret;
	int tls = 0;
    int polling = 0;
    lpomelo_client_t* client = NULL;
	lua_State* ls_ex_data;
    pc_client_config_t config = PC_CLIENT_CONFIG_DEFAULT;
	tls = lpomelo_lua_tobool(L, -3);
	polling = lpomelo_lua_tobool(L, -2);
    
	if (tls) {
        config.transport_name = PC_TR_NAME_UV_TLS;
    }
    if (polling) {
        config.enable_polling = 1;
    }
	
	client = lpomelo_lua_toclient(L, idx);
    
	config.local_storage_cb = local_storage_cb;
	config.ls_ex_data = L;
    ret = pc_client_init(client->client, NULL, &config);
	lua_pop(L, 3);
    if (ret != PC_RC_OK) {
		return -1;
    }
    return 0;
}

void lpomelo_lua_connect(lua_State* L, int idx) {
	lpomelo_client_t* client;
	char* host = NULL;
    int port = 0;
    int ret;

	client = lpomelo_lua_toclient(L, idx);
	assert(client && client->client);
	host = (char*)luaL_checkstring(L, -2);
	port = luaL_checkinteger(L, -1);

    ret = pc_client_connect(client->client, host, port, NULL);
	lua_pop(L, 2);
	lua_pushinteger(L, ret);
}

void lpomelo_lua_state(lua_State* L, int idx) {
	lpomelo_client_t* client;
	int state;
	client = lpomelo_lua_toclient(L, idx);
	assert(client && client->client);
    state = pc_client_state(client->client);
    lua_pushinteger(L, state);
}

lpomelo_buffer_t* lpomelo_buffer_create(int size) {
	lpomelo_buffer_t* buffer;
	buffer = (lpomelo_buffer_t* )malloc(sizeof(lpomelo_buffer_t));
	assert(buffer);
	buffer->size = size;
	buffer->free = 0;
	return buffer;
}


/*
 * this function is not thread safe
 */
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

/*
 * this function is not thread safe
 */
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
	while (buffer->free < buffer->size) {
		lpomelo_buffer_pop(buffer);
	}
	free(buffer);
}