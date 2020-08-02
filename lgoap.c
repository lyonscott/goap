#include "goap.h"
#include "lua.h"
#include "lauxlib.h"
#include <stdlib.h>
#define MAX_CONTEXTS 32
static context_t* __ctx_list[MAX_CONTEXTS];
static int mk_context(){
    for(int i=0;i<MAX_CONTEXTS;++i){
        if(__ctx_list[i]==NULL){
            __ctx_list[i]=(context_t*)malloc(sizeof(context_t));
            goap_ctx_clear(__ctx_list[i]);
            return i;
        }
    }
    return -1;
}
static void rm_context(int handle){
    if(handle<0 || handle>=MAX_CONTEXTS)
        return;
    if(__ctx_list[handle]!=NULL){
        free(__ctx_list[handle]);
        __ctx_list[handle]=NULL;
    }
}
//int mk_context();
static int lgoap_mk_context(lua_State* L){
    int handle=mk_context();
    lua_pushinteger(L,handle);
    return 1;
}
//void rm_context(int handle);
static int lgoap_rm_context(lua_State* L){
    int handle=(int)luaL_checkinteger(L,1);
    rm_context(handle);
    return 0;
}
static inline void lgoap_set_pre(lua_State* L,context_t* ctx,const char* act){
    lua_pushnil(L);
    while(lua_next(L,-2)){
        const char* atom=lua_tostring(L,-2);
        bool value=lua_toboolean(L,-1);
        goap_set_pre(ctx,act,atom,value);
        lua_pop(L,1);
    }
}
static inline void lgoap_set_pst(lua_State* L,context_t* ctx,const char* act){
    lua_pushnil(L);
    while(lua_next(L,-2)){
        const char* atom=lua_tostring(L,-2);
        bool value=lua_toboolean(L,-1);
        goap_set_pst(ctx,act,atom,value);
        lua_pop(L,1);
    }
}

//void set_action(int ctx,table action);
static int lgoap_set_action(lua_State* L){
    int handle=(int)luaL_checkinteger(L,1);
    context_t* ctx=__ctx_list[handle];
    if(ctx==NULL)return 0;

    const char* act=luaL_checkstring(L,2);
    luaL_checktype(L,3,LUA_TTABLE);
    lua_pushnil(L);
    while(lua_next(L,-2)){
        const char* key=lua_tostring(L,-2);
        if(!strcmp(key,"condition")){
            lgoap_set_pre(L,ctx,act);
        }else if(!strcmp(key,"effect")){
            lgoap_set_pst(L,ctx,act);
        }else if(!strcmp(key,"cost")){
            int cost=luaL_checkinteger(L,-1);
            goap_set_cost(ctx,act,cost);
        }
        lua_pop(L,1);
    }
    return 1;
}
static int lgoap_state_set(lua_State* L,context_t* ctx,state_t* stat){
    goap_state_clear(stat);
    lua_pushnil(L);
    while(lua_next(L,-2)){
        const char* atom=lua_tostring(L,-2);
        bool value=lua_toboolean(L,-1);
        goap_state_set(ctx,stat,atom,value);
        lua_pop(L,1);
    }
}
//table plan(int ctx,table start,table goal);
static int lgoap_plan(lua_State* L){
    int handle=(int)luaL_checkinteger(L,1);
    context_t* ctx=__ctx_list[handle];
    if(ctx==NULL)return 0;
    luaL_checktype(L,2,LUA_TTABLE);
    luaL_checktype(L,3,LUA_TTABLE);
    lua_pushvalue(L,2);
    state_t start;
    lgoap_state_set(L,ctx,&start);
    lua_pop(L,1);
    lua_pushvalue(L,3);
    state_t goal;
    lgoap_state_set(L,ctx,&goal);
    lua_pop(L,1);

    const char* plan[16];
    int size=16;
    goap_plan(ctx,start,goal,plan,&size);
    lua_newtable(L);
    for(int i=0;i<size;++i){
        lua_pushnumber(L,i+1);
        lua_pushstring(L,plan[i]);
        lua_settable(L,-3);
    }
    return 1;
}

LUAMOD_API int luaopen_goap(lua_State* L){
    luaL_checkversion(L);
    luaL_Reg l[]={
        {"mk_context", lgoap_mk_context},
        {"rm_context", lgoap_rm_context},
        {"set_action", lgoap_set_action},
        {"plan", lgoap_plan},
        {NULL,NULL},
    };
    luaL_newlib(L,l);
    return 1;
}
