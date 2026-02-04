#include "Bullet.h"
#include "Player.h"
#include"Novice.h"
#include <corecrt_math_defines.h>
#include <corecrt_math.h>

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

Bullet::Bullet() {
	bulletStatus.transform_.x = -1100;
	bulletStatus.transform_.y = -1100;
	bulletStatus.radius_ = 15;
	bulletStatus.speed = 10;
	bulletStatus.isShot = false;
}


void Bullet::Updata() {

	if (bulletStatus.isShot) {
		bulletStatus.transform_.y -= bulletStatus.speed;
		if (bulletStatus.transform_.y <= 0) {
			bulletStatus.isShot = false;

		}

	}
}


void Bullet::Draw() {
	if (bulletStatus.isShot) {
		Novice::SetBlendMode(kBlendModeAdd);

		static float bulletTimer = 0.0f;
		bulletTimer += 0.12f; // 回転速度

		// プレイヤーの色（青〜シアン）
		float hue = 30.0f;

		// --- 1. 多層四角形の重なり描画 ---
		// 敵と同じく、ループを使って「サイズ」と「角度」を少しずつ変えた四角形を重ねます
		int layers = 6;
		for (int i = 0; i < layers; i++) {
			// 外側に行くほど大きく、透明にする
			float currentRadius = bulletStatus.radius_ * (1.0f + i * 0.4f);
			unsigned char alpha = (unsigned char)(200 * (1.0f - (float)i / layers));
			unsigned int color = HSVToRGBA(hue, 0.8f, 1.0f, alpha);

			// 各層の角度。i * 0.2f で「少しずつズレる」のを表現し、
			// bulletTimer で全体を回転させます。
			float angleBase = bulletTimer + (i * 0.2f);

			// 四角形の4点（90度刻み）を計算して線で結ぶ
			Vector2 p[4];
			for (int j = 0; j < 4; j++) {
				float a = angleBase + (float)M_PI_2 * j; // 90度(PI/2)ずつ
				p[j].x = bulletStatus.transform_.x + cosf(a) * currentRadius;
				p[j].y = bulletStatus.transform_.y + sinf(a) * currentRadius;
			}

			// 四角形の外枠を描画
			for (int j = 0; j < 4; j++) {
				Novice::DrawLine(
					(int)p[j].x, (int)p[j].y,
					(int)p[(j + 1) % 4].x, (int)p[(j + 1) % 4].y,
					color
				);
			}
		}

		// --- 2. スコアGetter風：中心の「ひし形」コア ---
		// 中心の核も四角形を45度回転させた状態で塗りつぶし、最も明るくします
		unsigned int coreColor = 0xFFFFFFFF;
		float coreR = bulletStatus.radius_ * 0.7f;
		Vector2 cp[4];
		for (int i = 0; i < 4; i++) {
			float a = (bulletTimer * 1.5f) + (float)M_PI_2 * i; // 核は少し速く回すとかっこいい
			cp[i].x = bulletStatus.transform_.x + cosf(a) * coreR;
			cp[i].y = bulletStatus.transform_.y + sinf(a) * coreR;
		}
		// 四角形を2つの三角形で塗りつぶし
		Novice::DrawTriangle((int)cp[0].x, (int)cp[0].y, (int)cp[1].x, (int)cp[1].y, (int)cp[2].x, (int)cp[2].y, coreColor, kFillModeSolid);
		Novice::DrawTriangle((int)cp[0].x, (int)cp[0].y, (int)cp[2].x, (int)cp[2].y, (int)cp[3].x, (int)cp[3].y, coreColor, kFillModeSolid);

		// --- 3. 仕上げ：中心の点光 ---
		Novice::DrawEllipse((int)bulletStatus.transform_.x, (int)bulletStatus.transform_.y, 3, 3, 0.0f, WHITE, kFillModeSolid);

		Novice::SetBlendMode(kBlendModeNormal);
	}
}
