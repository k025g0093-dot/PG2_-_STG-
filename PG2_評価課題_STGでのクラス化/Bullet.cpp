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
        bulletTimer += 0.1f; // アニメーション速度

        float centerX = bulletStatus.transform_.x;
        float centerY = bulletStatus.transform_.y;
        float baseRadius = bulletStatus.radius_;

        // --- HSVによる色計算 ---
        // 時間経過で色相を変化させる (0~360度)
        

        // メインカラー (シアン〜青系の発色を良くするなら hue を 180~220 固定にしてもOK)
        // ここでは全色相が回るようにしています
        unsigned int mainColor = HSVToRGBA(0.4f, 0.7f, 1.0f, 0xAA);
        unsigned int coreColor = HSVToRGBA(0.4f, 0.3f, 1.0f, 0xFF); // 中心ほど白っぽく

        // --- 1. 外郭：幾何学的なリング (スリット入り) ---
        int segments = 8;
        for (int i = 0; i < segments; i++) {
            float angle = bulletTimer + (float)M_PI * 2.0f / segments * i;
            float r = baseRadius * 1.2f;

            float x1 = centerX + cosf(angle) * r;
            float y1 = centerY + sinf(angle) * r;
            float x2 = centerX + cosf(angle + 0.4f) * r;
            float y2 = centerY + sinf(angle + 0.4f) * r;

            Novice::DrawLine((int)x1, (int)y1, (int)x2, (int)y2, mainColor);
        }

        // --- 2. 中層：同心円のグリッド ---
        // 判定ライン（外側）
        Novice::DrawEllipse((int)centerX, (int)centerY, (int)baseRadius, (int)baseRadius, 0.0f, mainColor, kFillModeWireFrame);
        // 内側ライン（少し暗め）
        unsigned int subColor = HSVToRGBA(0.4f, 0.8f, 0.5f, 0x88);
        Novice::DrawEllipse((int)centerX, (int)centerY, (int)(baseRadius * 0.7f), (int)(baseRadius * 0.7f), 0.0f, subColor, kFillModeWireFrame);

        // --- 3. コア：幾何学的な十字 ---
        for (int i = 0; i < 2; i++) {
            // 十字は外側と逆に回転させるとメカニカルに見える
            float crossAngle = -bulletTimer * 1.5f + (i * (float)M_PI_2);
            float s = sinf(crossAngle) * baseRadius * 0.9f;
            float c = cosf(crossAngle) * baseRadius * 0.9f;

            Novice::DrawLine(
                (int)(centerX - c), (int)(centerY - s),
                (int)(centerX + c), (int)(centerY + s),
                coreColor
            );
        }

        // 最中心の点（最も輝く場所）
        Novice::DrawEllipse((int)centerX, (int)centerY, 2, 2, 0.0f, WHITE, kFillModeSolid);

        Novice::SetBlendMode(kBlendModeNormal);
    }
}
