import BinarySearchTree;
import AvlTree;
import <iostream>;

int main() {
	AvlTree<int> tree;
	tree.insert(20);
	tree.insert(30);
	tree.insert(40);
	tree.insert(10);
	tree.remove(10);
	std::cout << tree.findMax() << std::endl;
	std::cout << tree.findMin() << std::endl;
	tree.printTree(std::cout);
}