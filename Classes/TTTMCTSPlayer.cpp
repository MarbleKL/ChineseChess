
#include "TTTMCTSPlayer.h"

class TTTBackpropagation : public Backpropagation<Board> {
	Player player;

public:
	TTTBackpropagation(Player player) : player(player) {}

	float updateScore(Board* state, float backpropScore) {
		//当当前玩家是我们的玩家时，敌人已执行了其移动，并且得分应为倒数
		return state->getCurrentPlayer() == player ? 1-backpropScore : backpropScore;
	}

	~TTTBackpropagation() override {}
};

class TTTTerminationCheck : public TerminationCheck<Board>{
	/**
	 * 当游戏获胜或玩家平局时返回True
	 */
	bool isTerminal(Board* state) override {
		//return state->won()!=NONE || state->getTurns()==9;
		if (!state->allChessman1[state->red_king_id].isLive || !state->allChessman1[state->black_king_id].isLive)
			return true;

		return false;
	}
};

class TTTScoring : public Scoring<Board>{

	Player player;

public:

	TTTScoring(Player player) : player(player) {}

	/**
	 * 返回给定局分配的分数
	 */
	float score(Board* state) override {
		if (state->won()==player)
			return 1;
		else if (state->won()!=NONE)
			return 0;
		else
			return 0.75;
	}

	~TTTScoring() override {}
};

TTTMCTSPlayer::TTTMCTSPlayer(Board* board) : board(board), mcts() {}

ChessMove* TTTMCTSPlayer::calculateAction(){
	//board->flush();
	mcts = createMCTS();
	ChessMove* a = mcts->calculateAction();
	delete mcts;
	return a;
}

TTTMCTS* TTTMCTSPlayer::createMCTS(){
	return new TTTMCTS(new Board(*board), new TTTBackpropagation(board->getCurrentPlayer()), new TTTTerminationCheck(), new TTTScoring(board->getCurrentPlayer()));
}

TTTMCTSPlayer::~TTTMCTSPlayer() {}

