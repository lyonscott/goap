#include "goap.h" 

#include <string.h>
#include <stdio.h>

int main(int argc, char** argv){
    static context_t ctx;
    goap_ctx_clear(&ctx);
    goap_set_pre(&ctx,"idle","idleable",true);
    goap_set_pst(&ctx,"idle","attackable",true);

    goap_set_pre(&ctx,"attack","attackable",true);
    goap_set_pre(&ctx,"attack","stun",false);
    goap_set_pst(&ctx,"attack","killable",true);
    goap_set_pst(&ctx,"attack","spellable",true);

    goap_set_pre(&ctx,"spell","spellable",true);
    goap_set_pst(&ctx,"spell","spellable",false);
    goap_set_pst(&ctx,"spell","killable",true);


    state_t fr; 
    goap_state_clear( &fr );
    goap_state_set( &ctx, &fr, "stun", false );
    goap_state_set( &ctx, &fr, "idleable", true );
    goap_state_set( &ctx, &fr, "attackable", false );
    goap_state_set( &ctx, &fr, "spellable", true );
    goap_state_set( &ctx, &fr, "killable", false );

    goap_set_cost(&ctx,"idle",1);
    goap_set_cost(&ctx,"attack",2);
    goap_set_cost(&ctx,"spell",2);

    state_t goal;
    goap_state_clear( &goal );
    goap_state_set( &ctx, &goal, "killable", true );

    state_t states[16];
    const char* plan[16];
    int plansz=16;
    const int plancost=goap_plan( &ctx, fr, goal, plan, states, &plansz );
    printf( "plan cost = %d\n", plancost );

    for( int i=0; i<plansz && i<16; ++i){
        printf( "%d: %s\n", i, plan[i]);
    }

    return plancost;
}

