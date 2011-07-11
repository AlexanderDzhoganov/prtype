#include "headers.h"

Projectile::Projectile(Game* game_n) : Sprite(game_n), Collidable(game_n) {
	type = PROJECTILE;
	setCollision(false);
	setCollision(true);
	game = game_n;
	game->addNode(this);
	setSprite(this);

	setTileset("assets\\projectiles.png");
	//setSprite(0);
	
	setAnimationCallback(&animEnded);
	flags |= ACTIVE;

	AABB local;
	local.min.set(0.f, 0.f);
	local.max.set(INT_INFO(this, INT_SIZE_X), INT_INFO(this, INT_SIZE_Y));
	Collidable::setParams(local, 8.f);
	speed = 0.f;
	dying = false;
	p_event.eventActive = false;
	traveledDistance = 0;
	eventTriggered = false;
	stiffness = 1.f;
	lifeTime = 0.f;
}

Projectile::~Projectile() {
	
}

void Projectile::update(float frameDelta) {
	lifeTime += frameDelta;

	GameNode::update(frameDelta);
	Sprite::update(frameDelta);
	Collidable::update(frameDelta);

	
	if(dying && animEnded) {
		_markForDeletion = true;
	}


	vec2 cameraPos = game->getCameraPos();

	if(pos[0] < cameraPos[0] || pos[0] > cameraPos[0]+SCRWIDTH || pos[1] < cameraPos[1] || pos[1] > cameraPos[1]+SCRHEIGHT) {
		_markForDeletion = true;
	}
	
	CollisionEvent col_event;
	col_event.hasCollided = false;
	if(!dying && lifeTime > 100.f) col_event = game->getCollisionMgr()->collideObject(this, this, C_LEVEL|C_ENEMIES);
	if(!dying && col_event.hasCollided) {
		if(p_event.eventActive && (p_event.triggermask&DEATH)) {
			triggerEvent();
		} else {
			dying = true;
			setAnimation(ONCE, spriteOrigin, spriteOrigin+31, 10000.f);
			animEnded = false;
		}
	}

	if(dying) return;

	setFrame(spriteOrigin);

	Enemy* closestEnemy = NULL;
	float closestDist = 999999999999.f;
	
	if((p_event.eventActive && (p_event.triggermask&ENEMY_RANGE)) || t_type==HOMING)
	for(std::list<Enemy*>::iterator x = game->getEnemySpawner()->getEnemies()->begin(); x != game->getEnemySpawner()->getEnemies()->end(); x++) {
		float dist = ((*x)->getPosition() - pos).len2();
		if(dist < closestDist) {
			closestDist = dist;
			closestEnemy = *x;
		}
	}

	
	if(p_event.eventActive) {
		if(p_event.triggermask&DISTANCE_TRAVELED) {
			if(traveledDistance > p_event.distance) {
				triggerEvent();
			}
		}
		if(p_event.triggermask&ENEMY_RANGE) {
			if(closestDist < p_event.range) {
				triggerEvent();
			}
		}
	}

	switch(t_type) {
	case STRAIGHT:
		target.normalize();
		pos += target*speed*0.1f*frameDelta;
		traveledDistance += speed*0.1f*frameDelta;
		break;
	case SINE:
		target.normalize();
		pos += target*speed*0.1f*frameDelta;
		traveledDistance += speed*0.1f*frameDelta;
		break;
	case HOMING:
		if(closestEnemy && closestDist < 20000.f) {
			vec2 dir = closestEnemy->getPosition() - pos;
			dir.normalize();
			pos += target*(speed-stiffness*speed*0.01f)*0.1f*frameDelta;
			pos += dir*speed*stiffness*speed*0.001f*frameDelta;
			traveledDistance += speed*0.06f*frameDelta + speed*0.04f*frameDelta;
		} else {
			target.normalize();
			pos += target*speed*0.1f*frameDelta;
			traveledDistance += speed*0.1f*frameDelta;
		}
		break;
	}

}

void Projectile::setPosition(vec2 pos_n) {
	pos = pos_n;
}

vec2 Projectile::getPosition() {
	return pos;
}

void Projectile::setTargetingType(int t_type_n, vec2 target_n) {
	t_type = t_type_n;
	target = target_n;
	
}

void Projectile::setSpeed(float speed_n) {
	speed = speed_n;
}

void Projectile::setProjectileSprite(int index) {
	spriteOrigin = index*32;
	setFrame(spriteOrigin);
}

void Projectile::setFlag(int flag, bool state) {
	if(state) {
		flags |= flag;
	} else {
		flags &= flag;
	}
}

void Projectile::setProjectileEvent(ProjectileEvent p_event_n) {
	p_event = p_event_n;
}

void Projectile::triggerEvent() {
	if(eventTriggered) return;
	if(p_event.eventActive) {
		if(p_event.actionmask&SPAWN_SECONDARY) {
			ProjectileWeapon* s = p_event.secondaryweapon;
			s->fireFromPos(pos);
		}
		if(p_event.actionmask&DIE) {
			dying = true;
			setAnimation(ONCE, spriteOrigin, spriteOrigin+31, 500.f);
			animEnded = false;
		}
	}
	eventTriggered = true;
}

void Projectile::setStiffness(float stiffness_n) {
	stiffness = stiffness_n;
}

// enemy projectile class methods

EnemyProjectile::EnemyProjectile(Game* game_n) : Sprite(game_n), Collidable(game_n) {
	type = ENEMY_PROJECTILE;
	game = game_n;
	game->addNode(this);
	setTileset("assets\\enemy_projectile.png");
	setSprite(this);
	setCollision(false);
	setCollision(true);
	setFrame(0);
	flags |= ACTIVE;
	AABB local;
	local.min.set(0.f, 0.f);
	local.max.set(INT_INFO(this, INT_SIZE_X), INT_INFO(this, INT_SIZE_Y));
	Collidable::setParams(local, 8.f);
	speed = 0;
}

EnemyProjectile::~EnemyProjectile() {
	
}

void EnemyProjectile::update(float frameDelta) {
	GameNode::update(frameDelta);
	Sprite::update(frameDelta);
	Collidable::update(frameDelta);

	vec2 cameraPos = game->getCameraPos();

	if(pos[0]+size[0] < cameraPos[0]) {
		_markForDeletion = true;
		return;
	}

	CollisionEvent newEvent = game->getCollisionMgr()->collideObject(this, this, C_PLAYER|C_LEVEL);
	if(newEvent.hasCollided) {
		_markForDeletion = true;
		return;
	}

	pos += dir*speed*0.1f*frameDelta;
}

void EnemyProjectile::setSpeed(float speed_n) {
	speed = speed_n;
}

void EnemyProjectile::setDirection(vec2 dir_n) {
	dir = dir_n;
	dir.normalize();
}