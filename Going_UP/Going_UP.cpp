// Going_UP.cpp: определяет точку входа для приложения.
//

#define SDL_MAIN_USE_CALLBACKS 1

#include "Going_UP.h"
#include "Structures.h"
#include "Player.h"
#include "Platforms.h"
#include "Config.h"
#include "Screen.h"
#include "Artifact.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <SDL3_ttf/SDL_ttf.h>

#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <fstream>
#include <thread>
#include <algorithm>

const int BLOCK_SIZE_IN_PIXELS = 15;

const int WINDOW_WIDTH = 70;
const int GAME_WIDTH = 54;
const int GAME_HEIGHT = 40;

const int SDL_WINDOW_WIDTH = BLOCK_SIZE_IN_PIXELS * WINDOW_WIDTH;
const int SDL_WINDOW_HEIGHT = BLOCK_SIZE_IN_PIXELS * GAME_HEIGHT;

double TIME = 0;
double speedUp = 0;
double speedHorizont = 0;

bool movingScreen = false;

TTF_Font* font;
TTF_TextEngine* engine;
TTF_Text* text;

void SetRect(SDL_FRect* r, int i, int j)
{
	r->x = j * BLOCK_SIZE_IN_PIXELS;
	r->y = i * BLOCK_SIZE_IN_PIXELS;
	r->h = r->w = BLOCK_SIZE_IN_PIXELS;
}

struct AppState
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	GameLevel level;
	Screen screen;
	std::shared_ptr<Player> player;
	std::shared_ptr<ArtifactManager> artifacts;
	std::shared_ptr<Config> config;
	std::shared_ptr<Platforms> plat;
	std::chrono::time_point<std::chrono::steady_clock> prevLog;
	bool endOfGame = false;
};

bool UpdateLevel(AppState* as)
{
	if (!as->config->UpdateLevelData())
	{
		as->screen.EndOfGame();
		return false;
	}

	as->plat = std::make_shared<Platforms>(&as->level, GAME_WIDTH, GAME_HEIGHT, as->config);

	as->artifacts = std::make_shared<ArtifactManager>(&as->level, as->config);

	as->screen = Screen(&as->level, as->player, as->plat, as->artifacts, GAME_HEIGHT, GAME_WIDTH);
	return true;
}

void LoadFirstLevel(AppState* as)
{
	as->plat = std::make_shared<Platforms>(&as->level, GAME_WIDTH, GAME_HEIGHT, as->config);

	as->artifacts = std::make_shared<ArtifactManager>(&as->level, as->config);

	as->screen = Screen(&as->level, as->player, as->plat, as->artifacts, GAME_HEIGHT, GAME_WIDTH);
}

void ScreenInit(GameLevel* lvl, std::shared_ptr<Config> config)
{
	lvl->maxX = GAME_WIDTH * 2;
	lvl->maxY = GAME_HEIGHT * 5;
	lvl->curY = GAME_HEIGHT * 5;
	if (config->GetSide() == "right")
	{
		lvl->curX = lvl->maxX;
	}
	else
	{
		lvl->curX = GAME_WIDTH;
	}
}

void RestartLevel(AppState* as)
{
	as->plat = std::make_shared<Platforms>(&as->level, GAME_WIDTH, GAME_HEIGHT, as->config);

	as->artifacts = std::make_shared<ArtifactManager>(&as->level, as->config);

	as->screen = Screen(&as->level, as->player, as->plat, as->artifacts, GAME_HEIGHT, GAME_WIDTH);
	
	as->player->ResetPosition();

	ScreenInit(&as->level, as->config);
	//as->player = std::make_shared<Player>(&as->level, GAME_WIDTH, GAME_HEIGHT, as->level.maxY, BLOCK_SIZE_IN_PIXELS, as->config);
}

