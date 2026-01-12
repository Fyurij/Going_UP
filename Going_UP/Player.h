#pragma once
#include "Structures.h"
#include "Platforms.h"

class Player
{
private:
	int saveHeight = 1;
	Koordinates player;
	std::shared_ptr<Platforms> plat;
	GameLevel* lvl;
	bool onGround = true;
	int gameWidth;
	int gameHeight;
	int blockSizeInPixels;
	double speedUp = 0;
	double speedHorizont = 0;
	const int divide = 1000;
	Rectangle platform;
public:
	Player(GameLevel* lvl_, int gameWidth_, int gameHeight_, int blockSizeInPixels_, std::shared_ptr<Platforms> plat_)
		: lvl(lvl_)
		, gameWidth(gameWidth_)
		, gameHeight(gameHeight_)
		, blockSizeInPixels(blockSizeInPixels_)
		, plat(plat_)
	{
		player.x = gameWidth / 2;
		player.y = gameHeight - 2;
	}

	bool CheckPlatforms(double time);
	void CheckStayingOnGround();
	void HitAbove(double time);
	void Move(double time);
	void MoveHorizontalInAir(double time);
	void MoveRight();
	void MoveLeft();
	void SpeedUp();
	void SpeedLeft();
	void SpeedRight();
	Koordinates& GetPlayerPos();
	void SetOnGround(bool ground);
	bool IsOnGround();
};