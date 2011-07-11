#include "headers.h"


Background::Background(Game* game_n) {
	game = game_n;
	game->addNode(this);

	while(stars.size() < 500) {
		Star star;
		star.alpha = 0.f;
		star.alphaMax = 0.1f;
		star.alphaMin = 0.f;
		star.increasingAlpha = true;
		star._markForDeletion = false;
		star.pos = vec2(rand()%SCRWIDTH, rand()%SCRHEIGHT);
		stars.push_back(star);
	} 
}

Background::~Background() {

}

void Background::update(float frameDelta) {
	vec2 cameraPos = game->getCameraPos();

	while(stars.size() < 500) {
		Star star;
		star.alpha = 0.f;
		star.alphaMax = rand()%100 * 0.01f;
		star.alphaMin = rand()%25 * 0.01f;
		star.increasingAlpha = true;
		star._markForDeletion = false;
		star.pos = vec2(cameraPos[0]+rand()%(SCRWIDTH+100), rand()%SCRHEIGHT);
		stars.push_back(star);
	}

	for(std::list<Star>::iterator x = stars.begin(); x != stars.end(); x++) {
		if((*x).pos[0] < cameraPos[0] || (*x).pos[0] > cameraPos[0]+SCRWIDTH) {
			(*x)._markForDeletion = true;
		}
		if((*x).increasingAlpha) {
			(*x).alpha += rand()%3*0.003f*frameDelta;
			if((*x).alpha > (*x).alphaMax) {
				(*x).alpha = (*x).alphaMax;
				(*x).increasingAlpha = false;
			}
		} else {
			(*x).alpha -= rand()%3*0.003f*frameDelta;
			if((*x).alpha < (*x).alphaMin) {
				(*x).alpha = (*x).alphaMin;
				(*x).increasingAlpha = true;
			}
		}
	} 

	for(std::list<Star>::iterator x = stars.begin(); x != stars.end(); x++) {
		if((*x)._markForDeletion) {
			stars.erase(x);
			break;
		}
	}

	for(std::list<Star>::iterator x = stars.begin(); x != stars.end(); x++) {
		unsigned int c = (255.f*(*x).alpha);
		game->getScreen()->Plot((*x).pos[0]-cameraPos[0], (*x).pos[1]-cameraPos[1], c<<16|c<<8|c);
	}

}

// tilemap class methods

enum GRID_VALUE {
	EMPTY = 0, // to be painted with background tiles
	SOLID, // solid block
	EDGE, // edge block
};

Tilemap::Tilemap(Game* game_n) {
	game = game_n;
	tileset = NULL;
	tiles = NULL;
	tiles_n = 0;
	for(int x=0; x<GRID_SIZE_X; x++)
	for(int y=0; y<GRID_SIZE_Y; y++) {
			grid[x][y] = 0;
	}

	// finally, register with the scenegraph to receive updates
	game->addNode(this);
	collidable_ptr = 0;
	aabb = new AABB*[128];
	for(int x=0;x<128;x++) {
		aabb[x] = NULL;
	}
	aabb2 = aabb;

	bcircles = new BoundingCircle*[128];
	for(int x=0;x<128;x++) {
		bcircles[x] = NULL;
	}
}

Tilemap::~Tilemap() {
	
}

void Tilemap::update(float frameDelta) {
	drawSelf();
}

void Tilemap::drawSelf() {
	vec2 cameraPos = game->getCameraPos();
	int start_x, end_x;
	start_x = cameraPos[0] / tile_size_x;
	end_x = (cameraPos[0] + SCRWIDTH) / tile_size_x + 2;
	if(start_x < 0.f) start_x = 0.f;
	if(end_x >= GRID_SIZE_X) end_x = GRID_SIZE_X - 1;

	for(int x=start_x; x<end_x; x++)
	for(int y=0; y<GRID_SIZE_Y; y++) {
		if(grid[x][y] != 0)
			tiles[grid[x][y]]->CopyTo(game->getScreen(), x*tile_size_x - cameraPos[0], y*tile_size_y - cameraPos[1]);
	}
}

void Tilemap::createTileset(char* image_path, int tiles_x, int tiles_y, int tile_size_x_n, int tile_size_y_n) {
	tiles = new Tmpl8::Surface*[tiles_x*tiles_y];
	tiles_n = 0;
	tileset = new Tmpl8::Surface(image_path);
	tile_size_x = tile_size_x_n;
	tile_size_y = tile_size_y_n;
	for(int y=0; y<tiles_y; y++)
	for(int x=0; x<tiles_x; x++) {
		tiles[tiles_n] = new Tmpl8::Surface(tile_size_x, tile_size_y);
		tileset->CopyAreaTo(tiles[tiles_n], 0, 0, x*tile_size_x, y*tile_size_y, tile_size_x, tile_size_y);
		tiles_n++;
	}
}

