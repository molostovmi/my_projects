#include <iostream>
#include <string>
#include <vector>
#include <compare>

class Rational;

class BigInteger {
  friend Rational;
private:
  enum Sign : int {
    Positive = 1,
    Negative = -1,
    Zero = 0
  } sign_;

  std::vector<uint32_t> number_;
  const uint32_t kRang = 1000000000;

  void DecToOur(uint32_t value);
  BigInteger Abs(const BigInteger& big_int) const;
  BigInteger PositivePlusPositive(const BigInteger& big_int) const;
  BigInteger PositivePlusNegative(const BigInteger& big_int1, const BigInteger& big_int2) const;
  BigInteger Multi(const BigInteger& big_int1, const BigInteger& big_int2) const;
  BigInteger Division(const BigInteger& big_int1, const BigInteger& big_int2) const;
  void Swap(BigInteger& big_int);

public:
  BigInteger() : sign_(Zero), number_(1, 0) {}
  BigInteger(int value);
  BigInteger(const std::string& str);
  BigInteger(const BigInteger& big_int);

  BigInteger(BigInteger&& big_int) : sign_(big_int.sign_), number_(std::move(big_int.number_)) {
    big_int.sign_ = Zero;
  }

  BigInteger& operator=(BigInteger&& big_int) {

  }

  BigInteger& operator=(BigInteger big_int);

  ~BigInteger() = default;

  explicit operator bool() const;
  std::string toString(void) const;

  bool operator<(const BigInteger& big_int) const;
  bool operator>(const BigInteger& big_int) const { return big_int < *this; }
  bool operator<=(const BigInteger& big_int) const { return !(*this > big_int); }
  bool operator>=(const BigInteger& big_int) const { return !(*this < big_int); }
  bool operator==(const BigInteger& big_int) const;
  bool operator!=(const BigInteger& big_int) const { return !(*this == big_int); }

  BigInteger operator-() const;

  BigInteger& operator+=(const BigInteger& big_int);
  BigInteger operator+(const BigInteger& big_int) const;
  BigInteger& operator-=(const BigInteger& big_int);
  BigInteger operator-(const BigInteger& big_int) const;
  BigInteger& operator*=(const BigInteger& big_int);
  BigInteger operator*(const BigInteger& big_int) const;
  BigInteger& operator/=(const BigInteger& big_int);
  BigInteger operator/(const BigInteger& big_int) const;
  BigInteger& operator%=(const BigInteger& big_int);
  BigInteger operator%(const BigInteger& big_int) const;

  BigInteger& operator++();
  BigInteger& operator--();
  BigInteger operator++(int);
  BigInteger operator--(int);
};

void BigInteger::DecToOur(uint32_t value) {
  number_.clear();
  while (value > 0) {
    number_.emplace_back(value % kRang);
    value /= kRang;
  }
}

BigInteger BigInteger::Abs(const BigInteger& big_int) const {
  BigInteger new_big_int = big_int;

  if (new_big_int.sign_ == Negative) {
    new_big_int.sign_ = Positive;
  }

  return std::move(new_big_int);
}

BigInteger BigInteger::PositivePlusPositive(const BigInteger& big_int) const {
  uint32_t old_tmp = 0;
  BigInteger new_big_int;
  new_big_int.sign_ = Positive;
  new_big_int.number_.clear();

  if (big_int.number_.size() > number_.size()) {
    for (size_t i = 0; i < number_.size(); ++i) {
      uint32_t value = big_int.number_[i] + number_[i] + old_tmp;
      if (value >= kRang) {
        old_tmp = 1;
      } else {
        old_tmp = 0;
      }
      new_big_int.number_.emplace_back(value % kRang);
    }

    for (size_t i = number_.size(); i < big_int.number_.size(); ++i) {
      uint32_t value = big_int.number_[i] + old_tmp;
      if (value >= kRang) {
        old_tmp = 1;
      } else {
        old_tmp = 0;
      }
      new_big_int.number_.emplace_back(value % kRang);
    }

    if (old_tmp) {
      new_big_int.number_.emplace_back(old_tmp);
    }
  } else if (big_int.number_.size() < number_.size()) {
    for (size_t i = 0; i < big_int.number_.size(); ++i) {
      uint32_t value = big_int.number_[i] + number_[i] + old_tmp;
      if (value >= kRang) {
        old_tmp = 1;
      } else {
        old_tmp = 0;
      }
      new_big_int.number_.emplace_back(value % kRang);
    }

    for (size_t i = big_int.number_.size(); i < number_.size(); ++i) {
      uint32_t value = number_[i] + old_tmp;
      if (value >= kRang) {
        old_tmp = 1;
      } else {
        old_tmp = 0;
      }
      new_big_int.number_.emplace_back(value % kRang);
    }

    if (old_tmp) {
      new_big_int.number_.emplace_back(old_tmp);
    }
  } else {
    for (size_t i = 0; i < number_.size(); ++i) {
      uint32_t value = big_int.number_[i] + number_[i] + old_tmp;
      if (value >= kRang) {
        old_tmp = 1;
      } else {
        old_tmp = 0;
      }
      new_big_int.number_.emplace_back(value % kRang);
    }

    if (old_tmp) {
      new_big_int.number_.emplace_back(old_tmp);
    }
  }

  return std::move(new_big_int);
}

