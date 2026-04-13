#include "Player.h"
#include "Structures.h"

#include <cmath>

void Player::UpdateEndPlayer()
{
	endPlayer.x = player.x + playerSize - 1;
	endPlayer.y = player.y + playerSize - 1;
}

bool Player::IsGroundOnPlatform(double time, std::shared_ptr<Platforms> plat)
{
	std::vector<Rectangle> platforms = plat->GetPlatforms();
	for (int i = 0; i < platforms.size(); ++i)
	{
		if ( (endPlayer.x >= platforms[i].start.x && player.x < platforms[i].finish.x) && static_cast<int>(endPlayer.y + 1) == platforms[i].start.y && speedUp < 0)
		{
			saveHeight = platforms[i].start.y;
			platform = platforms[i];
			return true;
		}
	}
	saveHeight = maxY - 1;
	return false;
}

void Player::CheckStayingOnGround(std::shared_ptr<Platforms> plat)
{
	std::vector<Rectangle> platforms = plat->GetPlatforms();
	for (int i = 0; i < platforms.size(); ++i)
	{
		if (((endPlayer.x < platform.start.x) || player.x >= platform.finish.x) && (!CheckBottom() || endPlayer.y + 2 < lvl->maxY))	//
		{
			onGround = false;
			return;
		}
	}
	onGround = true;
}

void Player::HitAbove(double time, std::shared_ptr<Platforms> plat)
{
	std::vector<Rectangle> platforms = plat->GetPlatforms();
	for (int i = 0; i < platforms.size(); ++i)
	{
		if ( ( (endPlayer.x >= platforms[i].start.x && player.x < platforms[i].finish.x) && ( static_cast<int>(player.y) > platforms[i].start.y && static_cast<int>(std::round(player.y - 1)) <= platforms[i].start.y ) ) || static_cast<int>(endPlayer.y) <= 1 )
		{
			speedUp = -3;
			return;
		}
	}
}

void Player::Move(double time, std::shared_ptr<Platforms> plat)
{
	if (((!CheckBottom() || (endPlayer.y - (speedUp * time / divide)) * blockSizeInPixels < lvl->maxY * blockSizeInPixels) && !onGround && !IsGroundOnPlatform(time, plat)) && endPlayer.y <= lvl->maxY)		//
	{
		HitAbove(time, plat);
		player.y -= speedUp * (time / divide);
		speedUp -= 200 * (time / divide);
	}
	else
	{
		player.y = saveHeight - 1 - (playerSize - 1);					
		speedUp = 0;
		speedHorizont = 0;
		movingHorizontal = false;
		CheckStayingOnGround(plat);
	}
	UpdateEndPlayer();
}

void Player::MoveHorizontalInAir(double time, std::shared_ptr<Platforms> plat)
{
	std::vector<Rectangle> platforms = plat->GetPlatforms();
	for (int i = 0; i < platforms.size(); ++i)
	{
		if (player.x - speedHorizont * (time / divide) < platforms[i].finish.x && endPlayer.x + speedHorizont * (time / divide) > platforms[i].start.x && ( static_cast<int>(std::round(player.y)) == static_cast<int>(std::round(platforms[i].finish.y)) || static_cast<int>(std::round(player.y)) - 1 == static_cast<int>(std::round(platforms[i].finish.y))))
		{
			return;
		}
	}
	if (speedHorizont > 0 && player.x - speedHorizont * (time / divide) > 1)
	{
		player.x -= speedHorizont * (time / divide);
	}
	if (speedHorizont < 0 && endPlayer.x + abs(speedHorizont) * (time / divide) < lvl->maxX - 2)
	{
		player.x += abs(speedHorizont) * (time / divide);
	}
	UpdateEndPlayer();
}

void Player::MoveRight(double time)
{
	if (endPlayer.x + 1 < lvl->maxX - 1)
	{
		//++player.x;
		player.x += abs(20) * (time / divide);
	}
	UpdateEndPlayer();
}

void Player::MoveLeft(double time)
{
	if (player.x - 1 > 0)
	{
		//--player.x;
		player.x -= 20 * (time / divide);
	}
	UpdateEndPlayer();
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

Coordinates& Player::GetPlayerEndPos()
{
	return endPlayer;
}

void Player::SetPlayerPos(Coordinates& koord)
{
	player.y = koord.y;
	player.x = koord.x;
	UpdateEndPlayer();
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
	player.x = lvl->maxX - 2 - (playerSize - 1);
	lvl->curX = lvl->maxX;
	UpdateEndPlayer();
}

void Player::TeleportToLeftSide()
{
	player.x = 1;
	lvl->curX = gameWidth;
	UpdateEndPlayer();
}

void Player::ResetPosition()
{
	player.x = lvl->curX - (gameWidth / 2);
	player.y = lvl->maxY - 2;
	UpdateEndPlayer();
}

int Player::GetPlayerSize()
{
	return playerSize;
}