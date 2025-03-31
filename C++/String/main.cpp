#include "string.h"

/* For check */
void PrintString(const String& str) {
  std::cout << "size = " << str.size() << " ";
  std::cout << "capacity = " << str.capacity() << " ";

  size_t i = 0;

  while (str[i] != '\0') {
    std::cout << str[i];
    ++i;
  }
  std::cout << str << "\n";

  i = 0;

  while (*(str.data() + i) != '\0') {
    std::cout << *(str.data() + i);
    ++i;
  }
  std::cout << "\n";
}

void ComparisonString(const String& str1, const String& str2) {
  std::cout << str1 << " " << str2 << "\n";
  std::cout << "<= " << (str1 <= str2) << " ";
  std::cout << ">= " << (str1 >= str2) << " ";
  std::cout << "< " << (str1 < str2) << " ";
  std::cout << "> " << (str1 > str2) << " ";
  std::cout << "== " << (str1 == str2) << " ";
  std::cout << "!= " << (str1 != str2) << "\n";
}

void CheckFind(const String& str1, const String& str2) {
  std::cout << str1 << "\n";
  std::cout << str2 << "\n";
  std::cout << "find  = " << str1.find(str2) << "\n";
  std::cout << "rfind  = " << str1.rfind(str2) << "\n";
}

/* End for check */

int main() {
  String tmp0;
  PrintString(tmp0);
  String tmp1 = "popa";
  PrintString(tmp1);
  String tmp2(4, 'e');
  PrintString(tmp2);
  String tmp3(tmp1);
  PrintString(tmp3);
  tmp3.clear();
  PrintString(tmp3);

  String name1 = "Bob";
  String name2 = "Alice";

  ComparisonString(name1, name2);

  String tmp = "papa";

  for (char i = 0; i < 5; ++i) {
    PrintString(tmp);
    tmp.push_back('0' + i);
  }

  while (!tmp.empty()) {
    PrintString(tmp);
    tmp.pop_back();
  }

  for (char i = 0; i < 5; ++i) {
    PrintString(tmp);
    tmp.push_back('0' + i);
  }

  ComparisonString("123", "124");
  ComparisonString("111", "111");

  String check_ravno = tmp;
  PrintString(check_ravno);
  check_ravno = check_ravno;
  PrintString(check_ravno);

  check_ravno = tmp + '4';
  PrintString(check_ravno);
  check_ravno = '5' + tmp;
  PrintString(check_ravno);
  check_ravno = tmp + "sobaka";
  PrintString(check_ravno);

  check_ravno += '4';
  PrintString(check_ravno);
  check_ravno += "sobaka";
  PrintString(check_ravno);
  check_ravno.shrink_to_fit();
  PrintString(check_ravno);
  check_ravno.back() = '3';
  check_ravno.front() = '3';
  PrintString(check_ravno);

  String new_ex1 = "mamapapa";
  PrintString(new_ex1.substr(0, 4));
  PrintString(new_ex1.substr(4, 4));
  CheckFind(new_ex1, "mama");
  CheckFind(new_ex1, "ma");
  CheckFind(new_ex1, "papa");
  CheckFind(new_ex1, "pa");
  CheckFind(new_ex1, "sosa");

  int a;
  std::cin >> new_ex1 >> a;
  PrintString(new_ex1);
  new_ex1[0] = '0';
  std::cout << a << "\n";

  String tupaya_stroka = "mmamam\0nsdjkaj";
  std::string tupaya_stroka2 = "mmamam\0nsdjkaj";

  std::cout << tupaya_stroka << " " << tupaya_stroka2;

  return 0;
}