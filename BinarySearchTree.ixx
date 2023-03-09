export module BinarySearchTree;

import <iostream>;
import <stack>;

export template <typename Comparable>
class BinarySearchTree {
public:
  BinarySearchTree() = default;
  BinarySearchTree(const BinarySearchTree& rhs);
  BinarySearchTree(BinarySearchTree&& rhs);
  ~BinarySearchTree();

  const Comparable& findMin() const { return findMin(root)->element; }
  const Comparable& findMax() const { return findMax(root)->element; }
  bool contains(const Comparable& x) const;
  bool isEmpty() const { return root == nullptr; }
  void printTree(std::ostream& out = std::cout) const { printTree(root, out); }

  void makeEmpty() { makeEmpty(root); }
  void insert(const Comparable& x);
  void insert(Comparable&& x);
  void remove(const Comparable& x);

  BinarySearchTree& operator=(const BinarySearchTree& rhs);
  BinarySearchTree& operator=(BinarySearchTree&& rhs);

private:
  struct BinaryNode {
    Comparable element;
    BinaryNode* left;
    BinaryNode* right;

    BinaryNode(const Comparable& theElement, BinaryNode* lt, BinaryNode* rt)
      : element { theElement }, left { lt }, right { rt } {}

    BinaryNode(Comparable&& theElement, BinaryNode* lt, BinaryNode* rt)
      : element { theElement }, left { lt }, right { rt } {}
  };

  BinaryNode* root;

  void insert(const Comparable& x, BinaryNode*& t);
  void insert(Comparable&& x, BinaryNode*& t);
  void remove(const Comparable& x, BinaryNode*& t);
  BinaryNode* findMin(BinaryNode* t) const {
    if (t == nullptr)
      return nullptr;
    if (t->left == nullptr)
      return t;
    return findMin(t->left);
  }
  BinaryNode* findMax(BinaryNode* t) const {
    if (t != nullptr)
      while (t->right != nullptr)
        t = t->right;
    return t;
  }
  bool contains(const Comparable& x, BinaryNode* t) const;
  void makeEmpty(BinaryNode*& t);
  void printTree(BinaryNode* t, std::ostream& out) const {
    std::stack<BinaryNode*> s; //辅助栈
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
  BinaryNode* clone(BinaryNode* t) const {
    if (t == nullptr)
      return nullptr;
    else
      return new BinaryNode { t->element, clone(t->left), clone(t->right) };
  }
};

template <typename Comparable>
bool BinarySearchTree<Comparable>::contains(const Comparable& x) const {
  return contains(x, root);
}

template <typename Comparable>
void BinarySearchTree<Comparable>::insert(const Comparable& x) {
  return insert(x, root);
}

template <typename Comparable>
void BinarySearchTree<Comparable>::insert(Comparable&& x) {
  return insert(std::move(x), root);
}

template <typename Comparable>
void BinarySearchTree<Comparable>::remove(const Comparable& x) {
  remove(x, root);
}

template <typename Comparable>
bool BinarySearchTree<Comparable>::contains(const Comparable& x, BinarySearchTree<Comparable>::BinaryNode* t) const {
  if (t == nullptr)
    return false;
  else if (x < t->element)
    return contains(x, t->left);
  else if (t->element < x)
    return contains(x, t->right);
  else
    return true;
}

template <typename Comparable>
void BinarySearchTree<Comparable>::insert(const Comparable& x, BinarySearchTree<Comparable>::BinaryNode*& t) {
  if (t == nullptr)
    t = new BinaryNode { x, nullptr, nullptr };
  else if (x < t->element)
    insert(x, t->left);
  else if (t->element < x)
    insert(x, t->right);
  else
    ;		// 重复元
}

template <typename Comparable>
void BinarySearchTree<Comparable>::insert(Comparable&& x, BinarySearchTree<Comparable>::BinaryNode*& t) {
  if (t == nullptr)
    t = new BinaryNode { std::move(x), nullptr, nullptr };
  else if (x < t->element)
    insert(std::move(x), t->left);
  else if (t->element < x)
    insert(std::move(x), t->right);
  else
    ;		// 重复元
}

template <typename Comparable>
void BinarySearchTree<Comparable>::remove(const Comparable& x, BinarySearchTree<Comparable>::BinaryNode*& t) {
  if (t == nullptr)
    return;
  if (x < t->element)
    remove(x, t->left);
  else if (t->element < x)
    remove(x, t->right);
  else if (t->left != nullptr && t->right != nullptr) {
    t->element = findMin(t->right)->element;
    remove(t->element, t->right);
  }
  else {
    BinaryNode* oldNode = t;
    t = (t->left != nullptr) ? t->left : t->right;
    delete oldNode;
  }
}

template <typename Comparable>
BinarySearchTree<Comparable>::~BinarySearchTree() {
  makeEmpty();
}

template <typename Comparable>
void BinarySearchTree<Comparable>::makeEmpty(BinarySearchTree<Comparable>::BinaryNode*& t) {
  if (t != nullptr) {
    makeEmpty(t->left);
    makeEmpty(t->right);
    delete t;
  }
  t = nullptr;
}

template <typename Comparable>
BinarySearchTree<Comparable>::BinarySearchTree(const BinarySearchTree<Comparable>& rhs)
  : root { nullptr } {
  root = clone(rhs.root);
}

template <typename Comparable>
BinarySearchTree<Comparable>& BinarySearchTree<Comparable>::operator=(
  const BinarySearchTree<Comparable>& rhs) {
  root = clone(rhs.root);
  return *this;
}

template <typename Comparable>
BinarySearchTree<Comparable>& BinarySearchTree<Comparable>::operator=(
  BinarySearchTree<Comparable>&& rhs) {
  root = clone(std::move(rhs).root);
  return *this;
}

template <typename Comparable>
BinarySearchTree<Comparable>::BinarySearchTree(BinarySearchTree<Comparable>&& rhs)
  : root { nullptr } {
  root = clone(std::move(rhs).root);
}