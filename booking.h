#pragma once

namespace RAII {
template <class T>
class Booking {
 public:
  Booking(T* provider, int counter);
};

}  // namespace RAII