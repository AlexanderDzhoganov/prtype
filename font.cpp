#include "headers.h"

Font::Font(Game* game_n) {
	game = game_n;
	game->addNode(this);
	fnt10 = new Tmpl8::Surface("assets\\font_10.bmp");
	fnt14 = new Tmpl8::Surface("assets\\font_14.bmp");
	fnt18 = new Tmpl8::Surface("assets\\font_18.bmp");
	fnt24 = new Tmpl8::Surface("assets\\font_24.bmp");
	fnt = fnt14;
}

Font::~Font() {

}

void Font::update(float frameDelta) {

}

void Font::printText(char* text, vec2 pos, int size, unsigned int color) {
	switch(size) {
	case 10:
		fnt = fnt10;
		break;
	case 14:
		fnt = fnt14;
		break;
	case 18:
		fnt = fnt18;
		break;
	case 24:
		fnt = fnt24;
		break;
	default:
		fnt = fnt14;
		break;
	}

	int textlen = strlen(text);

	for(int i=0; i<textlen; i++) {
		unsigned char c = (unsigned char)text[i];
		int y = c / 16;
		int x = c - y*16;
		if(size == 24) {
			fnt->CopyAreaToColorkey(game->getScreen(), pos[0]+i*14, pos[1], x*32, y*32, 32, 32);
		} else if(size == 18) {
			fnt->CopyAreaToColorkey(game->getScreen(), pos[0]+i*16, pos[1], x*32, y*32, 32, 32);
		} else if(size == 14) {
			fnt->CopyAreaToColorkey(game->getScreen(), pos[0]+i*12, pos[1], x*32, y*32, 32, 32);
		} else {
			fnt->CopyAreaToColorkey(game->getScreen(), pos[0]+i*8, pos[1], x*32, y*32, 32, 32);
		}
	}
}