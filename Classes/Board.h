
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
	int map1[10][9];						// ���̵�ͼ
	Chessman allChessman1[32];				// ��������
	int red_king_id;						// �췽��ID
	int black_king_id;						// �ڷ���ID
	int(*m1)[9];
	Chessman **all1;
	/**
	 * @return The player occupying the given position on the Board
	 */
	//Player position(int x, int y);

	/**
	 * Ϊ��ǰ���ִ���ƶ�����
	 */
	void play(ChessMove chessMove);

	/**
	 * ���ػ�ʤ������ƽ�ֻ�δ����ʱ����NONE
	 */
	Player won();

	/**
	 * ����play���������õĴ���
	 */
	int getTurns();

	/**
	 * ���ص�ǰ�����ж���һ�� RED��BLACK
	 */
	Player getCurrentPlayer();
	bool canMove(int chessid, int toX, int toY);						// �ж�һ�����ӵ���λ�Ƿ�Ϸ�
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

