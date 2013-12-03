
struct collisionStatus
{
	bool left;
	bool right;
	bool top;
	bool bottom;

	collisionStatus(bool left = false,
		bool right = false,
		bool top = false,
		bool bottom = false) :
	left(left), right(right),
		top(top), bottom(bottom) {}

	bool contain() {
		return left && right && top && bottom;
	}
}; 