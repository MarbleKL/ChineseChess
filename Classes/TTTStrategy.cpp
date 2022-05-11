#include "TTTStrategy.h"

TTTExpansionStrategy::TTTExpansionStrategy(Board* state) :
ExpansionStrategy<Board, ChessMove>(state), currentX(0), currentY(0) {
	searchNextPossibleMove();
}

ChessMove* TTTExpansionStrategy::generateNext() {
	ChessMove* a = new ChessMove;
	*a = curMove;
	//currentY++;
	searchNextPossibleMove();
	return a;
}

void TTTExpansionStrategy::searchNextPossibleMove() {

	//std::vector<ChessMove> ret;
	int l, r;
	if (state->getCurrentPlayer()==BLACK)
	{
		l = 0;
		r = 16;
	}
	else
	{
		l = 16;
		r = 32;
	}
	for (int i = l; i < r; ++i)
	{
		if (state->allChessman1[i].isLive)
		{
			for (int row = 0; row <= 9; ++row)
				for (int col = 0; col <= 8; ++col)
				{
					ChessMove chessMove;
					int killid = state->map1[row][col];
					if (killid == 0)
					{
						chessMove.eaten_id = 0;
					}
					else
					{
						for (int j = 0; j < 32; j++)
						{
							if (state->allChessman1[j].m_x == row&&state->allChessman1[j].m_y == col)
							{
								chessMove.eaten_id = state->allChessman1[j].id;
							}
						}
					}
					if (state->canMove(i, row, col))
					{
						chessMove.id = i;
						chessMove.from.x = state->allChessman1[i].m_x;
						chessMove.from.y = state->allChessman1[i].m_y;
						chessMove.to.x = row;
						chessMove.to.y = col;
						chessMove.type = state->allChessman1[i].type;
						curMove = chessMove;
						return;
					}
				}
		}
	}

	//return ret;
}


TTTPlayoutStrategy::TTTPlayoutStrategy(Board* state) : PlayoutStrategy<Board, ChessMove>(state) {}

void TTTPlayoutStrategy::generateRandom(ChessMove* action) {
	//int x = rand() % 3, y = rand() % 3;

	// search the Board until an empty square is found
	//while (state->position(x, y) != NONE) {
	//	x = rand() % 3, y = rand() % 3;
	//}
	//action->setX(x);
	//action->setY(y);
	std::vector<ChessMove> ret;
	int l, r;
	if (state->getCurrentPlayer() == BLACK)
	{
		l = 0;
		r = 16;
	}
	else
	{
		l = 16;
		r = 32;
	}
	for (int i = l; i < r; ++i)
	{
		if (state->allChessman1[i].isLive)
		{
			for (int row = 0; row <= 9; ++row)
				for (int col = 0; col <= 8; ++col)
				{
					ChessMove chessMove;
					int killid = state->map1[row][col];
					if (killid == 0)
					{
						chessMove.eaten_id = 0;
					}
					else
					{
						for (int j = 0; j < 32; j++)
						{
							if (state->allChessman1[j].m_x == row&&state->allChessman1[j].m_y == col)
							{
								chessMove.eaten_id = state->allChessman1[j].id;
							}
						}
					}
					if (state->canMove(i, row, col))
					{
						chessMove.id = i;
						chessMove.from.x = state->allChessman1[i].m_x;
						chessMove.from.y = state->allChessman1[i].m_y;
						chessMove.to.x = row;
						chessMove.to.y = col;
						chessMove.type = state->allChessman1[i].type;
						ret.push_back(chessMove);
					}
				}
		}
	}
	if (ret.size() != 0)
	{
		int x = rand() % ret.size();
		*action = ret[x];
	}
}