BigInteger BigInteger::PositivePlusNegative(const BigInteger& big_int1, const BigInteger& big_int2) const {
  uint32_t old_tmp = 0;
  BigInteger new_big_int;
  new_big_int.number_.clear();

  if (big_int1.number_.size() > big_int2.number_.size()) {
    new_big_int.sign_ = Positive;

    for (size_t i = 0; i < big_int2.number_.size(); ++i) {
      if (big_int1.number_[i] != 0) {
        if (big_int1.number_[i] - old_tmp < big_int2.number_[i]) {
          new_big_int.number_.emplace_back(kRang + big_int1.number_[i] - old_tmp - big_int2.number_[i]);
          old_tmp = 1;
        } else {
          new_big_int.number_.emplace_back(big_int1.number_[i] - old_tmp - big_int2.number_[i]);
          old_tmp = 0;
        }
      } else {
        if ((old_tmp != 0) || (big_int2.number_[i] != 0)) {
          new_big_int.number_.emplace_back(kRang - old_tmp - big_int2.number_[i]);
          old_tmp = 1;
        } else {
          new_big_int.number_.emplace_back(0);
          old_tmp = 0;
        }
      }
    }

    for (size_t i = big_int2.number_.size(); i < big_int1.number_.size(); ++i) {
      if (big_int1.number_[i] != 0) {
        if ((i != big_int1.number_.size() - 1) || (big_int1.number_[i] - old_tmp != 0)) {
          new_big_int.number_.emplace_back(big_int1.number_[i] - old_tmp);
          old_tmp = 0;
        }
      } else {
        if (old_tmp != 0) {
          new_big_int.number_.emplace_back(kRang - old_tmp);
        } else {
          new_big_int.number_.emplace_back(0);
          old_tmp = 0;
        }
      }
    }
  } else if (big_int1.number_.size() < big_int2.number_.size()) {
    new_big_int = -PositivePlusNegative(big_int2, big_int1);
  } else {
    if (big_int1 < (-big_int2)) {
      return -PositivePlusNegative(-big_int2, -big_int1);
    }
    new_big_int.sign_ = Positive;

    for (size_t i = 0; i < big_int1.number_.size(); ++i) {
      if (big_int1.number_[i] != 0) {
        if (big_int1.number_[i] - old_tmp < big_int2.number_[i]) {
          new_big_int.number_.emplace_back(kRang + big_int1.number_[i] - old_tmp - big_int2.number_[i]);
          old_tmp = 1;
        } else {
          new_big_int.number_.emplace_back(big_int1.number_[i] - old_tmp - big_int2.number_[i]);
          old_tmp = 0;
        }
      } else {
        if ((old_tmp != 0) || (big_int2.number_[i] != 0)) {
          new_big_int.number_.emplace_back(kRang - old_tmp - big_int2.number_[i]);
          old_tmp = 1;
        } else {
          new_big_int.number_.emplace_back(0);
          old_tmp = 0;
        }
      }
    }
  }

  while ((!new_big_int.number_.empty()) && (new_big_int.number_.back() == 0)) {
    new_big_int.number_.pop_back();
  }

  if (new_big_int.number_.empty()) {
    new_big_int.number_.push_back(0);
    new_big_int.sign_ = Zero;
  }

  return std::move(new_big_int);
}

