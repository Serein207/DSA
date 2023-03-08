module;
#include <utility>

export module List;
import Exception;

export template <typename Object>
class List {
private:
	struct Node {
		Object data;
		Node* prev;
		Node* next;

		Node(const Object& d = Object {}, Node* p = nullptr,
			Node* n = nullptr)
			: data { d }, prev { p }, next { n } {}

		Node(Object&& d, Node* p = nullptr, Node* n = nullptr)
			: data { std::move(d) }, prev { p }, next { n } {}
	};

public:
	class const_iterator;
	class iterator;

public:
	List();
	List(const List& rhs);
	~List();
	List& operator=(const List& rhs);
	List(List&& rhs);
	List& operator=(List&& rhs);

	iterator begin() {
		return { head->next };
	}
	const_iterator begin() const {
		const_iterator itr { *this, head };
		return ++itr;
	}
	iterator end() {
		return { tail };
	}
	const_iterator end() const {
		return { tail };
	}

	int size() const {
		return theSize;
	}
	bool empty() const {
		return size() == 0;
	}

	void clear() {
		while (!empty())
			pop_front();
	}

	Object& front() {
		return *begin();
	}
	const Object& front() const {
		return *begin();
	}
	Object& back() {
		return *--end();
	}
	const Object& back() const {
		return *--end();
	}
	void push_front(const Object& x) {
		insert(begin(), x);
	}
	void push_front(Object&& x) {
		insert(begin(), std::move(x));
	}
	void push_back(const Object& x) {
		insert(end(), x);
	}
	void push_back(Object&& x) {
		insert(end(), std::move(x));
	}
	void pop_front() {
		erase(begin());
	}
	void pop_back() {
		erase(--end());
	}

	iterator insert(iterator itr, const Object& x);
	iterator insert(iterator itr, Object&& x);

	iterator erase(iterator itr);
	iterator erase(iterator from, iterator to);

private:
	int theSize;
	Node* head;
	Node* tail;

	void init();
};

template <typename Object>
class List<Object>::const_iterator {
public:
	const_iterator(const List<Object>& lst, Node* p) : theList { &lst }, current { p } {}

	void assertIsValid() const {
		if (theList == nullptr || current == nullptr || current == theList->head)
			throw IteratorOutOfBoundsException {};
	}

	const Object& operator*() const {
		return retrieve();
	}

	const_iterator& operator++() {
		this->current = this->current->next;
		return *this;
	}

	const_iterator& operator--() {
		this->current = this->current->prev;
		return *this;
	}

	const_iterator operator--(int) {
		const_iterator old = *this;
		--(*this);
		return old;
	}

	const_iterator operator++(int) {
		const_iterator old = *this;
		++(*this);
		return old;
	}

	bool operator==(const const_iterator& rhs) const {
		return current == rhs.current;
	}
	bool operator!=(const const_iterator& rhs) const {
		return !(*this == rhs);
	}

protected:
	const List<Object>* theList;
	Node* current;

	Object& retrieve() const {
		return current->data;
	}

	const_iterator(Node* p) : current { p } {}

	friend class List<Object>;
};

template <typename Object>
class List<Object>::iterator : public const_iterator {
public:
	iterator() = default;

	Object& operator* () {
		return const_iterator::retrieve();
	}
	const Object& operator* () const {
		return const_iterator::operator*();
	}

	iterator& operator++() {
		this->current = this->current->next;
		return *this;
	}

	iterator& operator--() {
		this->current = this->current->prev;
		return *this;
	}

	iterator operator++(int) {
		iterator old = *this;
		++(*this);
		return old;
	}

	iterator operator--(int) {
		iterator old = *this;
		--(*this);
		return old;
	}

protected:
	iterator(Node* p) : const_iterator { p } {}

	friend class List<Object>;
};


template <typename Object>
List<Object>::List() {
	init();
}

template <typename Object>
List<Object>::~List() {
	clear();
	delete head;
	delete tail;
}

template <typename Object>
List<Object>::List(const List<Object>& rhs) {
	init();
	for (auto& x : rhs)
		push_back(x);
}

template <typename Object>
List<Object>& List<Object>::operator=(const List<Object>& rhs) {
	List copy { rhs };
	std::swap(*this, copy);
	return *this;
}

template <typename Object>
List<Object>::List(List<Object>&& rhs) {
	std::swap(*this, rhs);
}

template <typename Object>
List<Object>& List<Object>::operator=(List<Object>&& rhs) {
	std::swap(*this, rhs);
	return *this;
}

template <typename Object>
void List<Object>::init() {
	theSize = 0;
	head = new Node;
	tail = new Node;
	head->next = tail;
	tail->prev = head;
}

template <typename Object>
List<Object>::iterator List<Object>::insert(iterator itr, const Object& x) {
	itr.assertIsValid();
	if (itr.theList != this)
		throw IteratorMismatchException {};

	Node* p = itr.current;
	theSize++;
	return { p->prev = p->prev->next = new Node{x, p->prev, p} };
}

template <typename Object>
List<Object>::iterator List<Object>::insert(iterator itr, Object&& x) {
	Node* p = itr.current;
	theSize++;
	return { p->prev = p->prev->next
		= new Node{std::move(x), p->prev,p} };
}

template <typename Object>
List<Object>::iterator List<Object>::erase(List<Object>::iterator itr) {
	Node* p = itr.current;
	iterator retVal { p->next };
	p->prev->next = p->next;
	p->next->prev = p->prev;
	delete p;
	theSize--;

	return retVal;
}

template <typename Object>
List<Object>::iterator List<Object>::erase(List<Object>::iterator from, List<Object>::iterator to) {
	for (iterator itr = from; itr != to; ++itr)
		itr = erase(itr);

	return to;
}