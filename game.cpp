#include "headers.h"


// score manager class methods

ScoreMgr::ScoreMgr(Game* game_n) {
	game = game_n;
	//game->addNode(this);
	ckeyval = new CKeyVal();
	hslist = NULL;
	scoresAvail = false;
}

ScoreMgr::~ScoreMgr() {

}

bool ScoreMgr::updateScores() {
	hslist = (HighScores*)ckeyval->getValue(OPENKEYVAL_HIGHSCORES_KEY);
	if(!hslist) scoresAvail = false;
	else scoresAvail = true;
	return scoresAvail;
}

bool ScoreMgr::scoresAvailable() {
	return scoresAvail;
}	

void ScoreMgr::update(float frameDelta) {

}

bool ScoreMgr::checkHighScore(int score) {
	if(!updateScores()) return false;

	int insertLoc = -1;

	for(int x=0; x<10; x++) {
		if(score > hslist->score[x]) {
			insertLoc = x;
			break;
		}
	}
	
	if(insertLoc != -1) {
		char username[100];
		DWORD size = sizeof(username);
		if(!GetUserName(username, &size)) {
			strcpy(username, "Unknown");
		}

		char* t_name = new char[128];
		strcpy(t_name, hslist->name[insertLoc]);
		int t_score = hslist->score[insertLoc];
		username[13] = 0;
		strcpy(hslist->name[insertLoc], username);
		hslist->score[insertLoc] = score;

		for(int x=insertLoc+1; x<10; x++) {
			strcpy(hslist->name[x], t_name);
			hslist->score[x] = t_score;
			if(x!=9) {
				strcpy(t_name, hslist->name[x+1]);
				t_score = hslist->score[x+1];
			}
		}

		delete [] t_name;

		ckeyval->setValue(OPENKEYVAL_HIGHSCORES_KEY, (unsigned char*)hslist, sizeof(HighScores));
		return true;
	}
	return false;
}

void ScoreMgr::createEmptyHSList() {
	HighScores hs;
	for(int x=0; x<10; x++) {
		strcpy(hs.name[x], "Beat me!");
		hs.score[x] = 20-x;
	}
	ckeyval->setValue(OPENKEYVAL_HIGHSCORES_KEY, (unsigned char*)&hs, sizeof(hs));
}

HighScores* ScoreMgr::getHighScores() {
	return hslist;
}

// game class methods

void Game::Init() {
	
	srand (time(NULL));
	frameCounter = 0;
	timeAccumulator = 5000.f;

	scoremgr = new ScoreMgr(this);
//	scoremgr->createEmptyHSList();
	scoremgr->updateScores();

	scenegraph = new GameNode();
	collisionmgr = new CollisionMgr(this);
	background = new Background(this);

	font = new Font(this);


	keymask = 0;
	mousemask = 0;
	state = STATE_NONE;
	setState(STATE_START);
}

void Game::enterStateStart() {
	
}

void Game::enterStateInGame() {
	background->setPosition(vec2(0.f, 0.f));
	cameraPos.set(0.f, 0.f);
	tilemap = new Tilemap(this);
	tilemap->createTileset("assets\\tileset.bmp", 4, 5, 32, 32);
	tilemap->generateLevel();

	texmgr = new TextureMgr(this);
	effect = new EffectMgr(this);

	hero = new Hero(this);
	enemyspawn = new EnemySpawner(this);
	powerupspawn = new PowerupSpawner(this);
	achievedHighScore = false;
}

void Game::enterStateGameOver() {
	//delNode(powerupspawn);

	if(scoremgr->scoresAvailable()) {
		achievedHighScore = scoremgr->checkHighScore(hero->getScore());
	}

//	
}

void Game::Tick(float frameDelta) {
	m_Screen->Clear(0);
	calculateFPS(frameDelta);
	scenegraph->update(frameDelta);

	cameraPos += vec2(0.3f, 0.f) * frameDelta;
	
	
	if(state == STATE_GAME_OVER) {
		updateGameOverScreen();
	}
	
	
	if(state == STATE_START)
		updateStartScreen();
}

