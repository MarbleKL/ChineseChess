#include "GameScene.h"
#include "SettingScene.h"
#include "MainScene.h"
#include <thread>
#include "AI.h"


Scene* GameScene::createScene(bool ai,bool normal)
{
    auto scene = Scene::create();
    
    auto layer = GameScene::create();
	layer->is_ai = ai;
	layer->is_normal = normal;
	layer->gameStart(ai,normal);
	layer->sim_board = new Board(layer->map, layer->allChessman,27,4);
	layer->mcts_ai = new TTTMCTSPlayer(layer->sim_board);
    scene->addChild(layer);
    return scene;
}

bool GameScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
	initUI();
	//gameStart(false);

    return true;
}

// ��ʼ������
void GameScene::initUI()
{
	size = Director::getInstance()->getWinSize();

	// ����
	board = Node::create();
	board->setPosition(Vec2(size.width / 2, size.height / 2));
	this->addChild(board);

	auto boardSprite = Sprite::createWithSpriteFrameName("background.png");
	boardSprite->setPosition(Vec2(size.width / 2, size.height / 2));
	boardSprite->setScale(0.5);
	this->addChild(boardSprite, -1);

	// ��ʼ������
	red_king_id = 27;
	black_king_id = 4;
	is_selected = false;
	is_wait = true;
	is_resigned = 0;
	is_gameStart = false;
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 9; j++)
		{
			map[i][j] = InitChessBoard[i][j];
		}

	// ��ʾ����ͼƬ1
	curChessman[0] = Sprite::createWithSpriteFrameName("1.png");
	curChessman[0]->setPosition(Vec2(size.width / 2, size.height * 7 / 8));
	curChessman[0]->setVisible(false);
	this->addChild(curChessman[0]);
	// ��ʾ����ͼƬ2
	curChessman[1] = Sprite::createWithSpriteFrameName("8.png");
	curChessman[1]->setPosition(Vec2(size.width / 2, size.height * 7 / 8));
	curChessman[1]->setVisible(false);
	this->addChild(curChessman[1]);

	// ѡ��ͼƬ1
	selected[0] = Sprite::createWithSpriteFrameName("selected1.png");
	selected[0]->setVisible(false);
	board->addChild(selected[0]);
	// ѡ��ͼƬ2
	selected[1] = Sprite::createWithSpriteFrameName("selected2.png");
	selected[1]->setVisible(false);
	board->addChild(selected[1]);

	// ������Ϣ
	status = Label::createWithSystemFont("", "Arial", 30);
	status->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
	status->setPosition(Vec2(size.width / 2, size.height));
	this->addChild(status);

	// �����Ϣ
	status2 = Label::createWithSystemFont("", "Arial", 30);
	status2->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
	status2->setPosition(Vec2(size.width / 2, size.height * 19 / 20));
	this->addChild(status2);

	// //��ť
	//auto menuItemReStart = MenuItemFont::create("Restart", CC_CALLBACK_1(GameScene::menuRestartCallBack, this));
	//menuItemReStart->setColor(Color3B::WHITE);
	//auto menuItemResign = MenuItemFont::create("Resign", CC_CALLBACK_1(GameScene::menuResignCallBack, this));
	//menuItemResign->setColor(Color3B::WHITE);

	//auto menu = Menu::create(menuItemReStart, menuItemResign, NULL);
	//menu->setPosition(Vec2::ZERO);
	//menuItemReStart->setPosition(size.width / 4, size.height / 8);
	//menuItemResign->setPosition(size.width * 3 / 4, size.height / 8);
	//this->addChild(menu);
}

