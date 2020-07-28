using System;
using System.Collections;
using System.Collections.Generic;

namespace goap.core{
    public struct State{
        public long meta;
        public long mask;
        public void add(int id,bool state){
            meta=state? (meta|(1L<<id)):(meta&~(1L<<id));
            mask|=(1L<<id);
        }
        public void rm(int id){
            meta&=~(1L<<id);
            mask&=~(1L<<id);
        }
        public static bool cmp(State a,State b){
            return (a.meta&b.mask)==b.meta;
        }

        public static void log(long val){
            UnityEngine.Debug.Log(Convert.ToString(val,2).PadLeft(16,'0'));
        }
    }
    public interface IActionNode{
        int id{get;set;}
        State condition{get;set;}
        State effect{get;set;}
        int get_cost();
        bool exec();
    }   
    public class ActionNode:IActionNode{
        public int id{get;set;}
        public State condition{get;set;}
        public State effect{get;set;}
        
        public ActionNode(int id){
            this.id=id;
        }
        public virtual int get_cost(){return 0;}
        public virtual bool exec(){return true;}
    }
    public class Graph{
        struct Edge{
            public int v,w,n;
            public Edge(int _v,int _w,int _n){v=_v;w=_w;n=_n;}
        }
        //static adjacency list
        Edge[] edges;
        int[] heads;
        int idx;
        public Graph(int count){
            if(count<=1)return;
            count=count*(count-1)/2;
            this.edges=new Edge[count];
            this.heads=new int[count];
            for(int i=0;i<count;++i)this.heads[i]=-1;
            this.idx=0;
        }
        public void add(int u,int v,int w){
            UnityEngine.Debug.Log(string.Format("{0}---{1}--->{2}",u,w,v));
            Edge edge=new Edge(v,w,heads[u]);
            edges[idx]=edge;
            heads[u]=idx;
            idx++;
        }
        
        public int adj(int p,ref List<int> list){
            list.Clear();
            int _i=heads[p];
            while(_i>=0){
                list.Add(edges[_i].v);
                _i=edges[_i].n;
            }
            return list.Count;
        }
        public void log(){
            for(int i=0;i<idx;++i){
                UnityEngine.Debug.Log(string.Format("{0},{1},{2},{3}",i,edges[i].v,edges[i].n,heads[i]));
            }
        }
    }
    public class GOAP{
        private int max_act_count; 
        public IActionNode[] actions;
        public State[] conditions;
        public State[] effects;
        private Graph graph_;
        public GOAP(int count=64){
            max_act_count=count;
            actions=new IActionNode[count];
            conditions=new State[count];
            effects=new State[count];
        }
        public void add_act(IActionNode act){
            if(act.id<0||act.id>max_act_count)return;
            if(actions[act.id]!=null)return;
            actions[act.id]=act;
            conditions[act.id]=act.condition;
            effects[act.id]=act.effect;
        }
        public void rm_act(IActionNode act){
            if(act.id<0||act.id>max_act_count)return;
            if(actions[act.id]==null)return;
            actions[act.id]=null;
        }
        public void build(){
            graph_=new Graph(max_act_count);
            for(int u=0;u<max_act_count;++u){
                if(actions[u]==null)continue;
                for(int v=0;v<max_act_count;++v){
                    if(u==v||actions[v]==null)continue;
                    if(State.cmp(actions[u].effect,actions[v].condition))
                        graph_.add(actions[u].id,actions[v].id,actions[v].get_cost());
                }
            }
            graph_.log();
        }
        public bool plan(State start,State goal,ref Stack<IActionNode> todo){
            this.build();
            for(int i=0;i<actions.Length;++i){
                var action=actions[i];
                if(action==null)continue;
                if(State.cmp(start,action.condition)){
                    todo.Push(action);
                }
            }
            return true;
        }
    }
}