#include "list.h"

using namespace task;

template <class T, class Alloc>
template <class U>
typename list<T, Alloc>::Node* list<T, Alloc>::_create_node(
    node_alloc_type& node_alloc, Alloc& data_alloc, U&& data) {
  Node* new_node = node_alloc.allocate(1);
  node_alloc.construct(new_node);
  new_node->value = _traits::allocate(data_alloc, 1);
  _traits::construct(data_alloc, new_node->value, std::forward<U>(data));
  return new_node;
}

template <class T, class Alloc>
void list<T, Alloc>::_del_node(node_alloc_type& node_alloc, Alloc& data_alloc,
                               Node* data) {
  if (data->value != nullptr) {
    _traits::destroy(data_alloc, data->value);
    _traits::deallocate(data_alloc, data->value, 1);
  }
  node_alloc.destroy(data);
  node_alloc.deallocate(data, 1);
}

template <class T, class Alloc>
typename list<T, Alloc>::Node* list<T, Alloc>::_copy_node(
    node_alloc_type& node_alloc, Alloc& data_alloc, Node* data) {
  Node* new_node = node_alloc.allocate(1);
  node_alloc.construct(new_node);
  new_node->value = _traits::allocate(data_alloc, 1);
  _traits::construct(data_alloc, new_node->value, data->value[0]);
  return new_node;
}

template <class T, class Alloc>
typename list<T, Alloc>::Node* list<T, Alloc>::_move_node(
    node_alloc_type& from_node_alloc, Alloc& from_data_alloc,
    node_alloc_type& to_node_alloc, Alloc& to_data_alloc, Node* data) {
  Node* new_node = to_node_alloc.allocate(1);
  to_node_alloc.construct(new_node);
  new_node->value = _traits::allocate(to_data_alloc, 1);
  _traits::construct(to_data_alloc, new_node->value, std::move(data->value[0]));
  _del_node(from_node_alloc, from_data_alloc, data);
  return new_node;
}

template <class T, class Alloc>
void list<T, Alloc>::_neutral_elem() {  // нейтральный элемент (root)
  Node* new_node = n_alloc.allocate(1);
  n_alloc.construct(new_node);
  root = new_node;
}

template <class T, class Alloc>
void list<T, Alloc>::_cut(Node* to_cut) {  // вырезать ноду
  to_cut->prev->next = to_cut->next;
  to_cut->next->prev = to_cut->prev;
  --length;
}

template <class T, class Alloc>
void list<T, Alloc>::_insert(Node* after,
                             Node* input) {  // вставить ноду после after
  input->prev = after;
  input->next = after->next;
  after->next = input;
  input->next->prev = input;
  ++length;
}

template <class T, class Alloc>
void list<T, Alloc>::_swap(Node* one,
                           Node* two) {  // меняем 2 ноды местами в листе
  std::swap(one->value, two->value);
}

template <class T, class Alloc>
list<T, Alloc>::list() : root(nullptr), allocator(Alloc()), length(0) {
  n_alloc = node_alloc_type(allocator);
  _neutral_elem();
}

template <class T, class Alloc>
list<T, Alloc>::list(const Alloc& alloc)
    : root(nullptr),
      allocator(alloc),
      length(0),
      n_alloc(node_alloc_type(alloc)) {
  _neutral_elem();
}

template <class T, class Alloc>
list<T, Alloc>::list(size_t count, const value_type& value, const Alloc& alloc)
    : root(nullptr),
      allocator(alloc),
      length(0),
      n_alloc(node_alloc_type(alloc)) {
  _neutral_elem();
  for (size_t i = 0; i < count; ++i) push_back(value);
}

template <class T, class Alloc>
list<T, Alloc>::list(size_t count, const Alloc& alloc)
    : root(nullptr),
      allocator(alloc),
      length(0),
      n_alloc(node_alloc_type(alloc)) {
  _neutral_elem();
  Node* to_add;
  for (size_t i = 0; i < count;
       ++i) {  // не через _create_node, чтобы создавать ноду с T() без
               // копирования или других конструкторов
    to_add = n_alloc.allocate(1);  // (для Immovable)
    n_alloc.construct(to_add);
    to_add->value = _traits::allocate(allocator, 1);
    _traits::construct(allocator, to_add->value);
    _insert(root->prev, to_add);
  }
}

