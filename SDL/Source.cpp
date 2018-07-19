#include <iostream>
#include <fstream>
#include <SDL.h>
#include <SDL_image.h>
#include <map>
#include <list>

// Ported by @valt to SDL from https://www.youtube.com/user/FamTrinli arkanoid tutorial in SFML

//figures coordinates
std::map<int, SDL_Rect> Figures;
std::map<const char, SDL_Texture *> Textures;
std::map<const char, std::string> Bitmaps;

const int posX = 100, posY = 100;
const int sizeX = 720, sizeY = 450;
const int FPS = 60;
SDL_Rect paddle = { sizeX / 2 , sizeY - 50 , 90, 9};
SDL_Rect board = { 0, 0, sizeX, sizeY };
Uint32 start;
std::list<SDL_Rect> blocks;


struct Ball
{
	float x, y, speed, angle;
	int h, w;

	Ball() { x = sizeX/2, y = sizeY/2, speed = 3.0; angle = -40.1; h = 10; w = 10; }

	SDL_Rect getRect() {
		SDL_Rect b = { x, y, h, w };
		return b;
	}

	void move()
	{
		x += sin(angle*M_PI / 180) * speed;
		y -= cos(angle*M_PI / 180) * speed;
		// edge of the map
		if (x < 0) {
			x = 0;
			angle = 360 - angle;
		}
		if (y < 0) {
			y = 0;
			angle = 180 - angle;
		}
		if (x > sizeX) {
			x = sizeX;
			angle = 360 - angle;
		}
		if (y > sizeY) {
			y = sizeY;
			angle = 180 - angle;
		}
		// paddle collision
		if(SDL_HasIntersection(&paddle, &getRect())){
			angle = 180 - angle;
		}
		// blocks collision
		for (auto it = blocks.begin(); it != blocks.end(); ) {
			SDL_Rect block = *it;
			if (SDL_HasIntersection(&block, &getRect())) {
				angle = 180 - angle;
				it = blocks.erase(it);
			}
			else
				++it;
		}
	}
};

int main(int argc, char ** argv) {
	SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	// Error checks
	std::cout << "SDL_Init\n";
	SDL_Init(SDL_INIT_VIDEO);
	win = SDL_CreateWindow("Arkanoid", posX, posY, sizeX, sizeY, 0);
	if (win == NULL) {
		std::cout << "SDL_CreateWindow error\n";
	}
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		std::cout << "SDL_CreateRenderer error\n";
	}
	// Load bitmaps
	Bitmaps[0] = "img/background.png";
	Bitmaps[1] = "img/ball.png";
	Bitmaps[2] = "img/paddle.png";
	Bitmaps[3] = "img/block01.png";
	// Create textures from bitmaps
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		std::cout << "SDL_image could not initialize! SDL_image Error:" << IMG_GetError() << std::endl;
	}
	IMG_Init(IMG_INIT_PNG);
	for (auto bitmap : Bitmaps) {
		Textures[bitmap.first] = IMG_LoadTexture(renderer, bitmap.second.c_str());
		if (Textures[bitmap.first] == NULL) {
			std::cout << bitmap.second.c_str() <<" SDL_CreateTextureFromSurface error\n";
		}
		else
			std::cout << bitmap.second.c_str() << " SDL_CreateTextureFromSurface OK\n";
	}
	Ball ball;
	// blocks 14 items in 5 rows, item size 42*20 pixels
	for(int j = 0; j < 5; j++) 
		for (int i = 0; i < 14; i++) 
			blocks.push_back(SDL_Rect{60 + i*42, 50 + j*20, 42, 20 });
	
	while (1) {
		start = SDL_GetTicks();
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
			/* Look for a keypress */
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_LEFT:
					if (paddle.x > 0)
						paddle.x -= 10;
					break;
				case SDLK_RIGHT:
					if (paddle.x < sizeX-paddle.w)
						paddle.x += 10;
					break;
				default:
					std::cout << "Key not supported" << std::endl;
					break;
				}
			}
		}
		SDL_RenderClear(renderer);
		// board texture
		auto texture = Textures.find(0);
		if (texture != Textures.end()) {
			SDL_RenderCopy(renderer, texture->second, nullptr, &board);
		}
		// ball texture
		texture = Textures.find(1);
		if (texture != Textures.end()) {
			SDL_RenderCopy(renderer, texture->second, nullptr, &ball.getRect());
		}
		// paddle texture
		texture = Textures.find(2);
		if (texture != Textures.end()) {
			SDL_RenderCopy(renderer, texture->second, nullptr, &paddle);
		}
		// blocks texture
		texture = Textures.find(3);
		if (texture != Textures.end()) {
			for (SDL_Rect block : blocks) {
				SDL_RenderCopy(renderer, texture->second, nullptr, &block);
			}
		}
		SDL_RenderPresent(renderer);
		ball.move();
		if (500 / FPS > SDL_GetTicks() - start)
			SDL_Delay(500 / FPS - (SDL_GetTicks() - start));
		if (blocks.empty())
			break;
	}
	IMG_Quit();
	for (auto texture : Textures) 
		SDL_DestroyTexture(texture.second);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();
	std::cout << "SDL_Quit\n";
	return 0;
}