#include "Enemy.h"
#include "Novice.h"
#include <corecrt_math.h>
#include <corecrt_math_defines.h>

//-------------------------------------------------
//パブリック関数群
//-------------------------------------------------

// HSVから0xRRGGBBAA形式に変換する関数
unsigned int HSVToRGBA(float h, float s, float v, unsigned char alpha) {
	float r, g, b;

	int i = (int)(h / 60.0f) % 6;
	float f = (h / 60.0f) - (int)(h / 60.0f);
	float p = v * (1.0f - s);
	float q = v * (1.0f - f * s);
	float t = v * (1.0f - (1.0f - f) * s);

	switch (i) {
	case 0: r = v; g = t; b = p; break;
	case 1: r = q; g = v; b = p; break;
	case 2: r = p; g = v; b = t; break;
	case 3: r = p; g = q; b = v; break;
	case 4: r = t; g = p; b = v; break;
	case 5: r = v; g = p; b = q; break;
	default: r = 0; g = 0; b = 0; break;
	}

	return ((unsigned int)(r * 255) << 24) |
		((unsigned int)(g * 255) << 16) |
		((unsigned int)(b * 255) << 8) |
		alpha;
}

//コンストラクタ
Enemy::Enemy(Vector2 pos, Vector2 speed,
	float radius, int MaxHp,
	int hp, bool isAlive) {
	pos_ = pos;
	speed_ = speed;
	radius_ = radius;
	MaxHp_ = MaxHp;
	hp_ = hp;
	isAlive_ = isAlive;
	rsTimer_ = 5;
}

//更新処理
void Enemy::EnemyUpdata() {
    particle_.UpdateBgParticle();
    if (invincibleTimer_ > 0) {
        invincibleTimer_--;
    }

	if (isAlive_) {
		MoveEnemy();
	}
	else {
		if (rsTimer_ > 0) {
			rsTimer_--;
		}
		else if (rsTimer_ <= 0) {
			isAlive_ = true;
            rsTimer_ = 5;
            hp_ = MaxHp_;
			pos_ = { (float)(rand() % 1180 + 50),50.0f };
		}
	}
}


