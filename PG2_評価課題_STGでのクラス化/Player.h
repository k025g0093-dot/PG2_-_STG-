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

	void PlayerUpdata(char keys[256]);
	void PlayerDraw();
	Bullet* bullet[250];

	void OnDamage(int damage); // ダメージを受けた時の関数
	bool GetIsAlive() const { return isAlive_; };

private://関数限定の
	void MovePlayer(char keys[256]);

private://変数限定

	Vector2 pos_;
	Vector2 speed_;
	float radius_;
	float  shotCt_ = 3.0f;
	int hp_;
	int MaxHp_;
	bool isAlive_;
	float chargeTimer_ = 0.0f;     // 溜まった時間
	const float kFullCharge = 60.0f; // 60フレーム(1秒)で最大
	bool isCharging_ = false;
	int invincibleTimer_ = 0; // 無敵時間タイマー
};

