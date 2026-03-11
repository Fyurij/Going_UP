#pragma once

#include "Structures.h"
#include "Config.h"

#include <fstream>
#include <vector>
#include <string>
#include <iostream>

class Platforms
{
private:
	GameLevel* lvl;
	std::vector<Rectangle> platforms;
	int gameWidth;
	int gameHeight;
	std::shared_ptr<Config> config;
public:
	bool CheckPlatformPlace(Rectangle& rect);

	Platforms(GameLevel* lvl_, int gameWidth_, int gameHeight_, std::shared_ptr<Config> config_)
		:lvl(lvl_)
		, gameWidth(gameWidth_)
		, gameHeight(gameHeight_)
		, config(config_)
	{
		platforms = config_->GetPlatforms();
	}

	std::vector<Rectangle>& GetPlatforms();
};