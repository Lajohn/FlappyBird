#ifndef _GAME_LAYER_H_
#define _GAME_LAYER_H_

#include "cocos2d.h"
#include "Box2D\Box2D.h"

class GameLayer:public cocos2d::Layer,public b2ContactListener
{
private:
	//Box2d世界
	b2World *world;
	//像素鸟刚体
	b2Body *birdBody;
	//背景精灵
	cocos2d::Sprite *bg;
	//标题精灵
	cocos2d::Sprite *title;
	//提示精灵
	cocos2d::Sprite *ready;
	//说明精灵
	cocos2d::Sprite *tutorial;
	//两个地板 用于地板滚动
	cocos2d::Sprite *ground_1;
	cocos2d::Sprite *ground_2;
	//地板刚体
	b2Body *groundBody;
	//现实世界 1m 与 游戏世界 1px 的比例
	float ratio;
	//是否开始游戏
	bool isStart;

	//滚动地板
	void rollGround();
public:
	cocos2d::Size visibleSize;

	GameLayer(void);
	~GameLayer(void);

	virtual bool init();
	static cocos2d::Scene* createScene();
	CREATE_FUNC(GameLayer);
	//更新函数
	virtual void update(float dt);
	//物理碰撞监听
	virtual void BeginContact(b2Contact* contact);
	//游戏初始化
	void gameInit();

	//添加 像素鸟
	void addBird();
	//添加地板
	void addGround();
	//定时添加柱子
	void addPipe(float dt);
	//游戏结束
	void gameOver();

};


#endif // ! _GAME_LAYER_H_
