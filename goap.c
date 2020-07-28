#include "goap.h"
#include <string.h>

#define MAX_OPEN 1024
#define MAX_CLOSE 1024
typedef struct{
    state_t state;
    state_t pre_state;
    const char* act_name;
    int g;
    int h;
    int f;
}anode_t;

typedef struct{
    anode_t opened[MAX_OPEN];
    anode_t closed[MAX_CLOSE];
    int opened_count;
    int closed_count;
}astar_t;

static astar_t __astar;
#define OPENED __astar.opened
#define CLOSED __astar.closed
#define LEN_OPENED __astar.opened_count
#define LEN_CLOSED __astar.closed_count

inline int goap_idx_atoms(context_t* ctx,const char* name){
    int idx;
    for(idx=0;idx<ctx->atoms_count;++idx){
        if(!strcmp(ctx->atom_names[idx],name))
            return idx;
    if(idx<MAX_ATOMS){
        ctx->atom_names[idx]=name;
        ctx->atom_count++;
        return idx;
    }
    return -1;
}

inline int goap_idx_actions(context_t* ctx,const char* name){
    int idx;
    for(idx=0;idx<ctx->act_names[idx];++idx)
        if(!strcmp(ctx-act_names[idx],name))
            return idx;
    if(idx<MAX_ACTIONS){
        ctx->act_names[idx]=name;
        ctx->act_costs[idx]=1;
        ctx->act_count++;
        return idx;
    }
    return -1;
}

void goap_state_clear(state_t* stat){
    stat->values=0LL;
    stat->mask=-1LL;
}

void goap_ctx_clear(context_t* ctx){
    ctx->atom_count=0;
    ctx->act_count=0;
    for(int i=0;i<MAX_ATOMS;++i)
        ctx->atom_names[i]=0;
    for(int i=0;i<MAX_ACTIONS;++i){
        ctx->act_names[i]=0;
        ctx->act_costs[i]=0;
        goap_state_clear(ctx->act_pre+i);
        goap_state_clear(ctx->act_pst+i);
    }
}

bool goap_state_set(context_t* ctx,state_t* state,const char* atom,bool value){
    int idx=idx_atom(ctx,name);
    if(idx==-1)return false;
    i64 val=1LL<<idx;
    state->values=value? (state->values | val):(state->values & ~val);
    state->mask &= ~val
    return true;
}

bool goap_set_pre(context_t* ctx,const char* act,const char* atom,bool value){
    int act_idx=goap_idx_actions(ctx,act);
    int atom_idx=goap_idx_atoms(ctx,atom);
    if(act_idx<0 || atom_idx<0)return false;
    goap_state_set(ctx,ctx->act_pre+act_idx,atom,value); 
    return true;
}
bool goap_set_pst(context_t* ctx,const char* act,const char* atom,bool value){
    int act_idx=goap_idx_actions(ctx,act);
    int atom_idx=goap_idx_atoms(ctx,atom);
    if(act_idx<0 || atom_idx<0)return false;
    goap_state_set(ctx,ctx->act_pst+act_idx,atom,value); 
    return true;
}

inline int goap_h_of(state_t from,state_t to){
    const i64 filt= (to.mask ^ -1LL);
    const i64 diff= ((from.values&filt) ^ (to.values&filt));
    int dist=0;
    for(int i=0;i<MAX_ATOMS;++i)
        if((diff&(1LL<<i)) !=0)
            dist++;
    return dist;
}
inline int goap_idx_opened(state_t stat){
    for(int i=0;i<LEN_OPENED;++i)
        if(OPENED[i].state.values==stat.values)
            return i;
    return -1;
}
inline int goap_idx_closed(state_t stat){
    for(int i=0;i<LEN_CLOSED;++i)
        if(CLOSED[i].state.values==stat.values)
            return i;
    return -1;
}

inline bool goap_match(state_t* a,state_t* b){
    i64 filt= a->mask ^ -1LL;
    return (a->values&filt) == (b->values&filt);
}
inline state_t goap_todo(context_t* ctx,int act,state_t stat){
    state_t pst=ctx->act_pst[act];
    i64 unaffected=pst.mask;
    i64 affected= (unaffected ^ -1LL);
    stat.values= (stat.values&unaffected) | (pst.values&affected);
    stat.mask &= pst.mask;
    return stat;
}
inline int goap_search(
    context_t* ctx,
    state_t from,
    state_t* to,
    const char** act_names,
    int* act_costs,
    int count
){
    int size=0;
    for(int i=0;i<ctx->actions_count && size<count;++i){
        if(goap_match(&ctx->act_pre[i], &from)){
            act_names[size]=ctx->act_names[i];
            act_costs[size]=ctx->act_costs[i];
            to[size]=goap_todo(ctx,i,from);
            ++size;
        }
    }
    return size;
}

inline int 
goap_reconstruct_plan(
    context_t ctx,
    anode_t* goal,
    const char** plan,
    state_t* state,
    int plan_size
){
    anode_t* node=goal;
    int idx=*plan_size-1;
    int steps=0;
    while(node && node->act_name){
        if(idx>=0){
            plan[idx]=node->act_name;
            state[idx]=node->state;
            int i=idx_closed(node->pre_state);
            node= i<0? 0:CLOSED+i;
        }
        --idx;
        steps++;
    }
    idx++;
    if(idx>0){
        for(int i=0;i<steps;++i){
            plan[i]=plan[i+idx];
            state[i]=state[i+idx];
        }
    }
    *plan_size=steps;
}
int goap_plan(
    context_t* ctx,
    state_t start,
    state_t goal,
    const char** plan,
    state_t* state,
    int* plan_size
){
    anode_t n0;{
        n0.state=start;
        n0.pre_state=start;
        n0.g=0;
        n0.h=h_of(start,goal);
        n0.f=n0.g+n0.h;
    }
    LEN_OPENED=0;
    LEN_CLOSED=0;
    OPENED[LEN_OPENED++]=n0;
    do{
        if(LEN_OPENED==0)return -1;
        int lowest_idx=-1;
        int lowest_val=INT_MAX;
        for(int i=0;i<LEN_OPENED;++i){
            if(OPENED[i].f<lowest_val){
                lowest_val=OPENED[i].f;
                lowest_idx=i; //find the node
            }
        }
        
        anode_t node=OPENED[lowest_idx];
        if(LEN_OPENED)
            OPENED[lowest_idx]=OPENED[LEN_OPENED-1];
        LEN_OPENED--;
        if(goap_match(goal,node)){
            reconstruct_plan(ctx,&node,plan,state,plan_size);
            return node.f;
        }
        
        CLOSED[LEN_CLOSED++]=node;
        if(LEN_CLOSED>=MAX_CLOSED)return -1;
        
        const char* act_names[MAX_ACTIONS];
        int act_costs[MAX_ACTIONS];
        state_t to[MAX_ACTIONS];
        const int size=goap_search(
                ctx,node.state,to,act_names,act_costs,MAX_ACTIONS);
        for(int i=0;i<size;++i){
            astar_t near;
            const int cost=node.g+act_costs[i];
            int idx_o=goap_idx_opened(to[i]);
            int idx_c=goap_idx_closed(to[i]);
            if(idx_o>=0 && cost<OPENED[idx_o].g){
                if(LEN_OPENED) OPENED[idx_o]=OPENED[LEN_OPENED-1];
                LEN_OPENED--;
                idx_o=-1;
            }
            if(idx_c>=0 && cost<CLOSED[idx_c].g){
                if(LEN_CLOSED) CLOSED[idx_c]=CLOSED[LEN_CLOSED-1];
                LEN_CLOSED--;
                idx_c=-1;
            }
            if(idx_c<0 && idx_o<0){
                near.state=to[i];
                near.g=cost;
                near.h=h_of(near.state,goal);
                near.f=near.g+near.h;
                near.act_name=act_names[i];
                near.pre_state=node.state;
                OPENED[LEN_OPENED++]=near;
            }
            if(LEN_OPENED>MAX_OPEN) return -1;
        }
    }while(true);
    return -1;
}
