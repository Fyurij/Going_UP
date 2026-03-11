// Going_UP.cpp: определяет точку входа для приложения.
//

#define SDL_MAIN_USE_CALLBACKS 1

#include "Going_UP.h"
#include "Structures.h"
#include "Player.h"
#include "Platforms.h"
#include "Config.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <fstream>
#include <thread>
#include <algorithm>

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

bool movingScreen = false;

void SetRect(SDL_FRect* r, int i, int j)
{
	r->x = j * BLOCK_SIZE_IN_PIXELS;
	r->y = i * BLOCK_SIZE_IN_PIXELS;
	r->h = r->w = BLOCK_SIZE_IN_PIXELS;
}

class ArtifactManager
{
private:
	GameLevel* lvl;
	std::vector<Artifact> artifacts;
	int max = 3;
	int collected = 0;
	std::string side;
	std::shared_ptr<Config> config;
public:
	ArtifactManager(GameLevel* lvl_, std::shared_ptr<Config> config_)
		:lvl(lvl_)
		,config(config_)
	{
		artifacts = config->GetArtifacts();
		side = config->GetSide();
	}

	void CollectingArtifact(Coordinates player)
	{
		for (int i = 0; i < artifacts.size(); ++i)
		{
			if (std::round(player.y) == artifacts[i].object.y && std::round(player.x) == artifacts[i].object.x)
			{
				artifacts[i].isCollected = true;
				++collected;
			}
		}
	}

	std::vector<Coordinates> GetArtifactsPos()
	{
		std::vector<Coordinates> artifactsPos;
		for (int i = 0; i < artifacts.size(); ++i)
		{
			if (!artifacts[i].isCollected)
			{
				artifactsPos.push_back(artifacts[i].object);
			}
		}
		return artifactsPos;
	}

	bool IsAllArtifactsCollected()
	{
		/*for (int i = 0; i < artifacts.size(); ++i)
		{
			if (!artifacts[i].isCollected)
			{
				return false;
			}
		}
		return true;*/
		return std::all_of(artifacts.cbegin(), artifacts.cend(), [](const auto&/* const Artifact& */ art) {return art.isCollected;});
	}

	std::string WhichSideDoor()
	{
		return side;
	}
};

class Screen
{
private:
	std::vector<std::vector<Pixel>> screen;
	GameLevel* lvl;
	std::shared_ptr<Platforms> platforms;
	std::shared_ptr<Player> player;
	std::shared_ptr<ArtifactManager> artifacts;
public:
	Screen(GameLevel* lvl_, std::shared_ptr<Player> player_, shared_ptr<Platforms> platforms_, std::shared_ptr<ArtifactManager> artifacts_)
		:lvl(lvl_)
		, screen(GAME_HEIGHT, std::vector(GAME_WIDTH, Pixel::Empty))
		, platforms(platforms_)
		, player(player_)
		, artifacts(artifacts_)
	{}

