import List;
import <iostream>;

int main() {
	List<int> list;
	list.push_back(10);
	list.push_back(20);
	list.push_back(30);
	list.push_back(40);
	list.erase(--list.end());
	for (auto e : list) {
		std::cout << e << std::endl;
	}
}