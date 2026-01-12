#pragma once

enum class Pixel
{
	Empty,
	Border,
	Platform,
	Player
};

struct Koordinates
{
	double x;
	double y;
};

struct Rectangle
{
	Koordinates start;
	Koordinates finish;
};

struct GameLevel
{
	int maxX;// = GAME_WIDTH;
	int maxY;// = GAME_HEIGHT * 5;
	//int curX;
	int curY;
};