BigInteger BigInteger::Multi(const BigInteger& big_int1, const BigInteger& big_int2) const {
  BigInteger new_big_int;
  new_big_int.number_.clear();

  uint64_t old_tmp = 0;
  new_big_int.number_.resize(big_int1.number_.size() + big_int2.number_.size());

  for (size_t i = 0; i < big_int1.number_.size(); ++i) {
    for (size_t j = 0; j < big_int2.number_.size(); ++j) {
      uint64_t value = static_cast<uint64_t>(big_int1.number_[i]) * static_cast<uint64_t>(big_int2.number_[j])
                       + old_tmp + new_big_int.number_[i + j];
      new_big_int.number_[i + j] = static_cast<uint32_t>(value % kRang);
      old_tmp = value / kRang;
    }

    if (old_tmp != 0) {
      size_t cur = i + big_int2.number_.size();
      new_big_int.number_[cur] += static_cast<uint32_t>(old_tmp);
      old_tmp = 0;
    }
  }

  while ((!new_big_int.number_.empty()) && (new_big_int.number_.back() == 0)) {
    new_big_int.number_.pop_back();
  }

  return std::move(new_big_int);
}

BigInteger BigInteger::Division(const BigInteger& big_int1, const BigInteger& big_int2) const {
  BigInteger result;
  BigInteger cur;

  for (size_t i = big_int1.number_.size(); i > 0; --i) {
    BigInteger cur_step = kRang;
    cur *= kRang;
    cur += big_int1.number_[i - 1];

    while ((cur < big_int2) && (i > 1)) {
      cur_step *= kRang;
      cur *= kRang;
      --i;
      cur += big_int1.number_[i - 1];
    }

    if ((i == 1) && (cur < big_int2)) {
      result *= cur_step;
      break;
    }

    BigInteger right = cur;
    BigInteger left = 1;
    BigInteger ans;

    while (right - left != 1) {
      BigInteger tmp = left + right;
      BigInteger average = tmp.number_[0] / 2;
      BigInteger step = 500000000;

      for (size_t i = 1; i < tmp.number_.size(); ++i) {
        average += step * tmp.number_[i];
        step *= kRang;
      }

      BigInteger value = big_int2 * average;
      if (value > cur) {
        right = average;
      } else if (value < cur) {
        left = average;
      } else {
        ans = average;
        break;
      }
    }

    if (right - left == 1) {
      ans = left;
    }

    result *= kRang;
    result += ans;
    cur -= ans * big_int2;
  }

  return std::move(result);
}

void BigInteger::Swap(BigInteger& big_int) {
  std::swap(number_, big_int.number_);
  std::swap(sign_, big_int.sign_);
}

BigInteger::BigInteger(int value) {
  if (value > 0) {
    sign_ = Positive;
    DecToOur(static_cast<uint32_t>(value));
  } else if (value == 0) {
    sign_ = Zero;
    number_.emplace_back(0);
  } else {
    sign_ = Negative;
    DecToOur(static_cast<uint32_t>(-value));
  }
}

BigInteger::BigInteger(const std::string& str) {
  size_t tmp = 0;

  if (str[0] == '-') {
    sign_ = Negative;
    while (str.size() - tmp >= 9) {
      number_.push_back(static_cast<uint32_t>(std::stoi(str.substr(str.size() - tmp - 9, 9))));
      tmp += 9;
    }
    if (str.size() - tmp > 1) {
      number_.push_back(static_cast<uint32_t>(std::stoi(str.substr(1, str.size() - tmp - 1))));
    }
  } else if (str[0] == '0') {
    sign_ = Zero;
    number_.emplace_back(0);
  } else {
    sign_ = Positive;
    while (str.size() - tmp >= 9) {
      number_.push_back(static_cast<uint32_t>(std::stoi(str.substr(str.size() - tmp - 9, 9))));
      tmp += 9;
    }
    if (str.size() - tmp > 0) {
      number_.push_back(static_cast<uint32_t>(std::stoi(str.substr(0, str.size() - tmp))));
    }
  }
}

BigInteger::BigInteger(const BigInteger& big_int) : sign_(big_int.sign_) {
  number_.resize(big_int.number_.size());
  std::copy(big_int.number_.begin(), big_int.number_.end(), number_.data());
}

BigInteger& BigInteger::operator=(BigInteger big_int) {
  Swap(big_int);
  return *this;
}

BigInteger::operator bool() const {
  if (sign_ == Zero) {
    return false;
  }

  return true;
}