template <class T, class Alloc>
list<T, Alloc>::list(const list& other)
    : root(nullptr),
      length(0),
      allocator(
          _traits::select_on_container_copy_construction(other.allocator)),
      n_alloc(std::allocator_traits<node_alloc_type>::
                  select_on_container_copy_construction(other.n_alloc)) {
  _neutral_elem();
  Node* temp = other.root->next;
  while (temp != other.root) {
    push_back(temp->value[0]);
    temp = temp->next;
  }
}

template <class T, class Alloc>
list<T, Alloc>::list(list&& other)
    : allocator(std::move(other.allocator)), n_alloc(std::move(other.n_alloc)) {
  length = std::move(other.length);
  root = std::move(other.root);
  other.root = nullptr;
}

template <class T, class Alloc>
list<T, Alloc>::~list() {
  length = 0;
  if (root != nullptr) {
    Node* temp = root;
    while (root->next != root) {
      temp = root->next;
      _cut(temp);
      _del_node(n_alloc, allocator, temp);
    }
    _del_node(n_alloc, allocator, root);
    root = nullptr;
  }
}

template <class T, class Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(const list& other) {
  if (this == &other) {
    return *this;
  } else {
    clear();
    if (_traits::propagate_on_container_copy_assignment::value) {
      allocator = other.allocator;
      n_alloc = other.n_alloc;
    }
    allocator_type data_alloc_use =
        (allocator == other.allocator) ? allocator : other.allocator;

    node_alloc_type node_alloc_use =
        (n_alloc == other.n_alloc) ? n_alloc : other.n_alloc;

    Node* tmp = other.root->next;
    Node* add;
    while (tmp != other.root) {
      add = _copy_node(node_alloc_use, data_alloc_use, tmp);
      _insert(root->prev, add);
      tmp = tmp->next;
    }
    return *this;
  }
}

template <class T, class Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(list&& other) {
  clear();
  if (!_traits::propagate_on_container_move_assignment::value &&
      allocator != other.allocator) {
    Node* tmp = other.root->next;
    Node* add;
    while (tmp != other.root) {
      add = tmp;
      tmp = tmp->next;
      other._cut(add);
      add = _move_node(other.n_alloc, other.allocator, n_alloc, allocator, add);
      _insert(root->prev, add);
    }
    other._del_node(other.n_alloc, other.allocator, other.root);
  } else {
    root = std::move(other.root);
    length = std::move(other.length);

    if (_traits::propagate_on_container_move_assignment::value) {
      allocator = std::move(other.allocator);
      n_alloc = std ::move(other.n_alloc);
    }
  }
  other.root = nullptr;
  return *this;
}

template <class T, class Alloc>
typename list<T, Alloc>::allocator_type list<T, Alloc>::get_allocator() const {
  return allocator;
}

template <class T, class Alloc>
typename list<T, Alloc>::value_type& list<T, Alloc>::front() {
  return root->next->value[0];
}

template <class T, class Alloc>
const typename list<T, Alloc>::value_type& list<T, Alloc>::front() const {
  return root->next->value[0];
}

template <class T, class Alloc>
typename list<T, Alloc>::value_type& list<T, Alloc>::back() {
  return root->prev->value[0];
}

template <class T, class Alloc>
const typename list<T, Alloc>::value_type& list<T, Alloc>::back() const {
  return root->prev->value[0];
}

template <class T, class Alloc>
bool list<T, Alloc>::empty() const {
  return !bool(length);
}

template <class T, class Alloc>
size_t list<T, Alloc>::size() const {
  return length;
}

template <class T, class Alloc>
size_t list<T, Alloc>::max_size() const {
  return MAX_LENGTH;
}

template <class T, class Alloc>
void list<T, Alloc>::clear() {
  Node* temp;
  while (root->next != root) {
    temp = root->next;
    _cut(temp);
    _del_node(n_alloc, allocator, temp);
  }
}

