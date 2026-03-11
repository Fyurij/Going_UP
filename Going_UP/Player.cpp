#include "Player.h"
#include "Structures.h"

#include <cmath>

bool Player::IsGroundOnPlatform(double time)
{
	std::vector<Rectangle> platforms = plat->GetPlatforms();
	for (int i = 0; i < platforms.size(); ++i)
	{
		if ( ( player.x >= platforms[i].start.x && player.x < platforms[i].finish.x ) && static_cast<int>(player.y + 1) == platforms[i].start.y && speedUp < 0)
		{
			saveHeight = platforms[i].start.y;
			platform = platforms[i];
			return true;
		}
	}
	saveHeight = maxY - 1;
	return false;
}

void Player::CheckStayingOnGround()
{
	std::vector<Rectangle> platforms = plat->GetPlatforms();
	for (int i = 0; i < platforms.size(); ++i)
	{
		if ((player.x < platform.start.x || player.x >= platform.finish.x) && (!CheckBottom() || player.y + 2 != lvl->maxY))	//
		{
			onGround = false;
			return;
		}
	}
	onGround = true;
}

void Player::HitAbove(double time)
{
	std::vector<Rectangle> platforms = plat->GetPlatforms();
	for (int i = 0; i < platforms.size(); ++i)
	{
		if ( ( (player.x >= platforms[i].start.x && player.x < platforms[i].finish.x) && ( static_cast<int>(player.y) > platforms[i].start.y && static_cast<int>(std::round(player.y - 1)) <= platforms[i].start.y ) ) || static_cast<int>(player.y) <= 1 )
		{
			speedUp = -3;
			return;
		}
	}
}

void Player::Move(double time)
{
	if (((!CheckBottom() || (player.y - (speedUp * time / divide)) * blockSizeInPixels < lvl->maxY * blockSizeInPixels) && !onGround && !IsGroundOnPlatform(time)) && player.y <= lvl->maxY)		//
	{
		HitAbove(time);
		player.y -= speedUp * (time / divide);
		speedUp -= 200 * (time / divide);
	}
	else
	{
		player.y = saveHeight - 1;					
		speedUp = 0;
		speedHorizont = 0;
		movingHorizontal = false;
		CheckStayingOnGround();
	}
}

void Player::MoveHorizontalInAir(double time)
{
	std::vector<Rectangle> platforms = plat->GetPlatforms();
	for (int i = 0; i < platforms.size(); ++i)
	{
		if (player.x - speedHorizont * (time / divide) < platforms[i].finish.x && player.x + speedHorizont * (time / divide) > platforms[i].start.x && ( static_cast<int>(std::round(player.y)) == static_cast<int>(std::round(platforms[i].finish.y)) || static_cast<int>(std::round(player.y)) - 1 == static_cast<int>(std::round(platforms[i].finish.y))))
		{
			return;
		}
	}
	if (speedHorizont > 0 && player.x - speedHorizont * (time / divide) > 1)
	{
		player.x -= speedHorizont * (time / divide);
	}
	if (speedHorizont < 0 && player.x + abs(speedHorizont) * (time / divide) < lvl->maxX - 2)
	{
		player.x += abs(speedHorizont) * (time / divide);
	}
}

void Player::MoveRight(double time)
{
	if (player.x + 1 < lvl->maxX - 1)
	{
		//++player.x;
		player.x += abs(20) * (time / divide);
	}
}

void Player::MoveLeft(double time)
{
	if (player.x - 1 > 0)
	{
		//--player.x;
		player.x -= 20 * (time / divide);
	}
}

void Player::SpeedUp()
{
	speedUp = 75;
}

void Player::SpeedLeft()
{
	speedHorizont = 20;
	movingHorizontal = true;
}

void Player::SpeedRight()
{
	speedHorizont = -20;
	movingHorizontal = true;
}

Coordinates& Player::GetPlayerPos()
{
	return player;
}

void Player::SetPlayerPos(Coordinates& koord)
{
	player.y = koord.y;
	player.x = koord.x;
}

void Player::SetOnGround(bool ground)
{
	onGround = ground;
}

bool Player::IsOnGround()
{
	return onGround;
}

bool Player::CheckBottom()
{
	return (lvl->curY == gameHeight * 5);
}

bool Player::IsMovingHorizontal()
{
	return movingHorizontal;
}

void Player::TeleportToRightSide()
{
	player.x = lvl->maxX - 2;
	lvl->curX = lvl->maxX;
}

void Player::TeleportToLeftSide()
{
	player.x = 1;
	lvl->curX = gameWidth;
}