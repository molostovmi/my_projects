#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>

class String {
private:
  char* data_;
  size_t size_;
  size_t capacity_;

  explicit String(size_t amount);

  void set_capacity(size_t new_size, size_t start);

  size_t finds_helper(const String& str, bool is_type_of_find) const;

public:
  String();
  String(const char* new_data);
  String(size_t number, char symbol);
  String(char symbol);
  String(const String& str);

  String& operator=(String str);

  ~String();

  void swap(String& str);

  void reserve(size_t new_capacity);

  char* data();
  const char* data() const;

  size_t length() const;
  size_t size() const;
  size_t capacity() const;

  void push_back(char symbol);
  void pop_back();

  char& operator[](size_t index);
  char operator[](size_t index) const;

  char& front();
  char front() const;
  char& back();
  char back() const;

  String& operator+=(char symbol);
  String& operator+=(const String& str);

  String substr(size_t start, size_t count) const;

  size_t find(const String& str) const;
  size_t rfind(const String& str) const;

  void clear();
  bool empty() const;

  void shrink_to_fit();
};

String::String(size_t amount) : data_(new char[amount + 1]{'\0'}),
                                size_(amount), capacity_(amount) {}

void String::set_capacity(size_t new_size, size_t start = 0) {
  char* old_str = new char[new_size + 1];
  std::copy(data_, data_ + size_ + 1, old_str + start);
  delete[] data_;
  data_ = old_str;
  capacity_ = new_size;
}

size_t String::finds_helper(const String& str, bool is_type_of_find) const {
  if (str.size_ > size_) {
    return length();
  }

  for (size_t i = 0; i + str.size_ <= size_; ++i) {
    size_t j = (is_type_of_find ? i : size_ - i - str.size_);

    if (std::memcmp(data_ + j, str.data_, str.size_) == 0) {
      return j;
    }
  }

  return length();
}

String::String() : String(static_cast<size_t>(0)) {}

String::String(const char* new_data) : String(strlen(new_data)) {
  std::copy(new_data, new_data + strlen(new_data) + 1, data_);
}

String::String(size_t number, char symbol) : String(number) {
  std::fill(data_, data_ + number, symbol);
  data_[number] = '\0';
}
String::String(char symbol) : String(1, symbol) {}

String::String(const String& str) : String(str.size_) {
  std::copy(str.data_, str.data_ + str.size_ + 1, data_);
}

String& String::operator=(String str) {
  swap(str);
  return *this;
}

String::~String() { delete[] data_; }

void String::swap(String& str) {
  std::swap(data_, str.data_);
  std::swap(size_, str.size_);
  std::swap(capacity_, str.capacity_);
}

void String::reserve(size_t new_capacity) {
  if (new_capacity <= capacity_) {
    return;
  }

  delete[] data_;
  data_ = new char[new_capacity + 1];
  capacity_ = new_capacity;
  size_ = 0;
}

char* String::data() { return data_; }

const char* String::data() const { return data_; }

size_t String::length() const { return size_; }

size_t String::size() const { return size_; }

size_t String::capacity() const { return capacity_; }

void String::push_back(char symbol) {
  if ((size_ == 0) && (size_ == capacity_)) {
    reserve(1);
  }

  if (size_ == capacity_) {
    set_capacity(2 * size_);
  }
  data_[size_] = symbol;
  data_[size_ + 1] = '\0';
  ++size_;
}

void String::pop_back() { data_[--size_] = '\0'; }

bool operator==(const String& str1, const String& str2) {
  return str1.size() == str2.size() &&
         std::memcmp(str1.data(), str2.data(), str1.size() + 1) == 0;
}

bool operator!=(const String& str1, const String& str2) {
  return !(str1 == str2);
}

bool operator<(const String& str1, const String& str2) {
  return std::memcmp(str1.data(), str2.data(), std::min(str1.size(), str2.size()) + 1) < 0;
}

bool operator>(const String& str1, const String& str2) {
  return str2 < str1;
}

bool operator<=(const String& str1, const String& str2) {
  return !(str1 > str2);
}

bool operator>=(const String& str1, const String& str2) {
  return !(str1 < str2);
}

char& String::operator[](size_t index) { return data_[index]; }

char String::operator[](size_t index) const { return data_[index]; }

char& String::front() { return data_[0]; }

char String::front() const { return data_[0]; }

char& String::back() { return data_[size_ - 1]; }

char String::back() const { return data_[size_ - 1]; }

String& String::operator+=(char symbol) {
  push_back(symbol);
  return *this;
}

String& String::operator+=(const String& str) {
  if (capacity_ >= size_ + str.size_) {
    std::copy(str.data_, str.data_ + str.size_ + 1, data_ + size_);
  } else {
    set_capacity(size_ + str.size_);
    std::copy(str.data_, str.data_ + str.size_ + 1, data_ + size_);
  }
  size_ += str.size_;

  return *this;
}

String operator+(const String& str1, const String& str2) {
  String new_str = str1;
  new_str += str2;
  return new_str;
}

String String::substr(size_t start, size_t count) const {
  if (start > size_) {
    throw std::out_of_range("start pos of substr > size\n");
  }

  count = std::min(count, size_ - start);
  String str(count);
  std::copy(data_ + start, data_ + start + count, str.data_);

  return str;
}

size_t String::find(const String& str) const { return finds_helper(str, true); }

size_t String::rfind(const String& str) const { return finds_helper(str, false); }

void String::clear() {
  size_ = 0;
  data_[0] = '\0';
}

bool String::empty() const { return size_ == 0; }

void String::shrink_to_fit() {
  if (capacity_ != size_) {
    set_capacity(size_);
  }
}

std::ostream& operator<<(std::ostream& out, const String& str) {
  for (size_t i = 0; i < str.size(); ++i) {
    out << str[i];
  }
  return out;
}

std::istream& operator>>(std::istream& in, String& str) {
  str.clear();

  char cur;
  while (in.get(cur) && !std::isspace(cur)) {
    str.push_back(cur);
  }
  return in;
}