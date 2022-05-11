#include "AI.h"

//enum TYPE{ CHE, MA, PAO, BING, JIANG, SHI, XIANG };

static int g_score[7] =
{
	1500,
	100,
	45,
	40,
	10,
	10,
	20
};


// 计算局面分
int AI::getScore(GameScene* game)
{
	int blackScore = 0;
	int redScore = 0;


	for (int i = 0; i < 16; ++i)
	{
		if (game->allChessman1[i].isLive)
			blackScore += g_score[game->allChessman1[i].type-1];
	}

	for (int i = 16; i < 32; ++i)
	{
		if (game->allChessman1[i].isLive)
			redScore += g_score[game->allChessman1[i].type-8];
	}

	int k = blackScore - redScore;
	if (k < 0)
	{
		int z = 1;
	}
	if (k>0)
	{
		int z = 1;
	}

	return k;
}

std::vector<ChessMove> AI::getAllPossibleMove(GameScene* game,bool is_ai)
{
	std::vector<ChessMove> ret;
	int l, r;
	if (is_ai)
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
		if (game->allChessman1[i].isLive)
		{
			for (int row = 0; row <= 9; ++row)
				for (int col = 0; col <= 8; ++col)
				{
					ChessMove chessMove;
					int killid = game->map1[row][col];
					if (killid == 0)
					{
						chessMove.eaten_id = 0;
					}
					else
					{
						for (int j = 0; j < 32; j++)
						{
							if (game->allChessman1[j].m_x == row&&game->allChessman1[j].m_y == col)
							{
								chessMove.eaten_id = game->allChessman1[j].id;
							}
						}
					}
					if (game->canMove(i, row, col))
					{
						chessMove.id = i;
						chessMove.from.x = game->allChessman1[i].m_x;
						chessMove.from.y = game->allChessman1[i].m_y;
						chessMove.to.x = row;
						chessMove.to.y = col;
						chessMove.type = game->allChessman1[i].type;
						ret.push_back(chessMove);
					}
				}
		}
	}

	return ret;
}

void AI::fakeMove(GameScene* game, ChessMove chessMove)
{
	Chessman* moveChess = &game->allChessman1[chessMove.id];
	moveChess->m_x = chessMove.to.x;
	moveChess->m_y = chessMove.to.y;
	game->map1[chessMove.from.x][chessMove.from.y] = NOCHESS;
	game->map1[chessMove.to.x][chessMove.to.y] = chessMove.type;
	if (chessMove.eaten_id != 0)
	{
		Chessman* killChess = &game->allChessman1[chessMove.eaten_id];
		killChess->isLive = false;
	}
}

void AI::unfakeMove(GameScene* game, ChessMove chessMove)
{

	Chessman* moveChess = &game->allChessman1[chessMove.id];
	moveChess->m_x = chessMove.from.x;
	moveChess->m_y = chessMove.from.y;
	if (chessMove.from.y < 0)
	{
		int z = 1;
	}
	game->map1[chessMove.from.x][chessMove.from.y] = chessMove.type;
	if (chessMove.eaten_id != 0)
	{
		game->map1[chessMove.to.x][chessMove.to.y] = game->allChessman1[chessMove.eaten_id].type;
		game->allChessman1[chessMove.eaten_id].isLive = true;
	}
	else
	{
		game->map1[chessMove.to.x][chessMove.to.y] = NOCHESS;
	}
}

//ChessMove AI::getChessMove(GameScene* game)
//{
//	int highScore = -3000;
//	ChessMove* ret = nullptr;
//
//	
//	std::vector<ChessMove*> allMove = getAllPossibleMove(game,true);
//	std::vector<ChessMove*>::iterator it;
//	for (it = allMove.begin(); it != allMove.end(); ++it)
//	{
//		ChessMove* ChessMove = *it;
//		fakeMove(game, ChessMove);
//		int score = getScore(game);
//		unfakeMove(game, ChessMove);
//		if (score > highScore)
//		{
//			highScore = score;
//			ret = ChessMove;
//		}
//	}
//
//	for (it = allMove.begin(); it != allMove.end(); ++it)
//	{
//		ChessMove* ChessMove = *it;
//		if (ChessMove != ret) delete ChessMove;
//	}
//
//	return ret;
//}


