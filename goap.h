#ifndef GOAP_H
#define GOAP_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_ATOMS 64
#define MAX_ACTIONS 64

typedef int64_t i64;

typedef struct{
    i64 values;
    i64 mask;
}state_t;

typedef struct{
    const char* atom_names[MAX_ATOMS];  //all atoms
    const char* act_names[MAX_ACTIONS]; //all actions
    state_t act_pre[MAX_ACTIONS]; //preconditions for all
    state_t act_pst[MAX_ACTIONS]; //effects for all
    int atoms_count;
    int acts_count;
}context_t;

extern void goap_ctx_clear(context_t* ctx);
extern void goap_state_clear(state_t* stat);

extern bool goap_state_set(context_t* ctx,state_t* stat,const char* atom,bool value);
extern bool goap_set_pre(context_t* ctx,const char* act,const char* atom,bool value);
extern bool goap_set_pst(context_t* ctx,const char* act,const char* atom,bool value);
extern bool goap_set_cost(context_t* ctx,const char* act,const char* atom,int cost);

extern int goap_plan(
    context_t* ctx,
    state_t* start,
    state_t* goal,
    const char** plan,
    state_t* state,
    int* plan_size
);

#ifdef LUA_BINDING
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

static int lgoap_ctx_clear(lua_State* L);
static int lgoap_state_clear(lua_State* L);
static int lgoap_state_set(lua_State* L);
static int lgoap_set_pre(lua_State* L);
static int lgoap_set_pst(lua_State* L);
static int lgoap_plan(lua_State* L);

LUAMOD_API int luaopen_goap(lua_State* L){
    luaL_checkversion(L);
    lua_Reg lib=[]={
        {"ctx_clear", lgoap_ctx_clear},
        {"state_clear", lgoap_state_clear},
        {"state_set", lgoap_state_set},
        {"set_pre", lgoap_set_pre},
        {"set_pst", lgoap_set_pst},
        {"set_cost", lgoap_set_cost},
        {"plan", lgoap_plan},
        {NULL,NULL},
    };
    luaL_newlib(L,lib);
    return 1;
}
#endif

#endif