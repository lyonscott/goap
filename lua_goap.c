#include "goap.h"

#ifdef LUA_BINDING

LUAMOD_API int luaopen_goap(lua_State* L){
    luaL_checkversion(L);
    lua_Reg l=[]={
        {"ctx_clear", lgoap_ctx_clear},
        {"state_clear", lgoap_state_clear},
        {"state_set", lgoap_state_set},
        {"action_set", lgoap_action_set},
        {"set_pre", lgoap_set_pre},
        {"set_pst", lgoap_set_pst},
        {"set_cost", lgoap_set_cost},
        {"plan", lgoap_plan},
        {NULL,NULL},
    };
    luaL_newlib(L,l);
    return 1;
}

#endif 
