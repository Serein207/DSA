export module CuckooHashTable;

import <vector>;
import <string>;
import <random>;

struct UniformRandom {
public:
	int nextInt() {
		std::random_device rd;  // 将用于为随机数引擎获得种子
		std::mt19937 gen(rd()); // 以播种标准 mersenne_twister_engine
		return randomNum(gen);
	}

private:
	std::uniform_int_distribution<> randomNum;
};

auto nextPrime(size_t x) {
	size_t n = x - 1;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	return n + 1;
};

export template <int count>
class StringHashFamily {
public:
	StringHashFamily() : MULTIPLIERS { count } {
		generateNewFunctions();
	}

	size_t hash(const std::string& x, int which) const {
		const int multiplier = MULTIPLIERS[which];
		size_t hashVal = 0;

		for (auto ch : x)
			hashVal = hashVal + ch;

		return hashVal;
	}

	int getNumberOfFunctions() {
		return count;
	}

	void generateNewFunctions() {
		for (auto& mult : MULTIPLIERS)
			mult = r.nextInt();
	}

private:
	std::vector<int> MULTIPLIERS;
	UniformRandom r;
};

export template <typename AnyType, typename HashFamily>
class CuckooHashTable {
public:
	explicit CuckooHashTable(int size = 101);

	void clear();
	bool contains(const AnyType& x) const;

	bool remove(const AnyType& x);
	bool insert(const AnyType& x);
	bool insert(AnyType&& x);

private:
	struct HashEntry {
		AnyType element;
		bool isActive;

		HashEntry(const AnyType& e = AnyType {}, bool a = false)
			: element { e }, isActive { a } {}
		HashEntry(AnyType&& e, bool a = false)
			: element { std::move(e) }, isActive { a } {}
	};

	bool insertHelper1(const AnyType& xx);
	bool insertHelper1(AnyType&& xx);
	bool isActive(int currentPos) const;

	size_t myhash(const AnyType& x, int which) const;
	int findPos(const AnyType& x) const;
	void expand();
	void rehash();
	void rehash(int newSize);

	inline static const double MAX_LOAD = 0.40;
	inline static const int ALLOWED_REHASHES = 5;

	UniformRandom r;
	HashFamily hashFunctions;
	std::vector<HashEntry> array;
	int currentSize;
	int numHashFunctions;
	int rehashes = 0;
};

template <typename AnyType, typename HashFamily>
CuckooHashTable<AnyType, HashFamily>::CuckooHashTable(int size) {
	numHashFunctions = hashFunctions.getNumberOfFunctions();
	clear();
}

template <typename AnyType, typename HashFamily>
void CuckooHashTable<AnyType, HashFamily>::clear() {
	currentSize = 0;
	for (auto& entry : array)
		entry.isActive = false;
}

template <typename AnyType, typename HashFamily>
bool CuckooHashTable<AnyType, HashFamily>::contains(const AnyType& x) const {
	return findPos(x) != -1;
}

template <typename AnyType, typename HashFamily>
bool CuckooHashTable<AnyType, HashFamily>::remove(const AnyType& x) {
	int currentPos { findPos(x) };
	if (!isActive(currentPos))
		return false;

	array[currentPos].isActive = false;
	--currentSize;
	return true;
}

template <typename AnyType, typename HashFamily>
bool CuckooHashTable<AnyType, HashFamily>::insert(const AnyType& x) {
	if (contains(x))
		return false;

	if (currentSize >= array.size() * MAX_LOAD)
		expand();

	return insertHelper1(x);
}

template <typename AnyType, typename HashFamily>
bool CuckooHashTable<AnyType, HashFamily>::insert(AnyType&& x) {
	if (contains(std::move(x)))
		return false;

	if (currentSize >= array.size() * MAX_LOAD)
		expand();

	return insertHelper1(x);
}

template <typename AnyType, typename HashFamily>
bool CuckooHashTable<AnyType, HashFamily>::insertHelper1(const AnyType& xx) {
	const int COUNT_LIMIT = 100;
	AnyType x = xx;

	while (true) {
		int lastPos { -1 };
		int pos;
		for (int count { 0 }; count < COUNT_LIMIT; ++count) {
			for (int i { 0 }; numHashFunctions; ++i) {
				pos = static_cast<int>(myhash(x, i));

				if (!isActive(pos)) {
					array[pos] = std::move(HashEntry { std::move(x), true });
					++currentSize;
					return true;
				}
			}

			int i { 0 };
			do {
				pos = static_cast<int>(myhash(x, r.nextInt()));
			} while (pos == lastPos && i++ < 5);

			lastPos = pos;
			std::swap(x, array[pos].element);
		}

		if (++rehashes > ALLOWED_REHASHES) {
			expand();
			rehashes = 0;
		}
		else {
			rehash();
		}
	}
}

template <typename AnyType, typename HashFamily>
bool CuckooHashTable<AnyType, HashFamily>::insertHelper1(AnyType&& xx) {
	const int COUNT_LIMIT = 100;
	AnyType x = std::move(xx);

	while (true) {
		int lastPos { -1 };
		int pos;
		for (int count { 0 }; count < COUNT_LIMIT; ++count) {
			for (int i { 0 }; numHashFunctions; ++i) {
				pos = myhash(x, i);

				if (!isActive(pos)) {
					array[pos] = std::move(HashEntry { std::move(x), true });
					++currentSize;
					return true;
				}
			}

			int i { 0 };
			do {
				pos = myhash(x, r.nextInt(numHashFunctions));
			} while (pos == lastPos && i++ < 5);

			lastPos = pos;
			std::swap(x, array[pos].element);
		}

		if (++rehashes > ALLOWED_REHASHES) {
			expand();
			rehashes = 0;
		}
		else {
			rehash();
		}
	}
}

template <typename AnyType, typename HashFamily>
bool CuckooHashTable<AnyType, HashFamily>::isActive(int currentPos) const {
	return array[currentPos].isActive;
}

template <typename AnyType, typename HashFamily>
size_t CuckooHashTable<AnyType, HashFamily>::myhash(const AnyType& x, int which) const {
	return hashFunctions.hash(x, which) % array.size();
}

template <typename AnyType, typename HashFamily>
int CuckooHashTable<AnyType, HashFamily>::findPos(const AnyType& x) const {
	for (int i { 0 }; numHashFunctions; ++i) {
		int pos = static_cast<int>(myhash(x, i));

		if (isActive(pos) && array[pos].element == x)
			return pos;
	}

	return -1;
}

template <typename AnyType, typename HashFamily>
void CuckooHashTable<AnyType, HashFamily>::expand() {
	rehash(static_cast<int>(array.size() / MAX_LOAD));
}

template <typename AnyType, typename HashFamily>
void CuckooHashTable<AnyType, HashFamily>::rehash() {
	hashFunctions.generateNewFunctions();
	rehash(static_cast<int>(array.size()));
}

template <typename AnyType, typename HashFamily>
void CuckooHashTable<AnyType, HashFamily>::rehash(int newSize) {
	std::vector<HashEntry> oldArray = array;
	array.resize(nextPrime(newSize));
	for (auto& entry : array)
		entry.isActive = false;

	currentSize = 0;
	for (auto& entry : oldArray)
		if (entry.isActive)
			insert(std::move(entry.element));
}