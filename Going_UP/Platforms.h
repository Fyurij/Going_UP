#pragma once

#include "Structures.h"

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
public:
	bool CheckPlatformPlace(Rectangle& rect);

	Platforms(GameLevel* lvl_, int gameWidth_, int gameHeight_)
		:lvl(lvl_)
		, gameWidth(gameWidth_)
		, gameHeight(gameHeight_)
	{
		const std::string filename = "../config/platforms.txt";
		std::ifstream file(filename);
		if (!file.is_open())
		{
			std::cout << "Error..." << std::endl;
			return;
		}
		while (!file.eof())
		{
			Rectangle rect;
			file >> rect.start.x;
			file >> rect.start.y;
			file >> rect.finish.x;
			file >> rect.finish.y;
			rect.start.y = lvl->maxY - rect.start.y;
			rect.finish.y = lvl->maxY - rect.finish.y;
			if (CheckPlatformPlace(rect))
			{
				platforms.push_back(rect);
			}
		}
	}

	std::vector<Rectangle>& GetPlatforms();
};