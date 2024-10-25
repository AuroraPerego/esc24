#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <cstring>

class String {
  char* s_ = nullptr; // nullptr or null-terminated
 public:
  String() = default;
  String(char const* s) {
    size_t size = std::strlen(s) + 1;
    s_ = new char[size];
    std::memcpy(s_, s, size);
  }
  ~String() { delete [] s_; }
  String(String const& other) {
    size_t size = std::strlen(other.s_) + 1;
    s_ = new char[size];
    std::memcpy(s_, other.s_, size);
  }
  String(String&& tmp)
      : s_(tmp.s_) {
    tmp.s_ = nullptr;
  }
  String& operator=(String const& other)
  {
    if (this != &other){
      delete [] s_;
      const auto size = other.size();
      s_ = new char[size];
      std::memcpy(s_, other.c_str(), size);
    }
    return *this;
  }
  String& operator=(String&& other)
  {
    if (this != &other){
      s_ = std::move(other.c_str());
    }
    return *this;
  }
  std::size_t size() const {
    return s_ ? strlen(s_) : 0;
  }
  char* c_str()
  {
    return s_;
  }
  char const* c_str() const
  {
    return s_;
  }
  char& operator[](std::size_t n)
  {
    return s_[n];
  }
  char const& operator[](std::size_t n) const
  {
    return s_[n];
  }
};

String get_string()
{
  return String{"Consectetur adipiscing elit"};
}

int main()
{
  String const s1("Lorem ipsum dolor sit amet");
  std::cout << "s1: " << s1.c_str() << '\n';

  String s2 = get_string();
  std::cout << "s2: " << s2.c_str() << '\n';

  String s3;
  s3 = s1;

  String s4;
  s4 = std::move(s2);
  std::cout << "s4: " << s4.c_str() << '\n';

  char& c1 = s4[4];
  char const& c2 = s1[4];
  std::cout << "c1 " << c1 << " c2 " << c2 << std::endl;

  std::cout << "s3: " << s3.c_str() << '\n';
}
