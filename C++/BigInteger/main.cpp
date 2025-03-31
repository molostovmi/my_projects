#include "biginteger.h"

void ComarisonCheck(void) {
  std::cout << "Comparison:\n";

  std::cout << "100 <= -100\n";
  std::cout << (BigInteger(100) <= BigInteger(-100)) << "\n";

  std::cout << "900 >= -1000000000\n";
  std::cout << (BigInteger(900) >= BigInteger("-1000000000")) << "\n";

  std::cout << "0 == 0\n";
  std::cout << (BigInteger(0) == BigInteger(0)) << "\n";

  std::cout << "89 < 787878\n";
  std::cout << (BigInteger(89) < BigInteger(787878)) << "\n";

  std::cout << "4 > 0\n";
  std::cout << (BigInteger(4) > BigInteger(0)) << "\n";

  std::cout << "1 != 0\n";
  std::cout << (BigInteger(1) != BigInteger(0)) << "\n\n";
}

int main() {

  std::string str1;
  std::string str2;
  for (int i = 0; i < 3000; ++i) {
    str1 += std::to_string(rand() % 10);
  }
  for (int i = 0; i < 500; ++i) {
    str2 += std::to_string(rand() % 10);
  }
  std::cout << "start\n";
  BigInteger a(str1);
  BigInteger b(str2);
  std::cout << a / b << '\n';

  //std::cout << 5_bi / 1_bi << "\n";

  BigInteger big_int = BigInteger("-1234567890987654567432123456789");
  std::cout << big_int << "\n";
  BigInteger mama(big_int);
  std::cout << mama << "\n";
  BigInteger papa = -mama;
  std::cout << papa << "\n\n";

  ComarisonCheck();

  std::cout << "123456543234567893456434 + 12345643456787655678976543:\n";
  std::cout << ((BigInteger("123456543234567893456434") + BigInteger("12345643456787655678976543")) == BigInteger("12469100000022223572432977")) << "\n\n";

  std::cout << "-1234545345456456566543234567893456434 + 12345643456787655678976543:\n";
  std::cout << ((BigInteger("-1234545345456456566543234567893456434") + BigInteger("12345643456787655678976543")) == BigInteger("-1234545345444110923086446912214479891")) << "\n\n";

  std::cout << "12435415314563412563452143651243562143 - 23567253672563256735163512536:\n";
  std::cout << ((BigInteger("12435415314563412563452143651243562143") - BigInteger("23567253672563256735163512536")) == BigInteger("12435415290996158890888886916080049607")) << "\n\n";

  std::cin >> mama >> papa;
  std::cout << " + = " << mama + papa << "\n";
  std::cout << " * = " << mama * papa << "\n\n";

  std::cout << ++BigInteger("89") << "\n";
  std::cout << --BigInteger("89") << "\n\n";

  std::cout << BigInteger("89")++ << "\n";
  std::cout << BigInteger("89")-- << "\n\n";

  BigInteger tmp1 = 3432736781263767832617826338127372189378912378912738972389789789723897894_bi;
  std::cout << tmp1 << "\n\n";

  std::cout << ((BigInteger("12435415314563412563452143651243562143") *= BigInteger("23567253672563256735163512536")) == BigInteger("293068587241993951243351797466286863363791687521486917504675524648"))<< "\n\n";
  std::cout << ((BigInteger("12435415314563412563452143651243562143") *= BigInteger("-23567253672563256735163512536")) == BigInteger("-293068587241993951243351797466286863363791687521486917504675524648"))<< "\n\n";

  std::cout << (100_bi / 20_bi) << "\n";

  std::cout << (1000000000_bi / 500000001_bi) << "\n\n";
  std::cout << ((2356512354253412563425613456_bi / 35216351673_bi) == 66915289128604578_bi) << "\n\n";
  std::cout << ((2356512354253412563425613456_bi % 35216351673_bi) == 20819854462_bi) << "\n\n";
  std::cout << (1000000000_bi / BigInteger(-1000)) << "\n\n";

  std::cout << (0_bi / -2_bi) << "\n\n";

  std::cout << (1000000000000000000000000000000000_bi / -1000000_bi) << "\n";

  /* std::cout << (Rational(15) / Rational(83456_bi)).asDecimal(10) << "\n\n";

  double tmptmp = static_cast<double>(Rational(123) / Rational(67));
  std::cout << tmptmp << "\n";

  Rational rat1 = 12_bi;
  Rational rat2 = 8_bi; */

  //std::cout << ((rat1 / rat2) - (Rational(1) / Rational(2))).toString() << "\n";

  //std::cout << 1000000000000000000000000000000000_bi / -1000000_bi << "\n";*/

  return 0;
}