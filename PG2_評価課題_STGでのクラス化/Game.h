#pragma once
#include "Enemy.h"
#include "Player.h"
#include <vector>
enum GameSceen
{
	TITLE,
	GAMEPLAY,
	GAMEOVER,
	RESULT
};
class Game
{

public:
	Game();//コンストラクタ
	~Game();//デストラクタ

	void Updata(char keys[256], char preKeys[256]);//更新処理
	void Draw();
	void Init();
	void SaveScore();
	void LoadRanking();
private:
	//ここから下にシーン旋回のアニメーションだったりの実装を行う・
	Enemy* enemy[100];
	Player* player_;
	float scrollY_ = 0.0f; // 背景を流すためのオフセット
	float shakeIntensity_ = 0.0f; // 現在の揺れの強さ
	int shakeTimer_ = 0;          // 揺れている残り時間
};

