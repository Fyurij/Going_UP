#include "Player.h"
#include "Structures.h"

#include <cmath>

bool Player::IsGroundOnPlatform(double time)
{
	std::vector<Rectangle> platforms = plat->GetPlatforms();
	for (int i = 0; i < platforms.size(); ++i)
	{
		if ( ( player.x >= platforms[i].start.x && player.x <= platforms[i].finish.x ) && static_cast<int>(player.y + 1) == platforms[i].start.y && speedUp < 0)
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
		if ((player.x < platform.start.x || player.x > platform.finish.x) && (!CheckBottom() || player.y + 2 != lvl->maxY))	//
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
		if ( ( (player.x >= platforms[i].start.x && player.x <= platforms[i].finish.x) && ( static_cast<int>(player.y) > platforms[i].start.y && static_cast<int>(std::round(player.y - 1)) <= platforms[i].start.y ) ) || static_cast<int>(player.y) <= 1 )
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
		speedUp -= 400 * (time / divide);
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
	if (speedHorizont > 0 && player.x - speedHorizont * (time / divide) > 1)
	{
		player.x -= speedHorizont * (time / divide);
	}
	if (speedHorizont < 0 && player.x + abs(speedHorizont) * (time / divide) < lvl->maxX - 1)
	{
		player.x += abs(speedHorizont) * (time / divide);
	}
}

void Player::MoveRight()
{
	if (player.x + 1 < lvl->maxX - 1)
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
	movingHorizontal = true;
}

void Player::SpeedRight()
{
	speedHorizont = -40;
	movingHorizontal = true;
}

Koordinates& Player::GetPlayerPos()
{
	return player;
}

void Player::SetPlayerPos(Koordinates& koord)
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