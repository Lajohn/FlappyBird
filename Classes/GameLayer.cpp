#include "GameLayer.h"

USING_NS_CC;

GameLayer::GameLayer(void)
{
}


GameLayer::~GameLayer(void)
{
}


bool GameLayer::init(){
	if(!Layer::init()){
		return false;
	}


	visibleSize=Director::getInstance()->getVisibleSize();

	auto sfc=SpriteFrameCache::getInstance();
	sfc->addSpriteFramesWithFile("gfx.plist");
	
	//根据屏幕计算现实世界与虚拟世界的长度比例 1 m=ratio px
	if(visibleSize.width>visibleSize.height){
		ratio=visibleSize.width/10;
	}else{
		ratio=visibleSize.height/10;
	}


	//初始化物理世界
	world=new b2World(b2Vec2(0,0));
	//设置物理碰撞事件监听
	world->SetContactListener(this);
	isStart=false;
	
	//初始化游戏
	gameInit();

	

	return true;
}

//游戏初始化
void GameLayer::gameInit(){
	//背景
	bg=Sprite::createWithSpriteFrameName("bg_night.png");
	bg->setPosition(Vec2(visibleSize.width/2,visibleSize.height/2));
	addChild(bg,-1);

	//背景
	title=Sprite::createWithSpriteFrameName("title.png");
	title->setPosition(Vec2(visibleSize.width/2,visibleSize.height*0.8));
	addChild(title,-1);

	//提示
	ready=Sprite::createWithSpriteFrameName("text_ready.png");
	ready->setPosition(Vec2(visibleSize.width/2,visibleSize.height*0.6));
	addChild(ready,3);

	//说明
	tutorial=Sprite::createWithSpriteFrameName("tutorial.png");
	tutorial->setPosition(Vec2(visibleSize.width/2,visibleSize.height*0.4));
	addChild(tutorial,3);


	//添加鸟
	addBird();
	//添加地板
	addGround();

	//开启计时器
	scheduleUpdate();
	

	//用户单机屏幕监听 实现像素鸟向上飞
	auto listener=EventListenerTouchOneByOne::create();
	listener->onTouchBegan=[&](Touch *t,Event *e){
		//判断是否开始游戏
		if(!isStart){
			isStart=true;
			
			world->SetGravity(b2Vec2(0,-15));
			auto fadeOut1=FadeOut::create(1);
			auto fadeOut2=FadeOut::create(1);
			auto fadeOut3=FadeOut::create(1);
			ready->runAction(fadeOut1);
			tutorial->runAction(fadeOut2);
			title->runAction(fadeOut3);
			//开启自动创建柱子的计时器
			schedule(schedule_selector(GameLayer::addPipe),1.0f);
		}

		return true;
	};
	listener->onTouchEnded=[&](Touch *t,Event *e){
		birdBody->SetLinearVelocity(b2Vec2(0,5));
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);

	//添加物理键盘事件监听实现 退出功能
	auto keyboardListener=EventListenerKeyboard::create();
	keyboardListener->onKeyPressed=[](EventKeyboard::KeyCode code, Event *e){
		log(">>>>>>>>>>>>>>>>>>>>>>%d",code);
		if(code==EventKeyboard::KeyCode::KEY_BACKSPACE){
			#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
				MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
				return;
			#endif

				Director::getInstance()->end();

			#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				exit(0);
			#endif
		}

	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener,this);
}



Scene* GameLayer::createScene(){
	auto s=Scene::create();
	auto gl=GameLayer::create();
	s->addChild(gl);
	return s;
}

//刷新物理世界
void GameLayer::update(float dt){
	world->Step(dt,8,3);
	Sprite *pipe;
	rollGround();
	b2Body* bodys[30];

	int i=0;
	for(b2Body *b=world->GetBodyList();b;b=b->GetNext()){
		if(b->GetUserData()){
			pipe=(Sprite*)b->GetUserData();
			pipe->setPosition(Vec2(b->GetPosition().x*ratio,b->GetPosition().y*ratio));
		}
	}
}


void GameLayer::addBird(){
	//创建像素鸟精灵
	auto bird=Sprite::createWithSpriteFrameName("bird0_0.png");
	Size size=bird->getContentSize();
	//创建动画
	Vector<SpriteFrame*> sfs;
	sfs.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("bird0_0.png"));
	sfs.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("bird0_1.png"));
	sfs.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("bird0_2.png"));
	auto animation=Animation::createWithSpriteFrames(sfs,0.1f);
	auto animate=Animate::create(animation);

	//创建刚体
	b2BodyDef def;
	def.position=b2Vec2(visibleSize.width/4/ratio,visibleSize.height/2/ratio);
	def.type=b2_dynamicBody;

	birdBody=world->CreateBody(&def);

	b2PolygonShape shape;
	shape.SetAsBox(size.width/2/ratio,size.height/2/ratio);

	b2FixtureDef fixtureDef;
	fixtureDef.density=1;
	fixtureDef.friction=0.3;
	fixtureDef.shape=&shape;
	birdBody->CreateFixture(&fixtureDef);

	//精灵与刚体绑定
	this->addChild(bird,2);
	birdBody->SetUserData(bird);

	//精灵执行动画
	bird->runAction(RepeatForever::create(animate));
}


