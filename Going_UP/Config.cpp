#include "Config.h"
#include "Structures.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <exception>
#include <vector>

using json = nlohmann::json;

Config::Config(GameLevel* lvl_)
	:lvl(lvl_)
{
	std::ifstream f("../config/settings.json");
	if (!f.is_open())
	{
		throw std::runtime_error("File is not opened...");
	}
	data = json::parse(f);
}

std::string Config::GetSide()
{
	return data["side"].get<std::string>();
}

std::vector<Artifact> Config::GetArtifacts()
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