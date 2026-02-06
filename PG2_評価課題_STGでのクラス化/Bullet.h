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
	};
	BulletStatus bulletStatus;

	Vector2 BulletGetPos() const { return bulletStatus.transform_; };
	float GetRadius() const { return bulletStatus.radius_; };
	Bullet();
	~Bullet();
	void Updata();
	void Draw();

};