SDL_AppResult KeyEvent(AppState* as, GameLevel* lvl, const bool* keys, std::shared_ptr<Player> player, Screen& screen)
{
	if (keys[SDL_SCANCODE_ESCAPE])
	{
		return SDL_APP_SUCCESS;
	}
	if (keys[SDL_SCANCODE_R])
	{
		RestartLevel(as);
	}
	if (keys[SDL_SCANCODE_UP])
	{
		if (player->IsOnGround())
		{
			player->SpeedUp();
			player->SetOnGround(false);
		}
	}
	if (keys[SDL_SCANCODE_DOWN])
	{
		Coordinates pos = player->GetPlayerPos();
		if (lvl->curY != lvl->maxY && lvl->curY - pos.y < 2 * GAME_HEIGHT / 3)
		{
			lvl->curY += 0.1;
			movingScreen = true;
		}
		player->SetPlayerPos(pos);
	}
	else
	{
		movingScreen = false;
	}
	if (keys[SDL_SCANCODE_LEFT])
	{
		Coordinates pos = player->GetPlayerPos();
		--pos.x;
		if (screen.IsNextMoveExit(pos))
		{
			player->TeleportToRightSide();

			if (!UpdateLevel(as))
			{
				as->endOfGame = true;
			}
		}
		if (!player->IsOnGround() && !player->IsMovingHorizontal())
		{
			player->SpeedLeft();
		}
		else if (player->IsOnGround())
		{
			player->MoveLeft(TIME);
		}
	}
	if (keys[SDL_SCANCODE_RIGHT])
	{
		Coordinates pos = player->GetPlayerPos();
		++pos.x;
		if (screen.IsNextMoveExit(pos))
		{
			player->TeleportToLeftSide();

			if (!UpdateLevel(as))
			{
				as->endOfGame = true;
			}
		}
		if (!player->IsOnGround() && !player->IsMovingHorizontal())
		{
			player->SpeedRight();
		}
		else if (player->IsOnGround())
		{
			player->MoveRight(TIME);
		}
	}
	return SDL_APP_CONTINUE;
}

void Log(AppState* as)
{
	std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
	TIME = std::chrono::duration_cast<std::chrono::milliseconds>(current - as->prevLog).count();
	std::cout << "Frame time: " << std::chrono::duration_cast<std::chrono::milliseconds>(current - as->prevLog) << std::endl;
	Coordinates play = as->player->GetPlayerPos();
	std::cout << "Player x: " << play.x << " y: " << play.y << std::endl;
	std::cout << "Current Y: " << as->level.curY << std::endl;
	std::cout << "Current X: " << as->level.curX << std::endl;
	as->prevLog = current;
}

void MovingScreen(std::shared_ptr<Player> player, GameLevel* lvl)
{
	Coordinates location = player->GetPlayerPos();
	if (lvl->curY - static_cast<int>(location.y) <= static_cast<int>(GAME_HEIGHT / 6) && !player->CheckBottom() && lvl->curY < lvl->maxY)
	{
		lvl->curY += static_cast<int>(GAME_HEIGHT / 12);
		if (lvl->curY > lvl->maxY)
		{
			lvl->curY = lvl->maxY;
		}
	}
	else if (lvl->curY - static_cast<int>(location.y) >= static_cast<int>(5 * GAME_HEIGHT / 6) && lvl->curY > GAME_HEIGHT)
	{
		lvl->curY -= static_cast<int>(GAME_HEIGHT / 12);
		if (lvl->curY < GAME_HEIGHT)
		{
			lvl->curY = GAME_HEIGHT;
		}
	}
	if (lvl->curX - static_cast<int>(location.x) <= static_cast<int>(GAME_WIDTH / 6) && lvl->curX < lvl->maxX)
	{
		lvl->curX += 1;//static_cast<int>(GAME_WIDTH / 12);
		if (lvl->curX > lvl->maxX)
		{
			lvl->curX = lvl->maxX;
		}
	}
	else if (lvl->curX - static_cast<int>(location.x) >= static_cast<int>(5 * GAME_WIDTH / 6) && lvl->curX > GAME_WIDTH)
	{
		lvl->curX -= 1;//static_cast<int>(GAME_WIDTH / 12);
		if (lvl->curX < GAME_WIDTH)
		{
			lvl->curX = GAME_WIDTH;
		}
	}
}

std::string GetKnowCurrentLevel(std::shared_ptr<Config> cfg)
{
	return "Level " + std::to_string(cfg->GetCurrentLevel()) + "/" + std::to_string(cfg->GetMaxLevel());
}

