#include "headers.h"

ProjectileWeapon::ProjectileWeapon(Game* game_n) {
	game = game_n;
	posLookup = NULL;
	game->addNode(this);
	secondaryWeapon = NULL;
	secondary = 0;
	randomizeAttributes();
	projectileSignSwap = false;
	delayTimer = 0.f;
}

ProjectileWeapon::~ProjectileWeapon() {
	if(secondaryWeapon)
		delete secondaryWeapon;
}

void ProjectileWeapon::setSecondary(int s) {
	secondary = s;
}

void ProjectileWeapon::update(float frameDelta) {
	if(posLookup) {
		pos = *posLookup + postransl;
	}
	GameNode::update(frameDelta);

	if(delayTimer > 0.f) {
		delayTimer -= frameDelta;
		if(delayTimer < 0.f) delayTimer = 0.f;
	}

	if(!firing) return;

	if(shootDelayTimer > 0.f) {
		shootDelayTimer -= frameDelta;
		if(shootDelayTimer < 0.f) {
			if((t_type == STRAIGHT || t_type == SINE || t_type == HOMING) && projectileAngle != 0.f) {
				Projectile* p = new Projectile(game);
				p->setPosition(pos);
				p->setSpeed(projectileSpeed);
				p->setTargetingType(STRAIGHT, vec2(1.f, 0.f));
				p->setProjectileSprite(projectileSprite);
				p->setFlag(MOTION_BLUR, motionblur);
				p->setFlag(ACTIVE, true);
				p->setProjectileEvent(t_event);
				p->setStiffness(projectileStiffness);
				projectilesFired++;

				for(float x=0; x<maxAngle; x += projectileAngle) {
					Projectile* p = new Projectile(game);
					p->setPosition(pos);
					p->setSpeed(projectileSpeed);
					p->setTargetingType(t_type, vec2(cos(x), sin(x)));
					p->setProjectileSprite(projectileSprite);
					p->setFlag(MOTION_BLUR, motionblur);
					p->setFlag(ACTIVE, true);
					p->setProjectileEvent(t_event);
					p->setStiffness(projectileStiffness);
					projectilesFired++;
					p = new Projectile(game);
					p->setPosition(pos);
					p->setSpeed(projectileSpeed);
					p->setTargetingType(t_type, vec2(cos(-x), sin(-x)));
					p->setProjectileSprite(projectileSprite);
					p->setFlag(MOTION_BLUR, motionblur);
					p->setFlag(ACTIVE, true);
					p->setProjectileEvent(t_event);
					p->setStiffness(projectileStiffness);
					projectilesFired++;
					if(projectilesFired >= projectilesPerShot) break;
				}
				firing = false;
			}
			if(!secondary)
				shootDelayTimer = shootDelay;
			else shootDelayTimer = 0.f;
			
			if(projectilesFired >= projectilesPerShot) firing = false;
		}
	}
}

void ProjectileWeapon::randomizeAttributes() {
	projectileSprite = rand() % 4;
	shootDelay = 200 + rand() % 500;
	projectileSpawnDelay = 1 + rand() % 100;
	if(!secondary) {
		projectilesPerShot = 2 + rand() % 6;
	} else if(rand()%7 == 1 && !secondary) {
		projectilesPerShot = 1 + rand() % 4;
	} else {
		if((3 - secondary) > 0)
			projectilesPerShot = 1 + rand() % (3 - secondary);
		else projectilesPerShot = 1;
	}
	projectileAngle = 0.1f + (rand() % 25) * 0.01f;
	maxAngle = float(78 + rand() % 78) * 0.01f;
	projectileSpeed = 5 + rand() % 5;
	projectileStiffness = 3 + rand() % 3;
	t_type = rand() % 3;
	if(rand() % 5 == 1 && projectilesPerShot <= 5 && secondary == 0)
		motionblur = true;
	else motionblur = false;
	if(rand() % 10 == 1 && projectilesPerShot <= 2 && secondary == 1)
		motionblur = true;

	if(rand() % 3 == 1 && projectilesPerShot < 6 && secondary < 3) {
		t_event.eventActive = true;
		t_event.triggermask = 0;
		if(rand() % 3 == 1) {
			t_event.triggermask |= DEATH;
		}
		if(rand() % 2 == 1) {
			t_event.triggermask |= DISTANCE_TRAVELED;
			t_event.distance = 50 + rand() % 500;
		}
		if(rand() % 3 == 1) {
			t_event.triggermask |= ENEMY_RANGE;
			t_event.range = 50 + rand() % 300;
		}
		t_event.actionmask = 0;
		if(rand() % 2 == 1) t_event.actionmask |= DIE;
		t_event.actionmask |= SPAWN_SECONDARY;
		//if(secondaryWeapon) delete secondaryWeapon;
		secondaryWeapon = new ProjectileWeapon(game);
		secondaryWeapon->setSecondary(secondary+1);
		secondaryWeapon->randomizeAttributes(); 
		t_event.secondaryweapon = secondaryWeapon;
	} else {
		t_event.eventActive = false;
	}

	//if(secondary) t_event.eventActive = false;

	shootDelayTimer = 0.f;
	projectileSpawnTimer = 0.f;
	projectilesFired = 0;
	currentAngle = 0.f;
	firing = false;
}

