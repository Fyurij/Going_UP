#include "Screen.h"
#include "Structures.h"
#include "Artifact.h"
#include "Player.h"
#include "Platforms.h"

#include <vector>
#include <string>


bool Screen::CheckCollision(Coordinates nextPos)
{
	int y = nextPos.y;
	if (screen[y][nextPos.x] == Pixel::Empty)
	{
		return false;
	}
	return true;
}

void Screen::ClearScreen()
{
	for (int i = 0; i < screen.size(); ++i)
	{
		for (int j = 0; j < screen[i].size(); ++j)
		{
			screen[i][j] = Pixel::Empty;
		}
	}
}

void Screen::GenerateScreen()
{
	ClearScreen();
	int visibleDoorHeight = 0;
	int maxDoorHeight = 7;
	Coordinates playerPos = player->GetPlayerPos();
	Coordinates playerEndPos = player->GetPlayerEndPos();
	std::string side = artifacts->WhichSideDoor();
	for (int i = 0; i < screen.size(); ++i)
	{
		visibleDoorHeight = maxDoorHeight - (lvl->maxY - std::round(lvl->curY));
		for (int j = 0; j < screen[i].size(); ++j)
		{
			if (i == 0 && std::round(lvl->curY) == gameHeight)
			{
				screen[i][j] = Pixel::Border;
			}
			if (i == (gameHeight - 1) && std::round(lvl->curY) >= lvl->maxY)
			{
				screen[i][j] = Pixel::Border;
			}
			if (j == 0 && std::round(lvl->curX) == gameWidth)
			{
				if (i > gameHeight - visibleDoorHeight && i < gameHeight && side == "left")
				{
					screen[i][j] = Pixel::Door;
					if (std::round(lvl->curY) >= lvl->maxY && i == gameHeight - 1)
					{
						screen[i][j] = Pixel::Border;
					}
				}
				else
				{
					screen[i][j] = Pixel::Border;
				}
			}
			if (j == (gameWidth - 1) && std::round(lvl->curX) == lvl->maxX)
			{
				if (i > gameHeight - visibleDoorHeight && i < gameHeight && side == "right")
				{
					screen[i][j] = Pixel::Door;
					if (std::round(lvl->curY) >= lvl->maxY && i == gameHeight - 1)
					{
						screen[i][j] = Pixel::Border;
					}
				}
				else
				{
					screen[i][j] = Pixel::Border;
				}
			}
			if (i <= static_cast<int>(std::round(lvl->curY - playerPos.y)) && i >= static_cast<int>(std::round(lvl->curY - playerEndPos.y)) && j <= static_cast<int>(std::round(lvl->curX - playerPos.x)) && j >= static_cast<int>(std::round(lvl->curX - playerEndPos.x)) && static_cast<int>(lvl->curY - playerPos.y) > 0 && static_cast<int>(lvl->curY - playerEndPos.y) > 0 && static_cast<int>(lvl->curX - playerPos.x) > 0 && static_cast<int>(lvl->curX - playerEndPos.x) > 0)
			{
				screen[gameHeight - i][gameWidth - j] = Pixel::Player;
			}
			if (i == static_cast<int>(std::round(lvl->curY - playerPos.y)) && j == static_cast<int>(std::round(lvl->curX - playerPos.x)) && static_cast<int>(lvl->curY - playerPos.y) > 0 && static_cast<int>(lvl->curX - playerPos.x) > 0)
			{
				screen[gameHeight - i][gameWidth - j] = Pixel::PlayerStart;
			}
		}
	}
	std::vector<Rectangle> plats = platforms->GetPlatforms();
	for (int i = 0; i < plats.size(); ++i)
	{
		int j = 0;
		while (plats[i].start.x + j < plats[i].finish.x)
		{
			if (plats[i].start.y <= lvl->curY && plats[i].start.y >= lvl->curY - gameHeight && std::round(plats[i].start.y - lvl->curY) != 0 && plats[i].start.x + j < lvl->curX && plats[i].start.x + j > lvl->curX - gameWidth - 1 && screen[gameHeight - std::round(lvl->curY - plats[i].start.y)][gameWidth - std::round(lvl->curX - plats[i].start.x) + j] != Pixel::Border)
			{
				screen[gameHeight - std::round(lvl->curY - plats[i].start.y)][gameWidth - std::round(lvl->curX - plats[i].start.x) + j] = Pixel::Platform;
			}
			++j;
		}
	}
	std::vector<Coordinates> artifactsPos = artifacts->GetArtifactsPos();
	for (int i = 0; i < artifactsPos.size(); ++i)
	{
		if (artifactsPos[i].y < lvl->curY && artifactsPos[i].y > lvl->curY - gameHeight && artifactsPos[i].x > lvl->curX - gameWidth && artifactsPos[i].x < lvl->curX && std::round(lvl->curY - artifactsPos[i].y) != 0 && std::round(lvl->curX - artifactsPos[i].x) != 0)
		{
			screen[gameHeight - std::round(lvl->curY - artifactsPos[i].y)][gameWidth - std::round(lvl->curX - artifactsPos[i].x)] = Pixel::Artifact;
		}
	}
}

bool Screen::IsNextMoveExit(Coordinates character)
{
	if (std::round(lvl->curY) != std::round(character.y) && std::round(lvl->curX) != std::round(character.x) && screen[gameHeight - std::round(lvl->curY - character.y)][gameWidth - std::round(lvl->curX - character.x)] == Pixel::Door && artifacts->IsAllArtifactsCollected())
	{
		return true;
	}
	return false;
}

std::vector<std::vector<Pixel>>& Screen::GetScreen()
{
	return screen;
}

void Screen::EndOfGame()
{
	std::cout << "YOU WIN!!!" << std::endl;
}