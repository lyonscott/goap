using NUnit.Framework;
using System.Collections.Generic;
using goap.core;

namespace goap.test{
    public enum StateTag{
        Money=0,
        Weapon=1,
        Food=2,
        Hunger=31,
        Happy=32,
        Angry=33,
    }
    public enum ActionTag{
        Buy=0,
        Eat=1,
        Work=2,
        Search=3,
        Sleep=4,
    }
    public class DoBuy:ActionNode{
        public DoBuy():base((int)ActionTag.Buy){
            var pre=new State();{
                pre.add((int)StateTag.Money,true);
                this.condition=pre;
            }
            var pst=new State();{
                pst.add((int)StateTag.Food,true);
                pst.add((int)StateTag.Money,false);
                this.effect=pst;
            }
        }
    }
    public class DoEat:ActionNode{
        public DoEat():base((int)ActionTag.Eat){
            var pre=new State();{
                pre.add((int)StateTag.Hunger,true);
                pre.add((int)StateTag.Food,true);
                this.condition=pre;
            }
            var pst=new State();{
                pst.add((int)StateTag.Hunger,false);
                pst.add((int)StateTag.Food,false);
                pst.add((int)StateTag.Happy,true);
                this.effect=pst;
            }
        }
    }
    public class DoWork:ActionNode{
        public DoWork():base((int)ActionTag.Work){
            var pre=new State();{
                pre.add((int)StateTag.Hunger,false);
                pre.add((int)StateTag.Angry,false);
                pre.add((int)StateTag.Happy,true);
                this.condition=pre;
            }
            var pst=new State();{
                pst.add((int)StateTag.Happy,false);
                pst.add((int)StateTag.Weapon,true);
                this.effect=pst;
            }   
        }
    }
    public class DoSearch:ActionNode{
        public DoSearch():base((int)ActionTag.Search){
            var pre=new State();{
                pre.add((int)StateTag.Weapon,false);
                pre.add((int)StateTag.Hunger,false);
                pre.add((int)StateTag.Angry,true);
                this.condition=pre;
            }
            var pst=new State();{
                pst.add((int)StateTag.Hunger,true);
                pst.add((int)StateTag.Angry,false);
                pst.add((int)StateTag.Weapon,true);
                this.effect=pst;
            }
        }
    }
    public class DoSleep:ActionNode{
        public DoSleep():base((int)ActionTag.Sleep){
            var pre=new State();{
                pre.add((int)StateTag.Weapon,true);
                this.condition=pre;
            }
            var pst=new State();{
                pst.add((int)StateTag.Hunger,true);
                pst.add((int)StateTag.Happy,true);
                this.effect=pst;
            }
        }
    }
    public class Tester{
        [Test]
        public void test_state(){
            State goal=new State();{
                goal.add(1,false);
                goal.add(2,true);
                goal.add(3,false);
                goal.add(4,true);
            }
            State tmp_1=new State();{
                tmp_1.add(3,false);
                tmp_1.add(4,true);
            }
            Assert.IsTrue(State.cmp(goal,tmp_1));
            tmp_1.rm(4);
            Assert.IsTrue(State.cmp(goal,tmp_1));
            tmp_1.add(5,true);
            Assert.IsFalse(State.cmp(goal,tmp_1));
        }
        [Test]
        public void test_action(){
            DoEat eat=new DoEat();
            DoSleep sleep=new DoSleep();
            DoWork work=new DoWork();
            Assert.IsTrue(State.cmp(eat.effect,work.condition));
            Assert.IsTrue(State.cmp(work.effect,sleep.condition));
            Assert.IsFalse(State.cmp(work.condition,sleep.effect));
        }
        [Test]
        public void test_goap(){
            GOAP goap=new GOAP(12);{
                goap.add_act(new DoBuy());
                goap.add_act(new DoEat());
                goap.add_act(new DoSearch());
                goap.add_act(new DoSleep());
                goap.add_act(new DoWork());
            }
            State start=new State();{
                start.add((int)StateTag.Angry,false);
                start.add((int)StateTag.Food,false);
                start.add((int)StateTag.Happy,false);
                start.add((int)StateTag.Weapon,false);
                start.add((int)StateTag.Hunger,true);
                start.add((int)StateTag.Money,true);
            }
            State goal=new State();{
                goal.add((int)StateTag.Happy,true);
                goal.add((int)StateTag.Weapon,true);
            }
            Stack<IActionNode> todo=new Stack<IActionNode>();
            goap.plan(start,goal,ref todo);
            foreach(var act in todo){
                UnityEngine.Debug.Log((ActionTag)act.id);
            }
        }
        [Test]
        public void test_graph(){
            Graph graph=new Graph(5);
            graph.add(1,2,1);
            graph.add(1,3,1);
            graph.add(1,4,1);
            graph.add(2,3,1);
            graph.add(2,4,1);
            graph.add(3,4,1);
            graph.add(4,5,1);
            graph.add(5,1,1);
            graph.log();
            List<int> adjs=new List<int>();
            Assert.IsTrue(graph.adj(4,ref adjs)==1);
            Assert.IsTrue(graph.adj(1,ref adjs)==3);
            Assert.IsFalse(graph.adj(5,ref adjs)==2);
        }
    }
}