void Tilemap::generateLevel() {
	int* ceiling = new int[GRID_SIZE_X];
	int* floor = new int[GRID_SIZE_X];

	for(int x=0; x<GRID_SIZE_X; x++) {
		if(x < 16) {
			ceiling[x] = 4;
			floor[x] = 4;
			continue;
		}
		ceiling[x] = rand() % int(GRID_SIZE_Y*0.4f);
		floor[x] = 1 + rand() % int(GRID_SIZE_Y*0.4f);
		if(rand() % 10 == 1) ceiling[x] += 5;
		else if(rand() % 10 == 1) floor[x] += 5;
	}

	float* tent = generateTentKernel(1.f);
	ceiling = convolute(ceiling, GRID_SIZE_X, tent, 8);
	
	tent = generateTentKernel(1.f);
	floor = convolute(floor, GRID_SIZE_X, tent, 8);

	float* box = generateBoxKernel();
	ceiling = convolute(ceiling, GRID_SIZE_X, box, 4);

	box = generateBoxKernel();
	floor = convolute(floor, GRID_SIZE_X, box, 4);

	for(int x=0; x<GRID_SIZE_X; x++) {
		for(int y=0; y<ceiling[x]; y++) {
			if(rand() % 10 == 1)
				grid[x][y] = 1 + rand() % 3;
			else grid[x][y] = 1;
		}

	}
	for(int x=0; x<GRID_SIZE_X; x++) {
		for(int y=0; y<floor[x]; y++) {
			if(rand() % 10 == 1)
				grid[x][GRID_SIZE_Y-y] = 1 + rand() % 3;
			else grid[x][GRID_SIZE_Y-y] = 1;
		}
	}
	

	// edge detection

	for(int y=0; y<GRID_SIZE_Y; y++)
	for(int x=0; x<GRID_SIZE_X; x++) {
		int ri = rand() % 3;
		if(grid[x][y] > 0) {
			int neighborA = 0, neighborB = 0, neighborC = 0, neighborD = 0;
			if(y-1 < 0) neighborA = 1;
			else neighborA = grid[x][y-1];
			if(x+1 >= GRID_SIZE_X) neighborB = 1;
			else neighborB = grid[x+1][y];
			if(y+1 >= GRID_SIZE_Y) neighborC = 1;
			else neighborC = grid[x][y+1];
			if(x-1 < 0) neighborD = 1;
			else neighborD = grid[x-1][y];
			if(neighborA && neighborB && !neighborC && !neighborD) {
				if(ri == 0) {
					grid[x][y] = 4;
				} else if(ri == 1) {
					grid[x][y] = 8;
				} else {
					grid[x][y] = 12;
				}
			} else if(neighborA && !neighborB && !neighborC && neighborD) {
				if(ri == 0) {
					grid[x][y] = 5;
				} else if(ri == 1) {
					grid[x][y] = 9;
				} else {
					grid[x][y] = 13;
				}
			} else if(!neighborA && neighborB && neighborC && !neighborD) {
				if(ri == 0) {
					grid[x][y] = 6;
				} else if(ri == 1) {
					grid[x][y] = 10;
				} else {
					grid[x][y] = 14;
				}
			} else if(!neighborA && !neighborB && neighborC && neighborD) {
				if(ri == 0) {
					grid[x][y] = 7;
				} else if(ri == 1) {
					grid[x][y] = 11;
				} else {
					grid[x][y] = 15;
				}
			} else if(!neighborA) {
				if(rand() % 10 == 1)
					grid[x][y] = 17;
				else grid[x][y] = 16;
			} else if(!neighborC) {
				if(rand() % 10 == 1)
					grid[x][y] = 19;
				else grid[x][y] = 18;
			}
		}
	}
}

void Tilemap::setupCollidables(vec2 pos) {
	aabb = aabb2;
	/*aabb = new AABB*[128];
	for(int x=0;x<128;x++) {
		aabb[x] = NULL;
	}*/
	collidable_ptr = 0;

	int tile_x = pos[0]/tile_size_x;
	int tile_y = pos[1]/tile_size_y;
	int start_x, end_x, start_y, end_y;
	start_x = tile_x - TILEMAP_COLLISION_EPSILON - 1;
	end_x = tile_x + TILEMAP_COLLISION_EPSILON + 1;
	start_y = tile_y - TILEMAP_COLLISION_EPSILON;
	end_y = tile_y + TILEMAP_COLLISION_EPSILON;

	if(start_x < 0) start_x = 0;
	if(end_x >= GRID_SIZE_X) end_x = GRID_SIZE_X - 1;
	if(start_y < 0) start_y = 0;
	if(end_y >= GRID_SIZE_Y) end_y = GRID_SIZE_Y;

	for(int x = start_x; x < end_x; x++) {
		for(int y = start_y; y < end_y; y++) {
			if(grid[x][y] > 0) {
				if(aabb[collidable_ptr]) delete aabb[collidable_ptr];
				aabb[collidable_ptr] = new AABB();
				if(bcircles[collidable_ptr]) delete bcircles[collidable_ptr];
				bcircles[collidable_ptr] = new BoundingCircle();
				aabb[collidable_ptr]->min[0] = x*tile_size_x;
				aabb[collidable_ptr]->min[1] = y*tile_size_y;
				aabb[collidable_ptr]->max[0] = (x+1)*tile_size_x;
				aabb[collidable_ptr]->max[1] = (y+1)*tile_size_y;
				bcircles[collidable_ptr]->pos = vec2((x+0.5f)*tile_size_x, (y+0.5f)*tile_size_y);
				bcircles[collidable_ptr]->radius = (bcircles[collidable_ptr]->pos - aabb[collidable_ptr]->min).len();
				collidable_ptr++;
				if(collidable_ptr >= 128) collidable_ptr = 0;
			}
		}
	}
}

AABB** Tilemap::getAABBs() {
	return aabb;
}

BoundingCircle** Tilemap::getBoundingCircles() {
	return bcircles;
}

int Tilemap::getCollidablePtr() {
	return collidable_ptr;
}

Tmpl8::Surface* Tilemap::getTileSurface(int index) {
	return tiles[index];
}

Tmpl8::Surface* Tilemap::createCollisionArea(int tile_index, int x, int y, int width, int height) {
	Tmpl8::Surface* tile = tiles[tile_index];
	Tmpl8::Surface* area = new Tmpl8::Surface(width, height);
	tile->CopyAreaTo(area, 0, 0, x, y, width, height);
	return area;
}