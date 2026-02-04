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

	Bullet();
	~Bullet();
	void Updata();
	void Draw();

};

