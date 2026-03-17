#pragma once
#include "Structures.h"

#include <nlohmann/json.hpp>

#include <vector>

using json = nlohmann::json;

class Config
{
private:
	json data;
	GameLevel* lvl;
public:
	Config(GameLevel* lvl_);
	std::string GetSide();
	std::vector<Artifact> GetArtifacts();
	std::vector<Rectangle> GetPlatforms();
};