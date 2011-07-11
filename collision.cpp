#include "headers.h"

AABB::AABB() {
}

AABB::AABB(vec2 min_n, vec2 max_n) {
	min = min_n;
	max = max_n;
}

AABB::~AABB() {

}

bool AABB::pointCheck(vec2 point) {
	if(point[0] > min[0] && point[0] < max[0])
		if(point[1] > min[1] && point[1] < max[1])
			return true;
	return false;
}

BoundingCircle::BoundingCircle() {

}

BoundingCircle::BoundingCircle(vec2 pos_n, float radius_n) {
	pos = pos_n;
	radius = radius_n;
}

BoundingCircle::~BoundingCircle() {

}

bool BoundingCircle::pointCheck(vec2 point) {
	if((pos - point).len2() < radius*radius)
		return true;
	return false;
}

bool intersection_aabb2aabb(AABB a, AABB b, bool phase=false) {
	vec2 cornerA = a.min;
	vec2 cornerB = vec2(a.max[0], a.min[1]);
	vec2 cornerC = a.max;
	vec2 cornerD = vec2(a.min[0], a.max[1]);
	if(b.pointCheck(cornerA)) return true;
	if(b.pointCheck(cornerB)) return true;
	if(b.pointCheck(cornerC)) return true;
	if(b.pointCheck(cornerD)) return true;
	if(!phase) {
		return intersection_aabb2aabb(b, a, true);
	}
	return false;
}

void calculate_aabb_overlap(AABB a, AABB b, vec2* a_start_r, vec2* a_end_r, vec2* b_start_r, vec2* b_end_r) {
	vec2 a_center, b_center;
	a_center = (a.min + a.max) * 0.5f;
	b_center = (b.min + b.max) * 0.5f;

	vec2 a_v[4];
	vec2 b_v[4];
	a_v[0] = a.min;
	a_v[1] = vec2(a.max[0], a.min[1]);
	a_v[2] = a.max;
	a_v[3] = vec2(a.min[0], a.max[1]);
	b_v[0] = b.min;
	b_v[1] = vec2(b.max[0], b.min[1]);
	b_v[2] = b.max;
	b_v[3] = vec2(b.min[0], b.max[1]);

	float a_closest_dist = 9999999999.f;
	float b_closest_dist = 9999999999.f;
	vec2* a_closest,* b_closest;

	for(int x=0; x<4; x++) {
		float dist = (a_v[x] - b_center).len();
		if(dist < a_closest_dist) {
			a_closest_dist = dist;
			a_closest = &(a_v[x]);
		}
	}
	for(int x=0; x<4; x++) {
		float dist = (b_v[x] - a_center).len();
		if(dist < b_closest_dist) {
			b_closest_dist = dist;
			b_closest = &(b_v[x]);
		}
	}

	*a_start_r = *a_closest - a.min;
	*a_end_r = *b_closest - a.min;

	*b_start_r = *b_closest - b.min;
	*b_end_r = *a_closest - b.min;

	if((*a_start_r)[0] > (*a_end_r)[0]) {
		float tmp = (*a_start_r)[0];
		(*a_start_r)[0] = (*a_end_r)[0];
		(*a_end_r)[0] = tmp;
	}
	if((*a_start_r)[1] > (*a_end_r)[1]) {
		float tmp = (*a_start_r)[1];
		(*a_start_r)[1] = (*a_end_r)[1];
		(*a_end_r)[1] = tmp;
	}
	if((*b_start_r)[0] > (*b_end_r)[0]) {
		float tmp = (*b_start_r)[0];
		(*b_start_r)[0] = (*b_end_r)[0];
		(*b_end_r)[0] = tmp;
	}
	if((*b_start_r)[1] > (*b_end_r)[1]) {
		float tmp = (*b_start_r)[1];
		(*b_start_r)[1] = (*b_end_r)[1];
		(*b_end_r)[1] = tmp;
	}

	
	if((*a_start_r)[0] < 0.f) (*a_start_r)[0] = 0.f;
	if((*a_start_r)[1] < 0.f) (*a_start_r)[1] = 0.f;

	if((*b_start_r)[0] < 0.f) (*b_start_r)[0] = 0.f;
	if((*b_start_r)[1] < 0.f) (*b_start_r)[1] = 0.f;

	if((*a_end_r)[0] > a.max[0]-a.min[0]) (*a_end_r)[0] = a.max[0]-a.min[0];
	if((*a_end_r)[1] > a.max[1]-a.min[1]) (*a_end_r)[1] = a.max[1]-a.min[1];



	if((*b_end_r)[0] > b.max[0]-b.min[0]) (*b_end_r)[0] = b.max[0]-b.min[0];
	if((*b_end_r)[1] > b.max[1]-b.min[1]) (*b_end_r)[1] = b.max[1]-b.min[1];

	if(a.min[0] + (*a_start_r)[0] < b.min[0] + (*b_start_r)[0]) {
		(*a_start_r)[0] += b.min[0] + (*b_start_r)[0] - a.min[0] + (*a_start_r)[0];
	}

	if(a.min[0] + (*a_end_r)[0] > b.min[0] + (*b_end_r)[0]) {
		(*a_end_r)[0] += b.min[0] + (*b_end_r)[0] - a.min[0] - (*a_end_r)[0];
	}

	//if((*a_start_r)[0] < (*b_start_r)[0]) (*a_start_r)[0] = b.min[0];
}

