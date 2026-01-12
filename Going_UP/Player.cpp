#include "Player.h"
#include "Structures.h"

#include <cmath>

bool Player::CheckPlatforms(double time)
{
	std::vector<Rectangle> platforms = plat->GetPlatforms();
	for (int i = 0; i < platforms.size(); ++i)
	{
		if (player.x >= platforms[i].start.x && player.x <= platforms[i].finish.x && static_cast<int>(player.y - (speedUp * time / divide) * blockSizeInPixels) == gameHeight - platforms[i].start.y)
		{
			saveHeight = platforms[i].start.y;
			return false;
		}
	}
	saveHeight = 1;
	return true;
}

void Player::Move(double time)
{
	if (((player.y - (speedUp * time / divide)) * blockSizeInPixels < gameHeight * blockSizeInPixels) && !onGround && CheckPlatforms(time))
	{
		player.y -= speedUp * (time / divide);
		speedUp -= 400 * (time / divide);
	}
	else
	{
		player.y = gameHeight - (saveHeight + 1);
		speedUp = 0;
		speedHorizont = 0;
		onGround = true;
	}
}

void Player::MoveHorizontalInAir(double time)
{
	if (speedHorizont > 0 && player.x - speedHorizont * (time / divide) > 1)
	{
		player.x -= speedHorizont * (time / divide);
	}
	if (speedHorizont < 0 && player.x + abs(speedHorizont) * (time / divide) < gameWidth - 1)
	{
		player.x += abs(speedHorizont) * (time / divide);
	}
}

void Player::MoveRight()
{
	if (player.x + 1 < gameWidth - 1)
	{
		++player.x;
	}
}

void Player::MoveLeft()
{
	if (player.x - 1 > 1)
	{
		--player.x;
	}
}

void Player::SpeedUp()
{
	speedUp = 100;
}

void Player::SpeedLeft()
{
	speedHorizont = 40;
}

void Player::SpeedRight()
{
	speedHorizont = -40;
}

Koordinates& Player::GetPlayerPos()
{
	return player;
}

void Player::SetOnGround(bool ground)
{
	onGround = ground;
}

bool Player::IsOnGround()
{
	return onGround;
}