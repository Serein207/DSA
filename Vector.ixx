module;
#include <utility>

export module Vector;

export template <typename Object>
class Vector {
public:
	explicit Vector(size_t initSize = 0) : theSize { 0 },
		theCapacity { initSize + SPARE_CAPACITY } {
		objects = new Object[theCapacity];
	}

	Vector(const Vector& rhs) : theSize { rhs.theSize },
		theCapacity { rhs.theCapacity }, objects { nullptr } {
		objects = new Object[theCapacity];
		for (int k { 0 }; k < theSize; ++k) {
			objects[k] = rhs.objects[k];
		}
	}

	Vector& operator=(const Vector& rhs) noexcept{
		Vector copy { rhs };
		swap(copy);
		return *this;
	}

	Vector(Vector&& rhs) noexcept {
		std::swap(*this, rhs);
	}

	Vector& operator=(Vector&& rhs) noexcept {
		std::swap(*this, rhs);
		return *this;
	}

	~Vector() {
		delete[] objects;
	}

	void swap(Vector& rhs) noexcept {
		std::swap(theSize, rhs.theSize);
		std::swap(theCapacity, rhs.theCapacity);
		std::swap(objects, rhs.objects);
	}

	void resize(size_t newSize) {
		if (newSize > theCapacity)
			reserve(newSize * 2);
		theSize = newSize;
	}

	void reserve(size_t newCapacity) {
		if (newCapacity < theSize)
			return;

		Object* newArray = new Object[newCapacity];
		for (int k { 0 }; k < theSize; ++k)
			newArray[k] = std::move(objects[k]);

		theCapacity = newCapacity;
		std::swap(objects, newArray);
		delete[] newArray;
	}

	Object& operator[] (int index) {
		return objects[index];
	}
	const Object& operator[](int index) const {
		return objects[index];
	}

	bool empty() const {
		return size() == 0;
	}
	size_t size() const {
		return theSize;
	}
	size_t capacity() const {
		return theCapacity;
	}

	void push_back(const Object& x) {
		if (theSize == theCapacity)
			reserve(2 * theCapacity + 1);
		objects[theSize++] = x;
	}

	void push_back(Object&& x) {
		if (theSize == theCapacity)
			reserve(2 * theCapacity + 1);
		objects[theSize++] = std::move(x);
	}

	void pop_back() {
		--theSize;
	}

	const Object& back() {
		return objects[theSize - 1];
	}

	using iterator = Object*;
	using const_iterator = const Object*;

	iterator begin() {
		return &objects[0];
	}
	const_iterator cbegin() const {
		return &objects[0];
	}
	iterator end(){
		return &objects[size()];
	}
	const_iterator cend() const {
		return &objects[size()];
	}

	static const size_t SPARE_CAPACITY = 16;

private:
	size_t theSize;
	size_t theCapacity;
	Object* objects;
};

export template <typename T>
void swap(Vector<T>&& lhs, Vector<T>&& rhs) noexcept {
	lhs.swap(rhs);
}