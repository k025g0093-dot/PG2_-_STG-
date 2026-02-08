#define _USE_MATH_DEFINES // M_PIを使用するために必ずmath.hより上に書く
#include "Player.h"
#include "Novice.h"
#include"Bullet.h"
#include <cmath> // <corecrt_math.h> より C++ 標準の <cmath> を推奨

// staticを付けてこのファイル内限定の関数にする（二重定義エラー防止）
static unsigned int HSVToRGBA(float h, float s, float v, unsigned char alpha) {
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
	case 4: r = t; g = p; b = q; break;
	case 5: r = v; g = p; b = q; break;
	default: r = 0; g = 0; b = 0; break;
	}

	return ((unsigned int)(r * 255) << 24) |
		((unsigned int)(g * 255) << 16) |
		((unsigned int)(b * 255) << 8) |
		alpha;
}

Player::Player(Vector2 pos, Vector2 speed, float radius, int MaxHp, int hp, bool isAlive) {
	pos_ = pos;
	speed_ = speed;
	radius_ = radius;
	MaxHp_ = MaxHp;
	hp_ = hp;
	isAlive = true;
	shotCt_;

	for (int i = 0; i < 250; i++) {
		//bullet[i] = nullptr;
		bullet[i] = new Bullet();
	}
}



void Player::PlayerUpdata(char keys[256]) {

    if (invincibleTimer_ >= 0) {
        invincibleTimer_--;
    }


	MovePlayer(keys);
}


void Player::PlayerDraw() {
#ifdef DEBUG
    Novice::DrawEllipse((int)pos_.x, (int)pos_.y, (int)radius_, (int)radius_, 0.0f, 0xFFFFFFFF, kFillModeSolid);

	Novice::ScreenPrintf(0, 0, "Player hp X: %d ", hp_);
#endif

    Novice::SetBlendMode(kBlendModeAdd);

    static float playerTimer = 0.0f;
    playerTimer += 0.05f; // 回転・アニメーション速度

    float centerX = pos_.x;
    float centerY = pos_.y;
    float baseRadius = radius_;

    // --- HSVによる色計算 ---
    // 弾と同じく、発光感を出すために彩度を抑えめ(0.4f)、輝度を最大(1.0f)に
    unsigned int mainColor = HSVToRGBA(0.6f, 0.4f, 1.0f, 0xFF);
    unsigned int coreColor = HSVToRGBA(0.6f, 0.2f, 1.0f, 0xFF); // 中心ほど白に近い

    // --- 1. 外郭：回転する二重正三角形 (オーラ) ---
    // 弾のスリットリングの手法を応用し、外側に広がる三角形を描画
    int auraSegments = 2; // 正位置と逆位置の2つ
    for (int i = 0; i < auraSegments; i++) {
        float progress = fmodf(playerTimer * 0.5f + (i * 0.5f), 1.0f);
        float auraRadius = baseRadius * (1.0f + progress * 1.5f);
        unsigned char alpha = (unsigned char)(180 * (1.0f - progress));
        unsigned int auraColor = (mainColor & 0x00FFFFFF) | (alpha << 24);

        float angleOffset = playerTimer + (i * (float)M_PI); // 逆方向に配置
        Vector2 p[3];
        for (int j = 0; j < 3; j++) {
            float angle = angleOffset + (float)(M_PI * 2.0f / 3.0f) * j;
            p[j].x = centerX + cosf(angle) * auraRadius;
            p[j].y = centerY + sinf(angle) * auraRadius;
        }
        Novice::DrawTriangle((int)p[0].x, (int)p[0].y, (int)p[1].x, (int)p[1].y, (int)p[2].x, (int)p[2].y, auraColor, kFillModeWireFrame);
    }

    // --- 2. 中層：幾何学的な六角形グリッド ---
    // 弾の同心円グリッドの代わりに、よりメカニカルな六角形（ヘキサゴン）を配置
    int hexSides = 6;
    float hexRadius = baseRadius * 0.8f;
    for (int i = 0; i < hexSides; i++) {
        float angle1 = -playerTimer + (float)M_PI * 2.0f / hexSides * i;
        float angle2 = -playerTimer + (float)M_PI * 2.0f / hexSides * (i + 1);

        float x1 = centerX + cosf(angle1) * hexRadius;
        float y1 = centerY + sinf(angle1) * hexRadius;
        float x2 = centerX + cosf(angle2) * hexRadius;
        float y2 = centerY + sinf(angle2) * hexRadius;

        Novice::DrawLine((int)x1, (int)y1, (int)x2, (int)y2, mainColor);
    }

    // --- 3. コア：輝く正三角形 ---
    // 弾の「十字」の代わりに、本体となる三角形を配置。逆回転させて対比を出す。
    Vector2 coreP[3];
    float coreAngleOffset = playerTimer * 1.5f;
    for (int i = 0; i < 3; i++) {
        float angle = coreAngleOffset + (float)(M_PI * 2.0f / 3.0f) * i;
        coreP[i].x = centerX + cosf(angle) * baseRadius;
        coreP[i].y = centerY + sinf(angle) * baseRadius;
    }
    // 塗りつぶしとワイヤーフレームを重ねて光を強調
    Novice::DrawTriangle((int)coreP[0].x, (int)coreP[0].y, (int)coreP[1].x, (int)coreP[1].y, (int)coreP[2].x, (int)coreP[2].y, coreColor & 0x88FFFFFF, kFillModeSolid);
    Novice::DrawTriangle((int)coreP[0].x, (int)coreP[0].y, (int)coreP[1].x, (int)coreP[1].y, (int)coreP[2].x, (int)coreP[2].y, WHITE, kFillModeWireFrame);

    // 最中心の点（最も輝く場所）
    Novice::DrawEllipse((int)centerX, (int)centerY, 3, 3, 0.0f, WHITE, kFillModeSolid);

    // 弾の描画
    for (int i = 0; i < 250; i++) {
        if (bullet[i] != nullptr) {
            bullet[i]->Draw();
        }
    }

    Novice::SetBlendMode(kBlendModeNormal);



}



