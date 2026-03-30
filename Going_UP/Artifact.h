#pragma once

#include "Structures.h"
#include "Config.h"

#include <string>
#include <vector>
#include <memory>

class ArtifactManager
{
private:
	GameLevel* lvl;
	std::vector<Artifact> artifacts;
	int max = 0;
	int collected = 0;
	std::string side;
	std::shared_ptr<Config> config;
public:
	ArtifactManager(GameLevel* lvl_, std::shared_ptr<Config> config_)
		:lvl(lvl_)
		, config(config_)
	{
		artifacts = config->GetArtifacts();
		max = artifacts.size();
		side = config->GetSide();
	}

	void CollectingArtifact(Coordinates player);
	std::vector<Coordinates> GetArtifactsPos();
	bool IsAllArtifactsCollected();
	std::string WhichSideDoor();
	int GetCollectedArtifacts();
	int GetMaxNumOfArtifacts();
};