std::string BigInteger::toString(void) const {
  std::string str;

  for (size_t j = 0; j < number_.size(); ++j) {
    std::string tmp;
    tmp.reserve(9);
    uint32_t tmp_value = number_[j];

    for (uint32_t i = 0; i < 9; ++i) {
      tmp.push_back('0' + static_cast<char>(tmp_value % 10));
      tmp_value /= 10;

      if ((tmp_value == 0) && (j == number_.size() - 1)) {
        break;
      }
    }
    std::reverse(tmp.begin(), tmp.end());
    str = tmp + str;
  }

  if (sign_ == Negative) {
    str = '-' + str;
  }

  return str;
}

bool BigInteger::operator<(const BigInteger& big_int) const {
  if (sign_ < big_int.sign_) {
    return true;
  }

  if (sign_ > big_int.sign_) {
    return false;
  }

  bool my_true = (sign_ == Positive);
  bool my_false = !my_true;

  if (big_int.number_.size() > number_.size()) {
    return my_true;
  }

  if (big_int.number_.size() < number_.size()) {
    return my_false;
  }

  for (size_t i = number_.size(); i > 0; --i) {
    if (big_int.number_[i - 1] > number_[i - 1]) {
      return my_true;
    }
    if (big_int.number_[i - 1] < number_[i - 1]) {
      return my_false;
    }
  }

  return false;
}

bool BigInteger::operator==(const BigInteger& big_int) const {
  if ((sign_ != big_int.sign_) || (number_.size() != big_int.number_.size())) {
    return false;
  }

  for (size_t j = 0; j < number_.size(); ++j) {
    if (number_[j] != big_int.number_[j]) {
      return false;
    }
  }

  return true;
}

BigInteger BigInteger::operator-() const {
  BigInteger big_int = *this;

  if (big_int.sign_ == Positive) {
    big_int.sign_ = Negative;
  } else {
    big_int.sign_ = Positive;
  }

  return big_int;
}

BigInteger& BigInteger::operator+=(const BigInteger& big_int) {
  if (sign_ == Positive) {
    if (big_int.sign_ == Positive) {
      *this = PositivePlusPositive(big_int);
      return *this;
    }

    if (big_int.sign_ == Negative) {
      *this = PositivePlusNegative(*this, big_int);
      return *this;
    }

    return *this;
  } else if (sign_ == Negative) {
    if (big_int.sign_ == Positive) {
      *this = PositivePlusNegative(big_int, *this);
      return *this;
    }
    if (big_int.sign_ == Negative) {
      *this = -PositivePlusPositive(big_int);
      return *this;
    }

    return *this;
  }

  *this = big_int;
  return *this;
}

BigInteger BigInteger::operator+(const BigInteger& big_int) const {
  BigInteger new_big_int = *this;
  return std::move(new_big_int += big_int);
}

BigInteger operator+(int value, const BigInteger& big_int) {
  return BigInteger(value) + big_int;
}

BigInteger& BigInteger::operator-=(const BigInteger& big_int) {
  return *this += -big_int;
}

BigInteger BigInteger::operator-(const BigInteger& big_int) const {
  BigInteger new_big_int = *this;
  return std::move(new_big_int -= big_int);
}

BigInteger operator-(int value, const BigInteger& big_int) {
  return BigInteger(value) - big_int;
}

BigInteger& BigInteger::operator*=(const BigInteger& big_int) {
  if ((sign_ != Zero) && (big_int.sign_ != Zero)) {
    Sign old_sign = sign_;

    if (number_.size() <= big_int.number_.size()) {
      *this = Multi(big_int, *this);
    } else {
      *this = Multi(*this, big_int);
    }
    if (old_sign == big_int.sign_) {
      sign_ = Positive;
    } else {
      sign_ = Negative;
    }

    return *this;
  }

  return (*this = std::move(BigInteger()));
}

BigInteger BigInteger::operator*(const BigInteger& big_int) const {
  BigInteger new_big_int = *this;
  return std::move(new_big_int *= big_int);
}

BigInteger operator*(int value, const BigInteger& big_int) {
  return BigInteger(value) * big_int;
}

BigInteger& BigInteger::operator/=(const BigInteger& big_int) {
  if (Abs(*this) < Abs(big_int)) {
    return *this = BigInteger();
  }

  if (big_int == 1) {
    return *this;
  }

  if (big_int == -1) {
    return *this = -(*this);
  }

  if (Abs(*this) == Abs(big_int)) {
    if (sign_ != big_int.sign_) {
      return *this = BigInteger(-1);
    }
    else {
      return *this = BigInteger(1);
    }
  }

  Sign old_sign = sign_;
  *this = Division(Abs(*this), Abs(big_int));
  sign_ = (old_sign == big_int.sign_ ? Positive : Negative);

  return *this;
}

