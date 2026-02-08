#pragma once
#include "Vector2.h"

// 定数名は一目で定数とわかるように書くのが一般的です
static const int kMaxParticles = 100;

class EnemyParticle
{
public:
	struct Particle {
		Vector2 pos;
		Vector2 prevPos;
		Vector2 vel;
		float radius;
		int alpha;
		bool isActive;
	};

	// 最大数分の配列をあらかじめ持っておく
	Particle particles_[kMaxParticles];

	EnemyParticle();
	~EnemyParticle();
	void InitBgParticle(Vector2 startPos);
	void UpdateBgParticle();
	void DrawBgParticle();

};