void Enemy::DrawEmemy() {
    if (!isAlive_) return;

#ifdef DEBUG
    Novice::DrawEllipse((int)pos_.x, (int)pos_.y, (int)radius_, (int)radius_, 0.0f, 0xFFFFFFFF, kFillModeSolid);

#endif // DEBUG

    if (hp_ <= 5) {
        // 5フレームに1回描画をスキップすることで点滅を表現
        // staticなカウント用変数を作るか、既存のtimerを利用します
        static int flashTimer = 0;
        flashTimer++;
        if (flashTimer % 10 < 5) { // 10フレーム周期で5フレーム分消す
            return;
        }
    }

    Novice::SetBlendMode(kBlendModeAdd);
    static float timer = 0.0f;
    timer += 0.025f; // 少しだけスピードアップして躍動感を出す

    // 色：より純粋で強いオレンジ (HSV: 30度付近)
    float baseHue = 30.0f;

    const int layerCount = 4;
    const int vertices = 6;

    for (int layer = 0; layer < layerCount; layer++) {
        float layerRot = (layer % 2 == 0) ? timer * (layer + 1) * 0.5f : -timer * (layer + 1) * 0.3f;
        float layerScale = 1.2f - (layer * 0.25f);
        float currentRadius = radius_ * layerScale;

        // アルファ値を以前より底上げ（最小でも100くらい確保して「強さ」を出す）
        unsigned char alpha = (unsigned char)(255 / (layer + 1));
        // 彩度(S)を 1.0f に固定して色を濃く、明度(V)も 1.0f で発光感を最大に
        unsigned int color = HSVToRGBA(baseHue, 1.0f, 1.0f, alpha);

        for (int i = 0; i < vertices; i++) {
            float t1 = (2.0f * (float)M_PI * i) / (float)vertices + layerRot;
            float t2 = (2.0f * (float)M_PI * (i + 1)) / (float)vertices + layerRot;

            int x1 = (int)(pos_.x + cosf(t1) * currentRadius);
            int y1 = (int)(pos_.y + sinf(t1) * currentRadius);
            int x2 = (int)(pos_.x + cosf(t2) * currentRadius);
            int y2 = (int)(pos_.y + sinf(t2) * currentRadius);

            // 【色の強化テクニック】
            // 全く同じ線を2回描く、または1ピクセルずらして描くと、色が重なって「太く強く」見えます
            Novice::DrawLine(x1, y1, x2, y2, color);
            Novice::DrawLine(x1 + 1, y1, x2 + 1, y2, color); // 擬似的に線を太く見せる

            // 層を繋ぐガイドラインも、少しだけ彩度を上げた白に近いオレンジに
            if (layer < layerCount - 1) {
                float nextLayerRot = ((layer + 1) % 2 == 0) ? timer * (layer + 2) * 0.5f : -timer * (layer + 2) * 0.3f;
                float nextRadius = radius_ * (1.2f - ((layer + 1) * 0.25f));
                float tNext = (2.0f * (float)M_PI * i) / (float)vertices + nextLayerRot;

                int nx = (int)(pos_.x + cosf(tNext) * nextRadius);
                int ny = (int)(pos_.y + sinf(tNext) * nextRadius);

                // ガイドラインの色も強化
                Novice::DrawLine(x1, y1, nx, ny, HSVToRGBA(baseHue + 5, 0.6f, 1.0f, alpha / 2));
            }
        }
    }

    // 中心部のコア：単なる十字ではなく、小さな塗りつぶし四角形を重ねて「眩しさ」を表現
    Novice::DrawBox((int)pos_.x - 4, (int)pos_.y - 4, 8, 8, 0.0f, 0xAAAA00FF, kFillModeSolid);
    Novice::DrawBox((int)pos_.x - 2, (int)pos_.y - 2, 4, 4, 0.0f, WHITE, kFillModeSolid);

    Novice::SetBlendMode(kBlendModeNormal);
}

void Enemy::DrawParticle() {
    particle_.DrawBgParticle();
}

//-----------------------------------------------
//プライベート関数とかを作成
//-----------------------------------------------

void Enemy::HitGet(int dmg) {
    if (invincibleTimer_ <= 0) {
        if (isAlive_) {
            hp_ -= dmg;          // HPを減らす
            invincibleTimer_ = 20;//無敵時間のリセット
            if (hp_ <= 0) {
                particle_.InitBgParticle(pos_);
                hp_ = 0;
                isAlive_ = false; // 死ぬ
                pos_ = { -10000.0f,-10000.0f };
            }
        }
    }else if(invincibleTimer_>=0){
		invincibleTimer_--;
	}
}

void Enemy::MoveEnemy() {
    // 1. 移動タイマー（静的変数、またはヘッダで定義してもOK）
    static int moveTimer = 0;
    moveTimer--;

    // 2. タイマーが切れたら、次の行き先と速度を決める
    if (moveTimer <= 0) {
        // 次の方向転換までの時間をランダムに設定 (30〜90フレーム)
        moveTimer = rand() % 60 + 30;

        // 速度をランダムに設定（例：-8.0f 〜 8.0f）
        speed_.x = (float)(rand() % 17 - 8);
        speed_.y = (float)(rand() % 17 - 8);
    }

    // 3. 移動実行
    pos_.x += speed_.x;
    pos_.y += speed_.y;

    // 4. 画面端で跳ね返る（これがないと画面外へ逃げてしまう）
    if (pos_.x <= radius_ || pos_.x >= 1280 - radius_) {
        speed_.x *= -1;
    }
    if (pos_.y <= radius_ || pos_.y >= 720 - radius_) { // 上半分で動くように制限
        speed_.y *= -1;
    }
}