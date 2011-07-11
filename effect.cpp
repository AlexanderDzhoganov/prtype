#include "headers.h"

// effect class

Effect::Effect(Game* game_n) : Sprite(game_n) {
	game = game_n;
	animEnded = false;
}

Effect::~Effect() {

}

void Effect::setType(EFFECT_TYPE type) {
	effect_type = type;
	switch(effect_type) {
	case EFFECT_TYPE_EXPLOSION:
		setTileset("assets\\explosion.png");
		setAnimation(ONCE, 0, 15, 500.f);
		setAnimationCallback(&animEnded);
		animEnded = false;
		flags |= ACTIVE;
		break;
	}
}

void Effect::setPosition(vec2 pos_n) {
	pos = pos_n;
}

void Effect::update(float frameDelta) {
	GameNode::update(frameDelta);
	Sprite::update(frameDelta);

	if(animEnded) {
		_markForDeletion = true;
	}
}

// effect mgr class

EffectMgr::EffectMgr(Game* game_n) {
	game = game_n;
	game->addNode(this);
}

EffectMgr::~EffectMgr() {

}

void EffectMgr::update(float frameDelta) {
	GameNode::update(frameDelta);
}

void EffectMgr::spawnEffect(EFFECT_TYPE type, vec2 pos) {
	Effect* effect = new Effect(game);
	effect->setType(type);
	effect->setPosition(pos);
	game->addNode(effect, this);
}