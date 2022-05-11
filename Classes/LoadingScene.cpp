#include "LoadingScene.h"
#include "SettingScene.h"
#include "MainScene.h"

USING_NS_CC;

Scene* LoadingScene::createScene()
{
	auto scene = Scene::create();
	auto layer = LoadingScene::create();
	scene->addChild(layer);
	return scene;
}

bool LoadingScene::init()
{
	if (!Layer::init())
		return false;

	auto size = Director::getInstance()->getWinSize();

	TTFConfig config("fonts/Marker Felt.ttf", 80);

	//��Ϸ����
	auto GameName = Label::createWithTTF(config, "Chinese Chess");
	GameName->setColor(Color3B::WHITE);
	GameName->setPosition(Point(size.width / 2, size.height * 3 / 5));
	this->addChild(GameName);

	//����
	auto author = Label::createWithTTF(config, "Game By Zhujinan");
	author->setColor(Color3B::WHITE);
	author->setScale(0.5);
	author->setPosition(Point(size.width / 2, size.height / 3));
	this->addChild(author);
	
	return true;
}

void LoadingScene::onEnter()
{
	//�첽����ͼƬ
	Director::getInstance()->getTextureCache()->addImageAsync("chinesechess.png", CC_CALLBACK_1(LoadingScene::jumpToGame, this));
}

void LoadingScene::jumpToGame(Texture2D *texture)
{
	//Ԥ����֡��������
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("chinesechess.plist", texture);

	//������ת
	auto scene = MainScene::createScene();
	TransitionScene *transition = TransitionFade::create(2, scene);
	Director::getInstance()->replaceScene(transition);
}