// ��ʼ������
void GameScene::initListener()
{
	// ��������
	listener = EventListenerTouchOneByOne::create();

	listener->onTouchBegan = [&](Touch * t, Event * e) {

		if (is_wait)
			return false;

		auto p = board->convertTouchToNodeSpace(t);
		int x = (p.x + BOARD_WIDTH / 2 + GRID_WIDTH / 2) / GRID_WIDTH;
		int y = (p.y + BOARD_HEIGHT / 2 + GRID_WIDTH / 2) / GRID_WIDTH;

		// �������
		if (isClickOnChessman(p.x, p.y, x, y))
		{
			// �����ѡ���һ�����ӣ����ҵ�����ǶԷ����ӣ������ǺϷ��߲��������
			if (is_selected && isRedOrBlack(map[9 - y][x], color) && isValidMove(9 - y, x))
			{
				captureChessman();
				unSelectChessman(0);
				selectChessman(1);
				is_wait = true;
				setCurChessman(is_wait);
				msg.type = MSG_CAPTURE;
				msg.move = moveChessMove;
				send();
			}
			// ���������Ǽ������ӣ���ѡ������
			else if (isRedOrBlack(map[9 - y][x], !color))
			{
				moveChessMove.from.x = allChessman[moveChessMove.id]->m_x;
				moveChessMove.from.y = allChessman[moveChessMove.id]->m_y;
				unSelectChessman(1);
				selectChessman(0);
			}
		}
		// ����ڿհ״�
		else if (is_selected && isValidMove(9 - y, x))
		{
			moveChessman();
			unSelectChessman(0);
			selectChessman(1);
			is_wait = true;
			setCurChessman(is_wait);
			msg.type = MSG_MOVE;
			msg.move = moveChessMove;
			send();
		}

		if (isGameOver())
			gameOver();

		return true;
	};

	// ��Ӵ�������
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

// ��ʼ������
void GameScene::initListenerAI()
{
	// ��������
	listener = EventListenerTouchOneByOne::create();

	listener->onTouchBegan = [&](Touch * t, Event * e) {

		if (is_wait)
			return false;

		auto p = board->convertTouchToNodeSpace(t);
		int x = (p.x + BOARD_WIDTH / 2 + GRID_WIDTH / 2) / GRID_WIDTH;
		int y = (p.y + BOARD_HEIGHT / 2 + GRID_WIDTH / 2) / GRID_WIDTH;

		// �������
		if (isClickOnChessman(p.x, p.y, x, y))
		{
			// �����ѡ���һ�����ӣ����ҵ�����ǶԷ����ӣ������ǺϷ��߲��������
			if (is_selected && isRedOrBlack(map[9 - y][x], color) && isValidMove(9 - y, x))
			{
				captureChessman();
				unSelectChessman(0);
				selectChessman(1);
				is_wait = true;
				setCurChessman(is_wait);
				//msg.type = MSG_CAPTURE;
				//msg.move = moveChessMove;
				//send();
			}
			// ���������Ǽ������ӣ���ѡ������
			else if (isRedOrBlack(map[9 - y][x], !color))
			{
				moveChessMove.from.x = allChessman[moveChessMove.id]->m_x;
				moveChessMove.from.y = allChessman[moveChessMove.id]->m_y;
				unSelectChessman(1);
				selectChessman(0);
			}
		}
		// ����ڿհ״�
		else if (is_selected && isValidMove(9 - y, x))
		{
			moveChessman();
			unSelectChessman(0);
			selectChessman(1);
			is_wait = true;
			setCurChessman(is_wait);
			//msg.type = MSG_MOVE;
			//msg.move = moveChessMove;
			//send();
		}

		if (isGameOver())
			gameOver();

		return true;
	};

	// ��Ӵ�������
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

// ��ʼ������
void GameScene::initChessman()
{
	// ��ʼ������
	int id = 0;
	if (color == 1)
	{
		for (int i = 0; i < 10; ++i)
			for (int j = 0; j < 9; ++j)
				if (map[i][j] != NOCHESS)
				{
					newChessman(i, j, id++);
				}
	}
	else
	{
		board->setRotation(180);

		for (int i = 0; i < 10; ++i)
			for (int j = 0; j < 9; ++j)
				if (map[i][j] != NOCHESS)
				{
					newChessman(i, j, id++, 180);
				}
	}
	
	// ��ǰ���ӷ�ͼƬ
	curChessman[1]->setVisible(true);
	if (is_ai)
	{
		initListenerAI();
	}
	else
	{
		initListener();
	}
	scheduleUpdate();
}

// ��������
void GameScene::newChessman(int x, int y, int id, int angle)
{
	auto pSprite = Chessman::create(InitChessBoard[x][y]);	
	pSprite->id = id;
	pSprite->setPositionRC(x, y);
	pSprite->setRotation(angle);
	board->addChild(pSprite);
	allChessman[id] = pSprite;
}

// �ƶ�����
void GameScene::moveChessman()
{
	float d = sqrt(pow(moveChessMove.from.x - moveChessMove.to.x, 2) + pow(moveChessMove.from.y - moveChessMove.to.y, 2));

	allChessman[moveChessMove.id]->runAction(MoveBy::create(d / 8.0, GRID_WIDTH * Vec2(moveChessMove.to.y - moveChessMove.from.y, moveChessMove.from.x - moveChessMove.to.x)));
	allChessman[moveChessMove.id]->setPos(moveChessMove.to.x, moveChessMove.to.y);
	map[moveChessMove.from.x][moveChessMove.from.y] = NOCHESS;
	map[moveChessMove.to.x][moveChessMove.to.y] = moveChessMove.type;
}

// ����
void GameScene::captureChessman()
{
	float d = sqrt(pow(moveChessMove.from.x - moveChessMove.to.x, 2) + pow(moveChessMove.from.y - moveChessMove.to.y, 2));
	//removeFromParent()
	allChessman[moveChessMove.id]->runAction(
		Sequence::create(MoveBy::create(d / 8.0, GRID_WIDTH * Vec2(moveChessMove.to.y - moveChessMove.from.y, moveChessMove.from.x - moveChessMove.to.x))
		//, CCCallFunc::create([&] {allChessman[moveChessMove.eaten_id]->setScale(0); })
		,NULL));
	//allChessman[moveChessMove.eaten_id]->removeFromParent();
	allChessman[moveChessMove.eaten_id]->setScale(0);
	allChessman[moveChessMove.id]->setPos(moveChessMove.to.x, moveChessMove.to.y);
	allChessman[moveChessMove.eaten_id]->isLive = false;
	map[moveChessMove.to.x][moveChessMove.to.y] = moveChessMove.type;
	map[moveChessMove.from.x][moveChessMove.from.y] = NOCHESS;
}

// �ж�һ�������Ǻ�ɫ���Ǻ�ɫ, 0�жϺ�ɫ, 1�жϺ�ɫ
bool GameScene::isRedOrBlack(int x, int type)
{
	if (type && IsBlack(x))
		return true;
	else if (!type && IsRed(x))
		return true;

	return false;
}

// �ж�һ�����ӵ���λ�Ƿ�Ϸ�
bool GameScene::isValidMove(int toX, int toY)
{
	int fromX = allChessman[moveChessMove.id]->m_x;
	int fromY = allChessman[moveChessMove.id]->m_y;
	int moveChessID = map[fromX][fromY];
	int targetID = map[toX][toY];

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
		if (map[(fromX + toX) / 2][(fromY + toY) / 2] != NOCHESS)
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
		if (map[(fromX + toX) / 2][(fromY + toY) / 2] != NOCHESS)
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
				if (map[fromX][i] != NOCHESS)
					return false;
		}
		else
		{
			int t1 = fromX < toX ? fromX : toX;
			int t2 = fromX > toX ? fromX : toX;
			for (int i = t1 + 1; i < t2; ++i)
				if (map[i][fromY] != NOCHESS)
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
		else if(fromX - toX == -2)
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
		if (map[i][j] != NOCHESS)
			return false;
		break;
	case B_CANON:
	case R_CANON:
		// ����ֱ��
		if (abs(fromX - toX) != 0 && abs(fromY - toY) != 0)
			return false;
		// �ڲ�����ʱ���м�����
		if (map[toX][toY] == NOCHESS)
		{
			if (fromX == toX)
			{
				int t1 = fromY < toY ? fromY : toY;
				int t2 = fromY > toY ? fromY : toY;
				for (int i = t1 + 1; i < t2; ++i)
					if (map[fromX][i] != NOCHESS)
						return false;
			}
			else
			{
				int t1 = fromX < toX ? fromX : toX;
				int t2 = fromX > toX ? fromX : toX;
				for (int i = t1 + 1; i < t2; ++i)
					if (map[i][fromY] != NOCHESS)
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
					if (map[fromX][i] != NOCHESS)
						count++;
			}
			else
			{
				int t1 = fromX < toX ? fromX : toX;
				int t2 = fromX > toX ? fromX : toX;
				for (int i = t1 + 1; i < t2; ++i)
					if (map[i][fromY] != NOCHESS)
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
	if ((moveChessID == B_KING && toY == allChessman[red_king_id]->m_y) || (moveChessID == R_KING && toY == allChessman[black_king_id]->m_y))
	{
		int count = 0;
		for (int i = allChessman[black_king_id]->m_x + 1; i < allChessman[red_king_id]->m_x; ++i)
		{
			if (map[i][toY] != NOCHESS)
				count++;
		}

		if (count == 0)
			return false;
	}
	// ��������ͬ�У��м�ֻʣһ�ӣ����Ҹ��ӽ�Ҫ�ƿ�����ɽ�������
	if (allChessman[black_king_id]->m_y == allChessman[red_king_id]->m_y)
	{
		int count = 0;
		for (int i = allChessman[black_king_id]->m_x + 1; i < allChessman[red_king_id]->m_x; ++i)
		{
			if (map[i][allChessman[black_king_id]->m_y] != NOCHESS)
				count++;
		}

		// �м�ֻ�� 1 �����,������ 1 �ӽ�Ҫ�ƿ�����Ƿ�
		if (count == 1 && fromY == allChessman[black_king_id]->m_y && toY != allChessman[black_king_id]->m_y)
			return false;
	}

	moveChessMove.type = moveChessID;
	moveChessMove.to.x = toX;
	moveChessMove.to.y = toY;

	return true;
}

bool GameScene::canMove(int chessid,int toX, int toY)
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


// �Ƿ�����������
bool GameScene::isClickOnChessman(int p_x, int p_y, int x, int y)
{
	for (int i = 0; i < 32; ++i)
	{
		//�жϵ��λ���Ƿ���Բ��
		int r = sqrt(pow(abs(p_x - allChessman[i]->p_x), 2) + pow(abs(p_y - allChessman[i]->p_y), 2));
		if (allChessman[i]->isLive && r <= GRID_WIDTH / 2)
		{
			if (!is_selected || isRedOrBlack(map[9 - y][x], !color))
				moveChessMove.id = allChessman[i]->id;
			else
				moveChessMove.eaten_id = allChessman[i]->id;
			return true;
		}
	}

	return false;
}

// ������ʾͼƬ
void GameScene::setCurChessman(int type)
{
	if (type)
	{
		curChessman[!color]->setVisible(true);
		curChessman[color]->setVisible(false);
	}
	else
	{
		curChessman[color]->setVisible(true);
		curChessman[!color]->setVisible(false);
	}
}

// ѡ������
void GameScene::selectChessman(int type)
{
	selected[type]->setPosition(Vec2(allChessman[moveChessMove.id]->p_x, allChessman[moveChessMove.id]->p_y));
	selected[type]->setVisible(true);
	if(type == 0)
		is_selected = true;
}

// ȡ��ѡ������
void GameScene::unSelectChessman(int type)
{
	selected[type]->setVisible(false);
	if (type == 0)
		is_selected = false;
}

// ���½��溯��
void GameScene::update(float t)
{
	if (is_ai > 1)
	{
		bool is_ai = UserDefault::getInstance()->getBoolForKey("IsAI");
		int z = 1;
	}
	if (map[6][2] == 14 && map[2][1]==0)
	{
		int z = 1;
	}

	if (messageQueue.size() == 0)
		return;

	messageQueueMutex.lock();
	if (messageQueue.size() == 0)
	{
		messageQueueMutex.unlock();
		return;
	}

	if (messageQueue.size() != 0)
	{
		int z = 1;
	}
	Message temp1 = *(*(messageQueue.begin()));
	Message *temp = &temp1;
	messageQueue.pop_front();

	switch (temp->type)
	{
	case MSG_MOVE:
		this->moveChessMove = temp->move;
		moveChessman();
		unSelectChessman(0);
		selectChessman(1);
		is_wait = false;
		break;
	case MSG_CAPTURE:
		this->moveChessMove = temp->move;
		captureChessman();
		unSelectChessman(0);
		selectChessman(1);
		is_wait = false;
		break;
	case MSG_RESIGN:
		status2->setString("the other side has resigned.");
		is_resigned = 1;
		break;
	default:
		break;
	}

	//CC_SAFE_DELETE(temp);
	//delete temp;
	messageQueueMutex.unlock();

	setCurChessman(is_wait);

	if (isGameOver())
		gameOver();
}

// ������Ϣ
void GameScene::accept()
{
	sockClient = new ODSocket();
	char *address = new char[20];
	int ret = 0;

	ret = sockServer->Accept(*sockClient, address);

	if (!ret)
		log("accept error");
	else
	{
		log("%s is coming", address);
		color = 1;
		is_wait = false;

		// ���������߳�
		thread t_recv(&GameScene::receive, this);
		t_recv.detach();

		initChessman();

		is_gameStart = true;
	}
}

// ������Ϣ
void GameScene::send()
{
	int ret = 0;
	ret = sockClient->Send((char *)&msg, sizeof(Message));
	if (ret <= 0)
		log("send error");
	else
		log("send %d", msg.type);
}

// ������Ϣ
void GameScene::receive()
{
	char buf[1024];
	int ret = 0;

	while (1)
	{
		ret = sockClient->Recv(buf, sizeof(buf));
		if (ret < 0)
		{
			log("recv error");
			break;
		}
		else
		{
			messageQueueMutex.lock();
			Message * temp = (Message*)malloc(sizeof(Message));
			temp = (Message*)buf;
			messageQueue.push_back(temp);
			log("recv %d", temp->type);
			messageQueueMutex.unlock();
		}
	}

	status->setString("the other side is offline.");
	disableGame();
	is_gameStart = false;
}

void GameScene::commove()
{
	char buf[1024];
	int ret = 0;
	//ret = sockClient->Recv(buf, sizeof(buf));
	while (is_gameStart)
	{
		if (is_wait==1)
		{
			messageQueueMutex.lock();
			Message * temp = (Message*)malloc(sizeof(Message));
			ChessMove ptr;
			if (is_normal)
			{
				ptr = AI::getChessMove(this, 4);
			}
			else
			{
				ptr = AI::getChessMoveByAlphaBetaSearch(this, 4);
			}

			temp->move = ptr;
			if (temp->move.eaten_id == 0)
			{
				temp->type = MSG_MOVE;
			}
			else
			{
				temp->type = MSG_CAPTURE;
			}
			messageQueue.push_back(temp);
			log("recv %d", temp->type);
			messageQueueMutex.unlock();
			is_wait = false;
		}
	}

	//status->setString("the other side is offline.");
	disableGame();
	is_gameStart = false;
}

////ptr = *mcts_ai->calculateAction();
// ����message
void GameScene::cleanMessage()
{
	// �����Ϣ����
	messageQueueMutex.lock();
	for (auto i : messageQueue)
	{
		CC_SAFE_DELETE(i);
	}
	messageQueueMutex.unlock();
}

// ��Ϸ��ʼ
void GameScene::gameStart(bool ai,bool normal)
{
	//bool is_ai = UserDefault::getInstance()->getBoolForKey("IsAI");
	is_ai = ai;
	if (is_ai)
	{
		bool is_normal = normal;

		color = 1;
		is_wait = false;
		initChessman();
		is_gameStart = true;
		// ���������߳�
		thread t_recv(&GameScene::commove, this);
		t_recv.detach();

		

	}
	else
	{
		bool is_host = UserDefault::getInstance()->getBoolForKey("IsHost");
		string ip = UserDefault::getInstance()->getStringForKey("IP");
		string port = UserDefault::getInstance()->getStringForKey("Port");
		int ret = 0;

		// ����������׼��
		if (is_host)
		{
			sockServer = new ODSocket();
			ret = sockServer->Init();
			if (ret < 0)
			{
				log("init error");
				return;
			}
			ret = sockServer->Create(AF_INET, SOCK_STREAM, 0);
			if (!ret)
			{
				log("create error");
				return;
			}
			ret = sockServer->Bind(atoi(port.c_str()));
			if (!ret)
			{
				log("bind error");
				return;
			}
			ret = sockServer->Listen(1);
			if (!ret)
			{
				log("listen error");
				return;
			}
			thread t_accept(&GameScene::accept, this);
			t_accept.detach();
			log("server start");
		}
		// �ͻ�������׼��
		else
		{
			sockClient = new ODSocket();
			ret = sockClient->Init();
			if (ret < 0)
			{
				log("init error");
				return;
			}
			ret = sockClient->Create(AF_INET, SOCK_STREAM, 0);
			if (!ret)
			{
				log("create error");
				return;
			}
			ret = sockClient->Connect(ip, atoi(port.c_str()));
			if (!ret)
			{
				status->setString("the server does not open,please restart.");
				return;
			}

			log("client start");
			color = 0;

			// ���������߳�
			thread t_recv(&GameScene::receive, this);
			t_recv.detach();

			initChessman();

			is_gameStart = true;
		}
	}

	
}

// ��Ϸ����
void GameScene::gameOver()
{
	is_gameStart = false;
	showGameOver();
	disableGame();
}

// ��Ϸ�Ƿ����
bool GameScene::isGameOver()
{
	if (!allChessman[red_king_id]->isLive || !allChessman[black_king_id]->isLive || is_resigned)
		return true;

	return false;
}

// ��ʾ��Ϸ�����ַ�
void GameScene::showGameOver()
{
	Label *gameover;

	if (is_resigned == 1)
		gameover = Label::createWithSystemFont("You Win", "Arial", 130);
	else if (is_resigned == 2)
		gameover = Label::createWithSystemFont("You Lose", "Arial", 130);
	else if (allChessman[red_king_id]->isLive)
	{
		if (color)
			gameover = Label::createWithSystemFont("You Win", "Arial", 130);
		else
			gameover = Label::createWithSystemFont("You Lose", "Arial", 130);
	}
	else
	{
		if (color)
			gameover = Label::createWithSystemFont("You Lose", "Arial", 130);
		else
			gameover = Label::createWithSystemFont("You Win", "Arial", 130);
	}

	gameover->setPosition(size / 2);
	this->addChild(gameover);
}

// ֹͣ��Ϸ����
void GameScene::disableGame()
{
	// ����message
	cleanMessage();

	// ɾ����������
	_eventDispatcher->removeEventListener(listener);

	// ֹͣ���½���
	unscheduleUpdate();
}

void GameScene::menuRestartCallBack(Ref* pSender)
{
	disableGame();
	 //�ر�socket
	if (!is_ai)
	{
		if (sockServer)
			sockServer->Close();
		if (sockClient)
			sockClient->Close();
	}
	//if (sockServer)
	//	sockServer->Close();
	//if (sockClient)
	//	sockClient->Close();

	auto scene = MainScene::createScene();
	Director::getInstance()->replaceScene(scene);
	this->cleanup();
}

void GameScene::menuResignCallBack(Ref* pSender)
{
	if (is_gameStart)
	{
		msg.type = MSG_RESIGN;
		is_resigned = 2;
		status2->setString("you have resigned.");
		send();
		gameOver();
	}
}