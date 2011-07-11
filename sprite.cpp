#include "headers.h"

Sprite::Sprite(Game* game_n) {
	game = game_n;
	flags = 0;
	nextFrameTimer = 0.f;
	animationFinishedCallback = NULL;
	currentFrame = 0;
	startFrame = 0;
	endFrame = 0;
	animRunning = false;
}

Sprite::~Sprite() {

}

void Sprite::update(float frameDelta) {
	frameCounter++;
	if(frameCounter > 100) frameCounter = 100;
	if(!(flags&ACTIVE)) return;

	vec2 cameraPos = game->getCameraPos();

	if(flags&MOTION_BLUR) {
		if(frameCounter % 3 == 1) {
			lastPos.push_back(pos);
			if(lastPos.size() > 3
				) lastPos.pop_front();
		}
		int alpha = -200;
		for(std::list<vec2>::iterator x = lastPos.begin(); x != lastPos.end(); x++) {
			frames[currentFrame]->CopyToAlphablend(game->getScreen(), (*x)[0] - cameraPos[0], (*x)[1] - cameraPos[1], alpha);
			alpha += 50;
		}
	}

	if(pos[0] > cameraPos[0]-10.f && pos[0] < cameraPos[0]+SCRWIDTH+10.f) {
 		frames[currentFrame]->CopyToAlphablend(game->getScreen(), pos[0] - cameraPos[0], pos[1] - cameraPos[1], -35);
	}

	if(!animRunning) return;

	if(nextFrameTimer > 0.f) {
		nextFrameTimer -= frameDelta;
		if(nextFrameTimer <= 0.f) {
			nextFrameTimer = 10000.f/animSpeed;
			currentFrame++;
		}
	}

	if(currentFrame > endFrame) {
		if(animType == LOOP) {
			currentFrame = startFrame;
		} else if(animType == ONCE) {
			currentFrame = endFrame;
		} else {
			currentFrame = startFrame;
		}
		if(animationFinishedCallback)
			*animationFinishedCallback = true;
	}

	if(currentFrame >= frames_n) {
		currentFrame = startFrame;
	}
}

void Sprite::setTileset(char* path) {
	frames = game->getTextureMgr()->getTileset(path, &frame_size_x, &frame_size_y, &frames_n);
	size.set(frame_size_x, frame_size_y);
}

void Sprite::setAnimation(SpriteAnimType type, int start_frame, int end_frame, float animSpeed_n) {
	animType = type;
	startFrame = start_frame;
	endFrame = end_frame;
	animSpeed = animSpeed_n;
	currentFrame = startFrame;
	nextFrameTimer = 10000.f/animSpeed;
	animRunning = true;
}

void Sprite::setAnimationCallback(bool* animationFinishedCallback_n) {
	animationFinishedCallback = animationFinishedCallback_n;
}

void Sprite::setAnimationState(bool state_n) {
	animRunning = state_n;
}

void Sprite::setFrame(int frame) {
	currentFrame = frame;
	animRunning = false;
}

Tmpl8::Surface* Sprite::createCollisionArea(int x, int y, int width, int height) {
	Tmpl8::Surface* area = new Tmpl8::Surface(width, height);
	frames[currentFrame]->CopyAreaTo(area, 0, 0, x, y, width, height);
	return area;
}