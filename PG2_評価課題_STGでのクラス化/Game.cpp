#include "Game.h"
#include "Enemy.h"
#include "Player.h"
#include "Novice.h"
#include <fstream> // ファイル操作に必要
#include <algorithm> // sort を使うために必要
#include <vector>
const int maxEnemy = 10;
int gameSceen = TITLE;
int score = 0;
Game::Game() {
	Init();
}

void Game::Init() {

	// 1. まず全ての要素を nullptr で初期化
	for (int i = 0; i < 100; i++) {
		enemy[i] = nullptr;
	}

	// 2. 使う分だけ new する
	for (int i = 0; i < maxEnemy; i++) {
		enemy[i] = new Enemy({ 100.0f + i * 100, 100.0f }, { 10.0f, 0.0f }, 32.0f, 10, 10, true);
	}

	player_ = new Player({ 640.0f, 500.0f }, { 10.0f, 10.0f }, 35.0f, 10, 10, true);
}

void Game::Updata(char keys[256], char preKeys[256]) {


	//各ゲームシーンの処理
	switch (gameSceen)
	{

	case TITLE:

		//初期化関数の呼び出し
		Init();

		//背景画面のスクロール処理
		scrollY_ += 2.0f;
		if (scrollY_ >= 64.0f) { // グリッドの幅を超えたらリセット
			scrollY_ = 0.0f;
		}

		//次のシーンへの線処理
		if (preKeys[DIK_SPACE] && !keys[DIK_SPACE]) {
			gameSceen = GAMEPLAY;
		}



		break;


	case GAMEPLAY:


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


		//敵と弾の当たり判定
		for (int i = 0; i < maxEnemy; i++) {
			// 敵が生きていないなら判定を飛ばす
			if (enemy[i]->GetIsAlive()){

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
					float currentBulletRadius = player_->bullet[b]->bulletStatus.radius_ * player_->bullet[b]->bulletStatus.chargeScale;

					float radiusSum = enemy[i]->GetRadius() + currentBulletRadius;
					float radiusSumSq = radiusSum * radiusSum;

					// --- 4. 判定 ---
					if (distLengthSq <= radiusSumSq) {
						// ダメージ計算
						int damage = (int)player_->bullet[b]->bulletStatus.chargeScale;
						enemy[i]->HitGet(damage);

						// --- ここでシェイクをセット！ ---
						// ダメージ1につき3px揺らす、時間は10フレームなど
						this->shakeIntensity_ = (float)damage * 3.0f;
						this->shakeTimer_ = 10;

						player_->bullet[b]->bulletStatus.isShot = false;
					}
				}
			}
			else {
				//スコア加算処理
				score += 100;
			}
		}


		// --- プレイヤーと敵の本体当たり判定 ---
		if (player_->GetIsAlive()) {
			for (int i = 0; i < maxEnemy; i++) {
				if (!enemy[i]->GetIsAlive()) continue;

				// 距離の計算
				Vector2 dist = {
					player_->PlayerGetPos().x - enemy[i]->EnemyGetPos().x,
					player_->PlayerGetPos().y - enemy[i]->EnemyGetPos().y
				};
				float distLengthSq = (dist.x * dist.x) + (dist.y * dist.y);

				// 半径の合計
				float radiusSum = player_->GetRadius() + enemy[i]->GetRadius();
				float radiusSumSq = radiusSum * radiusSum;

				if (distLengthSq <= radiusSumSq) {
					// プレイヤーにダメージ（10など固定値でもOK）
					player_->OnDamage(1);

					// プレイヤーがダメージを受けた時も少し画面を揺らすと「痛い」感じが出ます
					this->shakeIntensity_ = 10.0f;
					this->shakeTimer_ = 15;
				}
			}
		}


		//次のシーンへの遷移処理
		// player_が実在し、かつ死んでいる（Aliveがfalse）なら
		if (player_ != nullptr && !player_->GetIsAlive()) {
			SaveScore();//スコアの保存
			gameSceen = RESULT;
		}

		break;

	case RESULT:

		scrollY_ += 2.0f;
		if (scrollY_ >= 64.0f) { // グリッドの幅を超えたらリセット
			scrollY_ = 0.0f;
		}

		// ゲームオーバー画面でスペースキーを押したらタイトルに戻る
		if (preKeys[DIK_SPACE] && !keys[DIK_SPACE]) {
			gameSceen = TITLE;//タイトルシーンへ
		}


		break;


	default:
		break;
	}

}

void Game::Draw() {

	float offsetX = 0.0f;
	float offsetY = 0.0f;

	switch (gameSceen)
	{

	case TITLE:

#pragma region  タイトルシーンの描画
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

#pragma endregion

		break;

	case GAMEPLAY:

#pragma region  ゲーム実行中の描画



		if (shakeTimer_ > 0) {
			shakeTimer_--;
			// -intensity 〜 +intensity の範囲でランダムにずらす
			offsetX = (float)(rand() % 100 / 100.0f * shakeIntensity_ * 2.0f) - shakeIntensity_;
			offsetY = (float)(rand() % 100 / 100.0f * shakeIntensity_ * 2.0f) - shakeIntensity_;
		}

		// 背景（トロン風の濃い紺色）
		Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0x000020FF, kFillModeSolid);

		Novice::SetBlendMode(kBlendModeAdd);

		for (int i = 0; i <= 1280; i += 64) {
			int x = i + (int)offsetX; // 横の揺れを適用
			Novice::DrawLine(x, 0, x, 720, 0x0080FFFF);
		}

		// 横線（動かす！）に縦揺れ(offsetY)を足す
		for (int i = -64; i <= 720; i += 64) {
			int y = i + (int)scrollY_ + (int)offsetY; // 縦の揺れを適用
			Novice::DrawLine(0, y, 1280, y, 0x004080FF);
		}

		Novice::SetBlendMode(kBlendModeNormal);

		player_->PlayerDraw();

		// 敵の描画
		for (int i = 0; i < maxEnemy; i++) {
			if (enemy[i] != nullptr) enemy[i]->DrawEmemy();
		}
#pragma endregion 


		break;


	case RESULT://

#pragma region  ゲームオーバーの描画処理

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



#pragma endregion


		break;

	default:
		break;
	}



}



void Game::SaveScore() {
	// 1. 既存のランキングを読み込むためのリスト
	std::vector<int> scores;

	// 2. ファイルから過去のスコアをすべて読み込む
	std::ifstream ifs("score.txt");
	if (ifs.is_open()) {
		int s;
		while (ifs >> s) {
			scores.push_back(s);
		}
		ifs.close();
	}

	// 3. 今回のスコアをリストに追加
	scores.push_back(score);

	// 4. 高い順（降順）に並べ替える
	// std::greater<int>() を使うと、100, 80, 50... の順になります
	std::sort(scores.begin(), scores.end(), std::greater<int>());

	// 5. 上位5つだけに絞る（リストを最大5個にする）
	if (scores.size() > 5) {
		scores.resize(5);
	}

	// 6. ファイルに書き戻す（上書き）
	std::ofstream ofs("score.txt");
	if (ofs.is_open()) {
		for (int s : scores) {
			ofs << s << std::endl; // 1スコアごとに改行
		}
		ofs.close();
	}
}

void Game::LoadRanking(){
	// 1. ファイルからスコアを読み込む
	std::ifstream ifs("score.txt");
	if (ifs.is_open()) {
		int s;
		int line = 0;
		while (ifs >> s && line < 5) { // 最大5行まで
			Novice::ScreenPrintf(0, line * 20, "Rank %d: %d", line + 1, s);
			line++;
		}
		ifs.close();
	}
}
