#include "EnemyParticle.h"
#include "Novice.h"
#include <cmath>
#include <cstdlib>


// --- 色変換用の簡易関数 (Enemy.cppのものと同じロジック) ---
unsigned int GetParticleColor(float h, int alpha) {
    float s = 1.0f; // 彩度最大
    float v = 1.0f; // 明度最大
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

    return ((unsigned int)alpha << 24) |
        ((unsigned int)(r * 255) << 16) |
        ((unsigned int)(g * 255) << 8) |
        (unsigned int)(b * 255);
}

// コンストラクタ
EnemyParticle::EnemyParticle() {
    // 全てのパーティクルを初期化
    for (int i = 0; i < kMaxParticles; i++) {
        particles_[i].isActive = false;
    }
}

// デストラクタ
EnemyParticle::~EnemyParticle() {
    // 静的配列（または通常の構造体配列）なら特になし
}

// パーティクルの初期化（放出）
void EnemyParticle::InitBgParticle(Vector2 startPos) {
    int emitCount = 0;
    for (int i = 0; i < kMaxParticles; i++) {
        if (!particles_[i].isActive) {
            particles_[i].isActive = true;
            particles_[i].pos = startPos;
            particles_[i].prevPos = startPos;

            // 0～360度ランダム
            float angle = (float)(rand() % 360) * (3.1415f / 180.0f);
            float speed = (float)(rand() % 12 + 6); // 少し速めにしてキラキラ感を強調

            particles_[i].vel.x = cosf(angle) * speed;
            particles_[i].vel.y = sinf(angle) * speed;

            particles_[i].alpha = 255;

            // 【新機能】個別にランダムな色相(Hue)を持たせる
            // radius 変数を流用するか、構造体に float hue を追加してください
            particles_[i].radius = (float)(rand() % 360);

            emitCount++;
            if (emitCount >= 20) break; // 本数を少し増やすとより豪華に
        }
    }
}

void EnemyParticle::UpdateBgParticle() {
    for (int i = 0; i < kMaxParticles; i++) {
        if (particles_[i].isActive) {
            particles_[i].prevPos = particles_[i].pos;
            particles_[i].pos.x += particles_[i].vel.x;
            particles_[i].pos.y += particles_[i].vel.y;

            // 速度減衰
            particles_[i].vel.x *= 0.92f;
            particles_[i].vel.y *= 0.92f;

            // 【新機能】色を回転させてキラキラさせる
            particles_[i].radius += 10.0f;
            if (particles_[i].radius >= 360.0f) particles_[i].radius -= 360.0f;

            particles_[i].alpha -= 8; // 消えるのを少し速くしてキレを出す
            if (particles_[i].alpha <= 0) {
                particles_[i].isActive = false;
            }
        }
    }
}

void EnemyParticle::DrawBgParticle() {
    Novice::SetBlendMode(kBlendModeAdd);

    for (int i = 0; i < kMaxParticles; i++) {
        if (particles_[i].isActive) {
            // 1. メインの光（外光）
            unsigned int color = GetParticleColor(particles_[i].radius, particles_[i].alpha);

            // 少し太めに見せるために周囲を微かにずらして描く（あるいは単に描画）
            Novice::DrawLine(
                (int)particles_[i].pos.x, (int)particles_[i].pos.y,
                (int)particles_[i].prevPos.x, (int)particles_[i].prevPos.y,
                color
            );

            // 2. 中心部分の「芯」（白に近い色を重ねて明るさを底上げ）
            // アルファ値を少し抑えた白を重ねる
            unsigned int whiteCore = ((particles_[i].alpha / 2) << 24) | 0xFFFFFF;
            Novice::DrawLine(
                (int)particles_[i].pos.x, (int)particles_[i].pos.y,
                (int)particles_[i].prevPos.x, (int)particles_[i].prevPos.y,
                whiteCore
            );
        }
    }

    Novice::SetBlendMode(kBlendModeNormal);
}