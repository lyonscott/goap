#include "goap.h" 

#include <string.h>
#include <stdio.h>

int main(int argc, char** argv){
    static context_t ctx;
    goap_ctx_clear(&ctx);
    goap_set_pre(&ctx,"idle","idleable",true);
    goap_set_pst(&ctx,"idle","attackable",true);

    goap_set_pre(&ctx,"attack","attackable",true);
    goap_set_pre(&ctx,"attack","target",true);
    goap_set_pst(&ctx,"attack","killable",true);
    goap_set_pst(&ctx,"attack","spellable",true);

    goap_set_pre(&ctx,"spell","target",true);
    goap_set_pre(&ctx,"spell","spellable",true);
    goap_set_pst(&ctx,"spell","spellable",false);
    goap_set_pst(&ctx,"spell","killable",true);

    goap_set_pre(&ctx,"search","target",false);
    goap_set_pst(&ctx,"search","target",true);

    goap_set_pre(&ctx,"hide","lowhp",true);
    goap_set_pst(&ctx,"hide","lowhp",false);

    state_t start;
    goap_state_clear(&start);
    goap_state_set(&ctx,&start,"stun",false);
    goap_state_set(&ctx,&start,"idleable",true);
    goap_state_set(&ctx,&start,"attackable",false);
    goap_state_set(&ctx,&start,"spellable",false);
    goap_state_set(&ctx,&start,"killable",false);
    goap_state_set(&ctx,&start,"target",false);

    state_t goal;
    goap_state_clear(&goal );
    goap_state_set(&ctx,&goal,"killable",true);
    state_t states[16];
    const char* plan[16];
    int plansz=16;
    goap_plan(&ctx, fr, goal, plan, &plansz);
    for(int i=0;i<plansz;++i){
        printf( "%d: %s\n", i, plan[i]);
    }
    return 1;
}

