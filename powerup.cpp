#include "headers.h"

// powerup spawner class methods

PowerupSpawner::PowerupSpawner(Game* game_n) {
	game = game_n;
	game->addNode(this);
	delay = 0.f;
}

PowerupSpawner::~PowerupSpawner() {

}

void PowerupSpawner::update(float frameDelta) {
	GameNode::update(frameDelta);

	if(delay > 0.f) {
		delay -= frameDelta;
		if(delay < 0.f) delay = 0.f;
		return;
	}

	if(rand() % 500 == 1) {
		Powerup* pwrup = new Powerup(game);
		pwrup->setPosition(game->getCameraPos()+vec2(SCRWIDTH+100, 50 + rand() % (SCRHEIGHT-100)));
		powerups.push_back(pwrup);
		delay = 4000.f;
	}
}
void PowerupSpawner::delPowerup(Powerup* powerup) {
	powerups.remove(powerup);
}

std::list<Powerup*>* PowerupSpawner::getPowerups() {
	return &powerups;
}

// powerup class methods

Powerup::Powerup(Game* game_n) : Sprite(game_n), Collidable(game_n) {
	game = game_n;
	game->addNode(this);
	type = POWERUP;
	setCollision(false);
	setCollision(true);
	setSprite(this);
	setTileset("assets\\powerup.png");
	setAnimation(LOOP, 0, 15, 100.f);
	flags |= ACTIVE;

	AABB local;
	local.min.set(0.f, 0.f);
	local.max.set(INT_INFO(this, INT_SIZE_X), INT_INFO(this, INT_SIZE_Y));
	Collidable::setParams(local, 16.f);

	setCollisionCallback(&colevent);
	colevent.hasCollided = false;
}

Powerup::~Powerup() {

}

void Powerup::update(float frameDelta) {
	GameNode::update(frameDelta);
	Sprite::update(frameDelta);
	Collidable::update(frameDelta);

	if(colevent.hasCollided || pos[0]+size[0]<game->getCameraPos()[0]) {
		if(colevent.hasCollided)
			game->getHero()->increaseScore(2000);
		_markForDeletion = true;
		return;
	}

	CollisionEvent col_event = game->getCollisionMgr()->collideObject(this, this, C_LEVEL);
	if(col_event.hasCollided) {
		if(pos[1] < SCRHEIGHT*0.5f) {
			pos[1] += 10.f;
		} else {
			pos[1] -= 10.f;
		}
	}
}