export module HashTable;

import <vector>;
import <list>;
import <string>;
import <algorithm>;

export template <typename HashedObj>
class HashTable {
public:
	explicit HashTable(int size = 101) : currentSize { size } { theLists.resize(10); }

	bool contains(const HashedObj& x) const;

	void clear();
	bool insert(const HashedObj& x);
	bool insert(HashedObj&& x);
	bool remove(const HashedObj& x);

private:
	std::vector<std::list<HashedObj>> theLists;
	int currentSize;

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
	return hf(x) % theLists.size();
}

template <typename HashedObj>
void HashTable<HashedObj>::rehash() {
	std::vector<std::list<HashedObj>> oldLists = theLists;

	auto nextPrime = [](size_t x) {
		size_t n = x - 1;
		n |= n >> 1;
		n |= n >> 2;
		n |= n >> 4;
		n |= n >> 8;
		n |= n >> 16;
		return n + 1;
	};
	theLists.resize(nextPrime(2 * theLists.size()));
	for (auto& theList : theLists)
		theList.clear();

	currentSize = 0;
	for (auto& theList : theLists)
		for (auto& x : theList)
			insert(std::move(x));
}

template <typename HashedObj>
void HashTable<HashedObj>::clear() {
	for (auto& theList : theLists)
		theList.clear();
}

template <typename HashedObj>
bool HashTable<HashedObj>::contains(const HashedObj& x) const {
	auto& whichList = theLists[myhash(x)];
	return { std::find(begin(whichList), end(whichList), x) != end(whichList) };
}

template <typename HashedObj>
bool HashTable<HashedObj>::remove(const HashedObj& x) {
	auto& whichList = theLists[myhash(x)];
	auto itr = std::find(begin(whichList), end(whichList), x);

	if (itr == end(whichList))
		return false;

	whichList.erase(itr);
	--currentSize;
	return true;
}

template <typename HashedObj>
bool HashTable<HashedObj>::insert(const HashedObj& x) {
	auto& whichList = theLists[myhash(x)];
	if (std::find(begin(whichList), end(whichList), x) != end(whichList))
		return false;
	whichList.push_back(x);

	if (++currentSize > theLists.size())
		rehash();

	return true;
}

template <typename HashedObj>
bool HashTable<HashedObj>::insert(HashedObj&& x) {
	auto& whichList = theLists[myhash(std::move(x))];
	if (std::find(begin(whichList), end(whichList), std::move(x)) != end(whichList))
		return false;
	whichList.push_back(std::move(x));

	if (++currentSize > theLists.size())
		rehash();

	return true;
}

