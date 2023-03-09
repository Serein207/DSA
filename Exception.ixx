export module Exception;
import <exception>;

export class IteratorOutOfBoundsException : public std::exception {
public:
  const char* what() const noexcept override {
    return "iterator out of bounds";
  }
};

export class IteratorMismatchException :public std::exception {
public:
  const char* what() const noexcept override {
    return "iterator mismatch";
  }
};