BigInteger BigInteger::operator/(const BigInteger& big_int) const {
  BigInteger new_big_int = *this;
  return std::move(new_big_int /= big_int);
}

BigInteger operator/(int value, const BigInteger& big_int) {
  return BigInteger(value) / big_int;
}

BigInteger& BigInteger::operator%=(const BigInteger &big_int) {
  return *this = (*this - (*this / big_int) * big_int);
}

BigInteger BigInteger::operator%(const BigInteger &big_int) const {
  BigInteger new_big_int = *this;
  return new_big_int %= big_int;
}

BigInteger operator%(int value, const BigInteger& big_int) {
  return BigInteger(value) % big_int;
}

BigInteger& BigInteger::operator++() {
  return *this += 1;
}

BigInteger& BigInteger::operator--() {
  return *this -= 1;
}

BigInteger BigInteger::operator++(int) {
  BigInteger big_int = *this;
  ++*this;
  return big_int;
}

BigInteger BigInteger::operator--(int) {
  BigInteger big_int = *this;
  --*this;
  return big_int;
}

BigInteger operator""_bi(const char* str) {
  BigInteger big_int(str);
  return big_int;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& big_int) {
  out << big_int.toString();
  return out;
}

std::istream& operator>>(std::istream& in, BigInteger& big_int) {
  std::string str;
  in >> str;
  big_int = str;
  return in;
}

class Rational {
private:
  BigInteger numerator_;
  BigInteger denominator_;

  BigInteger Evklid(BigInteger big_int1, BigInteger big_int2) const;
  Rational Opposite(const Rational& rat) const;

public:
  Rational() : numerator_(0), denominator_(1) {}
  Rational(const BigInteger& big_int) : numerator_(big_int), denominator_(1) {}
  Rational(int value) : numerator_(value), denominator_(1) {}
  Rational(const Rational& rat) : numerator_(rat.numerator_), denominator_(rat.denominator_) {}

  Rational& operator=(const Rational& rat) = default;

  ~Rational() = default;

  Rational operator-() const;

  Rational& operator+=(const Rational& rat);
  Rational operator+(const Rational& rat) const;
  Rational& operator-=(const Rational& rat);
  Rational operator-(const Rational& rat) const;
  Rational& operator*=(const Rational& rat);
  Rational operator*(const Rational& rat) const;
  Rational& operator/=(const Rational& rat);
  Rational operator/(const Rational& rat) const;

  std::string toString() const;

  bool operator<(const Rational& rat) const;
  bool operator>(const Rational& rat) const { return rat < *this; }
  bool operator<=(const Rational& rat) const { return !(*this > rat); }
  bool operator>=(const Rational& rat) const { return !(*this < rat); }
  bool operator==(const Rational& rat) const;
  bool operator!=(const Rational& rat) const { return !(*this == rat); }

  std::string asDecimal(size_t precesion) const;
  explicit operator double() const;
};

BigInteger Rational::Evklid(BigInteger big_int1, BigInteger big_int2) const {
  while ((big_int1 > 0) && (big_int2 > 0)) {
    if (big_int1 < big_int2) {
      big_int2 %= big_int1;
    } else if (big_int2 < big_int1) {
      big_int1 %= big_int2;
    } else {
      return std::move(big_int1);
    }
  }

  if (big_int1 > big_int2) {
    return std::move(big_int1);
  }

  return std::move(big_int2);
}

Rational Rational::Opposite(const Rational &rat) const {
  Rational new_rat;
  new_rat.numerator_ = (rat.numerator_.sign_ == BigInteger::Negative ? -rat.denominator_ : rat.denominator_);
  new_rat.denominator_ = (rat.numerator_.sign_ == BigInteger::Negative ? -rat.numerator_ : rat.numerator_);
  return std::move(new_rat);
}

Rational Rational::operator-() const {
  Rational rat = *this;

  if (rat.numerator_.sign_ == BigInteger::Positive) {
    rat.numerator_.sign_ = BigInteger::Negative;
    return std::move(rat);
  }
  if (rat.numerator_.sign_ == BigInteger::Negative) {
    rat.numerator_.sign_ = BigInteger::Positive;
    return std::move(rat);
  }

  return std::move(rat);
}

