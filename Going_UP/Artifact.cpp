#include "Artifact.h"
#include "Structures.h"

#include <vector>
#include <string>
#include <algorithm>

void ArtifactManager::CollectingArtifact(Coordinates player)
{
	for (int i = 0; i < artifacts.size(); ++i)
	{
		if (std::round(player.y) == artifacts[i].object.y && std::round(player.x) == artifacts[i].object.x && !artifacts[i].isCollected)
		{
			artifacts[i].isCollected = true;
			++collected;
		}
	}
}

std::vector<Coordinates> ArtifactManager::GetArtifactsPos()
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

bool ArtifactManager::IsAllArtifactsCollected()
{
	return std::all_of(artifacts.cbegin(), artifacts.cend(), [](const auto&/* const Artifact& */ art) {return art.isCollected;});
}

std::string ArtifactManager::WhichSideDoor()
{
	return side;
}

int ArtifactManager::GetCollectedArtifacts()
{
	return collected;
}

int ArtifactManager::GetMaxNumOfArtifacts()
{
	return max;
}