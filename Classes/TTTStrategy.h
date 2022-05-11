#ifndef TTTACTIONGENERATOR_H_
#define TTTACTIONGENERATOR_H_

#include "Board.h"
//#include "TTTAction.h"
#include "define.h"
/**
 * 产生可能的动作
 */
class TTTExpansionStrategy : public ExpansionStrategy<Board, ChessMove>{
	int currentX,currentY;
	ChessMove curMove;
public:
	TTTExpansionStrategy(Board* state);

	ChessMove* generateNext();

	bool canGenerateNext(){
		return currentX!=-1 && currentY!=-1;
	};

	~TTTExpansionStrategy() override {}

private:
	/**
	 * 搜索可能的动作
	 */
	void searchNextPossibleMove();

};

class TTTPlayoutStrategy : public PlayoutStrategy<Board, ChessMove>{

public:
	TTTPlayoutStrategy(Board* state);

	void generateRandom(ChessMove* action) override;

	~TTTPlayoutStrategy() override {}

};


#endif /* TTTACTIONGENERATOR_H_ */
