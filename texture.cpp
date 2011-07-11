#include "headers.h"

TextureMgr::TextureMgr(Game* game_n) {
	game = game_n;
	tileset_n = 0;
	loadNewTileset("assets\\spaceship2.png", 1, 1, 42, 37);
	loadNewTileset("assets\\explosion.png", 4, 4, 48, 48);
	loadNewTileset("assets\\projectile_basic.bmp", 2, 1, 16, 16);
	loadNewTileset("assets\\enemies.png", 8, 80, 36, 36);
	loadNewTileset("assets\\projectiles.png", 8, 16, 32, 32);
	loadNewTileset("assets\\shield.png", 4, 1, 60, 60);
	loadNewTileset("assets\\powerup.png", 8, 2, 32, 32);
	loadNewTileset("assets\\enemy_projectile.png", 1, 1, 8, 8);
}

TextureMgr::~TextureMgr() {

}

bool TextureMgr::loadNewTileset(char* path, int frames_n_x, int frames_n_y, int frame_size_x, int frame_size_y) {
	Tmpl8::Surface** frames = new Tmpl8::Surface*[frames_n_x*frames_n_y];
	int frames_n = 0;
	
	Tmpl8::Surface* tileset = new Tmpl8::Surface(path);

	for(int y=0; y<frames_n_y; y++)
	for(int x=0; x<frames_n_x; x++) {
		frames[frames_n] = new Tmpl8::Surface(frame_size_x, frame_size_y);
		tileset->CopyAreaTo(frames[frames_n], 0, 0, x*frame_size_x, y*frame_size_y, frame_size_x, frame_size_y);
		frames_n++;
	}

	delete tileset;

	tilesets[tileset_n] = frames;
	size_x[tileset_n] = frame_size_x;
	size_y[tileset_n] = frame_size_y;
	tileset_frames_n[tileset_n] = frames_n;
	tileset_paths[tileset_n] = path;
	tileset_n++;

	

	return true;
}

Tmpl8::Surface** TextureMgr::getTileset(char* filepath, int* ret_size_x, int* ret_size_y, int* ret_frames_n) {
	for(int x=0; x<tileset_n; x++) {
		if(!strcmp(filepath, tileset_paths[x])) {
			
			if(ret_size_x) *ret_size_x = size_x[x];
			if(ret_size_y) *ret_size_y = size_y[x];
			if(ret_frames_n) *ret_frames_n = tileset_frames_n[x];
			return tilesets[x];
		}
	}
	return NULL;
}