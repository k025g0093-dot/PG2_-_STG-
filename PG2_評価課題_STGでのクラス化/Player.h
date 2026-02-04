#pragma once
#include "Vector2.h"
#include "Bullet.h"
class Player
{

public:

	Vector2 PlayerGetPos() const { return pos_; };
	Vector2 PlayerGetSpeed() const { return speed_; };
	float GetRadius()const { return radius_; };

	Player(Vector2 pos, Vector2 speed, float radius, int MaxHp, int hp, bool isAlive);

	void PlayerUpdata(char keys[256], char preKeys[256]);
	void PlayerDraw();
	Bullet* bullet;

private://関数限定の
	void MovePlayer(char keys[256], char preKeys[256]);

private://変数限定

	Vector2 pos_;
	Vector2 speed_;
	float radius_;

};

