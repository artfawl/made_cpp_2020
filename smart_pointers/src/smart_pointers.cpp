#include "smart_pointers.h"

namespace task {

template <class T>
UniquePtr<T>::UniquePtr(T* ptr) noexcept : data(ptr) {}

template <class T>
UniquePtr<T>::UniquePtr(UniquePtr&& other) noexcept : data(nullptr) {
  data = std::move(other.data);
  other.data = nullptr;
}
template <class T>
UniquePtr<T>::~UniquePtr() {
  delete data;
}
template <class T>
UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr&& other) noexcept {
  if (this == &other) return *this;
  data = std::move(other.data);
  other.data = nullptr;
  return *this;
}
template <class T>
T& UniquePtr<T>::operator*() const {
  return *data;
}
template <class T>
T* UniquePtr<T>::operator->() const noexcept {
  return data;
}
template <class T>
T* UniquePtr<T>::get() const noexcept {
  return data;
}
template <class T>
T* UniquePtr<T>::release() noexcept {
  T* tmp = data;
  data = nullptr;
  return tmp;
}
template <class T>
void UniquePtr<T>::reset(T* ptr) noexcept {
  T* old_pointer = data;
  data = ptr;
  delete old_pointer;
}
template <class T>
void UniquePtr<T>::swap(UniquePtr& other) noexcept {
  std::swap(data, other.data);
}

// SharedPtr

template <class T>
SharedPtr<T>::SharedPtr() noexcept : info(nullptr) {}

template <class T>
SharedPtr<T>::SharedPtr(T* ptr) : info(nullptr) {
  info = new Control<T>();
  info->data = ptr;
  info->s_ptr_count = 1;
}
template <class T>
SharedPtr<T>::SharedPtr(const SharedPtr& other) : info(nullptr) {
  info = other.info;
  if (info) ++info->s_ptr_count;
}
template <class T>
SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept : info(nullptr) {
  info = std::move(other.info);
  other.info = nullptr;
}
template <class T>
SharedPtr<T>::SharedPtr(const WeakPtr<T>& other) : info(nullptr) {
  info = other.info;
  if (info) ++info->s_ptr_count;
}
template <class T>
SharedPtr<T>::~SharedPtr() {
  if (info) {
    --info->s_ptr_count;
    if (info->s_ptr_count == 0) {
      delete info->data;
      info->data = nullptr;
      if (info->w_ptr_count == 0) delete info;
      info = nullptr;
    }
  }
}
template <class T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other) {
  if (this == &other) return *this;
  this->~SharedPtr();
  info = other.info;
  if (info) ++info->s_ptr_count;
  return *this;
}
template <class T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) noexcept {
  if (this == &other) return *this;
  this->~SharedPtr();
  info = std::move(other.info);
  other.info = nullptr;
  return *this;
}
template <class T>
T& SharedPtr<T>::operator*() const noexcept {
  return *info->data;  // без проверки, так как если не владеет, то результат
                       // undefined (cppreference)
}
template <class T>
T* SharedPtr<T>::operator->() const noexcept {
  return info->data;  // без проверки, так как если не владеет, то результат
                      // undefined (cppreference)
}
template <class T>
T* SharedPtr<T>::get() const noexcept {
  if (info) return info->data;
  return nullptr;
}
template <class T>
size_t SharedPtr<T>::use_count() const noexcept {
  return info->s_ptr_count;
}
template <class T>
void SharedPtr<T>::reset(T* ptr) {
  if (info->data != ptr) {
    *this = std::move(SharedPtr(ptr));
  }
}
template <class T>
void SharedPtr<T>::swap(SharedPtr& other) noexcept {
  std::swap(info, other.info);
}

// WeakPtr

template <class T>
WeakPtr<T>::WeakPtr() noexcept : info(nullptr) {}
template <class T>
WeakPtr<T>::WeakPtr(const WeakPtr& other) noexcept : info(nullptr) {
  info = other.info;
  if (info) ++info->w_ptr_count;
}
template <class T>
WeakPtr<T>::WeakPtr(WeakPtr&& other) noexcept : info(nullptr) {
  info = std::move(other.info);
  other.info = nullptr;
}
template <class T>
WeakPtr<T>::WeakPtr(const SharedPtr<T>& other) noexcept : info(nullptr) {
  info = other.info;
  if (info) ++info->w_ptr_count;
}
template <class T>
WeakPtr<T>::~WeakPtr() {
  if (info) {
    --info->w_ptr_count;
    if (info->w_ptr_count == 0 && info->s_ptr_count == 0) delete info;
  }
  info = nullptr;
}
template <class T>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr& other) noexcept {
  if (this == &other) return *this;
  this->~WeakPtr();
  info = other.info;
  if (info) ++info->w_ptr_count;
  return *this;
}
template <class T>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr&& other) noexcept {
  if (this == &other) return *this;
  this->~WeakPtr();
  info = std::move(other.info);
  other.info = nullptr;
  return *this;
}
template <class T>
WeakPtr<T>& WeakPtr<T>::operator=(const SharedPtr<T>& other) noexcept {
  info = other.info;
  if (info) ++info->w_ptr_count;
  return *this;
}

template <class T>
bool WeakPtr<T>::expired() const noexcept {
  if (info) return !bool(info->s_ptr_count);
  return true;
}
template <class T>
SharedPtr<T> WeakPtr<T>::lock() const {
  return expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
}

template <class T>
size_t WeakPtr<T>::use_count() const noexcept {
  if (info) return info->s_ptr_count;
  return 0;
}
template <class T>
void WeakPtr<T>::reset() noexcept {
  this->~WeakPtr();
}
template <class T>
void WeakPtr<T>::swap(WeakPtr& other) noexcept {
  std::swap(info, other.info);
}

}  // namespace task