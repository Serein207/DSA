import CuckooHashTable;
import <iostream>;

int main() {
	CuckooHashTable<std::string, StringHashFamily<6>> tree;
	tree.insert("10");
	tree.insert("30");
	tree.insert("40");
	tree.insert("10");
	tree.remove("10");
}