#include "headers.h"


// enemy spawner class methods

EnemySpawner::EnemySpawner(Game* game_n) {
	game = game_n;
	game->addNode(this);
	spawnDelay = 1000.f;
}

EnemySpawner::~EnemySpawner() {
}

void EnemySpawner::update(float frameDelta) {
	GameNode::update(frameDelta);

	if(spawnDelay > 0.f) {
		spawnDelay -= frameDelta;
		if(spawnDelay < 0.f) {
			if(rand() % 15 == 1)
				spawnRandomWave();
			else spawnRandomEnemy();
			spawnDelay = 400.f;
		}
	}
}

void EnemySpawner::spawnRandomEnemy() {
	Enemy* enemy = new Enemy(game);
	enemy->setPosition(vec2(game->getHero()->getPosition()[0]+2000.f, 200+rand()%(SCRHEIGHT-400)));
	enemy->setBehavior(rand()%4);
	enemy->setSineHeight(40+rand()%80);
	enemy->setSpeed((1+rand()%3)*0.1f);
	enemies.push_back(enemy);
}

void EnemySpawner::spawnRandomWave() {
	int length = 2 + rand() % 7;
	float groupSpeed = (1+rand()%3)*0.1f;
	Enemy* enemy;
	Enemy* prev;

	enemy = new Enemy(game);
	enemy->setPosition(game->getHero()->getPosition()+vec2(800.f, -250+rand()%500));
	enemy->setBehavior(rand()%4);
	enemy->setSineHeight(20+rand()%40);
	enemy->setSpeed(groupSpeed);
	enemy->setLeader();
	enemies.push_back(enemy);

	prev = enemy;

	vec2 dist = vec2(46.f, 0.f);

	for(int x=0; x<length; x++) {
		enemy = new Enemy(game);
		enemy->setPosition(prev->getPosition()+dist);
		enemy->setBehavior(EB_FOLLOW);
		enemy->setFollow(prev);
		enemy->setSpeed(groupSpeed);
		enemies.push_back(enemy);
		prev = enemy;
	}
}

void EnemySpawner::delEnemy(Enemy* enemy) {
	enemies.remove(enemy);
}

std::list<Enemy*>* EnemySpawner::getEnemies() {
	return &enemies;
}

void EnemySpawner::deleteEnemies() {
}

bool EnemySpawner::doesEnemyExist(Enemy* enemy) {
	for(std::list<Enemy*>::iterator x = enemies.begin(); x != enemies.end(); x++) {
		if(*x == enemy) return true;
	}
	return false;
}

// enemy class methods

Enemy::Enemy(Game* game_n) : Sprite(game_n), Collidable(game_n) {
	type = ENEMY;
	setCollision(false);
	setCollision(true);
	setCollisionCallback(&colevent);
	game = game_n;
	game->addNode(this);
	setSprite(this);
	setTileset("assets\\enemies.png");
	flags |= ACTIVE;
	setFrame(0);
	AABB local;
	local.min.set(0.f, 0.f);
	local.max.set(INT_INFO(this, INT_SIZE_X), INT_INFO(this, INT_SIZE_Y));
	Collidable::setParams(local, 27.f);
	colevent.hasCollided = false;
	rotation = 0.f;
	spriteOrigin = 0;
	spriteOrigin = (rand() % 14) * 40;
	height = 0;
	sineheight = 0;
	followEnemy = NULL;
	weapon = new EnemyWeapon(game);
	weapon->setPositionLookup(&pos);
	weapon->setPositionTranslation(size*0.5f);
	suicide_dist = 150 + rand()%300;
	suicide_dir_chosen = false;
	leader = false;
}

Enemy::~Enemy() {
	game->getEnemySpawner()->delEnemy(this);
	game->delNode(weapon);
}

void Enemy::setPosition(vec2 pos_n) {
	pos = pos_n;
}

vec2 Enemy::getPosition() {
	return pos;
}

