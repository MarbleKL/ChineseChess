#ifndef _CHESSMAN_H_
#define _CHESSMAN_H_

#include "cocos2d.h"
#include "GameDefine.h"

class Chessman : public cocos2d::Sprite
{
public:
	int m_x;	// ��¼������������
	int m_y;
	int p_x;	// ��¼������������
	int p_y;
	int id;		// ������allChessman�е�id
	int type;	// ��������
	bool isLive;// �Ƿ���

	void setPos(int row, int col);			// ��¼λ������
	void setPositionRC(int row, int col);	// ����λ��

	static Chessman* create(int type);
	virtual bool init(int type);
};

#endif