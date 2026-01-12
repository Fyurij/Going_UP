#include "Platforms.h"

bool Platforms::CheckPlatformPlace(Rectangle& rect)
{
	if (rect.start.x < 1 && rect.start.x > gameWidth - 1 && rect.finish.x < 1 && rect.finish.x > gameWidth - 1 && rect.start.x > rect.finish.x)
	{
		return false;
	}
	if (rect.start.y < 1 && rect.start.y > gameHeight - 1 && rect.finish.y < 1 && rect.finish.y > gameHeight - 1 && rect.start.y != rect.finish.y)
	{
		return false;
	}
	return true;
}

std::vector<Rectangle>& Platforms::GetPlatforms()
{
	return platforms;
}