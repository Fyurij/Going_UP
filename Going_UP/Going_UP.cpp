// Going_UP.cpp: определяет точку входа для приложения.
//

#define SDL_MAIN_USE_CALLBACKS 1

#include "Going_UP.h"
#include "Structures.h"
#include "Player.h"
#include "Platforms.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <fstream>

using namespace std;

int BLOCK_SIZE_IN_PIXELS = 15;

int GAME_WIDTH = 70;
int GAME_HEIGHT = 40;

int SDL_WINDOW_WIDTH = BLOCK_SIZE_IN_PIXELS * GAME_WIDTH;
int SDL_WINDOW_HEIGHT = BLOCK_SIZE_IN_PIXELS * GAME_HEIGHT;

int STEP = 3;

double TIME;
double speedUp = 0;
double speedHorizont = 0;



void SetRect(SDL_FRect* r, int i, int j)
{
	r->x = j * BLOCK_SIZE_IN_PIXELS;
	r->y = i * BLOCK_SIZE_IN_PIXELS;
	r->h = r->w = BLOCK_SIZE_IN_PIXELS;
}

class Screen
{
private:
	std::vector<std::vector<Pixel>> screen;
	GameLevel* lvl;
	shared_ptr<Platforms> platforms;
	std::shared_ptr<Player> player;
public:
	Screen(GameLevel* lvl_, std::shared_ptr<Player> player_, shared_ptr<Platforms> platforms_)
		:lvl(lvl_)
		, screen(GAME_HEIGHT, std::vector(GAME_WIDTH, Pixel::Empty))
		, platforms(platforms_)
		, player(player_)
	{}

	bool CheckCollision(Koordinates nextPos)
	{
		int y = nextPos.y;
		if (screen[y][nextPos.x] == Pixel::Empty)
		{
			return false;
		}
		return true;
	}

	void ClearScreen()
	{
		for (int i = 0; i < screen.size(); ++i)
		{
			for (int j = 0; j < screen[i].size(); ++j)
			{
				screen[i][j] = Pixel::Empty;
			}
		}
	}

	void GenerateScreen()
	{
		ClearScreen();
		Koordinates playerPos = player->GetPlayerPos();
		for (int i = 0; i < screen.size(); ++i)
		{
			for (int j = 0; j < screen[i].size(); ++j)
			{
				if (i == 0 && lvl->curY == GAME_HEIGHT)
				{
					screen[i][j] = Pixel::Border;
				}
				if (i == (GAME_HEIGHT - 1) && lvl->curY == lvl->maxY)
				{
					screen[i][j] = Pixel::Border;
				}
				if (j == 0)
				{
					screen[i][j] = Pixel::Border;
				}
				if (j == (lvl->maxX - 1))
				{
					screen[i][j] = Pixel::Border;
				}
				if (i == static_cast<int>(lvl->curY - playerPos.y) && j == static_cast<int>(playerPos.x) % GAME_WIDTH && static_cast<int>(lvl->curY - playerPos.y) > 0)
				{
					screen[GAME_HEIGHT - i][j] = Pixel::Player;
				}
			}
		}
		std::vector<Rectangle> plats = platforms->GetPlatforms();
		for (int i = 0; i < plats.size(); ++i)
		{
			int j = 0;
			while (plats[i].start.x + j < plats[i].finish.x)
			{
				if (plats[i].start.y <= lvl->curY && plats[i].start.y > lvl->curY - GAME_HEIGHT && plats[i].start.y != lvl->curY)
				{
					screen[GAME_HEIGHT - (lvl->curY - plats[i].start.y)][plats[i].start.x + j] = Pixel::Platform;
				}
				++j;
			}
		}
	}

	std::vector<std::vector<Pixel>>& GetScreen()
	{
		return screen;
	}
};

struct AppState
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	GameLevel level;
	Screen screen;
	std::shared_ptr<Player> player;
	std::chrono::time_point<std::chrono::steady_clock> prevLog;
};

void ScreenInit(GameLevel* lvl)
{
	lvl->curY = GAME_HEIGHT * 5;
	//lvl->curX = GAME_WIDTH / 2;
	lvl->maxX = GAME_WIDTH;
	lvl->maxY = GAME_HEIGHT * 5;
}

SDL_AppResult KeyEvent(GameLevel* lvl, SDL_Scancode keyCode, std::shared_ptr<Player> player)
{
	switch (keyCode)
	{
	case SDL_SCANCODE_ESCAPE:
		return SDL_APP_SUCCESS;
	case SDL_SCANCODE_R:
		ScreenInit(lvl);
		break;
	case SDL_SCANCODE_UP:
		if (player->IsOnGround())
		{
			player->SpeedUp();
			player->SetOnGround(false);
		}
		break;
	case SDL_SCANCODE_DOWN:
		break;
	case SDL_SCANCODE_LEFT:
		if (!player->IsOnGround())
		{
			player->SpeedLeft();
		}
		else
		{
			player->MoveLeft();
		}
		break;
	case SDL_SCANCODE_RIGHT:
		if (!player->IsOnGround())
		{
			player->SpeedRight();
		}
		else
		{
			player->MoveRight();
		}
		break;
	default:
		break;
	}
	return SDL_APP_CONTINUE;
}