Rational& Rational::operator+=(const Rational &rat) {
  //std::cerr << "+= " << toString() << " " << rat.toString() << "\n";
  numerator_ = numerator_ * rat.denominator_ + rat.numerator_ * denominator_;
  denominator_ *= rat.denominator_;
  return *this;
}

Rational Rational::operator+(const Rational &rat) const {
  Rational new_rat = *this;
  return std::move(new_rat += rat);
}

Rational operator+(int value, const Rational &rat) {
  return Rational(value) + rat;
}

Rational& Rational::operator-=(const Rational &rat) {
  //std::cerr << "-= " << toString() << " " << rat.toString() << "\n";
  return *this += -rat;
}

Rational Rational::operator-(const Rational &rat) const {
  Rational new_rat = *this;
  return std::move(new_rat -= rat);
}

Rational operator-(int value, const Rational &rat) {
  return Rational(value) - rat;
}

Rational& Rational::operator*=(const Rational &rat) {
  //std::cerr << "*= " << toString() << " " << rat.toString() << "\n";
  numerator_ *= rat.numerator_;
  denominator_ *= rat.denominator_;
  return *this;
}

Rational Rational::operator*(const Rational &rat) const {
  Rational new_rat = *this;
  return std::move(new_rat *= rat);
}

Rational operator*(int value, const Rational &rat) {
  return Rational(value) * rat;
}

Rational& Rational::operator/=(const Rational &rat) {
  //std::cerr << "/= " << toString() << " " << rat.toString() << "\n";
  return *this *= Opposite(rat);
}

Rational Rational::operator/(const Rational &rat) const {
  Rational new_rat = *this;
  return std::move(new_rat /= rat);
}

Rational operator/(int value, const Rational &rat) {
  return Rational(value) / rat;
}

std::string Rational::toString() const {
  //std::cerr << "toStr\n";

  if (denominator_ == 1) {
    return numerator_.toString();
  }

  BigInteger gcd = std::move(Evklid(numerator_.Abs(numerator_), denominator_.Abs(denominator_)));
  BigInteger new_num = numerator_ / gcd;
  BigInteger new_den = denominator_ / gcd;

  if ((new_den == 1) || (new_num == 0)) {
    return new_num.toString();
  }

  return std::move(new_num.toString() + "/" + new_den.toString());
}

bool Rational::operator<(const Rational &rat) const {
  //std::cerr << "<\n";
  if (numerator_.sign_ < rat.numerator_.sign_) {
    return true;
  }
  if (numerator_.sign_ > rat.numerator_.sign_) {
    return false;
  }

  return numerator_ * rat.denominator_ < rat.numerator_ * denominator_;
}

bool Rational::operator==(const Rational &rat) const {
  //std::cerr << "==\n";
  if (numerator_.sign_ != rat.numerator_.sign_) {
    return false;
  }

  return numerator_ * rat.denominator_ == rat.numerator_ * denominator_;
}

std::string Rational::asDecimal(size_t precision) const {
  //std::cerr << "asD\n";
  std::string str;
  Rational rat = *this;

  if (rat.numerator_.sign_ == BigInteger::Zero) {
    if (precision == 0) {
      return "0";
    } else {
      return "0." + std::string(precision, '0');
    }
  } else if (rat.numerator_.sign_ == BigInteger::Negative) {
    str.push_back('-');
  }

  BigInteger integer = std::move(rat.numerator_ / rat.denominator_);
  str += integer.Abs(integer).toString();

  if (precision == 0) {
    return str;
  }
  str.push_back('.');

  size_t ind = 0;
  BigInteger step = 10;
  std::string floating;
  rat -= integer;

  while (ind < precision) {
    BigInteger value;
    while (((value = (rat.numerator_ * step) / rat.denominator_) == 0) && (ind < precision)) {
      floating += "0";
      ind += 1;
      step *= 10;
    }

    if (ind != precision) {
      floating += value.Abs(value).toString();
      (rat.numerator_ *= step) -= value * rat.denominator_;
      step = 10;
    }
  }

  while (floating.size() > precision) {
    floating.pop_back();
  }

  return std::move(str + floating);
}

Rational::operator double() const {
  double value = std::stod(asDecimal(10));
  return value;
}

std::ostream& operator<<(std::ostream& out, const Rational& rat) {
  out << rat.toString();
  return out;
}