template <class T, class Alloc>
template <class U>
void list<T, Alloc>::_gen_push_back(U&& value) {
  if (length < MAX_LENGTH) {
    Node* new_node = _create_node(n_alloc, allocator, std::forward<U>(value));
    _insert(root->prev, new_node);
  } else {
    throw std::overflow_error("overflow");
  }
}

template <class T, class Alloc>
void list<T, Alloc>::push_back(value_type&& value) {
  _gen_push_back(std::forward<value_type>(value));
}

template <class T, class Alloc>
void list<T, Alloc>::push_back(const value_type& value) {
  _gen_push_back(value);
}

template <class T, class Alloc>
void list<T, Alloc>::pop_back() {
  if (root->next != root) {
    Node* tmp = root->prev;
    _cut(tmp);
    _del_node(n_alloc, allocator, tmp);
  }
}

template <class T, class Alloc>
template <class U>
void list<T, Alloc>::_gen_push_front(U&& value) {
  if (length < MAX_LENGTH) {
    Node* new_node = _create_node(n_alloc, allocator, std::forward<U>(value));
    _insert(root, new_node);
  } else {
    throw std::overflow_error("overflow");
  }
}

template <class T, class Alloc>
void list<T, Alloc>::push_front(value_type&& value) {
  _gen_push_front(std::forward<value_type>(value));
}

template <class T, class Alloc>
void list<T, Alloc>::push_front(const value_type& value) {
  _gen_push_front(value);
}

template <class T, class Alloc>
void list<T, Alloc>::pop_front() {
  if (root->next != root) {
    Node* tmp = root->next;
    _cut(tmp);
    _del_node(n_alloc, allocator, tmp);
  }
}

template <class T, class Alloc>
void list<T, Alloc>::resize(size_t count) {
  if (count <= MAX_LENGTH) {
    size_t old_len = length;
    if (count < length) {
      for (size_t i = 0; i < (old_len - count); ++i) pop_back();
    } else {
      for (size_t i = 0; i < (count - old_len); ++i)
        push_back(std::move(value_type()));
    }
  } else {
    throw std::overflow_error("overflow");
  }
}

template <class T, class Alloc>
void list<T, Alloc>::swap(list& other) {
  if (this != &other) {
    std::swap(length, other.length);
    std::swap(allocator, other.allocator);
    std::swap(n_alloc, other.n_alloc);
    std::swap(root, other.root);
  }
}

template <class T, class Alloc>
void list<T, Alloc>::merge(list& other) {
  if (length + other.length < MAX_LENGTH) {
    Node* oth_tmp = other.root->next;
    Node* this_tmp = root->next;
    Node* help;
    while (oth_tmp != other.root) {
      if (this_tmp == root) {
        help = oth_tmp;
        oth_tmp = oth_tmp->next;
        other._cut(help);
        _insert(root->prev, help);
      } else if (this_tmp->value[0] < oth_tmp->value[0]) {
        this_tmp = this_tmp->next;
      } else {
        help = oth_tmp;
        oth_tmp = oth_tmp->next;
        other._cut(help);
        _insert(this_tmp->prev, help);
      }
    }
  } else {
    throw std::overflow_error("overflow");
  }
}

template <class T, class Alloc>
void list<T, Alloc>::remove(const value_type& value) {
  Node* tmp = root->next;
  Node* to_del;
  value_type val = value;
  while (tmp != root) {
    if (tmp->value[0] == val) {
      to_del = tmp;
      tmp = tmp->next;
      _cut(to_del);
      _del_node(n_alloc, allocator, to_del);
    } else {
      tmp = tmp->next;
    }
  }
}

template <class T, class Alloc>
void list<T, Alloc>::reverse() {
  Node* tmp = root->next;
  Node* to_move;
  while (tmp != root) {
    to_move = tmp;
    tmp = tmp->next;
    _cut(to_move);
    _insert(root, to_move);
  }
}

