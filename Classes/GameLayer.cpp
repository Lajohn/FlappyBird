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
	
	//������Ļ������ʵ��������������ĳ��ȱ��� 1 m=ratio px
	if(visibleSize.width>visibleSize.height){
		ratio=visibleSize.width/10;
	}else{
		ratio=visibleSize.height/10;
	}


	//��ʼ����������
	world=new b2World(b2Vec2(0,0));
	//����������ײ�¼�����
	world->SetContactListener(this);
	isStart=false;
	
	//��ʼ����Ϸ
	gameInit();

	

	return true;
}

//��Ϸ��ʼ��
void GameLayer::gameInit(){
	//����
	bg=Sprite::createWithSpriteFrameName("bg_night.png");
	bg->setPosition(Vec2(visibleSize.width/2,visibleSize.height/2));
	addChild(bg,-1);

	//����
	title=Sprite::createWithSpriteFrameName("title.png");
	title->setPosition(Vec2(visibleSize.width/2,visibleSize.height*0.8));
	addChild(title,-1);

	//��ʾ
	ready=Sprite::createWithSpriteFrameName("text_ready.png");
	ready->setPosition(Vec2(visibleSize.width/2,visibleSize.height*0.6));
	addChild(ready,3);

	//˵��
	tutorial=Sprite::createWithSpriteFrameName("tutorial.png");
	tutorial->setPosition(Vec2(visibleSize.width/2,visibleSize.height*0.4));
	addChild(tutorial,3);


	//�����
	addBird();
	//��ӵذ�
	addGround();

	//������ʱ��
	scheduleUpdate();
	

	//�û�������Ļ���� ʵ�����������Ϸ�
	auto listener=EventListenerTouchOneByOne::create();
	listener->onTouchBegan=[&](Touch *t,Event *e){
		//�ж��Ƿ�ʼ��Ϸ
		if(!isStart){
			isStart=true;
			
			world->SetGravity(b2Vec2(0,-15));
			auto fadeOut1=FadeOut::create(1);
			auto fadeOut2=FadeOut::create(1);
			auto fadeOut3=FadeOut::create(1);
			ready->runAction(fadeOut1);
			tutorial->runAction(fadeOut2);
			title->runAction(fadeOut3);
			//�����Զ��������ӵļ�ʱ��
			schedule(schedule_selector(GameLayer::addPipe),1.0f);
		}

		return true;
	};
	listener->onTouchEnded=[&](Touch *t,Event *e){
		birdBody->SetLinearVelocity(b2Vec2(0,5));
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);

	//�����������¼�����ʵ�� �˳�����
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

//ˢ����������
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
	//������������
	auto bird=Sprite::createWithSpriteFrameName("bird0_0.png");
	Size size=bird->getContentSize();
	//��������
	Vector<SpriteFrame*> sfs;
	sfs.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("bird0_0.png"));
	sfs.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("bird0_1.png"));
	sfs.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("bird0_2.png"));
	auto animation=Animation::createWithSpriteFrames(sfs,0.1f);
	auto animate=Animate::create(animation);

	//��������
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

	//����������
	this->addChild(bird,2);
	birdBody->SetUserData(bird);

	//����ִ�ж���
	bird->runAction(RepeatForever::create(animate));
}


void GameLayer::addGround(){
	//���������ذ�
	ground_1=Sprite::createWithSpriteFrameName("ground.png");
	ground_2=Sprite::createWithSpriteFrameName("ground.png");
	
	Size size=ground_1->getContentSize();

	ground_1->setAnchorPoint(Vec2(0,0));
	ground_2->setAnchorPoint(Vec2(0,0));
	ground_1->setPosition(Vec2(0,0));
	ground_2->setPosition(Vec2(ground_1->getPosition().x+size.width,0));
	this->addChild(ground_1,2);
	this->addChild(ground_2,2);




	//�����������辫��
	auto s=Sprite::create();
	s->setTextureRect(Rect(0,0,size.width,size.height));
	s->setVisible(false);
	
	//��������
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

	//����������
	this->addChild(s);
	body->SetUserData(s);



}


//�����ذ�
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
	
	//����pipe����
	auto pipe_down=Sprite::createWithSpriteFrameName("pipe_down.png");
	pipe_down->setPosition(Vec2(visibleSize.width*2,visibleSize.height/2));
	auto pipe_up=Sprite::createWithSpriteFrameName("pipe_up.png");
	pipe_up->setPosition(Vec2(visibleSize.width*2,visibleSize.height/2));

	//��ȡpipe��С
	Size size=pipe_up->getContentSize();
	
	//��������
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
	
	//��������ӵ��� ���󶨵�����
	this->addChild(pipe_down,0,11);
	body_down->SetUserData(pipe_down);
	body_down->SetLinearVelocity(b2Vec2(-4,0));
	this->addChild(pipe_up,0,11);
	body_up->SetUserData(pipe_up);
	body_up->SetLinearVelocity(b2Vec2(-4,0));
	
}

//������ײ����
void GameLayer::BeginContact(b2Contact* contact){
	//���������������һ����bird����Ϸ����
	if(contact->GetFixtureA()->GetBody()==birdBody||
		contact->GetFixtureB()->GetBody()==birdBody){
			gameOver();
	}

}


//��Ϸ����
void GameLayer::gameOver(){
	//ֹͣ��ʱ��
	unscheduleUpdate();
	unschedule(schedule_selector(GameLayer::addPipe));

	//����Ϸ������ʾ�����Լ����¿�ʼ��ť
	auto gameover=Sprite::createWithSpriteFrameName("text_game_over.png");
	gameover->setPosition(Vec2(visibleSize.width/2,visibleSize.height/2));
	this->addChild(gameover,3);
	auto reStart=Sprite::createWithSpriteFrameName("button_play.png");
	reStart->setPosition(Vec2(visibleSize.width/2,visibleSize.height/4));
	this->addChild(reStart,3);

	//���¿�ʼ��ť����
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




