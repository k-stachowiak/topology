#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T>
struct singleton {

	T value;

	// Members of semiregular
	singleton() {}
	~singleton() {}
	singleton(const singleton& x) : value{ x.value } {}
	singleton& operator=(const singleton& x) { value = x.value; return *this; }

	// Members of regular
	friend bool operator==(const singleton& x, const singleton& y) {
		return x.value == y.value;
	}
	friend bool operator!=(const singleton& x, const singleton& y) {
		return !(x == y);
	}

	// Members of totally ordered
	friend bool operator<(const singleton& x, const singleton& y) {
		return x.value < y.value;
	}
	friend bool operator>(const singleton& x, const singleton& y) {
		return y < x;
	}
	friend bool operator<=(const singleton& x, const singleton& y) {
		return !(y > x);
	}
	friend bool operator>=(const singleton& x, const singleton& y) {
		return !(y < x);
	}
};

#endif