//プライベート軍--------------------


void Player::MovePlayer(char keys[256]) {
	// 1. プレイヤーの移動処理
	if (keys[DIK_A]) pos_.x -= speed_.x;
	if (keys[DIK_D]) pos_.x += speed_.x;
	if (keys[DIK_W]) pos_.y -= speed_.y;
	if (keys[DIK_S]) pos_.y += speed_.y;

    if (pos_.x <= 0 + radius_ ) {
        pos_.x =0+ radius_;
    }
    if (pos_.x >= 1280 - radius_) {
        pos_.x = 1280 - radius_;
	}
   
    if (pos_.y <= 0 + radius_) {
        pos_.y =0+ radius_;
    }
    if (pos_.y >= 720 - radius_) {
        pos_.y =720- radius_;
	}

	// 2. クールタイムのカウントダウン (ループの外で1フレームに1回だけ減らす)
	if (shotCt_ > 0) {
		shotCt_--;
	}

	// 3. 発射処理
    if (keys[DIK_SPACE]) {
        isCharging_ = true;
        if (chargeTimer_ < kFullCharge) chargeTimer_ += 1.0f;
    }
    else {
        if (isCharging_) { // ボタンを離した瞬間
            // 1. 溜まり具合を 0.0 〜 1.0 の範囲で出す
            float ratio = chargeTimer_ / kFullCharge;

            // 2. 最小サイズ(1.5) 〜 最大サイズ(5.0) の間で、溜めた分だけ大きくする
            // 式：最小 + (最大 - 最小) * 比率
            float scale = 1.0f + (5.0f - 1.5f) * ratio;

            // 弾を発射
            for (int i = 0; i < 250; i++) {
                if (!bullet[i]->bulletStatus.isShot) {
                    bullet[i]->bulletStatus.transform_ = pos_;
                    bullet[i]->bulletStatus.isShot = true;
                    bullet[i]->bulletStatus.chargeScale = scale; // 溜めた分だけのサイズが渡る！
                    break;
                }
            }
            chargeTimer_ = 0.0f;
            isCharging_ = false;
        }
    }

	// 4. 弾の更新処理 (発射の成否に関わらず、すべての弾の状態を更新)
	for (int i = 0; i < 250; i++) {
		bullet[i]->Updata();
	}
}


void Player::OnDamage(int damage) {
    if (invincibleTimer_ <= 0) { // 無敵中でなければ
        hp_ -= damage;
        invincibleTimer_ =20; // 1秒間無敵にする
        if (hp_ <= 0) {
            hp_ = 0;
            isAlive_ = false;
        }
    }

}
