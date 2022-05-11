
#ifndef BOARD_H
#define BOARD_H

#include "mcts.h"
#include "Chessman.h"
#include "define.h"

enum Player {RED, BLACK, NONE};

class Board : public State {
	Player* board;
	Player current;
	int turns;
public:
	Board(int m1[10][9], Chessman* all1[32],int r,int b);
	//void flush();
	Board(const Board& board);
	int map1[10][9];						// 棋盘地图
	Chessman allChessman1[32];				// 所有棋子
	int red_king_id;						// 红方将ID
	int black_king_id;						// 黑方将ID
	int(*m1)[9];
	Chessman **all1;
	/**
	 * @return The player occupying the given position on the Board
	 */
	//Player position(int x, int y);

	/**
	 * 为当前玩家执行移动操作
	 */
	void play(ChessMove chessMove);

	/**
	 * 返回获胜方，当平局或还未结束时返回NONE
	 */
	Player won();

	/**
	 * 返回play函数被调用的次数
	 */
	int getTurns();

	/**
	 * 返回当前进行行动的一边 RED或BLACK
	 */
	Player getCurrentPlayer();
	bool canMove(int chessid, int toX, int toY);						// 判断一个棋子的走位是否合法
	/**
	 * @return A single character representation of a Player
	 */
	//static char playerToChar(Player p){
	//	switch(p){
	//	case NONE:
	//		return '-';
	//	case CROSS:
	//		return 'x';
	//	case CIRCLE:
	//		return 'o';
	//	}
	//}

	//void print(std::ostream &strm) override;

	Board& operator=(const Board& other);
	~Board();
};


#endif

