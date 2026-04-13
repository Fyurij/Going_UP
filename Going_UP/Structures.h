#pragma once

enum class Pixel
{
	Empty,
	Border,
	Platform,
	Player,
	PlayerStart,
	Artifact,
	Door
};

struct Coordinates
{
	double x;
	double y;
};

struct Rectangle
{
	Coordinates start;
	Coordinates finish;
};

struct GameLevel
{
	int maxX;// = GAME_WIDTH;
	int maxY;// = GAME_HEIGHT * 5;
	int curX;
	double curY;
};

struct Artifact
{
	Coordinates object;
	bool isCollected;
};