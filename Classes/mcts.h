#include <vector>
#include <unordered_map>
#include <limits>
#include <cmath>
#include <iostream>
#include <chrono>
#include <cstdlib>

#ifndef MCTS_H
#define MCTS_H

using namespace std::chrono;


class State{
	friend std::ostream& operator<<(std::ostream& strm, State& s){
		s.print(strm);
	    return strm;
	};

protected:

	virtual void print(std::ostream &strm) {strm << this;};

public:
	virtual ~State(){};
};


template<class T>
class Action{
	friend std::ostream& operator<< (std::ostream& strm, Action<T>& a){
		a.print(strm);
	    return strm;
	};

protected:
	virtual void print(std::ostream &strm) {strm << this;};

public:

	virtual void execute(T* state)=0;

	virtual size_t hash() {return 0;};

	virtual bool equals(Action<T>* other) { return this==other; };

	virtual ~Action(){};
};

namespace std {

    template<class T>
    class hash<Action<T>*>{
    public:
        size_t operator() (Action<T>* const & a) const {
            return a->hash();
        }
    };

    template<class T>
    class equal_to<Action<T>*>{
    public :
        bool operator() (Action<T>* const & x, Action<T>* const & y) const {return x->equals(y);}
    };
}

template<class T>
class Strategy{
protected:

	T* state;

public:
	Strategy(T* state) : state(state) {}

	virtual ~Strategy(){};
};


template<class T, class A>
class ExpansionStrategy : public Strategy<T> {

public:
	ExpansionStrategy(T* state) : Strategy<T>(state) {}


	virtual A* generateNext()=0;

	virtual bool canGenerateNext()=0;

	virtual ~ExpansionStrategy() override {}
};


template<class T, class A>
class PlayoutStrategy : public Strategy<T>{

public:
	PlayoutStrategy(T* state) : Strategy<T>(state) {}

	virtual void generateRandom(A* action)=0;

	virtual ~PlayoutStrategy() override {}
};


template<class T>
class Backpropagation  {

public:
	Backpropagation() {}

	virtual float updateScore(T* state, float backpropScore)=0;

	virtual ~Backpropagation() {}
};


template<class T>
class TerminationCheck{

public:
	TerminationCheck() {}

	virtual bool isTerminal(T* state)=0;

	virtual ~TerminationCheck(){}
};


template<class T>
class Scoring {

public:
	Scoring() {}

	virtual float score(T* state)=0;

	virtual ~Scoring(){}
};



template<class T, class A, class E>
class Node{
	unsigned int id;
	T* data;
	Node<T,A,E>* parent;
	std::vector<Node<T,A,E>*> children;

	A* action;
	ExpansionStrategy<T,A>* expansion;
	int numVisits;
	float score;

public:
	Node(unsigned int id, T* data, Node<T,A,E>* parent, A* action) : id(id), data(data), parent(parent), action(action), expansion(new E(data)), numVisits(0), score(0) {
	};


	unsigned int getID(){
		return id;
	}

	T* getData(){
		return data;
	}

	Node<T,A,E>* getParent(){
		return parent;
	}

	std::vector<Node<T,A,E>*>& getChildren(){
		return children;
	}

	A* getAction(){
		return action;
	}

	A* generateNextAction(){
		return expansion->generateNext();
	}

	void addChild(Node<T,A,E>* child){
		children.push_back(child);
	}

	bool shouldExpand(){
		bool result=children.empty() || expansion->canGenerateNext();
		return result;
	}

	void update(float score){
		this->score += score;
		numVisits++;
	}

	float getAvgScore(){
		return score/numVisits;
	}

	int getNumVisits(){
		return numVisits;
	}

	~Node(){
		//delete data;
		delete action;
		delete expansion;
		for (Node<T,A,E>* child : children)
			delete child;
	}
};


template<class T, class A, class E, class P>
class MCTS {

	const int DEFAULT_TIME=500;

	const int DEFAULT_MIN_ITERATIONS=0;
	const static int DEFAULT_C=0.5;
	const static int DEFAULT_W=0.0;
	const int DEFAULT_MIN_T=5;

	const int DEFAULT_MIN_VISITS=5;

	Backpropagation<T>* backprop;
	TerminationCheck<T>* termination;
	Scoring<T>* scoring;

	Node<T,A,E>* root;

	std::unordered_map<Action<T>*, std::pair<int, float>> history;

	milliseconds time;

	int minIterations;
	float C;

	float W;

	int minT;

	int minVisits;

	unsigned int currentNodeID;

	microseconds selectTime, expandTime,simulateTime;
	long iterations;

public:
	/**
	 * 
	 */
	MCTS(T* rootData, Backpropagation<T>* backprop, TerminationCheck<T>* termination, Scoring<T>* scoring) :
	    backprop(backprop), termination(termination), scoring(scoring), root(new Node<T,A,E>(0, rootData, 0, new A())),
	        history(), time(milliseconds(DEFAULT_TIME)), minIterations(DEFAULT_MIN_ITERATIONS), C(DEFAULT_C),
	            W(DEFAULT_W), minT(DEFAULT_MIN_T), minVisits(DEFAULT_MIN_VISITS), currentNodeID(0),
	                selectTime(microseconds::zero()), expandTime(microseconds::zero()),
	                    simulateTime(microseconds::zero()), iterations(0) {}

