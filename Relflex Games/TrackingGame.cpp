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

unsigned int randomCount = 1;
unsigned int randomCountIncrease = 1;

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

class target {
	public:
		int x = 0;
		int y = 0;
		int radius = 15;

		float trueRadius = radius;

		int raduisChange = 1;
		int raduisChangeVel = 1;

		int timer = 0;
		int max = zorshift(randomCount) % 60 + 20;

		int timer1 = 0;
		int max1 = zorshift(randomCount) % 60 + 30;

		int direction = 0;

		int vel = 5;

		target () {
			x = (int)(SCREEN_WIDTH / 2);
			y = (int)(SCREEN_HEIGHT / 2);
		}

		void drawTarget(int i, std::vector<target> &Targets) {
			drawFilledCircle(x, y, radius, 240, 0, 0);
			drawFilledCircle(x, y, (int)(radius * 0.75), 255, 255, 255);
			drawFilledCircle(x, y, (int)(radius * 0.5), 240, 0, 0);

			if (raduisChange == 1) {
				trueRadius += raduisChangeVel;
			} if (raduisChange == 2) {
				trueRadius -= raduisChangeVel;
			}

			if (timer >= max) {
				timer = 0;
				raduisChange = zorshift(randomCount) % 2 + 1;
				randomCount += randomCountIncrease;
				max = zorshift(randomCount) % 60 + 20;
				randomCount += randomCountIncrease;
			}

			if (timer1 >= max1) {
				timer1 = 0;
				int randomNumber = zorshift(randomCount) % 8 + 1;

				if (randomNumber == 1 || randomNumber == 2 || randomNumber == 3) {
					direction = 2;
				}

				if (randomNumber == 4 || randomNumber == 5 || randomNumber == 6) {
					direction = 1;
				}

				if (randomNumber == 7) {
					direction = 3;
				}

				if (randomNumber == 8) {
					direction = 4;
				}

				randomCount += randomCountIncrease;
				max1 = zorshift(randomCount) % 60 + 30;
				randomCount += randomCountIncrease;
			}

			if (radius < 15) {
				raduisChange = 1;
			}

			if (radius > 50) {
				raduisChange = 2;
			}

			if (direction == 1) {
				x += vel;
			} if (direction == 2) {
				x -= vel;
			} if (direction == 3) {
				y += vel;
			} if (direction == 4) {
				y -= vel;
			}

			if (x + radius > SCREEN_WIDTH) {
				direction = 2;
			} if (x - radius < 0) {
				direction = 1;
			} if (y + radius > SCREEN_HEIGHT) {
				direction = 4;
			} if (y - radius < 0) {
				direction = 3;
			}

			radius = (int)trueRadius;
			timer += 1;
			timer1 += 1;
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

void updateEvents() {
    SDL_Event event;
	
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
            exit(0);
        }
	}
}

void draw() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	for (int i = 0; i < Targets.size(); i++) {
		Targets[i].drawTarget(i, Targets);
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
	int gameTimer = 0;
	int gameMax = howLongGameLast * 60;

	Targets.push_back(target());

    while (true) {
		int frameStart = SDL_GetTicks();

		draw();
		updateEvents();
		presentScene();

		for (int i = 0; i < Targets.size(); i++) {
			if (Targets[i].isOver()) {
				score += 50;
			}
		}
		
		SDL_Delay(calculateFPSDelay(frameStart));

		gameTimer += 1;

		if (gameTimer > gameMax) {
			std::cout << score;
			exit(0);
		}
	}

	return 0;
}