void Game::updateStartScreen() {
	if(keymask&KEY_ENTER) {
		setState(STATE_IN_GAME);
		return;
	}
	font->printText("PR-Type v1.0", vec2(SCRWIDTH*0.5f-100.f, 10.f), 24.f);
	font->printText("by", vec2(SCRWIDTH*0.5f-30.f, 40.f), 14.f);
	font->printText("Alexander Dzhoganov", vec2(SCRWIDTH*0.5f-140.f, 70.f), 18.f);
	font->printText("an attempt at a procedurally generated side-scrolling shooter", vec2(SCRWIDTH*0.5f-255.f, 100.f), 10.f);

	font->printText("Controls:", vec2(SCRWIDTH*0.5f-255.f, 135.f), 14.f);
	font->printText("Arrow keys or WSAD - movement", vec2(SCRWIDTH*0.5f-255.f, 155.f), 10.f);
	font->printText("Z - fire weapon", vec2(SCRWIDTH*0.5f-255.f, 175.f), 10.f);
	font->printText("Enter to start, ESC to exit", vec2(SCRWIDTH*0.5f-255.f, 195.f), 10.f);

	font->printText("________________________", vec2(SCRWIDTH*0.5f-255.f, 230.f), 14.f);
	font->printText("TOP 10 WORLD HIGH SCORES", vec2(SCRWIDTH*0.5f-255.f, 250.f), 14.f);
	font->printText("________________________", vec2(SCRWIDTH*0.5f-255.f, 255.f), 14.f);

	if(!scoremgr->scoresAvailable()) {
		font->printText("High scores not available (No internet connection)", vec2(SCRWIDTH*0.5f-255.f, 290.f), 10.f);
	} else {
		for(int x=0; x<10; x++) {
			char* text = new char[128];
			HighScores* hslist = scoremgr->getHighScores();
			char* spaces = new char [32];
			strcpy(spaces, "                   ");
			if(x!=9)
				spaces[10+strlen(hslist->name[x])+3] = 0;
			else spaces[10+strlen(hslist->name[x])+2] = 0;
			sprintf(text, "%i. %s%s%i", x+1, hslist->name[x], spaces, hslist->score[x]);
			font->printText(text, vec2(SCRWIDTH*0.5f-255.f, 290.f+x*15.f), 10.f);
			delete [] spaces;
			delete [] text;
		}
	}
}

void Game::updateGameOverScreen() {
	font->printText("GAME OVER", vec2(SCRWIDTH*0.5f-85.f, SCRHEIGHT*0.5f-20.f), 24.f);
	char* scoretxt = new char[64];
	sprintf(scoretxt, "FINAL SCORE: %0.f", hero->getScore());
	font->printText(scoretxt, vec2(SCRWIDTH*0.5f-130.f, SCRHEIGHT*0.5f+12.f), 18.f);
	delete [] scoretxt;

	if(scoremgr->scoresAvailable() && achievedHighScore) {
		font->printText("Congratulations! You achieved a new high score!", vec2(SCRWIDTH*0.5f-190.f, 290.f), 10.f);
	} else if(!scoremgr->scoresAvailable()) {
		font->printText("Online scores not available (No internet connection)", vec2(SCRWIDTH*0.5f-190.f, 290.f), 10.f);
	}

	font->printText("Press enter to exit", vec2(SCRWIDTH*0.5f-125.f, 350.f), 14.f);
	if(keymask&KEY_ENTER) {
		exit(0);
	}
}

void Game::calculateFPS(float frameDelta) {
	frameCounter++;

	timeAccumulator += frameDelta;
	if(timeAccumulator >= 1000.f) {
		fps = float(frameCounter);
		char* title = new char[128];
		sprintf(title, "PR-Type v1.0 - FPS: %0.f, ENTITIES: %i", fps, scenegraph->getAbsoluteChildrenNum());
		SDL_WM_SetCaption(title, NULL);
		delete [] title;
		frameCounter = 0;
		timeAccumulator -= 1000.f;
	}
}

void Game::addNode(GameNode* node_n, GameNode* parent) {
	if(parent) {
		parent->addChild(node_n);
		node_n->parent = parent;
	} else {
		scenegraph->addChild(node_n);
		node_n->parent = scenegraph;
	}
}

void Game::delNode(GameNode* node) {
	node->parent->deleteChild(node);
}

