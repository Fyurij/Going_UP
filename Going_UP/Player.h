#pragma once
#include "Structures.h"
#include "Platforms.h"
#include "Config.h"

class Player
{
private:
	int saveHeight;
	Coordinates player;
	//std::shared_ptr<Platforms> plat;
	std::shared_ptr<Config> config;
	GameLevel* lvl;
	bool onGround = true;
	bool movingHorizontal = false;
	int gameWidth;
	int gameHeight;
	int maxY;
	int blockSizeInPixels;
	double speedUp = 0;
	double speedHorizont = 0;
	const int divide = 1000;
	Rectangle platform;
public:
	Player(GameLevel* lvl_, int gameWidth_, int gameHeight_, int maxY_, int blockSizeInPixels_, std::shared_ptr<Config> config_)
		: lvl(lvl_)
		, gameWidth(gameWidth_)
		, gameHeight(gameHeight_)
		, blockSizeInPixels(blockSizeInPixels_)
		//, plat(plat_)
		, maxY(maxY_)
		, saveHeight(maxY_ - 1)
		, config(config_)
	{
		player.x = gameWidth_ / 2;
		player.y = lvl->maxY - 2;
	}

	bool IsGroundOnPlatform(double time, std::shared_ptr<Platforms> plat);
	void CheckStayingOnGround(std::shared_ptr<Platforms> plat);
	void HitAbove(double time, std::shared_ptr<Platforms> plat);
	void Move(double time, std::shared_ptr<Platforms> plat);
	void MoveHorizontalInAir(double time, std::shared_ptr<Platforms> plat);
	void MoveRight(double time);
	void MoveLeft(double time);
	void SpeedUp();
	void SpeedLeft();
	void SpeedRight();
	Coordinates& GetPlayerPos();
	void SetPlayerPos(Coordinates& koord);
	void SetOnGround(bool ground);
	bool IsOnGround();
	bool CheckBottom();
	bool IsMovingHorizontal();
	void TeleportToRightSide();
	void TeleportToLeftSide();
	void ResetPosition();
};