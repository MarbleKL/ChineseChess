#include "MainScene.h"
#include "GameScene.h"
#include "SettingScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;
using namespace std;
using namespace cocostudio::timeline;

Scene* MainScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = MainScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto rootNode = CSLoader::createNode("MainScene.csb");
	addChild(rootNode);

	CheckBox *checkbox_normal = (CheckBox*)rootNode->getChildByTag(1);
	CheckBox *checkbox_hard = (CheckBox*)rootNode->getChildByTag(2);
	Button *button_net = (Button*)rootNode->getChildByTag(3);
	Button *button_ai = (Button*)rootNode->getChildByTag(4);


	// 如有保存设置，则载入设置
	//string filepath = UserDefault::getInstance()->getXMLFilePath();
	//bool is_set = UserDefault::getInstance()->getBoolForKey("IsSet");
	checkbox_normal->setSelectedState(true);
	checkbox_hard->setSelectedState(false);
	is_normal = true;

	// 普通选项触摸监听
	checkbox_normal->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
		switch (type)
		{
		case Widget::TouchEventType::BEGAN:
			is_normal = true;
		case Widget::TouchEventType::ENDED:
			checkbox_hard->setSelectedState(false);
			break;
		default:
			break;
		}
	});

	// 困难选项触摸监听
	checkbox_hard->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
		switch (type)
		{
		case Widget::TouchEventType::BEGAN:
			is_normal = false;
		case Widget::TouchEventType::ENDED:
			checkbox_normal->setSelectedState(false);
			break;
		default:
			break;
		}
	});

	// 网络对战按钮触摸监听
	button_net->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
		switch (type)
		{
		case Widget::TouchEventType::BEGAN:
			break;
		case Widget::TouchEventType::ENDED:
			// 信息全部合法则保存,跳转到游戏界面
			//saveInfo();
			jumpToSet();
			break;
		default:
			break;
		}
	});

	// 人机对战按钮触摸监听
	button_ai->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
		switch (type)
		{
		case Widget::TouchEventType::BEGAN:
			break;
		case Widget::TouchEventType::ENDED:
			// 信息全部合法则保存,跳转到游戏界面
			//saveInfo();
			jumpToGame();
			break;
		default:
			break;
		}
	});

	return true;
}

void MainScene::jumpToSet()
{
	is_ai = false;
	auto scene = SettingScene::createScene();
	Director::getInstance()->replaceScene(scene);
}

void MainScene::jumpToGame()
{
	is_ai = true;
	saveInfo();
	auto scene = GameScene::createScene(true,is_normal);
	Director::getInstance()->replaceScene(scene);
}

void MainScene::saveInfo()
{
	UserDefault::getInstance()->setBoolForKey("IsSet", true);
	//UserDefault::getInstance()->setBoolForKey("IsAI", is_ai);
	UserDefault::getInstance()->setBoolForKey("IsNormal", is_normal);
	UserDefault::getInstance()->flush();
	string filepath = UserDefault::getInstance()->getXMLFilePath();
	log(filepath.c_str());
}