#pragma once
struct Color {
	int r;
	int g;
	int b;
};

inline bool operator ==(const Color &a, const Color &b) {
	return a.r == b.r && a.g == b.g && a.b == b.b;
}

inline bool operator !=(const Color &a, const Color &b) {
	return !(a.r == b.r && a.g == b.g && a.b == b.b);
}