template <class T, class Alloc>
void list<T, Alloc>::unique() {
  Node* tmp = root->next->next;
  Node* to_del;
  while (tmp != root) {
    if (tmp->prev->value[0] == tmp->value[0]) {
      to_del = tmp;
      tmp = tmp->next;
      _cut(to_del);
      _del_node(n_alloc, allocator, to_del);
    } else {
      tmp = tmp->next;
    }
  }
}

template <class T, class Alloc>
void list<T, Alloc>::sort() {
  Node* i = root->next;
  Node* j;
  while (i != root) {
    j = i;
    i = i->next;
    while (j->prev != root && j->value[0] < j->prev->value[0]) {
      _swap(j->prev, j);
      j = j->prev;
    }
  }
}

template <class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::begin() {
  return iterator(root->next);
}

template <class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::end() {
  return iterator(root);
}

template <class T, class Alloc>
typename list<T, Alloc>::const_iterator list<T, Alloc>::cbegin() const {
  return const_iterator(root->next);
}

template <class T, class Alloc>
typename list<T, Alloc>::const_iterator list<T, Alloc>::cend() const {
  return const_iterator(root);
}

template <class T, class Alloc>
typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rbegin() {
  return reverse_iterator(root);
}

template <class T, class Alloc>
typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rend() {
  return reverse_iterator(root->next);
}

template <class T, class Alloc>
typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crbegin()
    const {
  return const_reverse_iterator(root);
}

template <class T, class Alloc>
typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crend() const {
  return const_reverse_iterator(root->next);
}

template <class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(
    const_iterator pos, const value_type& value) {
  Node* new_node = _create_node(n_alloc, allocator, value);
  _insert(pos.ptr->prev, new_node);
  return iterator(new_node);
}

template <class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(const_iterator pos,
                                                         value_type&& value) {
  Node* new_node =
      _create_node(n_alloc, allocator, std::forward<value_type>(value));
  _insert(pos.ptr->prev, new_node);
  return iterator(new_node);
}

template <class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(
    const_iterator pos, size_t count, const value_type& value) {
  Node* new_node;
  for (size_t i = 0; i < count; ++i) {
    new_node = _create_node(n_alloc, allocator, value);
    _insert(pos.ptr->prev, new_node);
    --pos;
  }
  return iterator(pos.ptr);
}

template <class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(const_iterator pos) {
  Node* to_del = pos.ptr;
  if (to_del != root) {
    ++pos;
    _cut(to_del);
    _del_node(n_alloc, allocator, to_del);
  }
  return iterator(pos->ptr);
}

template <class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(const_iterator first,
                                                        const_iterator last) {
  Node* to_del;
  Node* begin = first.ptr;
  Node* end = last.ptr;
  while (begin != end) {
    if (begin != root) {
      to_del = begin;
      begin = begin->next;
      _cut(to_del);
      _del_node(n_alloc, allocator, to_del);
    } else {
      begin = begin->next;
    }
  }
  return iterator(end);
}

template <class T, class Alloc>
template <class... Args>
typename list<T, Alloc>::iterator list<T, Alloc>::emplace(const_iterator pos,
                                                          Args&&... args) {
  value_type val(std::forward<Args>(args)...);
  Node* new_node = _create_node(n_alloc, allocator, std::move(val));
  _insert(pos.ptr->prev, new_node);
  return iterator(pos.ptr->prev);
}

template <class T, class Alloc>
template <class... Args>
void list<T, Alloc>::emplace_back(Args&&... args) {
  value_type val(std::forward<Args>(args)...);
  Node* new_node = _create_node(n_alloc, allocator, std::move(val));
  _insert(root->prev, new_node);
}

template <class T, class Alloc>
template <class... Args>
void list<T, Alloc>::emplace_front(Args&&... args) {
  value_type val(std::forward<Args>(args)...);
  Node* new_node = _create_node(n_alloc, allocator, std::move(val));
  _insert(root, new_node);
}

template <class T, class Alloc>
void list<T, Alloc>::splice(const_iterator pos, list& other) {
  Node* tmp = other.root->next;
  Node* to_add;
  while (tmp != other.root) {
    to_add = tmp;
    tmp = tmp->next;
    other._cut(to_add);
    _insert(pos.ptr->prev, to_add);
  }
}