void GameLayer::addGround(){
	//创建滚动地板
	ground_1=Sprite::createWithSpriteFrameName("ground.png");
	ground_2=Sprite::createWithSpriteFrameName("ground.png");
	
	Size size=ground_1->getContentSize();

	ground_1->setAnchorPoint(Vec2(0,0));
	ground_2->setAnchorPoint(Vec2(0,0));
	ground_1->setPosition(Vec2(0,0));
	ground_2->setPosition(Vec2(ground_1->getPosition().x+size.width,0));
	this->addChild(ground_1,2);
	this->addChild(ground_2,2);




	//创建刚体所需精灵
	auto s=Sprite::create();
	s->setTextureRect(Rect(0,0,size.width,size.height));
	s->setVisible(false);
	
	//创建刚体
	b2BodyDef def;
	def.position=b2Vec2(size.width/2/ratio,size.height/2/ratio);
	def.type=b2_staticBody;
	
	b2Body* body=world->CreateBody(&def);
	
	b2PolygonShape shape;
	shape.SetAsBox(size.width/2/ratio,size.height/2/ratio);
	
	b2FixtureDef fixtureDef;
	fixtureDef.density=1;
	fixtureDef.friction=0.3;
	fixtureDef.shape=&shape;
	body->CreateFixture(&fixtureDef);

	//精灵与刚体绑定
	this->addChild(s);
	body->SetUserData(s);



}


//滚动地板
void GameLayer::rollGround(){
	Size size=ground_1->getContentSize();
	ground_1->setPosition(ground_1->getPosition()+Vec2(-3,0));
	ground_2->setPosition(ground_2->getPosition()+Vec2(-3,0));

	if(ground_1->getPosition().x+size.width<=0){
		ground_1->setPosition(ground_2->getPosition()+Vec2(size.width-3,0));
	}else if(ground_2->getPosition().x+size.width<=0){
		ground_2->setPosition(ground_1->getPosition()+Vec2(size.width-3,0));
	}



}



void GameLayer::addPipe(float dt){
	float r=CCRANDOM_0_1()*3;
	
	//创建pipe精灵
	auto pipe_down=Sprite::createWithSpriteFrameName("pipe_down.png");
	pipe_down->setPosition(Vec2(visibleSize.width*2,visibleSize.height/2));
	auto pipe_up=Sprite::createWithSpriteFrameName("pipe_up.png");
	pipe_up->setPosition(Vec2(visibleSize.width*2,visibleSize.height/2));

	//获取pipe大小
	Size size=pipe_up->getContentSize();
	
	//创建刚体
	b2BodyDef def_down;
	def_down.position=b2Vec2(visibleSize.width*2/ratio,r);
	def_down.type=b2_kinematicBody;
	
	b2BodyDef def_up;
	def_up.position=b2Vec2(visibleSize.width*2/ratio,size.height/ratio+def_down.position.y+2);
	def_up.type=b2_kinematicBody;
	
	b2Body* body_down=world->CreateBody(&def_down);
	b2Body* body_up=world->CreateBody(&def_up);
	
	b2PolygonShape shape_down;
	shape_down.SetAsBox(size.width/2/ratio,size.height/2/ratio);
	b2PolygonShape shape_up;
	shape_up.SetAsBox(size.width/2/ratio,size.height/2/ratio);
	
	b2FixtureDef fixtureDef_down;
	fixtureDef_down.density=1;
	fixtureDef_down.friction=0.3;
	fixtureDef_down.shape=&shape_down;
	body_down->CreateFixture(&fixtureDef_down);
	b2FixtureDef fixtureDef_up;
	fixtureDef_up.density=1;
	fixtureDef_up.friction=0.3;
	fixtureDef_up.shape=&shape_up;
	body_up->CreateFixture(&fixtureDef_up);
	
	//将精灵添加到层 并绑定到刚体
	this->addChild(pipe_down,0,11);
	body_down->SetUserData(pipe_down);
	body_down->SetLinearVelocity(b2Vec2(-4,0));
	this->addChild(pipe_up,0,11);
	body_up->SetUserData(pipe_up);
	body_up->SetLinearVelocity(b2Vec2(-4,0));
	
}

//物体碰撞监听
void GameLayer::BeginContact(b2Contact* contact){
	//如果两个物体中有一个是bird就游戏结束
	if(contact->GetFixtureA()->GetBody()==birdBody||
		contact->GetFixtureB()->GetBody()==birdBody){
			gameOver();
	}

}


//游戏结束
void GameLayer::gameOver(){
	//停止计时器
	unscheduleUpdate();
	unschedule(schedule_selector(GameLayer::addPipe));

	//在游戏界面显示结束以及重新开始按钮
	auto gameover=Sprite::createWithSpriteFrameName("text_game_over.png");
	gameover->setPosition(Vec2(visibleSize.width/2,visibleSize.height/2));
	this->addChild(gameover,3);
	auto reStart=Sprite::createWithSpriteFrameName("button_play.png");
	reStart->setPosition(Vec2(visibleSize.width/2,visibleSize.height/4));
	this->addChild(reStart,3);

	//重新开始按钮监听
	auto listener=EventListenerTouchOneByOne::create();
	listener->onTouchBegan=[](Touch *t,Event *e){
		if(e->getCurrentTarget()->getBoundingBox().containsPoint(t->getLocation())){
			Director::getInstance()->replaceScene(TransitionFade::create(1,GameLayer::createScene()));
		}

		return false;
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,reStart);


	auto fi=FadeIn::create(1);
	gameover->runAction(fi);
}




