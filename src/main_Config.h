#include <Arduino.h>


template<class T>
T Clamp(T x, T minimum, T maximum)
{
  return max(min(x, maximum), minimum);
}

template<class T>
T Lerp(T alpha, T a, T b)
{
  return a + alpha * (b-a);
}