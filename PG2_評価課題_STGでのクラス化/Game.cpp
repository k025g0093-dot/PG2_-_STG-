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

void Game::Updata(char keys[256]) {

    //プレイヤーの更新処理
    player_->PlayerUpdata(keys);

    //敵の更新処理
    for (int i = 0; i < maxEnemy; i++) {
        enemy[i]->EnemyUpdata();
    }
    scrollY_ += 2.0f;
    if (scrollY_ >= 64.0f) { // グリッドの幅を超えたらリセット
        scrollY_ = 0.0f;
    }

for (int i = 0; i < maxEnemy; i++) {
    // 敵が生きていないなら判定を飛ばす
    if (!enemy[i]->GetIsAlive()) continue; 

    for (int b = 0; b < 250; b++) {
        // 弾が発射されていないなら判定を飛ばす
        if (!player_->bullet[b]->bulletStatus.isShot) continue;

        // --- 1. 距離ベクトルを計算 ---
        // ※ bullet[i] ではなく bullet[b] に修正
        Vector2 dist = {
            enemy[i]->EnemyGetPos().x - player_->bullet[b]->bulletStatus.transform_.x,
            enemy[i]->EnemyGetPos().y - player_->bullet[b]->bulletStatus.transform_.y
        };

        // --- 2. 距離の2乗を計算 ---
        float distLengthSq = (dist.x * dist.x) + (dist.y * dist.y);

        // --- 3. 半径の合計の2乗を計算 ---
        float radiusSum = enemy[i]->GetRadius() + player_->bullet[b]->bulletStatus.radius_;
        float radiusSumSq = radiusSum * radiusSum;

        // --- 4. 判定 ---
        if (distLengthSq <= radiusSumSq) {
            // 当たった時の処理
            enemy[i]->HitGet(); // 敵にダメージなど
            player_->bullet[b]->bulletStatus.isShot = false; // 弾を消す
        }
    }
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
