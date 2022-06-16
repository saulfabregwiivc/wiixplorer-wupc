#ifndef RECT_H
#define RECT_H

template<class T>
class Rect
{
public:
	Rect<T>()
		: fx1(0), fy1(0), fx2(0), fy2(0)
	{}
	
	Rect<T>(const Rect &r)
		: fx1(r.fx1), fy1(r.fy1), fx2(r.fx2), fy2(r.fy2)
	{}

	Rect<T>(T x, T y, T x2, T y2)
		: fx1(x), fy1(y), fx2(x2), fy2(y2)
	{}
	
	~Rect() {}
	
	void setValues(T x, T y, T x2, T y2) {
		fx1 = x;
		fy1 = y;
		fx2 = x2;
		fy2 = y2;
	}
	
	const Rect &operator=(const Rect &r) {
		this->fx1 = r.fx1;
		this->fx2 = r.fx2;
		this->fy1 = r.fy1;
		this->fy2 = r.fy2;
		return *this;
	}

	T x1() const { return fx1; }
	T y1() const { return fy1; }
	T x2() const { return fx2; }
	T y2() const { return fy2; }
	
private:
	T fx1, fy1, fx2, fy2;
};

typedef Rect<int> iRect;
typedef Rect<float> fRect;

#endif // RECT_H
