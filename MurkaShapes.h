#pragma once

struct MurkaPoint {
	float x, y;

	MurkaPoint operator += (MurkaPoint p2) {
		this->operator=({ x + p2.x, y + p2.y });
        return *this;
	}
    
    MurkaPoint operator + (MurkaPoint p2) {
        return {x + p2.x, y + p2.y};
    }
};

struct MurkaShape {
	MurkaPoint position = { 0, 0 };
	MurkaPoint size = { 0, 0 };

	bool inside(MurkaPoint p) const {
		return ((p.x > position.x) &&
			(p.x < position.x + size.x) &&
			(p.y > position.y) &&
			(p.y < position.y + size.y));
	}

	MurkaShape() {

	}

	MurkaShape(float x, float y, float width, float height) {
		position.x = x;
		position.y = y;
		size.x = width;
		size.y = height;
	}
};