bool collide_pixel_perfect(Tmpl8::Surface* a, Tmpl8::Surface* b, vec2 b_pos) {/*
//Tmpl8::Surface* tmp = new Surface(a->GetWidth(), a->GetHeight());
	for(int x=0; x<tmp->GetWidth()*tmp->GetHeight(); x++) {
		tmp->GetBuffer()[x] = 0;
	}
	a->CopyToColorkey(tmp, 0, 0);
	for(int x=0;x<b->GetWidth()*b->GetHeight(); x++) {
		if(tmp->GetBuffer()[x]
	}*/
	return false;
}

bool intersection_circle2circle(BoundingCircle a, BoundingCircle b) {
	if((a.pos - b.pos).len2() < a.radius*a.radius+2.f*a.radius*b.radius+b.radius*b.radius)
		return true;
	return false;
}

// collidable class methods

Collidable::Collidable(Game* game_n) {
	game = game_n;
	game->getCollisionMgr()->registerNode(this);
	collisionState = true;
	sprite = NULL;
	callbackEvent = NULL;
}

Collidable::~Collidable() {
	if(collisionState) game->getCollisionMgr()->unregisterNode(this);
}

void Collidable::setParams(AABB aabb_local_n, float circle_radius) {
	aabb_local.min = aabb_local_n.min;
	aabb_local.max = aabb_local_n.max;
	circle.radius = circle_radius;
}


void Collidable::setCollision(bool state) {
	if(state && !collisionState) {
		collisionState = true;
		game->getCollisionMgr()->registerNode(this);
	} else if(!state && collisionState) {
		collisionState = false;
		game->getCollisionMgr()->unregisterNode(this);
	}
}

void Collidable::setCollisionCallback(CollisionEvent* callback_event) {
	callbackEvent = callback_event;
}

void Collidable::receiveCollisionEvent(CollisionEvent newEvent) {
	if(callbackEvent)
		*callbackEvent = newEvent;
}

void Collidable::update(float frameDelta) {
	aabb_world.min = pos;
	aabb_world.max = VEC2_INFO(this, VEC2_SIZE) + pos;
	circle.pos = pos + VEC2_INFO(this, VEC2_SIZE)*0.5f;
}

void Collidable::setSprite(Sprite* sprite_n) {
	sprite = sprite_n;
}

Sprite* Collidable::getSprite() {
	return sprite;
}

// collision manager class methods

CollisionMgr::CollisionMgr(Game* game_n) {
	game = game_n;
	game->addNode(this);
}

CollisionMgr::~CollisionMgr() {

}

void CollisionMgr::update(float frameDelta) {
	/*for(std::list<Collidable*>::iterator x = nodes.begin(); x != nodes.end(); x++) {
		
	}*/
}


void CollisionMgr::registerNode(Collidable* node) {
	nodes.push_back(node);
	switch(node->type){ 
	case ENEMY:
		enemies.push_back(node);
		break;
	case PROJECTILE:
		projectiles.push_back(node);
		break;
	case POWERUP:
		powerups.push_back(node);
		break;
	}
}

void CollisionMgr::unregisterNode(Collidable* node) {
	switch(node->type) {
	case ENEMY:
		enemies.remove(node);
		break;
	case PROJECTILE:
		projectiles.remove(node);
		break;
	case POWERUP:
		powerups.remove(node);
		break;
	}

	nodes.remove(node);
}

