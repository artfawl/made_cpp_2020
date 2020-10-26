#pragma once

#include <iostream>

template <class T>
class Allocator;

class Chunk {
  template <class T>
  friend class Allocator;
  Chunk* prev;
  uint8_t* storage;
  int empty;
  const size_t SIZE = 1024;
  Chunk() : prev(nullptr), storage(nullptr), empty(0) {
    storage = new uint8_t[SIZE];
  }
  ~Chunk() { delete[] storage; }
};

template <class T>
class Allocator {
 private:
  Chunk** last_storage;
  size_t bytes;
  int* copies;

 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  template <class U>
  struct rebind {
    typedef Allocator<U> other;
  };

  Allocator() : bytes(sizeof(T)) {
    last_storage = new Chunk* {nullptr};
    copies = new int{1};
  }

  Allocator(const Allocator& other) : bytes(other.bytes) {
    last_storage = other.last_storage;
    copies = other.copies;
    ++(*copies);
  }

  Allocator& operator=(const Allocator& other) {
    if (this == &other) return *this;
    this->~Allocator();
    copies = other.copies;
    ++(*copies);
    last_storage = other.last_storage;
    return *this;
  }

  T* allocate(const size_t n) {
    if (last_storage[0] == nullptr) last_storage[0] = new Chunk();
    if (n * bytes > last_storage[0]->SIZE) throw std::exception();
    Chunk* tmp = last_storage[0];
    while (tmp != nullptr) {
      if (tmp->SIZE - tmp->empty >= bytes * n) {
        uint8_t* to_return = tmp->storage + tmp->empty;
        tmp->empty += bytes * n;
        return reinterpret_cast<T*>(to_return);
      }
      tmp = tmp->prev;
    }
    Chunk* new_cnunk = new Chunk();
    new_cnunk->prev = last_storage[0];
    last_storage[0] = new_cnunk;
    uint8_t* to_return = last_storage[0]->storage;
    last_storage[0]->empty += bytes * n;
    return reinterpret_cast<T*>(to_return);
  }

  void deallocate(T* p, size_t n) { p = nullptr; }

  template <class... Args>
  void construct(T* p, Args&&... args) {
    new (p) T(std::forward<Args>(args)...);
  }

  void destroy(T* p) { p->~T(); }

  ~Allocator() {
    --(*copies);
    if (*copies == 0) {
      delete copies;
      Chunk* tmp = last_storage[0];
      while (tmp != nullptr) {
        last_storage[0] = last_storage[0]->prev;
        delete tmp;
        tmp = last_storage[0];
      }
      delete last_storage;
    }
  }
};