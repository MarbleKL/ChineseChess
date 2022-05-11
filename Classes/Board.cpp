
#include "Board.h"
#include<cstring>
#include <iostream>

#define WIN_CHECK(a,b,c) {if (board[a]==board[b] && board[b]==board[c] && board[a]!=NONE) return board[a];}


void ChessMove::execute(Board* data){
	//data->play(x,y);
	data->play(*this);
}

Board::Board(int m1[10][9], Chessman* all1[32],int r,int b) : turns(0){
	board = new Player[9];
	for (int i = 0; i < 9; i++) board[i] = NONE;
	current = BLACK;

	red_king_id = r;
	black_king_id = b;
	
	this->m1 = m1;
	this->all1 = all1;
	//flush();

}
//void Board::flush()
//{
//	for (int i = 0; i < 10; i++)
//	{
//		for (int j = 0; j < 9; j++)
//		{
//			map1[i][j] = m1[i][j];
//		}
//	}
//	for (int i = 0; i < 32; i++)
//	{
//		allChessman1[i].m_x = all1[i]->m_x;
//		allChessman1[i].m_y = all1[i]->m_y;
//		allChessman1[i].id = all1[i]->id;
//		allChessman1[i].type = all1[i]->type;
//		allChessman1[i].isLive = all1[i]->isLive;
//	}
//}

Board::Board(const Board& old){
	board = new Player[9];
	memcpy(board, old.board, sizeof(Player)* 9);
	current = old.current;
	turns=old.turns;

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			map1[i][j] = old.map1[i][j];
		}
	}
	for (int i = 0; i < 32; i++)
	{
		allChessman1[i].m_x = old.allChessman1[i].m_x;
		allChessman1[i].m_y = old.allChessman1[i].m_y;
		allChessman1[i].id = old.allChessman1[i].id;
		allChessman1[i].type = old.allChessman1[i].type;
		allChessman1[i].isLive = old.allChessman1[i].isLive;
	}
	red_king_id = old.red_king_id;
	black_king_id = old.black_king_id;

}

void Board::play(ChessMove chessMove){
	//board[y * 3 + x] = this->current;
	current = current == RED ? BLACK :RED;
	turns+=1;
	Chessman* moveChess = &allChessman1[chessMove.id];
	moveChess->m_x = chessMove.to.x;
	moveChess->m_y = chessMove.to.y;
	map1[chessMove.from.x][chessMove.from.y] = NOCHESS;
	map1[chessMove.to.x][chessMove.to.y] = chessMove.type;
	if (chessMove.eaten_id != 0)
	{
		Chessman* killChess = &allChessman1[chessMove.eaten_id];
		killChess->isLive = false;
	}
}

//Player Board::position(int x, int y){
//	return board[y * 3 + x];
//}

Player Board::won(){
	//for (int i = 0; i < 9; i += 3) WIN_CHECK(i, i + 1, i + 2);
	//for (int i = 0; i < 3; i++) WIN_CHECK(i, i + 3, i + 6);
	//WIN_CHECK(0, 4, 8);
	//WIN_CHECK(2, 4, 6);
	if (!allChessman1[red_king_id].isLive)
	{
		return BLACK;
	}
	if (!allChessman1[black_king_id].isLive)
	{
		return RED;
	}

	return NONE;
}


int Board::getTurns(){
	return turns;
}

Player Board::getCurrentPlayer(){
	return current;
}

Board& Board::operator=(const Board& other){
	if (this!=&other){
		memcpy(board, other.board, sizeof(Player)* 9);
		current=other.current;
		turns=other.turns;
	}
	return *this;
}

//void Board::print(std::ostream& strm) {
//	for (int i=0; i<9; i++){
//		if (i%3==0 && i!=0)
//			strm << "\\n";
//		strm << Board::playerToChar(this->board[i]);
//	}
//}

Board::~Board(){
	//delete[] board;
}

