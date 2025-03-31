#include <cstring>
#include <iostream>

struct Node {
  char* str;
  Node* prev;
};

void Push(Node*& end, char* new_str, size_t& size) {
  Node* new_node = new Node {new_str, end};
  end = new_node;
  ++size;
}

void Pop(Node*& end, size_t& size) {
  Node* tmp = end->prev;
  delete[] end->str;
  delete end;
  end = tmp;
  --size;
}

void Clear(Node*& end, size_t& size) {
  while (size != 0) {
    Pop(end, size);
  }
}

char* ScanStr() {
  char* str = new char[1]{'\0'};
  char cur;
  size_t len = 1;

  while (std::cin.get(cur) && !std::isspace(cur)) {
    char* new_str = new char[len + 1];
    std::copy(str, str + len - 1, new_str);
    new_str[len - 1] = cur;
    new_str[len] = '\0';
    delete[] str;
    str = new_str;
    ++len;
  }

  return str;
}

bool Exit() {
  std::cout << "bye\n";
  return true;
}

void Back(Node* end, size_t size) {
  if (size == 0) {
    std::cout << "error\n";
  } else {
    std::cout << end->str << "\n";
  }
}

void PopInMain(Node*& end, size_t& size) {
  if (size == 0) {
    std::cout << "error\n";
  } else {
    std::cout << end->str << "\n";
    Pop(end, size);
  }
}

void PushInMain(Node*& end, size_t& size) {
  char* new_str = ScanStr();
  Push(end, new_str, size);
  std::cout << "ok\n";
}

int main() {
  Node* end = nullptr;
  size_t size = 0;
  bool is_exit = false;

  while (!is_exit) {
    char* str = ScanStr();

    if (std::strcmp(str, "exit") == 0) {
      is_exit = Exit();
    } else if (std::strcmp(str, "size") == 0) {
      std::cout << size << "\n";
    } else if (std::strcmp(str, "back") == 0) {
      Back(end, size);
    } else if (std::strcmp(str, "clear") == 0) {
      Clear(end, size);
      std::cout << "ok\n";
    } else if (std::strcmp(str, "pop") == 0) {
      PopInMain(end, size);
    } else {
      PushInMain(end, size);
    }

    delete[] str;
  }

  Clear(end, size);
  return 0;
}