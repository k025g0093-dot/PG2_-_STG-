#pragma once
#include "Enemy.h"
#include "Player.h"
class Game
{

public:
	Game();//コンストラクタ
	~Game();//デストラクタ

	void Updata(char keys[256]);//更新処理
	void Draw();



private:
	//ここから下にシーン旋回のアニメーションだったりの実装を行う
	Enemy* enemy[100];
	Player* player_;
	float scrollY_ = 0.0f; // 背景を流すためのオフセット
};