void Game::KeyDown( unsigned int code ) {
	switch(code) {
	case 22:
	case 90:
		keymask |= DOWN;
		break;
	case 26:
	case 96:
		keymask |= UP;
		break;
	case 4:
	case 92:
		keymask |= LEFT;
		break;
	case 7:
	case 94:
		keymask |= RIGHT;
		break;
	case 29:
		keymask |= KEY_Z;
		break;
	case 27:
		keymask |= KEY_X;
		break;
	case 40:
		keymask |= KEY_ENTER;
		break;
	}
}

void Game::KeyUp( unsigned int code ) {
	switch(code) {
	case 22:
	case 90:
		keymask ^= DOWN;
		break;
	case 26:
	case 96:
		keymask ^= UP;
		break;
	case 4:
	case 92:
		keymask ^= LEFT;
		break;
	case 7:
	case 94:
		keymask ^= RIGHT;
		break;
	case 29:
		keymask ^= KEY_Z;
		break;
	case 27:
		keymask ^= KEY_X;
		break;
	case 40:
		keymask ^= KEY_ENTER;
		break;
	}
}

void Game::MouseMove(unsigned int x, unsigned int y) {
	mousePos[0] = x;
	mousePos[1] = y;
}

void Game::MouseUp(unsigned int button) {
	switch(button) {
	case 1:
		mousemask ^= LEFT_MB;
		break;
	case 3:
		mousemask ^= RIGHT_MB;
		break;
	}
}

void Game::MouseDown(unsigned int button) { 
	switch(button) {
	case 1:
		mousemask |= LEFT_MB;
		break;
	case 3:
		mousemask |= RIGHT_MB;
		break;
	}
}

Tmpl8::Surface* Game::getScreen() {
	return m_Screen;
}

Tilemap* Game::getTilemap() {
	return tilemap;
}

vec2 Game::getCameraPos() {
	return cameraPos;
}

int Game::getKeymask() {
	return keymask;
}

int Game::getMousemask() {
	return mousemask;
}

vec2 Game::getMousePos() {
	return mousePos;
}

CollisionMgr* Game::getCollisionMgr() {
	return collisionmgr;
}

TextureMgr* Game::getTextureMgr() {
	return texmgr;
}

EffectMgr* Game::getEffectMgr() {
	return effect;
}

Hero* Game::getHero() {
	return hero;
}

Font* Game::getFont() {
	return font;
}

EnemySpawner* Game::getEnemySpawner() {
	return enemyspawn;
}

void Game::setState(GAME_STATE state_n) {
	state = state_n;
	switch(state) {
	case STATE_NONE:
		break;
	case STATE_START:
		enterStateStart();
		break;
	case STATE_IN_GAME:
		enterStateInGame();
		break;
	case STATE_GAME_OVER:
		enterStateGameOver();
		break;
	}
}

GAME_STATE Game::getState() {
	return state;
}

void Game::drawPoint(vec2 p, unsigned long color) {
	drawCircle(p, 5.f, color);
}

void Game::drawCircle(vec2 c, float r, unsigned long color) {
	vec2 pos;
	for(float x = -PI; x <= PI; x += 0.1f) {
		pos[0] = r*cos(x);
		pos[1] = r*sin(x);
		pos += c;
		m_Screen->Plot(floor(pos[0]), floor(pos[1]), color);
	}
}

void Game::drawBox(vec2 min, vec2 max, unsigned long color) {
	vec2 v1, v2, v3, v4;
	v1 = min;
	v2 = vec2(min[0], max[1]);
	v3 = max;
	v4 = vec2(max[0], min[1]);

	m_Screen->Line(v1[0], v1[1], v2[0], v2[1], color);
	m_Screen->Line(v2[0], v2[1], v3[0], v3[1], color);
	m_Screen->Line(v3[0], v3[1], v4[0], v4[1], color);
	m_Screen->Line(v1[0], v1[1], v4[0], v4[1], color);
}

void Game::drawAABB(AABB aabb, unsigned long color) {
	vec2 v1, v2, v3, v4;
	v1 = aabb.min;
	v2 = vec2(aabb.min[0], aabb.max[1]);
	v3 = aabb.max;
	v4 = vec2(aabb.max[0], aabb.min[1]);

	m_Screen->Line(v1[0], v1[1], v2[0], v2[1], color);
	m_Screen->Line(v2[0], v2[1], v3[0], v3[1], color);
	m_Screen->Line(v3[0], v3[1], v4[0], v4[1], color);
	m_Screen->Line(v1[0], v1[1], v4[0], v4[1], color);
}