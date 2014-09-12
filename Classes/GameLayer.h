#ifndef _GAME_LAYER_H_
#define _GAME_LAYER_H_

#include "cocos2d.h"
#include "Box2D\Box2D.h"

class GameLayer:public cocos2d::Layer,public b2ContactListener
{
private:
	//Box2d����
	b2World *world;
	//���������
	b2Body *birdBody;
	//��������
	cocos2d::Sprite *bg;
	//���⾫��
	cocos2d::Sprite *title;
	//��ʾ����
	cocos2d::Sprite *ready;
	//˵������
	cocos2d::Sprite *tutorial;
	//�����ذ� ���ڵذ����
	cocos2d::Sprite *ground_1;
	cocos2d::Sprite *ground_2;
	//�ذ����
	b2Body *groundBody;
	//��ʵ���� 1m �� ��Ϸ���� 1px �ı���
	float ratio;
	//�Ƿ�ʼ��Ϸ
	bool isStart;

	//�����ذ�
	void rollGround();
public:
	cocos2d::Size visibleSize;

	GameLayer(void);
	~GameLayer(void);

	virtual bool init();
	static cocos2d::Scene* createScene();
	CREATE_FUNC(GameLayer);
	//���º���
	virtual void update(float dt);
	//������ײ����
	virtual void BeginContact(b2Contact* contact);
	//��Ϸ��ʼ��
	void gameInit();

	//��� ������
	void addBird();
	//��ӵذ�
	void addGround();
	//��ʱ�������
	void addPipe(float dt);
	//��Ϸ����
	void gameOver();

};


#endif // ! _GAME_LAYER_H_
