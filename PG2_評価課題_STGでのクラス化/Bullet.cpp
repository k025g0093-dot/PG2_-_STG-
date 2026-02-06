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
        // 加算合成を開始
        Novice::SetBlendMode(kBlendModeAdd);

        static float bulletTimer = 0.0f;
        bulletTimer += 0.08f; // 速度を微調整

        float centerX = bulletStatus.transform_.x;
        float centerY = bulletStatus.transform_.y;
        float baseRadius = bulletStatus.radius_;

        // --- 1. 明るい色の定義 ---
        // S(彩度)を0.3~0.5に下げ、V(輝度)を1.0に固定。Alphaを上げることで発光を強めます。
        // メイン：発光感の強いシアン/白
        unsigned int mainColor = HSVToRGBA(0.55f, 0.4f, 1.0f, 0xFF);
        // コア：ほぼ白に近い青
        unsigned int coreColor = HSVToRGBA(0.55f, 0.2f, 1.0f, 0xFF);

        // --- 2. 外郭：光の軌跡 (2回描画して光を強くする) ---
        int segments = 8;
        for (int i = 0; i < segments; i++) {
            float angle = bulletTimer + (float)M_PI * 2.0f / segments * i;
            float r = baseRadius * 1.2f;

            float x1 = centerX + cosf(angle) * r;
            float y1 = centerY + sinf(angle) * r;
            float x2 = centerX + cosf(angle + 0.5f) * r;
            float y2 = centerY + sinf(angle + 0.5f) * r;

            // 重ねて描画することで「光の芯」を作る
            Novice::DrawLine((int)x1, (int)y1, (int)x2, (int)y2, mainColor);
            Novice::DrawLine((int)x1, (int)y1, (int)x2, (int)y2, 0x88FFFFFF); // 白い光の芯
        }

        // --- 3. 中層：同心円グリッド ---
        // 外側の楕円（メインカラー）
        Novice::DrawEllipse((int)centerX, (int)centerY, (int)baseRadius, (int)baseRadius, 0.0f, mainColor, kFillModeWireFrame);
        // 内側の楕円（あえて白を混ぜる）
        Novice::DrawEllipse((int)centerX, (int)centerY, (int)(baseRadius * 0.7f), (int)(baseRadius * 0.7f), 0.0f, 0xAAFFFFFF, kFillModeWireFrame);

        // --- 4. コア：強い光の十字 ---
        for (int i = 0; i < 2; i++) {
            float crossAngle = -bulletTimer * 2.0f + (i * (float)M_PI_2);
            float s = sinf(crossAngle) * baseRadius * 1.0f;
            float c = cosf(crossAngle) * baseRadius * 1.0f;

            // 太い線がない場合は、少しずらして2本引くと光が強く見えます
            Novice::DrawLine((int)(centerX - c), (int)(centerY - s), (int)(centerX + c), (int)(centerY + s), coreColor);
            Novice::DrawLine((int)(centerX - c + 1), (int)(centerY - s), (int)(centerX + c + 1), (int)(centerY + s), coreColor);
        }

        // --- 5. 中心点（ブルーム効果のシミュレート） ---
        // 小さな塗りと、少し大きめの薄い塗りを重ねて眩しさを表現
        Novice::DrawEllipse((int)centerX, (int)centerY, (int)(baseRadius * 0.4f), (int)(baseRadius * 0.4f), 0.0f, mainColor & 0x66FFFFFF, kFillModeSolid);
        Novice::DrawEllipse((int)centerX, (int)centerY, 3, 3, 0.0f, WHITE, kFillModeSolid);

        // ブレンドモードを戻す
        Novice::SetBlendMode(kBlendModeNormal);
    }
}
