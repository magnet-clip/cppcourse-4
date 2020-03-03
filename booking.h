// #pragma once

namespace RAII {
template <class T> class Booking {
public:
  Booking(const Booking &) = delete;
  Booking &operator=(const Booking &) = delete;

  Booking(Booking &&other)
      : _provider(other._provider), _counter(other._counter) {
    other._provider = nullptr;
  }

  Booking &operator=(Booking &&other) {
    _provider = other._provider;
    _counter = other._counter;
    other._provider = nullptr;
    return *this;
  }

  Booking(T *provider, int &counter) : _provider(provider), _counter(counter) {}

  ~Booking() {
    if (_provider != nullptr && _counter > 0) {
      _provider->CancelOrComplete(*this);
    }
  }

private:
  T *_provider;
  int &_counter;
};

} // namespace RAII