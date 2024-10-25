#include <cassert>
#include <iostream>
#include <chrono>
#include <memory>
#include <vector>
#include <string>
#include <cstring>

class String {
  unsigned int size_;
  std::unique_ptr<char[]> s_ = nullptr; // nullptr or null-terminated
 public:
  String() = default;
  String(char const* s) :
    size_(strlen(s)+1),
    s_(std::make_unique<char[]>(size_)) {
    std::memcpy(s_.get(), s, size_);
    std::cout << &s_ << " constructor\n";
  }
  ~String() {std::cout << "desctructor\n";};
  String(String const& other) :
    size_(other.size()),
    s_(std::make_unique<char[]>(size_)) {
    std::memcpy(s_.get(), other.get_ptr().get(), size_);
    std::cout << &s_ << " copy constructor\n";
  }
  String(String&& tmp) :
    size_(tmp.size())
  {
    s_.swap(tmp.get_ptr());
    std::cout << &s_ << " move constructor\n";
  }
  String& operator=(String const& other)
  {
    std::cout << "copy assignment\n";
    if (this != &other){
      size_ = other.size();
      s_ = std::make_unique_for_overwrite<char[]>(size_);
      std::copy(other.get_ptr().get(), other.get_ptr().get() + size_, s_.get());
    }
    return *this;
  }
  String& operator=(String&& other)
  {
    std::cout << "move assignment\n";
    if (this != &other){
      size_ = other.size();
      s_ = std::move(other.get_ptr());
    }
    return *this;
  }
  std::size_t size() const {
    return size_;
  }
  std::unique_ptr<char[]>& get_ptr()
  {
    return s_;
  }
  std::unique_ptr<char[]> const& get_ptr() const
  {
    return s_;
  }
  char& operator[](std::size_t n)
  {
    return s_.get()[n];
  }
  char const& operator[](std::size_t n) const
  {
    return s_.get()[n];
  }
};

String get_string()
{
  return String{"Consectetur adipiscing elit"};
}

int main()
{
  String const s1("Lorem ipsum dolor sit amet");
  std::cout << "s1: " << s1.get_ptr() << '\n';

  String s2 = get_string();
  std::cout << "s2: " << s2.get_ptr() << '\n';

  String s3;
  s3 = s1;

  String s4;
  s4 = std::move(s2);
  std::cout << "s4: " << s4.get_ptr() << '\n';

  char& c1 = s4[4];
  char const& c2 = s1[4];
  std::cout << "c1 " << c1 << " c2 " << c2 << std::endl;

  std::cout << "s3: " << s3.get_ptr() << '\n';
}
