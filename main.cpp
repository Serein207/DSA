import BinarySearchTree;
import <iostream>;

int main() {
	BinarySearchTree<int> tree;
	tree.insert(10);
	tree.insert(40);
	tree.insert(30);
	tree.insert(20);
	tree.insert(90);
	tree.insert(80);
	std::cout << tree.findMax() << std::endl;
	tree.remove(80);
	tree.printTree();
}