	/**
	 * @brief 运行蒙特卡洛树搜索并返回最优动作
	 *
	 * @return 
	 */
	A* calculateAction(){

		system_clock::time_point old=system_clock::now();

		search();

        #ifdef _DEBUG
		std::cerr << iterations << " iterations in " << duration_cast<milliseconds>(system_clock::now()-old).count() << "ms" << std::endl;
		std::cerr << "Average select:" << (float)(selectTime.count()/1000)/iterations/1000 << "ms Average expand:" << (float)(expandTime.count())/iterations/1000 << "ms Average simulate:" << (float)(simulateTime.count())/iterations/1000 << "ms" << std::endl;
		#endif

		// Select the Action with the best score
		Node<T,A,E>* best=nullptr;
		float bestScore=-std::numeric_limits<float>::max();
		std::vector<Node<T,A,E>*>& children=root->getChildren();

		for (unsigned int i=0; i<children.size();i++){
			float score=children[i]->getAvgScore();
			if (score>bestScore){
				bestScore=score;
				best=children[i];
			}
		}

        for (auto kv : history)
            std::cout << kv.first->hash() << " " << kv.second.first << std::endl;

		return new A(*best->getAction());
	}

	/**
	 * 设置计算时间
	 * 
	 */
	void setTime(int time){
		this->time=milliseconds(time);
	}

	void setC(float C){
		this->C=C;
	}

	void setW(float W){
		this->W=W;
	}

	void setMinT(float minT){
		this->minT = minT;
	}

	void setMinVisits(int minVisits) {
	    this->minVisits = minVisits;
	}

	Node<T,A,E>* getRoot(){
		return root;
	}

	~MCTS(){
		for (auto kv : history)
			delete kv.first;

		delete root;
		delete backprop;
		delete termination;
		delete scoring;
	}
private:
	void search(){
		system_clock::time_point old=system_clock::now();


		#ifdef _DEBUG
		system_clock::time_point temp;
		#endif

		while (duration_cast<milliseconds>(system_clock::now()-old)<time || iterations < minIterations){

			#ifdef _DEBUG
			temp=system_clock::now();
			#endif


			iterations++;

			/**
			 * 选择
			 */
			Node<T,A,E>* selected=root;
			while(!selected->shouldExpand())
				selected=select(selected);

			if (termination->isTerminal(selected->getData())){
				backProp(selected, scoring->score(selected->getData()));
				continue;
			}

			#ifdef _DEBUG
			selectTime+=duration_cast<microseconds>(system_clock::now()-temp);
			temp=system_clock::now();
			#endif

			/**
			 * 扩展
			 */
			Node<T,A,E>* expanded;
			int numVisits=selected->getNumVisits();
			if (numVisits>=minT){
				expanded=expandNext(selected);
			}
			else{
				expanded=selected;
			}

			#ifdef _DEBUG
			expandTime+=duration_cast<microseconds>(system_clock::now()-temp);
			temp=system_clock::now();
			#endif

			/**
			 * 模拟
			 */
			simulate(expanded);

			if(selected->getChildren().size()>4)
			{
				int z = 1;
			}
			#ifdef _DEBUG
			simulateTime+=duration_cast<microseconds>(system_clock::now()-temp);
			#endif
		}
	}

	/** 在所有节点中选择最佳节点 */
	Node<T,A,E>* select(Node<T,A,E>* node){
		Node<T,A,E>* best=nullptr;
		float bestScore=-std::numeric_limits<float>::max();
		
		std::vector<Node<T,A,E>*>& children=node->getChildren();

		//如果当前节点访问次数太小，则随机选择节点
		if (node->getNumVisits()<minVisits)
			return children[rand()%children.size()];
		
		// 使用UCT公式计算每个节点的值
		for (Node<T,A,E>* n : children){


			float score=n->getAvgScore()+C*(float)sqrt(log(node->getNumVisits())/n->getNumVisits());

			#ifdef PROG_HIST
			auto stats=history.find(n->getAction());
			if (stats!=history.end()){
				score+=stats->second.second/stats->second.first*W/((1-n->getAvgScore())*n->getNumVisits()+1);
			}
            #endif


			if (score>bestScore)
			{
				bestScore=score;
				best=n;
			}
		}

		return best;
	}
	/** 获取给定节点的下一个动作，执行并将新节点添加到树中。 */
	Node<T,A,E>* expandNext(Node<T,A,E>* node){
		T* expandedData=new T(*node->getData());
		A* action = node->generateNextAction();
		action->execute(expandedData);
		Node<T,A,E>* newNode=new Node<T,A,E>(++currentNodeID, expandedData,node, action);
		node->addChild(newNode);
		return newNode;
	}
	/** 模拟直到达到停止条件。 */
	void simulate(Node<T,A,E>* node){
		T* state=new T(*node->getData());
		std::vector<Action<T>*> actions;

        A action;
		// 检查游戏是否结束，如果没有，则生成下一个状态
		while (!termination->isTerminal(state))
		{
			PlayoutStrategy<T,A>* playout=new P(state);
			playout->generateRandom(&action);
			action.execute(state);
			#ifdef PROG_HIST
			actions.push_back(new A(action));
            #endif
			delete playout;
		}

        //计算叶子节点得分（游戏结束）
		float s =scoring->score(state);

		#ifdef PROG_HIST
		// Update progressive history statistics
		for (Action<T>* a : actions){
			auto i = history.find(a);
			if (i!=history.end()){
				std::pair<int, float> stats = i->second;
				stats.first++;
				stats.second+=s;
				i->second=stats;

				delete a;
			}
			else{
				history[a]=std::pair<int, float>(1,s);
			}
		}
        #endif

		backProp(node, s);
		delete state;

	}
	/**反向传播树中的分数 */
	void backProp(Node<T,A,E>* node, float score){
		while (node->getParent()!=0){
			node->update(backprop->updateScore(node->getData(),score));
			node=node->getParent();
		}
		node->update(score);
	}
};

#endif
