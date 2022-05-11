

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
	 * 使用蒙特卡洛树搜索计算下一步
	 */
	ChessMove* calculateAction();

	~TTTMCTSPlayer();

private:
	/**
	 * 创建新的MCTS实例
	 */
	TTTMCTS* createMCTS();
};

#endif /* TTTMCTS_H_ */