	bool CheckCollision(Coordinates nextPos)
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
		int visibleDoorHeight = 0;
		int maxDoorHeight = 7;
		Coordinates playerPos = player->GetPlayerPos();
		std::string side = artifacts->WhichSideDoor();
		for (int i = 0; i < screen.size(); ++i)
		{
			visibleDoorHeight = maxDoorHeight - (lvl->maxY - std::round(lvl->curY));
			for (int j = 0; j < screen[i].size(); ++j)
			{
				if (i == 0 && std::round(lvl->curY) == GAME_HEIGHT)
				{
					screen[i][j] = Pixel::Border;
				}
				if (i == (GAME_HEIGHT - 1) && std::round(lvl->curY) >= lvl->maxY)
				{
					screen[i][j] = Pixel::Border;
				}
				if (j == 0 && std::round(lvl->curX) == GAME_WIDTH)
				{
					if (i > GAME_HEIGHT - visibleDoorHeight && i < GAME_HEIGHT && side == "left")
					{
						screen[i][j] = Pixel::Door;
						if (std::round(lvl->curY) >= lvl->maxY && i == GAME_HEIGHT - 1)
						{
							screen[i][j] = Pixel::Border;
						}
					}
					else
					{
						screen[i][j] = Pixel::Border;
					}
				}
				if (j == (GAME_WIDTH - 1) && std::round(lvl->curX) == lvl->maxX)
				{
					if (i > GAME_HEIGHT - visibleDoorHeight && i < GAME_HEIGHT && side == "right")
					{
						screen[i][j] = Pixel::Door;
						if (std::round(lvl->curY) >= lvl->maxY && i == GAME_HEIGHT - 1)
						{
							screen[i][j] = Pixel::Border;
						}
					}
					else
					{
						screen[i][j] = Pixel::Border;
					}
				}
				if (i == static_cast<int>(std::round(lvl->curY - playerPos.y)) && j == static_cast<int>(std::round(lvl->curX - playerPos.x)) && static_cast<int>(lvl->curY - playerPos.y) > 0 && static_cast<int>(lvl->curX - playerPos.x) > 0)
				{
					screen[GAME_HEIGHT - i][GAME_WIDTH - j] = Pixel::Player;
				}
			}
		}
		std::vector<Rectangle> plats = platforms->GetPlatforms();
		for (int i = 0; i < plats.size(); ++i)
		{
			int j = 0;
			while (plats[i].start.x + j < plats[i].finish.x)
			{
				if (plats[i].start.y <= lvl->curY && plats[i].start.y >= lvl->curY - GAME_HEIGHT && std::round(plats[i].start.y - lvl->curY) != 0 && plats[i].start.x + j <= lvl->curX && plats[i].start.x + j >= lvl->curX - GAME_WIDTH)
				{
					screen[GAME_HEIGHT - std::round(lvl->curY - plats[i].start.y)][GAME_WIDTH - std::round(lvl->curX - plats[i].start.x) + j] = Pixel::Platform;
				}
				++j;
			}
		}
		std::vector<Coordinates> artifactsPos = artifacts->GetArtifactsPos();
		for (int i = 0; i < artifactsPos.size(); ++i)
		{
			if (artifactsPos[i].y < lvl->curY && artifactsPos[i].y > lvl->curY - GAME_HEIGHT && artifactsPos[i].x > lvl->curX - GAME_WIDTH && artifactsPos[i].x < lvl->curX && std::round(lvl->curY - artifactsPos[i].y) != 0 && std::round(lvl->curX - artifactsPos[i].x) != 0)
			{
				screen[GAME_HEIGHT - std::round(lvl->curY - artifactsPos[i].y)][GAME_WIDTH - std::round(lvl->curX - artifactsPos[i].x)] = Pixel::Artifact;
			}
		}
	}

	bool IsNextMoveExit(Coordinates character)
	{
		if (std::round(lvl->curY) != std::round(character.y) && std::round(lvl->curX) != std::round(character.x) && screen[GAME_HEIGHT - std::round(lvl->curY - character.y)][GAME_WIDTH - std::round(lvl->curX - character.x)] == Pixel::Door && artifacts->IsAllArtifactsCollected())
		{
			return true;
		}
		return false;
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
	std::shared_ptr<ArtifactManager> artifacts;
	std::shared_ptr<Config> config;
	std::chrono::time_point<std::chrono::steady_clock> prevLog;
};

void ScreenInit(GameLevel* lvl)
{
	lvl->curY = GAME_HEIGHT * 5;
	lvl->curX = GAME_WIDTH;
	lvl->maxX = GAME_WIDTH * 2;
	lvl->maxY = GAME_HEIGHT * 5;
}

SDL_AppResult KeyEvent(GameLevel* lvl, const bool* keys, std::shared_ptr<Player> player, Screen& screen)
{
	if (keys[SDL_SCANCODE_ESCAPE])
	{
		return SDL_APP_SUCCESS;
	}
	if (keys[SDL_SCANCODE_R])
	{
		ScreenInit(lvl);
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

void MovingScreen(shared_ptr<Player> player, GameLevel* lvl)
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

SDL_AppResult SDL_AppIterate(void* appstate)
{
	AppState* as = (AppState*)appstate;
	GameLevel* lvl = &as->level;
	Screen* scr = &as->screen;
	shared_ptr<Player> player = as->player;
	const Uint64 now = SDL_GetTicks();
	SDL_FRect r;
	int ct;

	bool playerMoving = false;

	std::thread logThread(Log, as);
	//Log(as);

	std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
	TIME = std::chrono::duration_cast<std::chrono::milliseconds>(current - as->prevLog).count();
	while (TIME < 5)
	{
		std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
		TIME = std::chrono::duration_cast<std::chrono::milliseconds>(current - as->prevLog).count();
	}
	as->prevLog = current;

	int color = 0;

	const bool* keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_ESCAPE] || keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_R])
	{
		KeyEvent(lvl, keys, player, as->screen);
		playerMoving = true;
	}
	else
	{
		playerMoving = false;
	}

	if (!movingScreen)		//TODO
	{
		lvl->curY = std::round(lvl->curY);
	}

	player->Move(TIME);

	as->artifacts->CollectingArtifact(player->GetPlayerPos());
	if (!player->IsOnGround())
	{
		player->MoveHorizontalInAir(TIME);
	}
	if ((player->IsOnGround() || player->IsGroundOnPlatform(TIME)) && !playerMoving)
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
	SDL_RenderPresent(as->renderer);
	logThread.join();
	return SDL_APP_CONTINUE;
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

	ScreenInit(&as->level);

	as->config = std::make_shared<Config>();

	shared_ptr<Platforms> plat = std::make_shared<Platforms>(&as->level, GAME_WIDTH, GAME_HEIGHT, as->config);

	as->artifacts = std::make_shared<ArtifactManager>(&as->level, as->config);

	as->player = std::make_shared<Player>(&as->level, GAME_WIDTH, GAME_HEIGHT, as->level.maxY, BLOCK_SIZE_IN_PIXELS, plat, as->config);

	as->screen = Screen(&as->level, as->player, plat, as->artifacts);

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