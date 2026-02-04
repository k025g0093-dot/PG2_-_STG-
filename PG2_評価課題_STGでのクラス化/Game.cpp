#include "Game.h"
#include "Enemy.h"
#include "Player.h"
#include "Novice.h"
const int maxEnemy = 5;

Game::Game() {

    // 1. まず全ての要素を nullptr で初期化
    for (int i = 0; i < 100; i++) {
        enemy[i] = nullptr;
    }

    // 2. 使う分だけ new する
    for (int i = 0; i < maxEnemy; i++) {
        enemy[i] = new Enemy({ 100.0f + i * 100, 100.0f }, { 10.0f, 0.0f }, 32.0f, 10, 10, true);
    }

	player_ = new Player({ 640.0f, 500.0f }, { 10.0f, 10.0f }, 35.0f, 100, 100, true);

}

void Game::Updata(char keys[256], char preKeys[256]) {

    //プレイヤーの更新処理
    player_->PlayerUpdata(keys, preKeys);

    //敵の更新処理
    for (int i = 0; i < maxEnemy; i++) {
        enemy[i]->EnemyUpdata();
    }
    scrollY_ += 2.0f;
    if (scrollY_ >= 64.0f) { // グリッドの幅を超えたらリセット
        scrollY_ = 0.0f;
    }
}

void Game::Draw() {
    // 背景（トロン風の濃い紺色）
    Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0x000020FF, kFillModeSolid);

    Novice::SetBlendMode(kBlendModeAdd);

    // 縦線（固定）
    for (int i = 0; i <= 1280; i += 64) {
        Novice::DrawLine(i, 0, i, 720, 0x0080FFFF); // シアン（水色）
    }
    // 横線（動かす！）
    for (int i = -64; i <= 720; i += 64) {
        int y = i + (int)scrollY_;
        Novice::DrawLine(0, y, 1280, y, 0x004080FF); // 少し暗めの青
    }

    Novice::SetBlendMode(kBlendModeNormal);

    player_->PlayerDraw();

    // 敵の描画
    for (int i = 0; i < maxEnemy; i++) {
        if (enemy[i] != nullptr) enemy[i]->DrawEmemy();
    }
}
