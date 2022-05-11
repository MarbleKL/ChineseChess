

#ifndef TTTMCTS_H_
#define TTTMCTS_H_

#include "define.h"
#include "mcts.h"
#include "TTTStrategy.h"
#include "Board.h"

typedef MCTS<Board, ChessMove, TTTExpansionStrategy, TTTPlayoutStrategy> TTTMCTS;

class TTTMCTSPlayer {
	Board* board;
	TTTMCTS* mcts;

public:
	TTTMCTSPlayer(Board* board);

	/**
	 * ʹ�����ؿ���������������һ��
	 */
	ChessMove* calculateAction();

	~TTTMCTSPlayer();

private:
	/**
	 * �����µ�MCTSʵ��
	 */
	TTTMCTS* createMCTS();
};

#endif /* TTTMCTS_H_ */
