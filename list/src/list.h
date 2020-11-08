#pragma once
#include <iostream>
#include <iterator>

namespace task {

template <class T, class Alloc = std::allocator<T>>
class list {
 public:
  using value_type = T;
  using allocator_type = Alloc;
  using reference = value_type&;
  using const_reference = const value_type&;

 private:
  struct Node {
    Node* prev;
    Node* next;
    T* value;  // ссылка, так как мне нужен нейтральный элемент (root),
               // root->prev - последний, root->next первый
    //если сделать просто T value, то создание нейтрального элемента вызывает
    //базовый конструктор, который не всегда существует а без нейтрального
    //элемента возникает куча других проблем/костылей при пуше, в итераторах и
    //тд
    Node() : prev(this), next(this), value(nullptr) {}
  };

  using _traits = std::allocator_traits<Alloc>;
  using node_alloc_type = typename _traits::template rebind_alloc<Node>;

  Node* root;
  allocator_type allocator;
  node_alloc_type n_alloc;
  size_t length;
  const size_t MAX_LENGTH = 1000000000;

  void _neutral_elem();

  void _swap(Node* one, Node* two);

  void _insert(Node* after, Node* input);

  void _cut(Node* to_cut);

  template <class U>
  static Node* _create_node(node_alloc_type& node_alloc,
                            allocator_type& data_alloc, U&& data);

  static void _del_node(node_alloc_type& node_alloc, allocator_type& data_alloc,
                        Node* data);

  static Node* _copy_node(node_alloc_type& node_alloc,
                          allocator_type& data_alloc, Node* data);

  static Node* _move_node(node_alloc_type& from_node_alloc,
                          allocator_type& from_data_alloc,
                          node_alloc_type& to_node_alloc,
                          allocator_type& to_data_alloc, Node* data);

  template <class U>
  void _gen_push_back(U&& other);

  template <class U>
  void _gen_push_front(U&& other);

 public:
  class iterator {
   public:
    using difference_type = ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::bidirectional_iterator_tag;

    iterator() = delete;
    iterator(Node* node) : ptr(node) {}
    iterator(const iterator& other) : ptr(other.ptr) {}
    iterator& operator=(const iterator& other) {
      ptr = other.ptr;
      return *this;
    }

    iterator& operator++() {
      ptr = ptr->next;
      return *this;
    }
    iterator operator++(int) {
      iterator res = *this;
      ptr = ptr->next;
      return res;
    }
    reference operator*() const { return ptr->value[0]; }

    pointer operator->() const { return ptr->value; }

    iterator& operator--() {
      ptr = ptr->prev;
      return *this;
    }

    iterator operator--(int) {
      iterator res = *this;
      ptr = ptr->prev;
      return res;
    }

    bool operator==(iterator other) const { return (ptr == other.ptr); }
    bool operator!=(iterator other) const { return !(ptr == other.ptr); }

    // Your code goes here?..

   private:
    friend list;
    Node* ptr;
  };

  class const_iterator {
   public:
    using difference_type = ptrdiff_t;
    using value_type = const T;
    using pointer = const T*;
    using reference = const T&;
    using iterator_category = std::bidirectional_iterator_tag;

    const_iterator() = delete;
    const_iterator(iterator other) : ptr(other.ptr) {}
    const_iterator(Node* node) : ptr(node) {}
    const_iterator(const const_iterator& other) : ptr(other.ptr) {}
    const_iterator& operator=(const const_iterator& other) {
      ptr = other.ptr;
      return *this;
    }

    const_iterator& operator++() {
      ptr = ptr->next;
      return *this;
    }

    const_iterator operator++(int) {
      iterator res = *this;
      ptr = ptr->next;
      return res;
    }

    reference operator*() const { return ptr->value[0]; }

    pointer operator->() const { return ptr->value; }

    const_iterator& operator--() {
      ptr = ptr->prev;
      return *this;
    }

    const_iterator operator--(int) {
      iterator res = *this;
      ptr = ptr->prev;
    }

    bool operator==(const_iterator other) const { return (ptr == other.ptr); }
    bool operator!=(const_iterator other) const { return !(ptr == other.ptr); }

    // Your code goes here?..

   private:
    friend list;
    friend iterator;
    Node* ptr;
  };

  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  list();
  explicit list(const allocator_type& alloc);
  list(size_t count, const value_type& value,
       const allocator_type& alloc = Alloc());
  explicit list(size_t count, const allocator_type& alloc = Alloc());

  ~list();

  list(const list& other);
  list(list&& other);
  list& operator=(const list& other);
  list& operator=(list&& other);

  allocator_type get_allocator() const;

  value_type& front();
  const value_type& front() const;

  value_type& back();
  const value_type& back() const;

  iterator begin();
  iterator end();

  const_iterator cbegin() const;
  const_iterator cend() const;

  reverse_iterator rbegin();
  reverse_iterator rend();

  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;

  bool empty() const;
  size_t size() const;
  size_t max_size() const;
  void clear();

  iterator insert(const_iterator pos, const value_type& value);
  iterator insert(const_iterator pos, value_type&& value);
  iterator insert(const_iterator pos, size_t count, const value_type& value);

  iterator erase(const_iterator pos);
  iterator erase(const_iterator first, const_iterator last);

  void push_back(const value_type& value);
  void push_back(value_type&& value);
  void pop_back();

  void push_front(const value_type& value);
  void push_front(value_type&& value);
  void pop_front();

  template <class... Args>
  iterator emplace(const_iterator pos, Args&&... args);

  template <class... Args>
  void emplace_back(Args&&... args);

  template <class... Args>
  void emplace_front(Args&&... args);

  void resize(size_t count);
  void swap(list& other);

  void merge(list& other);
  void splice(const_iterator pos, list& other);
  void remove(const value_type& value);
  void reverse();
  void unique();
  void sort();

  // Your code goes here?..
};

// Your template function definitions may go here...

}  // namespace task

#include "list.cpp"
