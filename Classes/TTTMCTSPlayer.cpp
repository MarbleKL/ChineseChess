
#include "TTTMCTSPlayer.h"

class TTTBackpropagation : public Backpropagation<Board> {
	Player player;

public:
	TTTBackpropagation(Player player) : player(player) {}

	float updateScore(Board* state, float backpropScore) {
		//����ǰ��������ǵ����ʱ��������ִ�������ƶ������ҵ÷�ӦΪ����
		return state->getCurrentPlayer() == player ? 1-backpropScore : backpropScore;
	}

	~TTTBackpropagation() override {}
};

class TTTTerminationCheck : public TerminationCheck<Board>{
	/**
	 * ����Ϸ��ʤ�����ƽ��ʱ����True
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
	 * ���ظ����ַ���ķ���
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