void ProjectileWeapon::fire() {
	if(firing || delayTimer != 0) return;
	
	firing = true;
	shootDelayTimer = 1.f;
	projectileSpawnTimer = 0.f;
	projectilesFired = 0;
	currentAngle = 0.f;
	if(!secondary)
		delayTimer = 750.f;
	else delayTimer = 0.f;
}

void ProjectileWeapon::fireFromPos(vec2 pos_n) {
	pos = pos_n;
	projectilesFired = 0;
	currentAngle = 0.f;
	if((t_type == STRAIGHT || t_type == SINE || t_type == HOMING) && projectileAngle != 0.f) {
		Projectile* p = new Projectile(game);
		p->setPosition(pos);
		p->setSpeed(projectileSpeed);
		p->setTargetingType(STRAIGHT, vec2(1.f, 0.f));
		p->setProjectileSprite(projectileSprite);
		p->setFlag(MOTION_BLUR, motionblur);
		p->setFlag(ACTIVE, true);
		p->setProjectileEvent(t_event);
		projectilesFired++;

		for(float x=1.f; x<maxAngle*2; x += projectileAngle) {
			Projectile* p = new Projectile(game);
			p->setPosition(pos);
			p->setSpeed(projectileSpeed);
			p->setTargetingType(t_type, vec2(cos(x), sin(x)));
			p->setProjectileSprite(projectileSprite);
			p->setFlag(MOTION_BLUR, motionblur);
			p->setFlag(ACTIVE, true);
			p->setProjectileEvent(t_event);
			projectilesFired++;
			p = new Projectile(game);
			p->setPosition(pos);
			p->setSpeed(projectileSpeed);
			p->setTargetingType(t_type, vec2(cos(-x), sin(-x)));
			p->setProjectileSprite(projectileSprite);
			p->setFlag(MOTION_BLUR, motionblur);
			p->setFlag(ACTIVE, true);
			p->setProjectileEvent(t_event);
			projectilesFired++;
			if(projectilesFired >= projectilesPerShot) break;
		}
	}
}

void ProjectileWeapon::setPositionLookup(vec2* poslookup) {
	posLookup = poslookup;
}

void ProjectileWeapon::setPositionTranslation(vec2 postransl_n) {
	postransl = postransl_n;
}

// enemy weapon


EnemyWeapon::EnemyWeapon(Game* game_n) {
	game = game_n;
	game->addNode(this);
	posLookup = NULL;
	randomizeAttributes();
}

EnemyWeapon::~EnemyWeapon() {

}

void EnemyWeapon::update(float frameDelta) {
	GameNode::update(frameDelta);
	if(posLookup) {
		pos = *posLookup + posTransl;
	}
}

void EnemyWeapon::fire() {
	int projectilesFired = 0;
	for(float x=-maxAngle; x <maxAngle; x += projectileAngle) {
		EnemyProjectile* proj = new EnemyProjectile(game);
		proj->setPosition(pos);
		proj->setDirection(vec2(cos(x), sin(x)));
		proj->setSpeed(projectileSpeed);
		projectilesFired++;
		proj = new EnemyProjectile(game);
		proj->setPosition(pos);
		proj->setDirection(vec2(cos(-x), sin(-x)));
		proj->setSpeed(projectileSpeed);
		projectilesFired++;
		if(projectilesFired >= projectilesPerShot) return;
	}/*
	EnemyProjectile* proj = new EnemyProjectile(game);
	proj->setPosition(pos);
	proj->setDirection(vec2(-1.f, 0.f));
	proj->setSpeed(projectileSpeed);*/
}

void EnemyWeapon::setPositionLookup(vec2* poslookup) {
	posLookup = poslookup;
}

void EnemyWeapon::setPositionTranslation(vec2 postransl) {
	posTransl = postransl;
}

void EnemyWeapon::randomizeAttributes() {
	projectilesPerShot = 1 + rand()%4;
	projectileAngle = 0.1f + (rand() % 25) * 0.01f;
	maxAngle = float(156 + rand() % 156) * 0.01f;
	projectileSpeed = (1 + rand()%5);
}