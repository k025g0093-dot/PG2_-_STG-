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
	MaxHp = 10;
	hp = MaxHp;
	isAlive = true;
	bullet = new Bullet();
}



void Player::PlayerUpdata(char keys[256], char preKeys[256]) {
	MovePlayer(keys, preKeys);
}



void Player::PlayerDraw() {
	//Novice::DrawEllipse((int)pos_.x, (int)pos_.y, (int)radius_, (int)radius_, 0.0f, 0xFFFFFFFF, kFillModeSolid);

#ifdef DEBUG
	Novice::DrawEllipse((int)pos_.x, (int)pos_.y, (int)radius_, (int)radius_, 0.0f, 0xFFFFFFFF, kFillModeSolid);

#endif // DEBUG

	Novice::SetBlendMode(kBlendModeAdd);

	static float timer = 0.0f;
	timer += 0.05f;
	float hue = 100.0f; // プレイヤーカラー

	// --- 1. 中心の「光の点」（視認性アップの必殺技） ---
	// 三角形の真ん中に真っ白な光の弾を置くことで、どこにいても自機が見えるようになります
	Novice::DrawEllipse((int)pos_.x, (int)pos_.y, 5, 5, 0.0f, 0xFFFFFFFF, kFillModeSolid);

	// --- 2. 本体（正三角形） ---
	float angleOffset = timer * 0.1f;
	unsigned int coreColor = HSVToRGBA(hue, 0.5f, 1.0f, 0xFF); // ほぼ白に近い青
	Vector2 p[3];
	for (int i = 0; i < 3; i++) {
		float angle = angleOffset + (float)(M_PI * 2.0f / 3.0f) * i + (float)(M_PI * 3.0f / 2.0f);
		p[i].x = pos_.x + radius_ * cosf(angle);
		p[i].y = pos_.y + radius_ * sinf(angle);
	}
	Novice::DrawTriangle((int)p[0].x, (int)p[0].y, (int)p[1].x, (int)p[1].y, (int)p[2].x, (int)p[2].y, coreColor, kFillModeSolid);

	// --- 3. 重なり合うオーラ（正三角形と逆三角形の混合） ---
	int auraLayers = 12; // 層を増やして密度を上げる
	float maxSpread = 60.0f;

	for (int i = 0; i < auraLayers; i++) {
		float progress = fmodf(timer * 0.5f + (i * (1.0f / auraLayers)), 1.0f);
		float currentSpread = progress * maxSpread;
		unsigned char alpha = (unsigned char)(255 * (1.0f - progress)); // 最大輝度を上げる

		// 色の設定：ここを変えると外側の色が変わります
		unsigned int auraColor = HSVToRGBA(hue, 60.0f, 1.0f, alpha);

		Vector2 ap[3];
		float currentRadius = radius_ + currentSpread;

		// i が偶数なら正三角形、奇数なら逆三角形にする
		float reverseAngle = (i % 2 == 0) ? 0.0f : (float)M_PI;

		for (int j = 0; j < 3; j++) {
			// reverseAngle を足すことで三角形の向きを交互に変える
			float angle = angleOffset + (float)(M_PI * 2.0f / 3.0f) * j + (float)(M_PI * 3.0f / 2.0f) + reverseAngle;
			ap[j].x = pos_.x + currentRadius * cosf(angle);
			ap[j].y = pos_.y + currentRadius * sinf(angle);
		}

		Novice::DrawTriangle(
			(int)ap[0].x, (int)ap[0].y, (int)ap[1].x, (int)ap[1].y, (int)ap[2].x, (int)ap[2].y,
			auraColor, kFillModeWireFrame
		);
	}
	bullet->Draw();

	Novice::SetBlendMode(kBlendModeNormal);
}



//プライベート軍--------------------


void Player::MovePlayer(char keys[256], char preKeys[256]) {


	//プレイヤーの移動処理
	if (keys[DIK_A]) {
		pos_.x -= speed_.x;
	}

	if (keys[DIK_D]) {
		pos_.x += speed_.x;
	}

	if (keys[DIK_W]) {
		pos_.y -= speed_.y;
	}

	if (keys[DIK_S]) {
		pos_.y += speed_.y;
	}

	//プレイヤーの移動制限処理


	if (!bullet->bulletStatus.isShot) {
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {

			bullet->bulletStatus.transform_.x = pos_.x;
			bullet->bulletStatus.transform_.y = pos_.y;
			bullet->bulletStatus.isShot = true;

		}
	}
	bullet->Updata();


}
