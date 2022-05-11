#pragma once
#include "cocos2d.h"

class MainScene : public cocos2d::Layer
{
public:
	bool is_ai;
	bool is_normal;

	void jumpToSet();
	void jumpToGame();
	void saveInfo();

	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(MainScene);
};

