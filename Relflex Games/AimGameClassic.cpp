# include <SDL2/SDL.h>
# include <iostream>
# include <vector>
# include <string>

#include <windows.h>
#include <mmsystem.h>

# pragma comment(lib, "winmm.lib")

# define SCREEN_WIDTH 600
# define SCREEN_HEIGHT 600
# define FPS 60

int randomCount = 1;
int randomCountIncrease = 1;

int speedRange = 20;
int speedRangeBase = 25;

int howLongGameLast = 60;

int score = 0;

SDL_Window *window = SDL_CreateWindow("Aim Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
SDL_Renderer *renderer =  SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

unsigned int zorshift(unsigned int state) {
	unsigned int x = state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	state - x;
	return x;
}

void playHitTarget() {
	PlaySound("Audio/Reflex Game Hit.wav", NULL, SND_FILENAME | SND_ASYNC);
}

void playMissTarget() {
	if (zorshift(randomCount) % 1 + 1 == 1) {
		PlaySound("Audio/Reflex Game Miss 1.wav", NULL, SND_FILENAME | SND_ASYNC);
	} else {
		PlaySound("Audio/Reflex Game Miss 2.wav", NULL, SND_FILENAME | SND_ASYNC);
	}

	randomCount += randomCountIncrease;
}

void playMissTargetSupreme() {
	PlaySound("Audio/Reflex Game Miss Target.wav", NULL, SND_FILENAME | SND_ASYNC);
}

void drawFilledCircle(int x0, int y0, int radius, int r, int g, int b) {
	SDL_SetRenderDrawColor(renderer, r, g, b, 255);

	while (radius != 1) {
		int x = radius;
		int y = 0;
		int d = 0;
	
		while (x >= y)
		{
			SDL_RenderDrawPoint(renderer, x0 + x, y0 + y);
			SDL_RenderDrawPoint(renderer, x0 + y, y0 + x);
			SDL_RenderDrawPoint(renderer, x0 - y, y0 + x);
			SDL_RenderDrawPoint(renderer, x0 - x, y0 + y);
			SDL_RenderDrawPoint(renderer, x0 - x, y0 - y);
			SDL_RenderDrawPoint(renderer, x0 - y, y0 - x);
			SDL_RenderDrawPoint(renderer, x0 + y, y0 - x);
			SDL_RenderDrawPoint(renderer, x0 + x, y0 - y);
		
			if (d <= 0)
			{
				y += 1;
				d += 2 * y + 1;
			}
		
			if (d > 0)
			{
				x -= 1;
				d -= 2 * x + 1;
			}
		}

		radius -= 1;
	}
}

void CreateOutline(int x0, int y0, int radius, int r, int g, int b, int alpha) {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, alpha);

	int x = radius;
	int y = 0;
	int d = 0;

	while (x >= y)
	{
		SDL_RenderDrawPoint(renderer, x0 + x, y0 + y);
		SDL_RenderDrawPoint(renderer, x0 + y, y0 + x);
		SDL_RenderDrawPoint(renderer, x0 - y, y0 + x);
		SDL_RenderDrawPoint(renderer, x0 - x, y0 + y);
		SDL_RenderDrawPoint(renderer, x0 - x, y0 - y);
		SDL_RenderDrawPoint(renderer, x0 - y, y0 - x);
		SDL_RenderDrawPoint(renderer, x0 + y, y0 - x);
		SDL_RenderDrawPoint(renderer, x0 + x, y0 - y);
	
		if (d <= 0)
		{
			y += 1;
			d += 2 * y + 1;
		}
	
		if (d > 0)
		{
			x -= 1;
			d -= 2 * x + 1;
		}
	}

	radius -= 1;
}

class outline {
	public:
		int x = 0;
		int y = 0;
		int radius = 0;

		int alpha = 255;
		int decreaseSpeed = 5;

		outline(int x_, int y_, int radius_) {
			x = x_;
			y = y_;
			radius = radius_;
		}

		void update(int i, std::vector<outline> &Outlines) {
			CreateOutline(x, y, radius, 240, 0, 0, alpha);

			if (alpha - decreaseSpeed < 0) {
				Outlines.erase(Outlines.begin() + i);
			}

			alpha -= decreaseSpeed;
		}
};

class target {
	public:
		int x = 0;
		int y = 0;
		int radius = 3;

		float trueRadius = radius;
		float radiusChangeSpeed = 0.2;

		bool sizeDirection = true;

		target () {
			randomCount += randomCountIncrease;
			x = zorshift(randomCount) % SCREEN_WIDTH - 1;
			randomCount += randomCountIncrease;
			y = zorshift(randomCount) % SCREEN_WIDTH - 1;
		}

		void drawTarget(int i, std::vector<target> &Targets) {
			drawFilledCircle(x, y, radius, 240, 0, 0);
			drawFilledCircle(x, y, (int)(radius * 0.75), 255, 255, 255);
			drawFilledCircle(x, y, (int)(radius * 0.5), 240, 0, 0);

			if (sizeDirection) { trueRadius += radiusChangeSpeed; }
			else { trueRadius -= radiusChangeSpeed; }

			if (radius > 28 && sizeDirection) { sizeDirection = false; }

			if (radius < 3) {
				Targets.erase(Targets.begin() + i);
				score -= 100;
				playMissTargetSupreme();
			}

			radius = (int)trueRadius;
		}

		bool isOver() {
			int tx, ty;

			SDL_GetMouseState(&tx, &ty);

			int distancex = x - tx;
			int distancey = y - ty;

			if (abs(distancex) < radius && abs(distancey) < radius) {
				return true;
			}

			return false;
		}
};

std::vector<target> Targets;
std::vector<outline> Outlines;

void updateEvents() {
    SDL_Event event;
	
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
            exit(0);
        } if (event.type == SDL_MOUSEBUTTONDOWN) {
			bool flag = false;

			for (int i = 0; i < Targets.size(); i++) {
				if (Targets[i].isOver()) {
					Outlines.push_back(outline(Targets[i].x, Targets[i].y, Targets[i].radius));
					Targets.erase(Targets.begin() + i);
					score += 200;
					playHitTarget();
					flag = true;
				}
			}

			if (!flag) {
				score -= 10;
				playMissTarget();
			}
		}
	}
}

void draw() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	for (int i = 0; i < Targets.size(); i++) {
		Targets[i].drawTarget(i, Targets);
	}

	for (int i = 0; i < Outlines.size(); i++) {
  		Outlines[i].update(i, Outlines);
	}
}

void presentScene() {
    SDL_RenderPresent(renderer);
}

int calculateFPSDelay(int frameStart) {
	int frameTime = SDL_GetTicks() - frameStart;

	if (1000 / FPS > frameTime) {
		return 1000 / FPS - frameTime;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int timer = 0;
	int max = zorshift(randomCount) % speedRange + speedRangeBase;

	int gameTimer = 0;
	int gameMax = howLongGameLast * 60;

    while (true) {
		int frameStart = SDL_GetTicks();

		draw();
		updateEvents();
		presentScene();
		
		SDL_Delay(calculateFPSDelay(frameStart));

		if (timer >= max) {
			Targets.push_back(target());

			max = zorshift(randomCount) % speedRange + speedRangeBase;

			randomCount += randomCountIncrease;
			
			timer = 0;
		}

		timer += 1;
		gameTimer += 1;

		if (gameTimer > gameMax) {
			std::cout << score;
			exit(0);
		}
	}

	return 0;
}