bool Board::canMove(int chessid, int toX, int toY)
{
	int fromX = allChessman1[chessid].m_x;
	int fromY = allChessman1[chessid].m_y;
	int moveChessID = map1[fromX][fromY];
	int targetID = map1[toX][toY];

	// ͬһλ�ã��Ƿ�
	if (fromX == toX && fromY == toY)
		return false;
	// ͬɫ���Ƿ�
	if (IsSameSide(moveChessID, targetID))
		return false;

	switch (moveChessID)
	{
	case B_KING:
		// �ھŹ����⣬�Ƿ�
		if (toX < 0 || toX > 2 || toY < 3 || toY > 5)
			return false;
		// ����һ��ֱ��
		if (abs(fromX - toX) + abs(fromY - toY) > 1)
			return false;
		break;
	case R_KING:
		// �ھŹ����⣬�Ƿ�
		if (toX < 7 || toX > 9 || toY < 3 || toY > 5)
			return false;
		// ����һ��ֱ��
		if (abs(fromX - toX) + abs(fromY - toY) > 1)
			return false;
		break;
	case B_BISHOP:
		// �ھŹ����⣬�Ƿ�
		if (toX < 0 || toX > 2 || toY < 3 || toY > 5)
			return false;
		// ����һ��б��
		if (abs(fromX - toX) != 1 || abs(fromY - toY) != 1)
			return false;
		break;
	case R_BISHOP:
		// �ھŹ����⣬�Ƿ�
		if (toX < 7 || toX > 9 || toY < 3 || toY > 5)
			return false;
		// ����һ��б��
		if (abs(fromX - toX) != 1 || abs(fromY - toY) != 1)
			return false;
		break;
	case B_ELEPHANT:
		// ���ܹ���
		if (toX > 4)
			return false;
		// ��������
		if (abs(fromX - toX) != 2 || abs(fromY - toY) != 2)
			return false;
		// ���۱���ס
		if (map1[(fromX + toX) / 2][(fromY + toY) / 2] != NOCHESS)
			return false;
		break;
	case R_ELEPHANT:
		// ���ܹ���
		if (toX < 5)
			return false;
		// ��������
		if (abs(fromX - toX) != 2 || abs(fromY - toY) != 2)
			return false;
		// ���۱���ס
		if (map1[(fromX + toX) / 2][(fromY + toY) / 2] != NOCHESS)
			return false;
		break;
	case B_PAWN:
		// ������ͷ
		if (toX < fromX)
			return false;
		// ������ǰֻ��ֱ��
		if (fromX < 5 && fromX == toX)
			return false;
		// ����һ��ֱ��
		if (abs(fromX - toX) + abs(fromY - toY) > 1)
			return false;
		break;
	case R_PAWN:
		// ������ͷ
		if (toX > fromX)
			return false;
		// ������ǰֻ��ֱ��
		if (fromX > 4 && fromX == toX)
			return false;
		// ����һ��ֱ��
		if (abs(fromX - toX) + abs(fromY - toY) > 1)
			return false;
		break;
	case B_CAR:
	case R_CAR:
		// ����ֱ��
		if (fromX != toX && fromY != toY)
			return false;
		// ֱ���м���������
		if (fromX == toX)
		{
			int t1 = fromY < toY ? fromY : toY;
			int t2 = fromY > toY ? fromY : toY;
			for (int i = t1 + 1; i < t2; ++i)
				if (map1[fromX][i] != NOCHESS)
					return false;
		}
		else
		{
			int t1 = fromX < toX ? fromX : toX;
			int t2 = fromX > toX ? fromX : toX;
			for (int i = t1 + 1; i < t2; ++i)
				if (map1[i][fromY] != NOCHESS)
					return false;
		}
		break;
	case B_HORSE:
	case R_HORSE:
		// ������
		if (!((abs(fromX - toX) == 1 && abs(fromY - toY) == 2) || (abs(fromX - toX) == 2 && abs(fromY - toY) == 1)))
			return false;
		// ������
		int i, j;
		if (fromX - toX == 2)
		{
			i = fromX - 1;
			j = fromY;
		}
		else if (fromX - toX == -2)
		{
			i = fromX + 1;
			j = fromY;
		}
		else if (fromY - toY == 2)
		{
			i = fromX;
			j = fromY - 1;
		}
		else if (fromY - toY == -2)
		{
			i = fromX;
			j = fromY + 1;
		}
		if (map1[i][j] != NOCHESS)
			return false;
		break;
	case B_CANON:
	case R_CANON:
		// ����ֱ��
		if (abs(fromX - toX) != 0 && abs(fromY - toY) != 0)
			return false;
		// �ڲ�����ʱ���м�����
		if (map1[toX][toY] == NOCHESS)
		{
			if (fromX == toX)
			{
				int t1 = fromY < toY ? fromY : toY;
				int t2 = fromY > toY ? fromY : toY;
				for (int i = t1 + 1; i < t2; ++i)
					if (map1[fromX][i] != NOCHESS)
						return false;
			}
			else
			{
				int t1 = fromX < toX ? fromX : toX;
				int t2 = fromX > toX ? fromX : toX;
				for (int i = t1 + 1; i < t2; ++i)
					if (map1[i][fromY] != NOCHESS)
						return false;
			}
		}
		// �ڳ���ʱ���м���һ��
		else
		{
			int count = 0;
			if (fromX == toX)
			{
				int t1 = fromY < toY ? fromY : toY;
				int t2 = fromY > toY ? fromY : toY;
				for (int i = t1 + 1; i < t2; ++i)
					if (map1[fromX][i] != NOCHESS)
						count++;
			}
			else
			{
				int t1 = fromX < toX ? fromX : toX;
				int t2 = fromX > toX ? fromX : toX;
				for (int i = t1 + 1; i < t2; ++i)
					if (map1[i][fromY] != NOCHESS)
						count++;
			}
			if (count != 1)
				return false;
		}
		break;
	default:
		return false;
	}

	// �������ڲ�ͬ�У��ƶ�������ͬ�����м����ӣ����棬�Ƿ�
	if ((moveChessID == B_KING && toY == allChessman1[red_king_id].m_y) || (moveChessID == R_KING && toY == allChessman1[black_king_id].m_y))
	{
		int count = 0;
		for (int i = allChessman1[black_king_id].m_x + 1; i < allChessman1[red_king_id].m_x; ++i)
		{
			if (map1[i][toY] != NOCHESS)
				count++;
		}

		if (count == 0)
			return false;
	}
	// ��������ͬ�У��м�ֻʣһ�ӣ����Ҹ��ӽ�Ҫ�ƿ�����ɽ�������
	if (allChessman1[black_king_id].m_y == allChessman1[red_king_id].m_y)
	{
		int count = 0;
		for (int i = allChessman1[black_king_id].m_x + 1; i < allChessman1[red_king_id].m_x; ++i)
		{
			if (map1[i][allChessman1[black_king_id].m_y] != NOCHESS)
				count++;
		}

		// �м�ֻ�� 1 �����,������ 1 �ӽ�Ҫ�ƿ�����Ƿ�
		if (count == 1 && fromY == allChessman1[black_king_id].m_y && toY != allChessman1[black_king_id].m_y)
			return false;
	}

	//moveChessMove.type = moveChessID;
	//moveChessMove.to.x = toX;
	//moveChessMove.to.y = toY;

	return true;
}