#include "Config.h"
#include "Structures.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <exception>
#include <vector>
#include <filesystem>

using json = nlohmann::json;

Config::Config(GameLevel* lvl_)
	:lvl(lvl_)
	,currentLevel(1)
{
	std::string path = "../config";
	maxNumLevels = 0;
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		++maxNumLevels;
	}
	LoadLevel();
}

std::string Config::GetSide()
{
	if(data["side"] != nullptr)
	return data["side"].get<std::string>();
}

std::vector<Artifact> Config::GetArtifacts(int& max)
{
	std::vector<Artifact> arts;
	for (const auto& item : data["artifacts"])
	{
		if (item["active"].get<bool>())
		{
			Artifact temp;
			temp.object.x = item["x"].get<double>();
			temp.object.y = lvl->maxY - item["y"].get<double>();
			temp.isCollected = false;
			arts.push_back(temp);
		}
	}
	max = arts.size();
	return arts;
}

std::vector<Rectangle> Config::GetPlatforms()
{
	std::vector<Rectangle> platforms;
	for (const auto& item : data["platforms"])
	{
		Rectangle temp;
		temp.start.x = item["startX"].get<double>();
		temp.start.y = lvl->maxY - item["startY"].get<double>();
		temp.finish.x = item["finishX"].get<double>();
		temp.finish.y = lvl->maxY - item["finishY"].get<double>();
		platforms.push_back(temp);
	}
	return platforms;
}

void Config::LoadLevel()
{
	std::string name = "../config/Level_" + std::to_string(currentLevel) + ".json";
	std::ifstream f(name);
	if (!f.is_open())
	{
		throw std::runtime_error("File is not opened...");
	}
	data = json::parse(f);
}

bool Config::UpdateLevelData()
{
	++currentLevel;
	if (currentLevel > maxNumLevels)
	{
		return false;
	}
	LoadLevel();
	return true;
}

int Config::GetCurrentLevel()
{
	return currentLevel;
}

int Config::GetMaxLevel()
{
	return maxNumLevels;
}