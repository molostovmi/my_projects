#include <stdint.h>
#include <string.h>

#include "ieee754_clf.h"

float_class_t classify(double x) {
  union for_present {
    double a;
    uint64_t b;
  };

  union for_present c;
  uint64_t bit_present;
  c.a = x;
  bit_present = c.b;

  if (bit_present == 0) {
    return Zero;
  }

  if ((bit_present ^ ((uint64_t)1 << 63)) == 0) {
    return MinusZero;
  }

  if (((bit_present >> 52) & ~(1 << 11)) == (1 << 11) - 1) {
    if ((bit_present << 12) == 0) {
      if ((bit_present >> 63) == 1) {
        return MinusInf;
      }
      return Inf;
    }
    return NaN;
  }

  if ((bit_present >> 63) == 1) {
    if (((bit_present >> 52) & ~(1 << 11)) == 0) {
      return MinusDenormal;
    }

    return MinusRegular;
  } else {
    if (((bit_present >> 52) & ~(1 << 11)) == 0) {
      return Denormal;
    }

    return Regular;
  }
}