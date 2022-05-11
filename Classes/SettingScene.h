#ifndef __SETTING_SCENE_H__
#define __SETTING_SCENE_H__

#include "cocos2d.h"
class SettingScene : public cocos2d::Layer
{
public:
	bool is_host;
	std::string ip;
	std::string port;

	bool isInfoValid;	// ��Ϣ�Ƿ�Ϸ�
	bool isIpValid();	// ���ip�Ϸ���
	bool isPortValid();	// ���˿ںϷ���
	void saveInfo();	// ����������Ϣ���Ա��´�ʹ��
	void jumpToGame();	// ��ת����Ϸ����

	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(SettingScene);
};

#endif // __SETTING_SCENE_H__