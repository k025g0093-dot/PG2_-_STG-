#pragma once

struct Vector2
{

	float x;
	float y;

	// 演算子オーバーロード追加
	Vector2 operator-(const Vector2& rhs) const {
		return Vector2{ x - rhs.x, y - rhs.y };
	}
};