CollisionEvent CollisionMgr::collideObject(Collidable* object, Sprite* sprite, int mode) {
	CollisionEvent c_event;
	c_event.hasCollided = false;

	BoundingCircle bc = *((BoundingCircle*)object->getInfo(BOUNDING_CIRCLE));
	AABB aabb = *((AABB*)object->getInfo(AABB_WORLD));

	
	if(mode&C_LEVEL) {	
		game->getTilemap()->setupCollidables(bc.pos);
		BoundingCircle** bcircles;
		AABB** aabbs;
		bcircles = game->getTilemap()->getBoundingCircles();
		aabbs = game->getTilemap()->getAABBs();
		int ptr = game->getTilemap()->getCollidablePtr();
		
		for(int x = 0; x < ptr; x++) {
		//	game->drawAABB(aabbs[x], 0xff0000);
			if(intersection_circle2circle(bc, *(bcircles[x]))) {
				
				if(intersection_aabb2aabb(aabb, *(aabbs[x]))) {
					vec2 a_start, a_wh, b_start, b_wh;

					calculate_aabb_overlap(aabb, *(aabbs[x]), &a_start, &a_wh, &b_start, &b_wh);

					a_wh -= a_start;
					b_wh -= b_start;

					Tmpl8::Surface* tmp = sprite->createCollisionArea(a_start[0], a_start[1], a_wh[0], a_wh[1]);
					Tmpl8::Surface* tmp2 = game->getTilemap()->createCollisionArea(2, b_start[0], b_start[1], b_wh[0], b_wh[1]);

					unsigned long* a_buffer = tmp->GetBuffer(), * b_buffer = tmp2->GetBuffer();
					
					// hard part - start comparing pixels. hopefully the overlapping bounding area is very small
					for(int x=0; x<tmp->GetWidth()*tmp->GetHeight(); x++) {
						if(a_buffer[x] != 0xff000000 && b_buffer[x] != 0xff000000) {
							unsigned int alpha_a = (a_buffer[x]&ALPHAMASK)>>24;
							unsigned int alpha_b = (b_buffer[x]&ALPHAMASK)>>24;
							if(alpha_a != 0 && alpha_b != 0) {
								c_event.hasCollided = true;
								break;
							}
						}
					}

					delete tmp;
					delete tmp2;

					if(c_event.hasCollided) break;
				}
			}
		}
	}

	if(mode&C_ENEMIES) {
		for(std::list<Collidable*>::iterator x = enemies.begin(); x != enemies.end(); x++) {
			if(intersection_circle2circle(bc, *(BoundingCircle*)(*x)->getInfo(BOUNDING_CIRCLE))) {
				if(intersection_aabb2aabb(aabb, *(AABB*)(*x)->getInfo(AABB_WORLD))) {
					vec2 a_start, a_wh, b_start, b_wh;

					calculate_aabb_overlap(aabb, *(AABB*)(*x)->getInfo(AABB_WORLD), &a_start, &a_wh, &b_start, &b_wh);

					a_wh -= a_start;
					b_wh -= b_start;

					Tmpl8::Surface* tmp = sprite->createCollisionArea(a_start[0], a_start[1], a_wh[0], a_wh[1]);
					Tmpl8::Surface* tmp2 = (*x)->getSprite()->createCollisionArea(b_start[0], b_start[1], b_wh[0], b_wh[1]);

					unsigned long* a_buffer = tmp->GetBuffer(), * b_buffer = tmp2->GetBuffer();
					
					// hard part - start comparing pixels. hopefully the overlapping bounding area is very small
					for(int i=0; i<tmp->GetWidth()*tmp->GetHeight(); i++) {
						if(a_buffer[i] != 0xff000000 && b_buffer[i] != 0xff000000) {
							unsigned int alpha_a = (a_buffer[i]&ALPHAMASK)>>24;
							unsigned int alpha_b = (b_buffer[i]&ALPHAMASK)>>24;
							if(alpha_a != 0 && alpha_b != 0) {
								c_event.hasCollided = true;
								(*x)->receiveCollisionEvent(c_event);
								break;
							}
						}
					}

					delete tmp;
					delete tmp2;

					if(c_event.hasCollided) break;
				}
			}
		}
	}

	if(mode&C_PLAYER) {
		BoundingCircle* player_bc = (BoundingCircle*)game->getHero()->getInfo(BOUNDING_CIRCLE);
		AABB* player_aabb = (AABB*)game->getHero()->getInfo(AABB_WORLD);
		if(intersection_circle2circle(bc, *player_bc)) {
			if(intersection_aabb2aabb(aabb, *player_aabb)) {
				vec2 a_start, a_wh, b_start, b_wh;
				calculate_aabb_overlap(aabb, *player_aabb, &a_start, &a_wh, &b_start, &b_wh);

				a_wh -= a_start;
				b_wh -= b_start;

				Tmpl8::Surface* tmp = sprite->createCollisionArea(a_start[0], a_start[1], a_wh[0], a_wh[1]);
				Tmpl8::Surface* tmp2 = game->getHero()->createCollisionArea(b_start[0], b_start[1], b_wh[0], b_wh[1]);

				unsigned long* a_buffer = tmp->GetBuffer(), * b_buffer = tmp2->GetBuffer();
					
				// hard part - start comparing pixels. hopefully the overlapping bounding area is very small
				for(int x=0; x<tmp->GetWidth()*tmp->GetHeight(); x++) {
					if(a_buffer[x] != 0xff000000 && b_buffer[x] != 0xff000000) {
						unsigned int alpha_a = (a_buffer[x]&ALPHAMASK)>>24;
						unsigned int alpha_b = (b_buffer[x]&ALPHAMASK)>>24;
						if(alpha_a != 0 && alpha_b != 0) {
							c_event.hasCollided = true;
							game->getHero()->receiveCollisionEvent(c_event);
							break;
						}
					}
				}

				delete tmp;
				delete tmp2;
			}
		}
	}

	if(mode&C_PROJECTILES) {
		for(std::list<Collidable*>::iterator x = projectiles.begin(); x != projectiles.end(); x++) {
			if(intersection_circle2circle(bc, *(BoundingCircle*)(*x)->getInfo(BOUNDING_CIRCLE))) {
				if(intersection_aabb2aabb(aabb, *(AABB*)(*x)->getInfo(AABB_WORLD))) {
					vec2 a_start, a_wh, b_start, b_wh;

					calculate_aabb_overlap(aabb, *(AABB*)(*x)->getInfo(AABB_WORLD), &a_start, &a_wh, &b_start, &b_wh);

					a_wh -= a_start;
					b_wh -= b_start;

					Tmpl8::Surface* tmp = sprite->createCollisionArea(a_start[0], a_start[1], a_wh[0], a_wh[1]);
					Tmpl8::Surface* tmp2 = (*x)->getSprite()->createCollisionArea(b_start[0], b_start[1], b_wh[0], b_wh[1]);

					unsigned long* a_buffer = tmp->GetBuffer(), * b_buffer = tmp2->GetBuffer();
					
					// hard part - start comparing pixels. hopefully the overlapping bounding area is very small
					for(int x=0; x<tmp->GetWidth()*tmp->GetHeight(); x++) {
						if(a_buffer[x] != 0xff000000 && b_buffer[x] != 0xff000000) {
							unsigned int alpha_a = (a_buffer[x]&ALPHAMASK)>>24;
							unsigned int alpha_b = (b_buffer[x]&ALPHAMASK)>>24;
							if(alpha_a != 0 && alpha_b != 0) {
								c_event.hasCollided = true;
								break;
							}
						}
					}

					delete tmp;
					delete tmp2;

					if(c_event.hasCollided) break;
				}
			}
		}
	}

	if(mode&C_POWERUPS) {
		for(std::list<Collidable*>::iterator x = powerups.begin(); x != powerups.end(); x++) {
			if(intersection_circle2circle(bc, *(BoundingCircle*)(*x)->getInfo(BOUNDING_CIRCLE))) {
				if(intersection_aabb2aabb(aabb, *(AABB*)(*x)->getInfo(AABB_WORLD))) {
					vec2 a_start, a_wh, b_start, b_wh;

					calculate_aabb_overlap(aabb, *(AABB*)(*x)->getInfo(AABB_WORLD), &a_start, &a_wh, &b_start, &b_wh);

					a_wh -= a_start;
					b_wh -= b_start;

					Tmpl8::Surface* tmp = sprite->createCollisionArea(a_start[0], a_start[1], a_wh[0], a_wh[1]);
					Tmpl8::Surface* tmp2 = (*x)->getSprite()->createCollisionArea(b_start[0], b_start[1], b_wh[0], b_wh[1]);

					unsigned long* a_buffer = tmp->GetBuffer(), * b_buffer = tmp2->GetBuffer();
					
					// hard part - start comparing pixels. hopefully the overlapping bounding area is very small
					for(int i=0; i<tmp->GetWidth()*tmp->GetHeight(); i++) {
						if(a_buffer[i] != 0xff000000 && b_buffer[i] != 0xff000000) {
							unsigned int alpha_a = (a_buffer[i]&ALPHAMASK)>>24;
							unsigned int alpha_b = (b_buffer[i]&ALPHAMASK)>>24;
							if(alpha_a != 0 && alpha_b != 0) {
								c_event.hasCollided = true;
								(*x)->receiveCollisionEvent(c_event);
								break;
							}
						}
					}

					delete tmp;
					delete tmp2;

					if(c_event.hasCollided) break;
				}
			}
		}
	}

	return c_event;
}

CollisionEvent CollisionMgr::collidePoint(vec2 p, int mode) {
	CollisionEvent c_event;
	c_event.hasCollided = false;
	BoundingCircle bc;
	bc.pos = p;
	bc.radius = 1.f;

	if(mode&C_LEVEL) {
		game->getTilemap()->setupCollidables(bc.pos);
		BoundingCircle** bcircles;
		bcircles = game->getTilemap()->getBoundingCircles();
		int ptr = game->getTilemap()->getCollidablePtr();
		
		for(int x = 0; x < ptr; x++) {
			if(intersection_circle2circle(bc, *(bcircles[x]))) {
				c_event.hasCollided = true;
				break;
			}
		}
	}

	return c_event;
}