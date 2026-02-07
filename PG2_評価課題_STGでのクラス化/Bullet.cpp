#include "Bullet.h"
#include "Player.h"
#include"Novice.h"
#include <corecrt_math_defines.h>
#include <corecrt_math.h>

// staticを付けてこのファイル内限定の関数にする（二重定義エラー防止）
//static unsigned int HSVToRGBA(float h, float s, float v, unsigned char alpha) {
//	float r, g, b;
//	int i = (int)(h / 60.0f) % 6;
//	float f = (h / 60.0f) - (int)(h / 60.0f);
//	float p = v * (1.0f - s);
//	float q = v * (1.0f - f * s);
//	float t = v * (1.0f - (1.0f - f) * s);
//
//	switch (i) {
//	case 0: r = v; g = t; b = p; break;
//	case 1: r = q; g = v; b = p; break;
//	case 2: r = p; g = v; b = t; break;
//	case 3: r = p; g = q; b = v; break;
//	case 4: r = t; g = p; b = q; break;
//	case 5: r = v; g = p; b = q; break;
//	default: r = 0; g = 0; b = 0; break;
//	}
//
//	return ((unsigned int)(r * 255) << 24) |
//		((unsigned int)(g * 255) << 16) |
//		((unsigned int)(b * 255) << 8) |
//		alpha;
//}

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
    if (!bulletStatus.isShot) return;

    // チャージ倍率を適用した半径
    float drawRadius = bulletStatus.radius_ * bulletStatus.chargeScale;

    Novice::SetBlendMode(kBlendModeAdd);

    static float bulletTimer = 0.0f;
    bulletTimer += 0.1f; // 回転速度

    float centerX = bulletStatus.transform_.x;
    float centerY = bulletStatus.transform_.y;

    // --- 1. 色の定義 ---
    // チャージ弾ならより白く、通常弾ならシアンに
    unsigned int mainColor = (bulletStatus.chargeScale > 1.5f) ? 0xFFFFFF88 : 0x00A0FFFF;
    unsigned int edgeColor = 0x00FFFFFF | 0xFF; // 外枠の白光

    // --- 2. 複数の三角形を回転させて重ねる ---
    int triangleCount = 3; // 三角形の数
    for (int i = 0; i < triangleCount; i++) {
        // 三角形ごとに回転をずらす
        float angleOffset = (float)M_PI * 2.0f / triangleCount * i + bulletTimer;

        // 正三角形の頂点を計算
        Vector2 p[3];
        for (int j = 0; j < 3; j++) {
            float theta = angleOffset + (float)M_PI * 2.0f / 3.0f * j;
            p[j].x = centerX + cosf(theta) * drawRadius;
            p[j].y = centerY + sinf(theta) * drawRadius;
        }

        // 塗りつぶしの三角形（少し透明度を下げる）
        Novice::DrawTriangle(
            (int)p[0].x, (int)p[0].y, (int)p[1].x, (int)p[1].y, (int)p[2].x, (int)p[2].y,
            mainColor & 0x66FFFFFF, kFillModeSolid
        );

        // 外枠の三角形（ハッキリ見せる）
        Novice::DrawTriangle(
            (int)p[0].x, (int)p[0].y, (int)p[1].x, (int)p[1].y, (int)p[2].x, (int)p[2].y,
            edgeColor, kFillModeWireFrame
        );
    }

    // --- 3. 中心に光の芯を作る ---
    // チャージ弾なら中心をより眩しく
    float coreSize = drawRadius * 0.4f;
    Novice::DrawEllipse((int)centerX, (int)centerY, (int)coreSize, (int)coreSize, 0.0f, WHITE, kFillModeWireFrame);
    Novice::DrawEllipse((int)centerX, (int)centerY, 4, 4, 0.0f, WHITE, kFillModeSolid);

    Novice::SetBlendMode(kBlendModeNormal);
}
