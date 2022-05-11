#pragma once

#ifndef __AI_H__
#define __AI_H__
#include <vector>
#include "GameScene.h"

class AI
{
public:
	//static ChessMove cchessMove;
	static int _maxLevel;

	static ChessMove getChessMove(GameScene* game);
	static int getScore(GameScene* game);

	static std::vector<ChessMove> getAllPossibleMove(GameScene* game, bool is_ai);

	static void fakeMove(GameScene* game, ChessMove ChessMove);
	static void unfakeMove(GameScene* game, ChessMove ChessMove);
	
	static ChessMove getChessMove(GameScene* game, int level);
	static int getMinScore(GameScene* game, int level,int curMinScore);
	static int getMaxScore(GameScene* game, int level, int curMaxScore);


	// alphaBeta 
	static ChessMove getChessMoveByAlphaBetaSearch(GameScene* game, int level);
	static int alphaBetaSearch(GameScene* game, int alpha, int beta, int level, bool side, ChessMove &cchessMove);
	
};
#endif