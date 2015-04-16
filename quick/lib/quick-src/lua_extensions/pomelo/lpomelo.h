#include <pomelo.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#ifndef __LPOMELO_H_
#define __LPOMELO_H_

struct lpomelo_event_recv_s {
	int ev;
	char* arg1;
	char* arg2;
};
typedef struct lpomelo_event_recv_s lpomelo_event_recv_t;

struct lpomelo_request_recv_s {
	int ret;
    int rc;
    int rec;
	char* resp;
};
typedef struct lpomelo_request_recv_s lpomelo_request_recv_t;

struct lpomelo_notify_recv_s {
	int ret;
    int rc;
    int rec;
};
typedef struct lpomelo_notify_recv_s lpomelo_notify_recv_t;

struct lpomelo_buffer_entity_s {
	void* data;
	struct lpomelo_buffer_entity_s* next;
};
typedef struct lpomelo_buffer_entity_s lpomelo_buffer_entity_t;

struct lpomelo_buffer_s {
	int size;
	int free;
    int flag;
    int hid;
	lpomelo_buffer_entity_t* first;
	lpomelo_buffer_entity_t* last;
};
typedef struct lpomelo_buffer_s lpomelo_buffer_t;

struct lpomelo_client_s {
    pc_client_t* client;
    lua_State* state;
};
typedef struct lpomelo_client_s lpomelo_client_t;


int lpomelo_lua_tobool(lua_State* L, int idx);
lpomelo_client_t* lpomelo_toclient(lua_State* L, int idx);
const char* lpomelo_toevstr(lua_State* L, int idx);
const char* lpomelo_torcstr(lua_State* L, int idx);
const char* lpomelo_tostatestr(lua_State* L, int idx);
int lpomelo_initlib(lua_State* L);
lpomelo_client_t* lpomelo_newclient(lua_State* L);
int lpomelo_popclient(lua_State* L);
int lpomelo_init(lua_State* L, int idx);
int lpomelo_connect(lua_State* L, int idx);
int lpomelo_state(lua_State* L, int idx);
int lpomelo_sub(lua_State* L, int idx);
int lpomelo_sub_close(lua_State* L, int idx);
int lpomelo_sub_result(lua_State* L, int idx);
int lpomelo_request(lua_State* L, int idx);
int lpomelo_request_close(lua_State* L, int idx);
int lpomelo_request_result(lua_State* L, int idx);
int lpomelo_notify(lua_State* L, int idx);
int lpomelo_notify_close(lua_State* L, int idx);
int lpomelo_notify_result(lua_State* L, int idx);
int lpomelo_poll(lua_State* L, int idx);
int lpomelo_quality(lua_State* L, int idx);
int lpomelo_disconnect(lua_State* L, int idx);
int lpomelo_destroy(lua_State* L, int idx);

lpomelo_buffer_t* lpomelo_buffer_create(int size, int flag);
int lpomelo_buffer_push(lpomelo_buffer_t* buffer, void* data);
void* lpomelo_buffer_pop(lpomelo_buffer_t* buffer);
void lpomelo_buffer_destroy(lpomelo_buffer_t* buffer);


#endif