void Log(AppState* as)
{
	std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
	TIME = std::chrono::duration_cast<std::chrono::milliseconds>(current - as->prevLog).count();
	std::cout << "Frame time: " << std::chrono::duration_cast<std::chrono::milliseconds>(current - as->prevLog) << std::endl;
	Koordinates play = as->player->GetPlayerPos();
	std::cout << "Player x: " << play.x << " y: " << play.y << std::endl;
	std::cout << "Current Y: " << as->level.curY << std::endl;
	as->prevLog = current;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	AppState* as = (AppState*)appstate;
	GameLevel* lvl = &as->level;
	Screen* scr = &as->screen;
	shared_ptr<Player> player = as->player;
	const Uint64 now = SDL_GetTicks();
	SDL_FRect r;
	int ct;

	Log(as);
	std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
	TIME = std::chrono::duration_cast<std::chrono::milliseconds>(current - as->prevLog).count();
	while (TIME < 5)
	{
		std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
		TIME = std::chrono::duration_cast<std::chrono::milliseconds>(current - as->prevLog).count();
	}
	as->prevLog = current;
	// run game logic if we're at or past the time to run it.
	// if we're _really_ behind the time to run it, run it
	// several times.
	int color = 0;

	player->Move(TIME);
	if (!player->IsOnGround())
	{
		player->MoveHorizontalInAir(TIME);
	}
	Koordinates location = player->GetPlayerPos();
	if (lvl->curY - static_cast<int>(location.y) <= static_cast<int>(GAME_HEIGHT / 6) && !player->CheckBottom() && lvl->curY < lvl->maxY)
	{
		lvl->curY += static_cast<int>(GAME_HEIGHT / 12);
		player->SetPlayerPos(location);
	}
	else if (lvl->curY - static_cast<int>(location.y) >= static_cast<int>(5 * GAME_HEIGHT / 6) && lvl->curY > GAME_HEIGHT)
	{
		lvl->curY -= static_cast<int>(GAME_HEIGHT / 12);
		player->SetPlayerPos(location);
	}
	SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(as->renderer);
	scr->GenerateScreen();
	std::vector<std::vector<Pixel>> view = scr->GetScreen();
	for (int i = 0; i < GAME_HEIGHT; ++i) 
	{
		for (int j = 0; j < GAME_WIDTH; ++j) 
		{
			SetRect(&r, i, j);
			switch (view[i][j])
			{
			case Pixel::Empty:
				SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
				break;
			case Pixel::Border:
				SDL_SetRenderDrawColor(as->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
				break;
			case Pixel::Platform:
				SDL_SetRenderDrawColor(as->renderer, 0, 180, 180, SDL_ALPHA_OPAQUE);
				break;
			case Pixel::Player:
				SDL_SetRenderDrawColor(as->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
				break;
			default:
				break;
			}
			SDL_RenderFillRect(as->renderer, &r);
		}
	}
	//SDL_SetRenderDrawColor(as->renderer, 255, 255, 0, SDL_ALPHA_OPAQUE); /*head*/
	//SDL_RenderFillRect(as->renderer, &r);
	SDL_RenderPresent(as->renderer);
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	GameLevel* lvl = &((AppState*)appstate)->level;
	std::shared_ptr<Player> player = ((AppState*)appstate)->player;
	switch (event->type)
	{
	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;
	case SDL_EVENT_KEY_DOWN:
		return KeyEvent(lvl, event->key.scancode, player);
	default:
		break;
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	size_t i;

	if (!SDL_SetAppMetadata("Going UP", "1.0", "com.example.Snake")) 
	{
		return SDL_APP_FAILURE;
	}

	if (!SDL_Init(SDL_INIT_VIDEO)) 
	{
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	AppState* as = (AppState*)SDL_calloc(1, sizeof(AppState));
	if (!as) 
	{
		return SDL_APP_FAILURE;
	}

	*appstate = as;

	if (!SDL_CreateWindowAndRenderer("Going UP", SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, 0, &as->window, &as->renderer)) 
	{
		return SDL_APP_FAILURE;
	}

	//as->level.maxX = GAME_WIDTH;
	//as->level.maxY = GAME_HEIGHT * 5;

	ScreenInit(&as->level);

	shared_ptr<Platforms> plat = std::make_shared<Platforms>(&as->level, GAME_WIDTH, GAME_HEIGHT);

	as->player = std::make_shared<Player>(&as->level, GAME_WIDTH, GAME_HEIGHT, as->level.maxY, BLOCK_SIZE_IN_PIXELS, plat);

	as->screen = Screen(&as->level, as->player, plat);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{ 
	if (appstate != NULL) 
	{
		AppState* as = (AppState*)appstate;
		SDL_DestroyRenderer(as->renderer);
		SDL_DestroyWindow(as->window);
		SDL_free(as);
	}
}