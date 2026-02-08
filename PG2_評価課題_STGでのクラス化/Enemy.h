#pragma once
#include "Vector2.h"
class Enemy
{

public:
	Vector2 EnemyGetPos() const { return pos_; };
	Vector2 EnemyGetSpeed() const { return speed_; };
	float GetRadius() const { return radius_; };
	void HitGet(int dmg);
	bool GetIsAlive() const { return isAlive_; };
	Enemy(Vector2 pos, Vector2 speed, float radius,int MaxHp,int hp,bool isAlive);
	void EnemyUpdata();
	void DrawEmemy();
	int invincibleTimer_;//無敵時間
private:
	void MoveEnemy();
private:
	Vector2 pos_;
	Vector2 speed_;
	float radius_;
	int MaxHp_;
	int hp_;
	bool isAlive_;
	int rsTimer_;

};

