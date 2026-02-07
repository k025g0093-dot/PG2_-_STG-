#pragma once
#include "Vector2.h"

class Bullet
{

public:

	struct BulletStatus
	{
		Vector2 transform_;
		float radius_;
		int speed;
		bool isShot;
		bool isChargeShot; // チャージ弾フラグ
		float chargeScale;       // 弾の大きさ倍率
	};
	BulletStatus bulletStatus;

	Vector2 BulletGetPos() const { return bulletStatus.transform_; };
	float GetRadius() const { return bulletStatus.radius_; };
	Bullet();
	~Bullet();
	void Updata();
	void Draw();

};