void Enemy::update(float frameDelta) {
	GameNode::update(frameDelta);
	Sprite::update(frameDelta);
	Collidable::update(frameDelta);

	if(rand() % 350 == 1) weapon->fire();

	if(rand() % 2500 == 1) behavior = rand() % 4;

	vec2 dir = pos - lastPos;
	dir.normalize();
	vec2 axis(0.f, -1.f);
	desiredRotation = atan2(dir[1], dir[0]) - atan2(axis[1], axis[0]);
	desiredRotation *= 180.f/3.1415f;

	while(desiredRotation < 0) desiredRotation = 360 - rotation;
	if(desiredRotation > 360) desiredRotation -= 360;

	if(rotation + 5.f < desiredRotation) {
		rotation += 0.3333f*frameDelta;
	} else if(rotation - 5.f > desiredRotation) {
		rotation -= 0.3333f*frameDelta;
	}

	setFrame(spriteOrigin+int(rotation/9.f));

	if(pos[0] < game->getCameraPos()[0]-25.f) {
		_markForDeletion = true;
		return;
	}

	if(colevent.hasCollided && !_markForDeletion) {
		_markForDeletion = true;
		game->getEffectMgr()->spawnEffect(EFFECT_TYPE_EXPLOSION, pos);
		game->getHero()->increaseScore(250+rand()%250);
		return;
	}

	lastPos = pos;

	CollisionEvent levelevent = game->getCollisionMgr()->collideObject(this, this, C_LEVEL);
	if(levelevent.hasCollided && !_markForDeletion) {
		_markForDeletion = true;
		game->getEffectMgr()->spawnEffect(EFFECT_TYPE_EXPLOSION, pos);
		return;
	}

	CollisionEvent front, up, down;

	float heroDist = (pos - game->getHero()->getPosition()).len();
	vec2 heroDir = (pos - game->getHero()->getPosition());
	heroDir.normalize();


	switch(behavior) {
	case EB_STRAIGHT:
		if(pos[1]+size[1] < SCRHEIGHT*0.5f) {
			front = game->getCollisionMgr()->collidePoint(pos-vec2(50.f, 0.f), C_LEVEL);
			if(front.hasCollided) {
				pos[1] += speed*frameDelta;
			}
		} else if(pos[1] > SCRHEIGHT*0.5f) {
			front = game->getCollisionMgr()->collidePoint(pos-vec2(50.f, -size[1]), C_LEVEL);
			if(front.hasCollided) {
				pos[1] -= speed*frameDelta;
			}
		}

		pos[0] -= speed*frameDelta;

		break;
	case EB_SINE:
		if(pos[1]+size[1] < SCRHEIGHT*0.5f) {
			front = game->getCollisionMgr()->collidePoint(pos-vec2(50.f, 0.f), C_LEVEL);
			if(front.hasCollided) {
				pos[1] += speed*frameDelta;
			}
		} else if(pos[1] > SCRHEIGHT*0.5f) {
			front = game->getCollisionMgr()->collidePoint(pos-vec2(50.f, -size[1]), C_LEVEL);
			if(front.hasCollided) {
				pos[1] -= speed*frameDelta;
			}
		}
		
		pos[0] -= speed*0.3f*frameDelta;
		if(height == 0) {
			height = pos[1];
		}
		pos[1] = height + sineheight*sin(0.025*pos[0]);
		break;
	case EB_SUICIDER:
		if(!suicide_dir_chosen) {
			if(pos[1]+size[1] < SCRHEIGHT*0.5f) {
				front = game->getCollisionMgr()->collidePoint(pos-vec2(50.f, 0.f), C_LEVEL);
				if(front.hasCollided) {
					pos[1] += speed*frameDelta;
				}
			} else if(pos[1] > SCRHEIGHT*0.5f) {
				front = game->getCollisionMgr()->collidePoint(pos-vec2(50.f, -size[1]), C_LEVEL);
				if(front.hasCollided) {
					pos[1] -= speed*frameDelta;
				}
			}
			pos[0] -= speed*frameDelta;
			
			if(heroDist < suicide_dist) {
				suicide_dir_chosen = true;
				suicide_dir = game->getHero()->getPosition() - pos;
				suicide_dir.normalize();
			}
		} else {
			pos += suicide_dir*speed*1.75f*frameDelta;
		}
		break;
	case EB_RANDOM_SPLINE:
		behavior = 0;
		break;
	case EB_FOLLOW:
		if(!game->getEnemySpawner()->doesEnemyExist(followEnemy)) {
			behavior = rand()%4;
			break;
		}
		if(!followEnemy) break;
		vec2 dir = (followEnemy->getPosition() - pos);
		dir.normalize();
		pos += dir*speed*frameDelta;
		break;
	}


	if(heroDist < 75.f && behavior != EB_SUICIDER && !leader) {
		pos[1] += heroDir[1]*speed*1.5f*frameDelta;
	} else if(heroDist < 150.f && behavior != EB_SUICIDER && !leader) {
		pos[1] += heroDir[1]*speed*frameDelta;
	}

	float closestDist = 9999999999999.f;
	Enemy* closestEnemy = NULL;
	for(std::list<Enemy*>::iterator x = game->getEnemySpawner()->getEnemies()->begin(); x != game->getEnemySpawner()->getEnemies()->end(); x++) {
		if(*x == this) continue;
		float dist = ((*x)->getPosition() - pos).len();
		if(dist < closestDist) {
			closestDist = dist;
			closestEnemy = *x;
		}
	}


	if(closestEnemy && behavior != EB_FOLLOW) {
		vec2 enemyDir = closestEnemy->getPosition() - pos;
		enemyDir.normalize();
		if(closestDist < 30.f) {
			pos[1] += enemyDir[1]*speed*0.5f*frameDelta;
		} else if(closestDist < 100.f) {
			pos[1] += enemyDir[1]*speed*0.25f*frameDelta;
		}
	}
}

void Enemy::setSpeed(float speed_n) {
	speed = speed_n;
}

void Enemy::setBehavior(int behavior_n) {
	behavior = behavior_n;
}

void Enemy::setSineHeight(float sineheight_n) {
	sineheight = sineheight_n;
}

void Enemy::setFollow(Enemy* followEnemy_n) {
	followEnemy = followEnemy_n;
}

void Enemy::setLeader() {
	leader = true;
}