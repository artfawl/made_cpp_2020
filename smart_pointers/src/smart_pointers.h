#pragma once
#include <iostream>

namespace task {

template <class T>
class UniquePtr {
  T* data;

 public:
  explicit UniquePtr(T* ptr) noexcept;
  UniquePtr(UniquePtr&& other) noexcept;
  UniquePtr& operator=(UniquePtr&& other) noexcept;
  ~UniquePtr();

  T& operator*() const;
  T* operator->() const noexcept;
  T* get() const noexcept;
  T* release() noexcept;
  void reset(T* ptr = nullptr) noexcept;
  void swap(UniquePtr& other) noexcept;
};

template <class T>
struct Control {
  T* data = nullptr;
  size_t s_ptr_count = 0;
  size_t w_ptr_count = 0;
};

template <class T>
class WeakPtr;

template <class T>
class SharedPtr {
  friend class WeakPtr<T>;
  Control<T>* info;

 public:
  SharedPtr() noexcept;
  explicit SharedPtr(T* ptr);
  SharedPtr(const SharedPtr& other);
  SharedPtr(SharedPtr&& other) noexcept;
  SharedPtr(const WeakPtr<T>& other);
  SharedPtr& operator=(const SharedPtr& other);
  SharedPtr& operator=(SharedPtr&& other) noexcept;
  ~SharedPtr();

  T& operator*() const noexcept;
  T* operator->() const noexcept;
  T* get() const noexcept;

  size_t use_count() const noexcept;
  void reset(T* ptr = nullptr);
  void swap(SharedPtr& other) noexcept;
};

template <class T>
class WeakPtr {
  friend class SharedPtr<T>;
  Control<T>* info;

 public:
  WeakPtr() noexcept;
  WeakPtr(const WeakPtr& other) noexcept;
  WeakPtr(WeakPtr&& other) noexcept;
  WeakPtr(const SharedPtr<T>& other) noexcept;
  ~WeakPtr();

  WeakPtr& operator=(const WeakPtr& other) noexcept;
  WeakPtr& operator=(WeakPtr&& other) noexcept;
  WeakPtr& operator=(const SharedPtr<T>& other) noexcept;

  bool expired() const noexcept;
  SharedPtr<T> lock() const;
  size_t use_count() const noexcept;

  void reset() noexcept;
  void swap(WeakPtr& other) noexcept;
};
}  // namespace task

#include "smart_pointers.cpp"
