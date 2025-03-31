#include "floats.h"
#include <iostream>
#include <cmath>
#include <limits.h>

BigInt& operator+=(BigInt& big_int, int val) {
  BigInt new_val = big_int;
  new_val = val;
  big_int += new_val;
  return big_int;
}

BigInt& operator-=(BigInt& big_int, int val) {
  BigInt new_val = big_int;
  new_val = val;
  big_int -= new_val;
  return big_int;
}

#ifdef __cplusplus
extern "C" {
#endif

#define size_of_uint64 64

void float_destroy(Float* self) {}

int float_init(Float* self, int exponent_bits, int mantissa_bits) {
  self->exponent_bits = exponent_bits;
  self->mantissa_bits = mantissa_bits;
  self->size_of_big_int = 2 * (((exponent_bits + mantissa_bits) / size_of_uint64) +
                               ((exponent_bits + mantissa_bits) % size_of_uint64 == 0 ? 0 : 1));

  try {
    self->exponent = BigInt::with_digits(self->size_of_big_int);
    self->exponent = 0;
  } catch (...) {
    return -1;
  }

  try {
    self->mantissa = BigInt::with_digits(self->size_of_big_int);
    self->mantissa = 0;
  } catch (...) {
    return -1;
  }

  try {
    self->pow_2_mantissa_bits = BigInt::with_digits(self->size_of_big_int);
    self->pow_2_mantissa_bits = 1;
    self->pow_2_mantissa_bits <<= mantissa_bits;
  } catch (...) {
    return -1;
  }

  self->value = Float::Value::Denormal;
  self->sign = false;

  return 0;
}

int float_get_exponent_bits(Float* self) {
  return self->exponent_bits;
}

int float_get_mantissa_bits(Float* self) {
  return self->mantissa_bits;
}

bool float_get_sign(const Float* self) {
  return self->sign;
}

void float_get_exponent(const Float* self, void* target) {
  memcpy(target, self->exponent.get_digits(),
         (self->exponent_bits / CHAR_BIT) + (self->exponent_bits % CHAR_BIT == 0 ? 0 : 1));
}

void float_get_mantissa(const Float* self, void* target) {
  memcpy(target, self->mantissa.get_digits(),
         (self->mantissa_bits / CHAR_BIT) + (self->mantissa_bits % CHAR_BIT == 0 ? 0 : 1));
}

void float_set_sign(Float* self, bool sign) {
  self->sign = sign;
}

BigInt get_big_exp(const Float* self) {
  return ((BigInt::with_digits(self->size_of_big_int) = 1) <<= self->exponent_bits) -= 1;
}

void float_set_exponent(Float* self, const void* exponent) {
  self->exponent = 0;
  memcpy(self->exponent.get_digits(), exponent,
         (self->exponent_bits / CHAR_BIT) + (self->exponent_bits % CHAR_BIT == 0 ? 0 : 1));

  if (self->exponent.is_zero()) {
    self->value = Float::Value::Denormal;
  } else if (self->exponent == get_big_exp(self)) {
    self->value = Float::Value::Special;
  } else {
    self->value = Float::Value::Normal;
  }
}

void float_set_mantissa(Float* self, const void* mantissa) {
  self->mantissa = 0;
  memcpy(self->mantissa.get_digits(), mantissa,
         (self->mantissa_bits / CHAR_BIT) + (self->mantissa_bits % CHAR_BIT == 0 ? 0 : 1));
}

bool find_bit(BigInt* big_int, int shift) {
  *big_int >>= (shift - 1);
  bool res = !big_int->is_even();
  *big_int >>= 1;
  return res;
}

void norm(Float* self) {
  bool last_bit = false;

  if (!self->mantissa.is_zero()) {
    int pos = self->size_of_big_int * size_of_uint64 - self->mantissa.countl_zero() - self->mantissa_bits - 1;

    if (pos >= 0) {
      last_bit = find_bit(&self->mantissa, pos);
      self->exponent += pos;

      if (!self->exponent.is_zero() && !self->exponent.is_negative()) {
        self->mantissa -= self->pow_2_mantissa_bits;
        self->value = Float::Value::Normal;
      }
    } else {
      pos *= -1;
      self->mantissa <<= pos;
      self->exponent -= pos;

      if (!self->exponent.is_zero() && !self->exponent.is_negative()) {
        self->mantissa -= self->pow_2_mantissa_bits;
        self->value = Float::Value::Normal;
      }
    }
  }

  if (self->exponent.is_negative() || self->exponent.is_zero()) {
    last_bit = find_bit(&self->mantissa, 1 - self->exponent.to_int());
    self->exponent = 0;
    self->value = Float::Value::Denormal;
  }

  if (last_bit) {
    self->mantissa += 1;
  }

  if (self->mantissa == self->pow_2_mantissa_bits) {
    self->exponent += 1;
    self->mantissa = 0;
    self->value = Float::Value::Normal;
  }

  BigInt big_exp = get_big_exp(self);
  if (self->exponent >= big_exp) {
    self->exponent = big_exp;
    self->mantissa = 0;
    self->value = Float::Value::Special;
  }
}

void float_add(Float* result, const Float* a, const Float* b) {
  // TODO
}

void float_sub(Float* result, const Float* a, const Float* b) {
  // TODO
}

bool mul_special(Float* result, const Float* a, const Float* b) {
  if ((a->exponent.is_zero() && a->mantissa.is_zero() &&
       b->value == Float::Value::Special && b->mantissa.is_zero()) ||
      (b->exponent.is_zero() && b->mantissa.is_zero() &&
       a->value == Float::Value::Special && a->mantissa.is_zero())) {
    result->exponent = get_big_exp(result);

    result->mantissa = 1;
    result->mantissa <<= (result->mantissa_bits - 1);
    result->sign = false;
    result->value = Float::Value::Special;
    return true;
  }

  if (a->value == Float::Value::Special && !a->mantissa.is_zero()) {
    result->mantissa = a->mantissa;
    result->exponent = a->exponent;
    result->sign = a->sign;
    result->value = Float::Value::Special;
    return true;
  }

  if (b->value == Float::Value::Special && !b->mantissa.is_zero()) {
    result->mantissa = b->mantissa;
    result->exponent = b->exponent;
    result->sign = b->sign;
    result->value = Float::Value::Special;
    return true;
  }

  if (a->value == Float::Value::Special && a->mantissa.is_zero()) {
    result->exponent = a->exponent;
    result->mantissa = 0;
    result->sign = a->sign ^ b->sign;
    result->value = Float::Value::Special;
    return true;
  }

  if (b->value == Float::Value::Special && b->mantissa.is_zero()) {
    result->exponent = b->exponent;
    result->mantissa = 0;
    result->sign = a->sign ^ b->sign;
    result->value = Float::Value::Special;
    return true;
  }

  return false;
}

BigInt get_real_exp(const Float* self) {
  BigInt exp = (BigInt::with_digits(self->size_of_big_int) = 0);

  if (self->value == Float::Value::Normal) {
    exp += self->exponent;
  } else {
    exp += 1;
  }

  return exp;
}

BigInt get_real_mant(const Float* self) {
  BigInt mant = self->mantissa;
  if (self->value == Float::Value::Normal) {
    mant += self->pow_2_mantissa_bits;
  }

  return mant;
}

void float_mul(Float* result, const Float* a, const Float* b) {
  if (mul_special(result, a, b)) {
    return;
  }

  result->sign = a->sign ^ b->sign;

  BigInt m_bits = BigInt::with_digits(result->size_of_big_int);
  m_bits = a->mantissa_bits;

  BigInt b_denumerator = BigInt::with_digits(result->size_of_big_int);
  b_denumerator = 1;
  b_denumerator <<= (b->exponent_bits - 1);
  b_denumerator = -b_denumerator;

  BigInt b_exp = ((get_real_exp(b) += b_denumerator) += 1);
  result->exponent = get_real_exp(a) + b_exp - m_bits;

  result->mantissa = get_real_mant(a) * get_real_mant(b);

  norm(result);
}

void float_div(Float* result, const Float* a, const Float* b) {
  // TODO
}

void float_next(Float* self) {
  if (self->value == Float::Value::Special) {
    if (self->mantissa.is_zero() && self->sign) {
      self->sign = true;
      self->exponent -= 1;
      self->mantissa = self->pow_2_mantissa_bits;
      self->mantissa -= 1;
      self->value = Float::Value::Normal;
    }
  } else if (self->value == Float::Value::Normal) {
    if (self->sign) {
      self->mantissa -= 1;

      if (self->mantissa.is_negative()) {
        self->exponent -= 1;
        self->mantissa = self->pow_2_mantissa_bits;
        self->mantissa -= 1;
      }

      if (self->exponent.is_zero()) {
        self->value = Float::Value::Denormal;
      }
    } else {
      self->mantissa += 1;

      if (self->mantissa == self->pow_2_mantissa_bits) {
        self->exponent += 1;
        self->mantissa = 0;
      }

      BigInt big_exp = get_big_exp(self);
      if (self->exponent >= big_exp) {
        self->exponent = big_exp;
        self->mantissa = 0;
        self->value = Float::Value::Special;
      }
    }
  } else {
    if (self->mantissa.is_zero()) {
      if (self->sign) {
        self->sign = false;
      } else {
        self->mantissa += 1;
      }
    } else {
      if (self->sign) {
        self->mantissa -= 1;
      } else {
        self->mantissa += 1;

        if (self->mantissa == self->pow_2_mantissa_bits) {
          self->exponent = 1;
          self->mantissa = 0;
          self->value = Float::Value::Normal;
        }
      }
    }
  }
}

void float_prev(Float* self) {
  if (self->value == Float::Value::Special) {
    if (self->mantissa.is_zero() && !self->sign) {
      self->sign = false;
      self->exponent -= 1;
      self->mantissa = self->pow_2_mantissa_bits;
      self->mantissa -= 1;
      self->value = Float::Value::Normal;
    }
  } else if (self->value == Float::Value::Normal) {
    if (self->sign) {
      self->mantissa += 1;

      if (self->mantissa == self->pow_2_mantissa_bits) {
        self->exponent += 1;
        self->mantissa = 0;
      }

      BigInt big_exp = get_big_exp(self);
      if (self->exponent >= big_exp) {
        self->exponent = big_exp;
        self->mantissa = 0;
        self->value = Float::Value::Special;
      }
    } else {
      self->mantissa -= 1;

      if (self->mantissa.is_negative()) {
        self->exponent -= 1;
        self->mantissa = self->pow_2_mantissa_bits;
        self->mantissa -= 1;
      }

      if (self->exponent.is_zero()) {
        self->value = Float::Value::Denormal;
      }
    }
  } else {
    if (self->mantissa.is_zero()) {
      if (self->sign) {
        self->mantissa += 1;
      } else {
        self->sign = true;
      }
    } else {
      if (self->sign) {
        self->mantissa += 1;

        if (self->mantissa == self->pow_2_mantissa_bits) {
          self->exponent = 1;
          self->mantissa = 0;
          self->value = Float::Value::Normal;
        }
      } else {
        self->mantissa -= 1;
      }
    }
  }
}

void float_parse(Float* self, const char* string) {
  // TODO
}

int float_string(const Float* self, char* string, int n) {
  // TODO
  return 0;
}

#ifdef __cplusplus
}
#endif