std::string GetKnowCurrentNumOfArtifacts(std::shared_ptr<ArtifactManager> arts)
{
	return "Artifacts " + std::to_string(arts->GetCollectedArtifacts()) + "/" + std::to_string(arts->GetMaxNumOfArtifacts());
}

class ThreadJoin
{
private:
	std::thread uniqThread;
public:
	ThreadJoin(std::thread&& uniqThread_)
		:uniqThread(std::move(uniqThread_))
	{}

	~ThreadJoin()
	{
		if (uniqThread.joinable())
		{
			uniqThread.join();
		}
	}
};

SDL_AppResult SDL_AppIterate(void* appstate)
{
	AppState* as = (AppState*)appstate;
	GameLevel* lvl = &as->level;
	Screen* scr = &as->screen;
	std::shared_ptr<Player> player = as->player;
	const Uint64 now = SDL_GetTicks();
	SDL_FRect r;
	SDL_AppResult result = SDL_APP_CONTINUE;
	int ct;

	bool playerMoving = false;

	std::thread logThread(Log, as);
	ThreadJoin thrd(std::move(logThread));

	std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
	TIME = std::chrono::duration_cast<std::chrono::milliseconds>(current - as->prevLog).count();
	while (TIME < 5)
	{
		std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
		TIME = std::chrono::duration_cast<std::chrono::milliseconds>(current - as->prevLog).count();
	}
	as->prevLog = current;

	const bool* keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_ESCAPE] || keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_R])
	{
		result = KeyEvent(as, lvl, keys, player, as->screen);
		playerMoving = true;
	}
	else
	{
		playerMoving = false;
	}

	if (as->endOfGame)
	{
		SDL_RenderClear(as->renderer);
		std::string finish = "CONGRATULATIONS!\nYOU WIN!\nPress \"esc\" to exit";
		TTF_SetTextString(text, finish.c_str(), 0);
		int x = (WINDOW_WIDTH * BLOCK_SIZE_IN_PIXELS) / 3;
		int y = (GAME_HEIGHT * BLOCK_SIZE_IN_PIXELS) / 3;
		SDL_SetRenderDrawColor(as->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		TTF_DrawRendererText(text, x, y);
		SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderPresent(as->renderer);
		return result;
	}

	int color = 0;

	if (!movingScreen)		//TODO
	{
		lvl->curY = std::round(lvl->curY);
	}

	player->Move(TIME, as->plat);
	
	as->artifacts->CollectingArtifact(player->GetPlayerPos());
	if (!player->IsOnGround())
	{
		player->MoveHorizontalInAir(TIME, as->plat);
	}
	if ((player->IsOnGround() || player->IsGroundOnPlatform(TIME, as->plat)) && !playerMoving)
	{
		Coordinates location = player->GetPlayerPos();
		location.x = std::round(location.x);
		player->SetPlayerPos(location);
	}

	MovingScreen(player, lvl);
	SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(as->renderer);
	scr->GenerateScreen();
	std::vector<std::vector<Pixel>> view = scr->GetScreen();
	const int widthInfoColumn = WINDOW_WIDTH - GAME_WIDTH;
	for (int i = 0; i < GAME_HEIGHT; ++i) 
	{
		for (int j = WINDOW_WIDTH - GAME_WIDTH; j < WINDOW_WIDTH; ++j) 
		{
			SetRect(&r, i, j);
			switch (view[i][j - (WINDOW_WIDTH - GAME_WIDTH)])
			{
			case Pixel::Empty:
				SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
				break;
			case Pixel::Border:
				SDL_SetRenderDrawColor(as->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
				break;
			case Pixel::Platform:
				SDL_SetRenderDrawColor(as->renderer, 0, 255, 255, SDL_ALPHA_OPAQUE);
				break;
			case Pixel::Player:
				SDL_SetRenderDrawColor(as->renderer, 160, 0, 255, SDL_ALPHA_OPAQUE);
				break;
			case Pixel::Artifact:
				SDL_SetRenderDrawColor(as->renderer, 255, 165, 0, SDL_ALPHA_OPAQUE);
				break;
			case Pixel::Door:
				if (!as->artifacts->IsAllArtifactsCollected())
				{
					SDL_SetRenderDrawColor(as->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
				}
				else
				{
					SDL_SetRenderDrawColor(as->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
				}
				break;
			default:
				break;
			}
			SDL_RenderFillRect(as->renderer, &r);
		}
	}
	SDL_SetRenderDrawColor(as->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderLine(as->renderer, widthInfoColumn * BLOCK_SIZE_IN_PIXELS, GAME_HEIGHT * BLOCK_SIZE_IN_PIXELS, widthInfoColumn * BLOCK_SIZE_IN_PIXELS, 0);
	SDL_RenderLine(as->renderer, widthInfoColumn * BLOCK_SIZE_IN_PIXELS, 0, 0, 0);
	SDL_RenderLine(as->renderer, 0, GAME_HEIGHT * BLOCK_SIZE_IN_PIXELS, 0, 0);
	SDL_RenderLine(as->renderer, 0, GAME_HEIGHT * BLOCK_SIZE_IN_PIXELS - 1, widthInfoColumn * BLOCK_SIZE_IN_PIXELS, GAME_HEIGHT * BLOCK_SIZE_IN_PIXELS - 1);

	//TTF_SetFontSize(font, 100);
	std::string levelCounter = GetKnowCurrentLevel(as->config);
	TTF_SetTextString(text, levelCounter.c_str(), 0);
	int x = 10;
	int y = 10;
	SDL_SetRenderDrawColor(as->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	TTF_DrawRendererText(text, x, y);

	std::string artifactCounter = GetKnowCurrentNumOfArtifacts(as->artifacts);
	TTF_SetTextString(text, artifactCounter.c_str(), 0);
	x = 10;
	y = 60;
	SDL_SetRenderDrawColor(as->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	TTF_DrawRendererText(text, x, y);

	SDL_RenderPresent(as->renderer);
	return result;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	//const bool* keys = SDL_GetKeyboardState(NULL);
	GameLevel* lvl = &((AppState*)appstate)->level;
	std::shared_ptr<Player> player = ((AppState*)appstate)->player;
	switch (event->type)
	{
	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;
	//case SDL_EVENT_KEY_DOWN:
		//return KeyEvent(lvl, keys, player);
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

	if (!TTF_Init()) 
	{
		SDL_Log("Couldn't initialize SDL_ttf: %s\n", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	font = TTF_OpenFont("../config/Font/Pixelletters.ttf", 52);
	if (!font) 
	{
		SDL_Log("Couldn't open font: %s\n", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	engine = TTF_CreateRendererTextEngine(as->renderer);
	if (!engine) {
		SDL_Log("Couldn't create text engine: %s\n", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	text = TTF_CreateText(engine, font, "Calculating...", 0);
	if (!text) 
	{
		SDL_Log("Couldn't create text: %s\n", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	TTF_SetTextColor(text, 255, 255, 255, SDL_ALPHA_OPAQUE);

	as->config = std::make_shared<Config>(&as->level);

	ScreenInit(&as->level, as->config);

	as->player = std::make_shared<Player>(&as->level, GAME_WIDTH, GAME_HEIGHT, as->level.maxY, BLOCK_SIZE_IN_PIXELS, as->config);

	LoadFirstLevel(as);

	/*as->config = std::make_shared<Config>(&as->level);

	std::shared_ptr<Platforms> plat = std::make_shared<Platforms>(&as->level, GAME_WIDTH, GAME_HEIGHT, as->config);

	as->artifacts = std::make_shared<ArtifactManager>(&as->level, as->config);

	as->player = std::make_shared<Player>(&as->level, GAME_WIDTH, GAME_HEIGHT, as->level.maxY, BLOCK_SIZE_IN_PIXELS, plat, as->config);

	as->screen = Screen(&as->level, as->player, plat, as->artifacts);*/

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
		if (text) 
		{
			TTF_DestroyText(text);
		}
		if (engine) 
		{
			TTF_DestroyRendererTextEngine(engine);
		}
		if (font) 
		{
			TTF_CloseFont(font);
		}
		TTF_Quit();
	}
}