//蒙特卡洛树搜索
ChessMove AI::getChessMove(GameScene* game, int level)
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			game->map1[i][j] = game->map[i][j];
		}
	}
	for (int i = 0; i < 32; i++)
	{
		game->allChessman1[i].m_x = game->allChessman[i]->m_x;
		game->allChessman1[i].m_y = game->allChessman[i]->m_y;
		game->allChessman1[i].id = game->allChessman[i]->id;
		game->allChessman1[i].type = game->allChessman[i]->type;
		game->allChessman1[i].isLive = game->allChessman[i]->isLive;
	}

	int highScore = -3000;
	ChessMove ret;

	
	std::vector<ChessMove> allMove = getAllPossibleMove(game,true);
	std::vector<ChessMove>::iterator it;
	for (it = allMove.begin(); it != allMove.end(); ++it)
	{
		ChessMove ChessMove = *it;
		fakeMove(game, ChessMove);
		int score = getMinScore(game, level - 1,highScore);
		unfakeMove(game, ChessMove);
		if (score > highScore)
		{
			highScore = score;
			ret = ChessMove;
		}
	}

	//for (it = allMove.begin(); it != allMove.end(); ++it)
	//{
	//	ChessMove ChessMove = *it;
	//	if (ChessMove != ret) delete ChessMove;
	//}
	allMove.clear();
	return ret;
}

int AI::getMinScore(GameScene* game, int level,int curMinScore)
{
	if (level == 0)
		return getScore(game);

	int minScore = 3000;

	std::vector<ChessMove> allMove = getAllPossibleMove(game,false);
	std::vector<ChessMove>::iterator it;
	for (it = allMove.begin(); it != allMove.end(); ++it)
	{
		ChessMove ChessMove = *it;
		fakeMove(game, ChessMove);
		int score = getMaxScore(game, level - 1,minScore);
		unfakeMove(game, ChessMove);

		if (score <= curMinScore)
		{
			minScore = score;
			break;
		}

		if (score < minScore)
		{
			minScore = score;
		}
	}

	//for (it = allMove.begin(); it != allMove.end(); ++it)
	//{
	//	ChessMove ChessMove = *it;
	//	delete ChessMove;
	//}
	allMove.clear();
	return minScore;
}

int AI::getMaxScore(GameScene* game, int level, int curMaxScore)
{
	if (level == 0)
		return getScore(game);

	int maxScore = -3000;

	std::vector<ChessMove> allMove = getAllPossibleMove(game,true);
	std::vector<ChessMove>::iterator it;
	for (it = allMove.begin(); it != allMove.end(); ++it)
	{
		ChessMove ChessMove = *it;
		fakeMove(game, ChessMove);
		int score = getMinScore(game, level - 1,maxScore);
		unfakeMove(game, ChessMove);

		if (score >= curMaxScore)
		{
			maxScore = score;
			break;
		}
		if (score > maxScore)
		{
			maxScore = score;
		}
	}

	//for (it = allMove.begin(); it != allMove.end(); ++it)
	//{
	//	ChessMove ChessMove = *it;
	//	delete ChessMove;
	//}
	allMove.clear();
	return maxScore;
}


int AI::_maxLevel = 0;


ChessMove AI::getChessMoveByAlphaBetaSearch(GameScene* game, int level)
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			game->map1[i][j] = game->map[i][j];
		}
	}
	for (int i = 0; i < 32; i++)
	{
		game->allChessman1[i].m_x = game->allChessman[i]->m_x;
		game->allChessman1[i].m_y = game->allChessman[i]->m_y;
		game->allChessman1[i].id = game->allChessman[i]->id;
		game->allChessman1[i].type = game->allChessman[i]->type;
		game->allChessman1[i].isLive = game->allChessman[i]->isLive;
	}
	_maxLevel = level;

	ChessMove cchessMove;
	alphaBetaSearch(game, -3000, 3000, level, false, cchessMove);

	return cchessMove;
}

int AI::alphaBetaSearch(GameScene* game, int alpha, int beta, int level, bool side, ChessMove &cchessMove)
{
	if (level == 0)//递归终点
		return getScore(game);

	ChessMove recordChessMove;
	recordChessMove.type = 0;
	std::vector<ChessMove> allMove = getAllPossibleMove(game,!side);
	std::vector<ChessMove>::reverse_iterator iter;
	for (iter = allMove.rbegin(); iter != allMove.rend(); ++iter)//遍历所有可走的步骤
	{
		ChessMove chessMove = *iter;
		fakeMove(game, chessMove);
		int score = -alphaBetaSearch(game, -beta, -alpha, level - 1, !side, cchessMove);//递归
		unfakeMove(game, chessMove);
	
		if (score >= beta)
		{
			alpha = score;
			break;
		}

		if (score > alpha)
		{
			alpha = score;
			
			if (level == _maxLevel)
			{
				recordChessMove = chessMove;
			}
		}
	}

	//for (iter = allMove.begin(); iter != allMove.end(); ++iter)//清空内存，防止内存泄漏
	//{
	//	ChessMove* ChessMove = *iter;
	//	if (recordChessMove != ChessMove) delete ChessMove;
	//}

	//if (recordChessMove)
	//{
	//	if (_ChessMove) delete _ChessMove;
	//	_ChessMove = recordChessMove;
	//}
	if (recordChessMove.type!=0)
	{
		cchessMove = recordChessMove;
	}
	allMove.clear();
	return alpha;
}