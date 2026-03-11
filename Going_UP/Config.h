#pragma once
#include "Structures.h"

#include <nlohmann/json.hpp>

#include <vector>

using json = nlohmann::json;

class Config
{
private:
	json data;
public:
	Config();
	std::string GetSide();
	std::vector<Artifact> GetArtifacts();
	std::vector<Rectangle> GetPlatforms();
};