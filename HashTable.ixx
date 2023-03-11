export module HashTable;

import <vector>;
import <list>;
import <string>;
import <algorithm>;

auto nextPrime(size_t x) {
	size_t n = x - 1;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	return n + 1;
};

export template <typename HashedObj>
class HashTable {
public:
	explicit HashTable(size_t size = 101) : array { nextPrime(size) } { clear(); }

	bool contains(const HashedObj& x) const;

	void clear();
	bool insert(const HashedObj& x);
	bool insert(HashedObj&& x);
	bool remove(const HashedObj& x);

	enum class EntryType { ACTIVE, EMPTY, DELETED };

private:
	struct HashEntry {
		HashedObj element;
		EntryType info;

		HashEntry(const HashedObj& e = HashedObj {}, EntryType i = EntryType::EMPTY)
			: element { e }, info { i } {}
		HashEntry(HashedObj&& e, EntryType i = EntryType::EMPTY)
			: element { std::move(e) }, info { i } {}
	};

	std::vector<HashEntry> array;
	int currentSize;

	bool isActive(int currentPos) const;
	int findPos(const HashedObj& x) const;
	void rehash();
	size_t myhash(const HashedObj& x) const;
};

template <typename Key>
class hash {
public:
	size_t operator()(const Key& k) const {
		return k * 37 + k;
	}
};

template<>
class hash<std::string> {
public:
	size_t operator()(const std::string& key) {
		size_t hashVal { 0 };

		for (char ch : key)
			hashVal = 37 * hashVal + ch;

		return hashVal;
	}
};

template <typename HashedObj>
size_t HashTable<HashedObj>::myhash(const HashedObj& x) const {
	static hash<HashedObj> hf;
	return hf(x) % array.size();
}

template <typename HashedObj>
void HashTable<HashedObj>::rehash() {
	std::vector<HashEntry> oldArray = array;

	array.resize(nextPrime(2 * oldArray.size()));
	for (auto& entry : array)
		entry.info = EntryType::EMPTY;

	currentSize = 0;
	for (auto& entry : oldArray)
		if (entry.info == EntryType::ACTIVE)
			insert(std::move(entry.element));
}

template <typename HashedObj>
void HashTable<HashedObj>::clear() {
	currentSize = 0;
	for (auto& entry : array)
		entry.info = EntryType::EMPTY;
}

template <typename HashedObj>
bool HashTable<HashedObj>::contains(const HashedObj& x) const {
	return isActive(findPos(x));
}

template <typename HashedObj>
bool HashTable<HashedObj>::remove(const HashedObj& x) {
	int currentPos = findPos(x);
	if (!isActive(currentPos))
		return false;

	array[currentPos].info = EntryType::DELETED;
	return true;
}

template <typename HashedObj>
bool HashTable<HashedObj>::insert(const HashedObj& x) {
	int currentPos = findPos(x);
	if (isActive(currentPos))
		return false;

	array[currentPos].element = x;
	array[currentPos].info = EntryType::ACTIVE;

	if (++currentSize > array.size() / 2)
		rehash();

	return true;
}

template <typename HashedObj>
bool HashTable<HashedObj>::insert(HashedObj&& x) {
	int currentPos = findPos(x);
	if (isActive(currentPos))
		return false;

	array[currentPos].element = std::move(x);
	array[currentPos].info = EntryType::ACTIVE;

	if (++currentSize > array.size() / 2)
		rehash();

	return true;
}

template <typename HashedObj>
bool HashTable<HashedObj>::isActive(int currentPos) const {
	return array[currentPos].info == EntryType::ACTIVE;
}

template <typename HashedObj>
int HashTable<HashedObj>::findPos(const HashedObj& x) const {
	int offset = 1;
	int currentPos = myhash(x);

	while (array[currentPos].info != EntryType::EMPTY &&
		array[currentPos].element != x) {
		currentPos += offset;
		if (currentPos >= array.size())
			currentPos -= array.size();
	}
	return currentPos;
}