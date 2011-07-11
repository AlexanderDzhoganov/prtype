#include "headers.h" 

Hero::Hero(Game* game_n) : Sprite(game_n), Collidable(game_n) {
	game = game_n;
	setTileset("assets\\spaceship2.png");
	setAnimation(LOOP, 0, 0, 12.f);
	flags |= ACTIVE;
	pos[1] += 300.f;

	AABB local;
	local.min.set(0.f, 0.f);
	local.max.set(INT_INFO(this, INT_SIZE_X), INT_INFO(this, INT_SIZE_Y));
	Collidable::setParams(local, 40.f);

	game->addNode(this);

	shootDelay = 1.f;
	shootProj = 0;
	
	weapon = new ProjectileWeapon(game);
	weapon->setPositionLookup(&pos);
	weapon->setPositionTranslation(size+vec2(25.f, 7.5f));

	setCollisionCallback(&colevent);
	colevent.hasCollided = false;

	shield = new Sprite(game);
	shield->setTileset("assets\\shield.png");
	shield->setFrame(0);
	shield->flags &= ACTIVE;
	game->addNode(shield, this);
	shieldPower = 100.f;
	shieldShowTimer = 0.f;
	dying = false;
	score = 0;
}

Hero::~Hero() {
	delete weapon;
}

void Hero::update(float frameDelta) {
	if(!dying) {
		char* scoretext = new char[64];
		sprintf(scoretext, "Score: %0.f", score);
		game->getFont()->printText(scoretext, vec2(50.f, 50.f), 18);
		delete [] scoretext;
		score += 0.1f*frameDelta;

	}
	
	shield->setPosition(pos-vec2(15.f, 10.f));


	if(shieldShowTimer > 0.f) {
		shieldShowTimer -= frameDelta;
		if(shieldShowTimer < 0.f) {
			shieldShowTimer = 0.f;
			shield->flags ^= ACTIVE;
		}
	}


	GameNode::update(frameDelta);
	Sprite::update(frameDelta);
	Collidable::update(frameDelta);

	if(dying) return;

	if(shieldPower < 0.f) {
		if(!dying) {
			game->setState(STATE_GAME_OVER);
			dying = true;
			shield->flags ^= ACTIVE;
			flags ^= ACTIVE;
			game->getEffectMgr()->spawnEffect(EFFECT_TYPE_EXPLOSION, pos);
		}
	}
	

	if(colevent.hasCollided) {
		takeDamage(1.f);
		colevent.hasCollided = false;
	}

	CollisionEvent col_event = game->getCollisionMgr()->collideObject(this, this, C_LEVEL);
	if(col_event.hasCollided) {
			takeDamage(100.f);
	}
	
	col_event = game->getCollisionMgr()->collideObject(this, this, C_POWERUPS);
	if(col_event.hasCollided) {
		weapon->randomizeAttributes();
	}

	col_event = game->getCollisionMgr()->collideObject(this, this, C_ENEMIES);
	if(col_event.hasCollided) {
		takeDamage(7.5f);
	}
	//bc.pos = pos + vec2(frame_size_x, frame_size_y) * 0.5f;
	vec2 cameraPos = game->getCameraPos();
	//game->drawCircle(bc.pos - cameraPos, bc.radius, 0xfffff

	if(shootDelay > 0.f) {
		shootDelay -= frameDelta;
		if(shootDelay < 0.f) {
			shootDelay = 0.f;
		}
	}

	int keymask = game->getKeymask();
	int mousemask = game->getMousemask();
	vec2 mousePos = game->getMousePos();


	if(keymask&KEY_Z) {
		weapon->fire();
	}

	if(keymask&KEY_X) {
		
	}
	
	if(keymask&UP) {
		pos[1] -= 0.2f*frameDelta;
		//setAnimation(ONCE, 1, 1, 12.f);
	} else {
		//setAnimation(LOOP, 0, 0, 12.f);
	}

	if(keymask&DOWN) {
		pos[1] += 0.2f*frameDelta;
		//setAnimation(ONCE, 2, 2, 12.f);
	} else if(!(keymask&UP)) {
		//setAnimation(LOOP, 0, 0, 12.f);
	}

	if(keymask&RIGHT) {
		pos[0] += 0.4f*frameDelta;
	} else {
		pos[0] += 0.3f*frameDelta;
	}

	if(keymask&LEFT) {
		pos[0] -= 0.2f*frameDelta;
		//if(keymask&DOWN)
			//setAnimation(ONCE, 4, 4, 12.f);
		//else if(keymask&UP)
			//setAnimation(ONCE, 5, 5, 12.f);
		//else
			//setAnimation(ONCE, 3, 3, 12.f);
	}

	keymask = 0;
	mousemask = 0;

	// check if within screen
	if(pos[0] < cameraPos[0]) pos[0] = cameraPos[0];
	if(pos[1] < 0.f) pos[1] = 0.f;
	if(pos[0]+frame_size_x > cameraPos[0]+SCRWIDTH) pos[0] = cameraPos[0]+SCRWIDTH-frame_size_x;
	if(pos[1]+frame_size_y > SCRHEIGHT) pos[1] = SCRHEIGHT-frame_size_y;
}

vec2 Hero::getPosition() {
	return pos;
}

void Hero::takeDamage(float damage) {
	shieldPower -= damage;
	shield->flags |= ACTIVE;
	shieldShowTimer = 450.f;
	if(shieldPower < 75.f) {
		shield->setFrame(1);
	}
	if(shieldPower < 40.f) {
		shield->setFrame(2);
	} if(shieldPower < 15.f) {
		shield->setFrame(3);
	}
}

void Hero::increaseScore(float score_n) {
	if(!dying)
		score += score_n;
}

float Hero::getScore() {
	return score;
}