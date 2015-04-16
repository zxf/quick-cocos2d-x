#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <pomelo.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lpomelo.h"

static int lib_init(lua_State* L) {
    lua_pushvalue(L, -3);
	lua_pushvalue(L, -3);
	lua_pushvalue(L, -3);
	lpomelo_initlib(L);
	return 0;
}

static int lib_cleanup(lua_State* L) {
    pc_lib_cleanup();
    return 0;
}

static int ev_to_str(lua_State* L) {
    lua_pushstring(L, lpomelo_toevstr(L, 1));
    return 1;
}

static int rc_to_str(lua_State* L) {
    lua_pushstring(L, lpomelo_torcstr(L, 1));
    return 1;
}

static int state_to_str(lua_State* L) {
    lua_pushstring(L, lpomelo_tostatestr(L, 1));
    return 1;
}

static int create(lua_State* L) {
	lpomelo_client_t* client;
    client = lpomelo_newclient(L);
	if (!client) {
		return 0;
	}
	lua_pushvalue(L, -4);
	lua_pushvalue(L, -4);
	lua_pushvalue(L, -4);
	if(lpomelo_init(L, -4)){
        lua_pop(L, 1);
		lpomelo_popclient(L);
		return 0;
	}
    return 1;
}

static int connect(lua_State* L) {
	lua_pushvalue(L, -2);
	lua_pushvalue(L, -2);
	return lpomelo_connect(L, 1);
}

static int state(lua_State* L) {
    return lpomelo_state(L, 1);
}

static int sub(lua_State* L) {
    lua_pushvalue(L, -2);
    lua_pushvalue(L, -2);
	return lpomelo_sub(L, 1);
} 

static int sub_close(lua_State* L) {
    lua_pushvalue(L, -1);
    return lpomelo_sub_close(L, 1);
}

static int sub_result(lua_State* L) {
    return lpomelo_sub_result(L, 1);
}

static int request(lua_State* L) {
    lua_pushvalue(L, -3);
    lua_pushvalue(L, -3);
    lua_pushvalue(L, -3);
    lpomelo_request(L, 1);
    return 1;
}

static int request_close(lua_State* L) {
    return lpomelo_request_close(L, 1);
}

static int request_result(lua_State* L) {
    return lpomelo_request_result(L, 1);
}

static int notify(lua_State* L) {
    lua_pushvalue(L, -3);
    lua_pushvalue(L, -3);
    lua_pushvalue(L, -3);
    lpomelo_notify(L, 1);
    return 1;
}

static int notify_close(lua_State* L) {
    return lpomelo_notify_close(L, 1);
}

static int notify_result(lua_State* L) {
    return lpomelo_notify_result(L, 1);
}

static int poll(lua_State* L) {
    return lpomelo_poll(L, -1);
}

static int quality(lua_State* L) {
	return lpomelo_poll(L, -1);
}

static int disconnect(lua_State* L) {
    return lpomelo_disconnect(L, -1);
}

static int destroy(lua_State* L) {
	return lpomelo_destroy(L, -1);
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
    {"sub", sub},
    {"sub_close", sub_close},
    {"sub_result", sub_result},
    {"request", request},
    {"request_close", request_close},
    {"request_result", request_result},
    {"notify", notify},
    {"notify_close", notify_close},
    {"notify_result", notify_result},
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