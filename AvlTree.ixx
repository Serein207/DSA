module;
#include <utility>

export module AvlTree;
import <iostream>;
import <stack>;

export template <typename Comparable>
class AvlTree {
public:
	AvlTree() = default;
	AvlTree(const AvlTree& rhs) { root = clone(rhs.root); }
	AvlTree(AvlTree&& rhs) { root = clone(rhs.root); }
	~AvlTree() { clear(); }

	AvlTree& operator=(const AvlTree& rhs) {
		root = clone(rhs.root);
		return *this;
	}
	AvlTree& operator=(AvlTree&& rhs) {
		root = clone(rhs.root);
		return *this;
	}

	bool empty() const {
		return root == nullptr;
	}

private:
	struct AvlNode {
		Comparable element;
		AvlNode* left;
		AvlNode* right;
		int height;

		AvlNode(const Comparable& ele, AvlNode* lt, AvlNode* rt, int h = 0)
			: element { ele }, left { lt }, right { rt }, height { h } {}

		AvlNode(Comparable&& ele, AvlNode* lt, AvlNode* rt, int h = 0)
			: element { std::move(ele) }, left { lt }, right { rt }, height { h } {}
	};

	AvlNode* root = nullptr;

	AvlNode* clone(AvlNode* t) const {
		if (t == nullptr)
			return nullptr;
		else
			return new AvlNode { t->element, clone(t->left), clone(t->right) };
	}

private:
	void insert(const Comparable& x, AvlNode*& t) {
		if (t == nullptr)
			t = new AvlNode { x, nullptr, nullptr };
		else if (x < t->element)
			insert(x, t->left);
		else if (t->element < x)
			insert(x, t->right);

		balance(t);
	}

	void insert(Comparable&& x, AvlNode*& t) {
		if (t == nullptr)
			t = new AvlNode { std::move(x), nullptr, nullptr };
		else if (x < t->element)
			insert(std::move(x), t->left);
		else if (t->element < x)
			insert(std::move(x), t->right);

		balance(t);
	}

public:
	void insert(const Comparable& x) {
		return insert(x, root);
	}

	void insert(Comparable&& x) {
		return insert(std::move(x), root);
	}

	static const int ALLOWED_IMBALANCE = 1;

private:
	void balance(AvlNode*& t) {
		if (t == nullptr)
			return;
		if (height(t->left) - height(t->right) > ALLOWED_IMBALANCE)
			if (height(t->left->left) >= height(t->left->right))
				rotateWithLeftChild(t);
			else
				doubleWithLeftChild(t);
		else
			if (height(t->right) - height(t->left) > ALLOWED_IMBALANCE)
				if (height(t->right->right) >= height(t->right->left))
					rotateWithRightChild(t);
				else
					doubleWithRightChild(t);

		t->height = std::max(height(t->left), height(t->right)) + 1;
	}

	void rotateWithLeftChild(AvlNode*& k2) {
		AvlNode* k1 = k2->left;
		k2->left = k1->right;
		k1->right = k2;
		k2->height = std::max(height(k2->left), height(k2->right)) + 1;
		k1->height = std::max(height(k1->left), height(k2)) + 1;
		k2 = k1;
	}

	void rotateWithRightChild(AvlNode*& k2) {
		AvlNode* k1 = k2->right;
		k2->right = k1->left;
		k1->left = k2;
		k2->height = std::max(height(k2->left), height(k2->right)) + 1;
		k1->height = std::max(height(k1->right), height(k2)) + 1;
		k2 = k1;
	}

	void doubleWithLeftChild(AvlNode*& k3) {
		rotateWithRightChild(k3->left);
		rotateWithLeftChild(k3);
	}

	void doubleWithRightChild(AvlNode*& k3) {
		rotateWithLeftChild(k3->right);
		rotateWithRightChild(k3);
	}

	int height(AvlNode* t) {
		if (t == nullptr)
			return -1;
		return t->height;
	}

	void remove(const Comparable& x, AvlNode*& t) {
		if (t == nullptr)
			return;

		if (x < t->element)
			remove(x, t->left);
		else if (t->element < x)
			remove(x, t->right);
		else if (t->left && t->right) {
			t->element = findMin(t->right)->element;
			remove(t->element, t->right);
		}
		else {
			AvlNode* oldNode = t;
			t = t->left ? t->left : t->right;
			delete oldNode;
		}

		balance(t);
	}

	void clear(AvlNode*& t) {
		if (t != nullptr) {
			clear(t->left);
			clear(t->right);
			delete t;
		}
		t = nullptr;
	}

public:
	void remove(const Comparable& x) {
		return remove(x, root);
	}

	void clear() {
		return clear(root);
	}

private:
	AvlNode* findMin(AvlNode* t) const {
		if (t == nullptr)
			return nullptr;
		if (t->left == nullptr)
			return t;
		return findMin(t->left);
	}

	AvlNode* findMax(AvlNode* t) const {
		if (t != nullptr)
			while (t->right)
				t = t->right;
		return t;
	}

public:
	Comparable findMin() const {
		return findMin(root)->element;
	}

	Comparable findMax() const {
		return findMax(root)->element;
	}

private:
	void printTree(AvlNode* t, std::ostream& out) const {
		std::stack<AvlNode*> s; //辅助栈
		if (t)
			s.push(t); //根节点入栈
		while (!s.empty()) { //在栈变空之前反复循环
			t = s.top();
			out << t->element << ' '; //弹出并访问当前节点
			s.pop();

			//先让右孩子先入栈，再让左孩子入栈
			if (t->right)
				s.push(t->right); //右孩子先入后出
			if (t->left)
				s.push(t->left); //左孩子后入先出
		}
		out << '\n';
	}

public:
	void printTree(std::ostream& out = std::cout) const { printTree(root, out); }

private:
	bool contains(const Comparable& x, AvlNode* t) const {
		if (t == nullptr)
			return false;
		else if (x < t->element)
			return contains(x, t->left);
		else if (t->element < x)
			return contains(x, t->right);
		else
			return true;
	}

public:
	bool contains(const Comparable& x) const {
		return contains(x, root);
	}
};