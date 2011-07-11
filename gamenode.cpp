#include "headers.h"


GameNode::GameNode() {
	children = new std::list<GameNode*>;
	_markForDeletion = false;
	name = "root";
	type = NONE;
}

GameNode::~GameNode() {
	deleteChildren();
	delete children;
}

void GameNode::setName(char* newName) {
	if(name) delete name;
	name = new char[strlen(newName)+1];
	for(int x=0; x<strlen(newName); x++) name[x] = newName[x];
	name[strlen(newName)] = 0;
}

char* GameNode::getName() {
	return name;
}

void GameNode::addChild(GameNode* node) {	
	children->push_back(node);
}

void GameNode::deleteChild(GameNode* node) {
	for(std::list<GameNode*>::iterator x = children->begin(); x != children->end(); x++) {
		if(*x == node) {
			delete *x;
			children->erase(x);	
			return;
		}
	}
}

void GameNode::deleteChildren() {	
	for(std::list<GameNode*>::iterator x = children->begin(); x != children->end(); x++) {
     		delete (*x);
	}

	children->clear();
}

void GameNode::update(float frameDelta) {
	if(children->size() <= 0) return;
	
	for(std::list<GameNode*>::iterator x = children->begin(); x != children->end(); x++) {
		(*x)->update(frameDelta);
	}
	
	for(std::list<GameNode*>::iterator x = children->begin(); x != children->end(); x++) {
		if((*x)->_markForDeletion) {
			deleteChild(*x);
			return;
		}
	}
}

std::list<GameNode*>* GameNode::getChildren() {
	return children;	
}

int GameNode::getChildrenNum() {
	return children->size();	
}

int GameNode::getAbsoluteChildrenNum() {
	int n = children->size();
	for(std::list<GameNode*>::iterator x = children->begin(); x != children->end(); x++) {
		n += (*x)->getAbsoluteChildrenNum();
	}
	return n;
}

bool GameNode::doesNodeExist(GameNode* node) {
	for(std::list<GameNode*>::iterator x = children->begin(); x != children->end(); x++) {
		if(*x == node) return true;
	}
	return false;
}

GameNode* GameNode::findNodeByName(char* search_name) {
	// slow! do not use every frame!
	if(!strcmp(name, search_name)) return this;
	GameNode* tmp;
	for(std::list<GameNode*>::iterator x = children->begin(); x != children->end(); ++x) {
		tmp = (*x)->findNodeByName(search_name);
		if(tmp) return tmp;
	}
	return NULL;
}

GameNode* GameNode::findNodeByType(GameNodeType search_type) {
	// slow! do not use every frame!
	if(type == search_type) return this;
	GameNode* tmp;
	for(std::list<GameNode*>::iterator x = children->begin(); x != children->end(); ++x) {
		tmp = (*x)->findNodeByType(search_type);
		if(tmp) return tmp;
	}
	return NULL;
}

void* GameNode::getInfo(InfoType type) {
	Sprite* sprite;
	Collidable* collidable;

	switch(type) {
	case VEC2_POSITION:
		return &pos;
		break;
	case VEC2_SIZE:
		sprite = dynamic_cast<Sprite*>(this);
		if(!sprite) return NULL;
		return &(sprite->size);
		break;
	case BOUNDING_CIRCLE:
		collidable = dynamic_cast<Collidable*>(this);
		if(!collidable) return NULL;
		return &(collidable->circle);
		break;
	case AABB_LOCAL:
		collidable = dynamic_cast<Collidable*>(this);
		if(!collidable) return NULL;
		return &(collidable->aabb_local);
		break;
	case AABB_WORLD:
		collidable = dynamic_cast<Collidable*>(this);
		if(!collidable) return NULL;
		return &(collidable->aabb_world);
		break;
	case FLOAT_POSITION_X:
		return &(pos[0]);
		break;
	case FLOAT_POSITION_Y:
		return &(pos[1]);
		break;
	case INT_SIZE_X:
		sprite = dynamic_cast<Sprite*>(this);
	//	if(!sprite) return NULL;
		return &(sprite->frame_size_x);

		break;
	case INT_SIZE_Y:
		sprite = dynamic_cast<Sprite*>(this);
	//	if(!sprite) return NULL;
		return &(sprite->frame_size_y);
		break;
	}
	return NULL;
}

void GameNode::setPosition(vec2 pos_n) {
	pos = pos_n;
}

vec2 GameNode::getPosition() {
	return pos;
}