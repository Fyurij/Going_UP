#pragma once

#include "Player.h"
#include "Platforms.h"
#include "Artifact.h"
#include "Structures.h"

#include <vector>
#include <memory>

class Screen
{
private:
	std::vector<std::vector<Pixel>> screen;
	GameLevel* lvl;
	std::shared_ptr<Platforms> platforms;
	std::shared_ptr<Player> player;
	std::shared_ptr<ArtifactManager> artifacts;
	int gameHeight;
	int gameWidth;
public:
	Screen(GameLevel* lvl_, std::shared_ptr<Player> player_, std::shared_ptr<Platforms> platforms_, std::shared_ptr<ArtifactManager> artifacts_, int gameHeight_, int gameWidth_)
		:lvl(lvl_)
		, screen(gameHeight_, std::vector(gameWidth_, Pixel::Empty))
		, platforms(platforms_)
		, player(player_)
		, artifacts(artifacts_)
		, gameHeight(gameHeight_)
		, gameWidth(gameWidth_)
	{}

	bool CheckCollision(Coordinates nextPos);
	void ClearScreen();
	void GenerateScreen();
	bool IsNextMoveExit(Coordinates character);
	std::vector<std::vector<Pixel>>& GetScreen();
	void EndOfGame();
};