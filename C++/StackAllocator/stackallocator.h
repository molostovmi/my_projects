#include <iterator>
#include <memory>
#include <exception>

template<typename T, typename Alloc = std::allocator<T>>
class List {
private:
  struct BaseNode {
    BaseNode* prev;
    BaseNode* next;
  };

  struct Node : BaseNode {
    T data;
  };

  BaseNode end_;
  Node* start_;
  size_t size_;

  using NodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
  using AllocRebindTraits = typename std::allocator_traits<Alloc>::template rebind_traits<Node>;
  [[no_unique_address]] NodeAlloc alloc_;

  void clear();

  void swap(List<T, Alloc>& list);

public:
  template<bool is_const>
  class base_iterator {
    friend class List<T, Alloc>;
  private:
    std::conditional_t<is_const, const BaseNode*, BaseNode*> node_ = nullptr;

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::conditional_t<is_const, const T, T>;
    using difference_type = std::ptrdiff_t;
    using pointer = std::conditional_t<is_const, const T*, T*>;
    using reference = std::conditional_t<is_const, const T&, T&>;

    base_iterator() = default;

    explicit base_iterator(std::conditional_t<is_const, const BaseNode*, BaseNode*> node);

    ~base_iterator();

    operator typename List<T, Alloc>::base_iterator<true>() const;

    std::conditional_t<is_const, const T&, T&> operator*();

    std::conditional_t<is_const, const T*, T*> operator->() const;

    typename List<T, Alloc>::base_iterator<is_const>& operator++();

    typename List<T, Alloc>::base_iterator<is_const>& operator--();

    typename List<T, Alloc>::base_iterator<is_const> operator++(int);

    typename List<T, Alloc>::base_iterator<is_const> operator--(int);

    bool operator==(const typename List<T, Alloc>::base_iterator<is_const>& iter) const;

    bool operator!=(const typename List<T, Alloc>::base_iterator<is_const>& iter) const;
  };

  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;
  using reverse_iterator = std::reverse_iterator<base_iterator<false>>;
  using const_reverse_iterator = std::reverse_iterator<base_iterator<true>>;

  List();

  explicit List(const Alloc& alloc);

  explicit List(size_t count, const Alloc& alloc = Alloc());

  List(size_t count, const T& elem, const Alloc& alloc = Alloc());

  List(const List<T, Alloc>& list);

  List<T, Alloc>& operator=(const List<T, Alloc>& list);

  ~List();

  void push_back(const T& data);

  void push_front(const T& data);

  void pop_back();

  void pop_front();

  size_t size() const noexcept;

  Alloc get_allocator() const noexcept;

  void insert(iterator iter, const T& data);

  void insert(const_iterator const_iter, const T& data);

  void erase(iterator iter);

  void erase(const_iterator const_iter);

  iterator begin();

  const_iterator begin() const;

  iterator end();

  const_iterator end() const;

  const_iterator cbegin() const;

  const_iterator cend() const;

  reverse_iterator rbegin();

  const_reverse_iterator rbegin() const;

  reverse_iterator rend();

  const_reverse_iterator rend() const;

  const_reverse_iterator crbegin() const;

  const_reverse_iterator crend() const;
};

template<typename T, typename Alloc>
void List<T, Alloc>::clear() {
  while (size_ != 0) {
    pop_back();
  }
}

template<typename T, typename Alloc>
void List<T, Alloc>::swap(List<T, Alloc> &list) {
  std::swap(list.alloc_, alloc_);
  std::swap(list.start_, start_);
  std::swap(list.size_, size_);
}

template<typename T, typename Alloc>
List<T, Alloc>::List() : end_ {&end_, &end_}, start_(static_cast<Node*>(&end_)), size_(0), alloc_() {}

template<typename T, typename Alloc>
List<T, Alloc>::List(const Alloc &alloc) : end_ {&end_, &end_}, start_(static_cast<Node*>(&end_)),
                                           size_(0), alloc_(alloc) {}

template<typename T, typename Alloc>
List<T, Alloc>::List(size_t count, const Alloc& alloc) : List(alloc) {
  try {
    while (count != 0) {
      Node* new_node = AllocRebindTraits::allocate(alloc_, 1);

      new_node->next = &end_;
      new_node->prev = end_.prev;

      if (start_ == &end_) {
        start_ = new_node;
      }
      end_.prev->next = new_node;
      end_.prev = new_node;

      ++size_;
      --count;

      AllocRebindTraits::construct(alloc_, &new_node->data);
    }
  } catch (...) {
    Node* old_end = static_cast<Node*>(end_.prev);
    old_end->prev->next = &end_;
    end_.prev = old_end->prev;

    if (size_ == 2) {
      start_->next = &end_;
    }
    if (size_ == 1) {
      start_ = static_cast<Node*>(&end_);
    }

    AllocRebindTraits::deallocate(alloc_, old_end, 1);
    --size_;

    clear();
    throw;
  }
}

template<typename T, typename Alloc>
List<T, Alloc>::List(size_t count, const T& elem, const Alloc& alloc) : List(alloc) {
  while (count != 0) {
    push_back(elem);
    --count;
  }
}

template<typename T, typename Alloc>
List<T, Alloc>::List(const List<T, Alloc>& list)
        : end_ {&end_, &end_},
          start_(static_cast<Node*>(&end_)),
          size_(0),
          alloc_(AllocRebindTraits::select_on_container_copy_construction(list.alloc_)) {
  try {
    for (const auto& elem : list) {
      push_back(elem);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template<typename T, typename Alloc>
List<T, Alloc>& List<T, Alloc>::operator=(const List<T, Alloc>& list) {
  List new_list(list);
  clear();
  new_list.end_.prev->next = &end_;
  end_.prev = new_list.end_.prev;
  swap(new_list);

  if (AllocRebindTraits::propagate_on_container_copy_assignment::value) {
    alloc_ = list.alloc_;
  }

  new_list.end_ = {&new_list.end_, &new_list.end_};
  new_list.start_ = static_cast<Node*>(&end_);
  start_->prev = &end_;
  end_.next = start_;

  return *this;
}

template<typename T, typename Alloc>
List<T, Alloc>::~List() { clear(); }

template<typename T, typename Alloc>
void List<T, Alloc>::push_back(const T& data) {
  Node* new_node = AllocRebindTraits::allocate(alloc_, 1);
  new_node->next = &end_;
  new_node->prev = end_.prev;

  if (start_ == &end_) {
    start_ = new_node;
  }
  end_.prev->next = new_node;
  end_.prev = new_node;

  ++size_;
  try {
    AllocRebindTraits::construct(alloc_, &new_node->data, data);
  } catch (...) {
    Node* old_end = static_cast<Node*>(end_.prev);
    old_end->prev->next = &end_;
    end_.prev = old_end->prev;

    if (size_ == 2) {
      start_->next = &end_;
    }
    if (size_ == 1) {
      start_ = static_cast<Node*>(&end_);
    }

    AllocRebindTraits::deallocate(alloc_, old_end, 1);
    --size_;
    throw;
  }
}

template<typename T, typename Alloc>
void List<T, Alloc>::push_front(const T& data) {
  Node* new_node = AllocRebindTraits::allocate(alloc_, 1);

  new_node->next = start_;
  new_node->prev = &end_;

  if (start_ == &end_) {
    end_.prev = new_node;
  } else {
    start_->prev = new_node;
  }
  start_ = new_node;
  end_.next = new_node;

  ++size_;

  try {
    AllocRebindTraits::construct(alloc_, &new_node->data, data);
  } catch (...) {
    Node* old_start = start_;
    end_.next = old_start->next;
    old_start->next->prev = &end_;
    start_ = static_cast<Node*>(end_.next);

    AllocRebindTraits::deallocate(alloc_, old_start, 1);
    --size_;
    throw;
  }
}

template<typename T, typename Alloc>
void List<T, Alloc>::pop_back() {
  Node* old_end = static_cast<Node*>(end_.prev);
  old_end->prev->next = &end_;
  end_.prev = old_end->prev;

  if (size_ == 2) {
    start_->next = &end_;
  }
  if (size_ == 1) {
    start_ = static_cast<Node*>(&end_);
  }

  AllocRebindTraits::destroy(alloc_, &old_end->data);
  AllocRebindTraits::deallocate(alloc_, old_end, 1);
  --size_;
}

template<typename T, typename Alloc>
void List<T, Alloc>::pop_front() {
  Node* old_start = start_;
  end_.next = old_start->next;
  old_start->next->prev = &end_;
  start_ = static_cast<Node*>(end_.next);

  AllocRebindTraits::destroy(alloc_, &old_start->data);
  AllocRebindTraits::deallocate(alloc_, old_start, 1);
  --size_;
}

template<typename T, typename Alloc>
size_t List<T, Alloc>::size() const noexcept { return size_; }

template<typename T, typename Alloc>
Alloc List<T, Alloc>::get_allocator() const noexcept { return alloc_; }

template<typename T, typename Alloc>
void List<T, Alloc>::insert(iterator iter, const T& data) {
  if (iter == end()) {
    push_back(data);
  } else if (iter == begin()) {
    push_front(data);
  } else {
    Node* new_node = AllocRebindTraits::allocate(alloc_, 1);

    try {
      AllocRebindTraits::construct(alloc_, &new_node->data, data);
    } catch (...) {
      AllocRebindTraits::destroy(alloc_, &new_node->data);
      AllocRebindTraits::deallocate(alloc_, new_node, 1);
      throw;
    }

    new_node->prev = iter.node_->prev;
    new_node->next = iter.node_;
    iter.node_->prev->next = new_node;
    iter.node_->prev = new_node;
    ++size_;
  }
}

template<typename T, typename Alloc>
void List<T, Alloc>::insert(const_iterator const_iter, const T& data) {
  if (const_iter == cend()) {
    push_back(data);
  } else if (const_iter == cbegin()) {
    push_front(data);
  } else {
    Node* new_node = AllocRebindTraits::allocate(alloc_, 1);

    try {
      AllocRebindTraits::construct(alloc_, &new_node->data, data);
    } catch (...) {
      AllocRebindTraits::destroy(alloc_, &new_node->data);
      AllocRebindTraits::deallocate(alloc_, new_node, 1);
      throw;
    }

    new_node->prev = const_cast<BaseNode*>(const_iter.node_)->prev;
    new_node->next = const_cast<BaseNode*>(const_iter.node_);
    const_cast<BaseNode*>(const_iter.node_)->prev->next = new_node;
    const_cast<BaseNode*>(const_iter.node_)->prev = new_node;
    ++size_;
  }
}

template<typename T, typename Alloc>
void List<T, Alloc>::erase(iterator iter) {
  if (iter == begin()) {
    pop_front();
  } else {
    Node* old_node = static_cast<Node*>(iter.node_);
    old_node->prev->next = old_node->next;
    old_node->next->prev = old_node->prev;
    AllocRebindTraits::destroy(alloc_, &old_node->data);
    AllocRebindTraits::deallocate(alloc_, old_node, 1);
    --size_;
  }
}

template<typename T, typename Alloc>
void List<T, Alloc>::erase(const_iterator const_iter) {
  if (const_iter == cbegin()) {
    pop_front();
  } else {
    Node* old_node = static_cast<Node*>(const_cast<BaseNode*>(const_iter.node_));
    old_node->prev->next = old_node->next;
    old_node->next->prev = old_node->prev;
    AllocRebindTraits::destroy(alloc_, &old_node->data);
    AllocRebindTraits::deallocate(alloc_, old_node, 1);
    --size_;
  }
}

template<typename T, typename Alloc>
typename List<T, Alloc>::iterator List<T, Alloc>::begin() {
  return iterator(start_);
}

template<typename T, typename Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::begin() const {
  return const_iterator(start_);
}

template<typename T, typename Alloc>
typename List<T, Alloc>::iterator List<T, Alloc>::end() {
  return iterator(&end_);
}

template<typename T, typename Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::end() const {
  return const_iterator(&end_);
}

template<typename T, typename Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::cbegin() const {
  return const_iterator(start_);
}

template<typename T, typename Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::cend() const {
  return const_iterator(&end_);
}

template<typename T, typename Alloc>
typename List<T, Alloc>::reverse_iterator List<T, Alloc>::rbegin() {
  return reverse_iterator(end());
}

template<typename T, typename Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::rbegin() const {
  return const_reverse_iterator(cend());
}

template<typename T, typename Alloc>
typename List<T, Alloc>::reverse_iterator List<T, Alloc>::rend() {
  return reverse_iterator(begin());
}

template<typename T, typename Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::rend() const {
  return const_reverse_iterator(cbegin());
}

template<typename T, typename Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::crbegin() const {
  return const_reverse_iterator(cend());
}

template<typename T, typename Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::crend() const {
  return const_reverse_iterator(cbegin());
}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::base_iterator<is_const>::base_iterator(
        std::conditional_t<is_const, const BaseNode*, BaseNode*> node) : node_(node) {}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::base_iterator<is_const>::~base_iterator() {}

template<typename T, typename Alloc>
template<bool is_const>
List<T, Alloc>::base_iterator<is_const>::operator typename List<T, Alloc>::base_iterator<true>() const {
  return base_iterator<true>(node_);
}

template<typename T, typename Alloc>
template<bool is_const>
std::conditional_t<is_const, const T&, T&> List<T, Alloc>::base_iterator<is_const>::operator*() {
  return static_cast<std::conditional_t<is_const, const Node*, Node*>>(node_)->data;
}

template<typename T, typename Alloc>
template<bool is_const>
std::conditional_t<is_const, const T*, T*> List<T, Alloc>::base_iterator<is_const>::operator->() const {
  return &static_cast<std::conditional_t<is_const, const Node*, Node*>>(node_)->data;
}

template<typename T, typename Alloc>
template<bool is_const>
typename List<T, Alloc>::template base_iterator<is_const>& List<T, Alloc>::base_iterator<is_const>::operator++() {
  node_ = node_->next;
  return *this;
}

template<typename T, typename Alloc>
template<bool is_const>
typename List<T, Alloc>::template base_iterator<is_const>& List<T, Alloc>::base_iterator<is_const>::operator--() {
  node_ = node_->prev;
  return *this;
}

template<typename T, typename Alloc>
template<bool is_const>
typename List<T, Alloc>::template base_iterator<is_const> List<T, Alloc>::base_iterator<is_const>::operator++(int) {
  typename List<T, Alloc>::base_iterator<is_const> old = *this;
  ++*this;
  return old;
}

template<typename T, typename Alloc>
template<bool is_const>
typename List<T, Alloc>::template base_iterator<is_const> List<T, Alloc>::base_iterator<is_const>::operator--(int) {
  typename List<T, Alloc>::base_iterator<is_const> old = *this;
  --*this;
  return old;
}

template<typename T, typename Alloc>
template<bool is_const>
bool List<T, Alloc>::base_iterator<is_const>::operator==(
        const typename List<T, Alloc>::base_iterator<is_const>& iter) const {
  return node_ == iter.node_;
}

template<typename T, typename Alloc>
template<bool is_const>
bool List<T, Alloc>::base_iterator<is_const>::operator!=(
        const typename List<T, Alloc>::base_iterator<is_const>& iter) const {
  return node_ != iter.node_;
}

template<size_t N>
class StackStorage {
public:
  char data_[N];
  void* top_;
  size_t size_;

public:
  StackStorage();

  StackStorage(const StackStorage&) = delete;

  template<typename T>
  T* allocate(size_t count, size_t align = alignof(T));
};

template<size_t N>
StackStorage<N>::StackStorage() : data_(), top_{data_}, size_ {N} {}

template<size_t N>
template<typename T>
T* StackStorage<N>::allocate(size_t count, size_t align) {
  if (std::align(align, sizeof(T) * count, top_, size_)) {
    T* new_memory = reinterpret_cast<T*>(top_);
    top_ = reinterpret_cast<char*>(top_) + sizeof(T) * count;
    size_ -= sizeof(T) * count;
    return new_memory;
  }

  return nullptr;
}

template<typename T, size_t N>
class StackAllocator {
public:
  StackStorage<N>* storage_;

  using value_type = T;

  StackAllocator();

  StackAllocator(StackStorage<N>& storage);

  template<typename U>
  StackAllocator(const StackAllocator<U, N>& stack_allocator);

  StackAllocator<T, N>& operator=(const StackAllocator<T, N>& stack_allocator);

  ~StackAllocator();

  T* allocate(size_t count);

  void deallocate(T* ptr, size_t count);

  template<typename U>
  struct rebind {
    using other = StackAllocator<U, N>;
  };

  struct propagate_on_container_copy_assignment {
    static const bool value = true;
  };
};

template<typename T, size_t N>
StackAllocator<T, N>::StackAllocator() : storage_(nullptr) {}

template<typename T, size_t N>
StackAllocator<T, N>::StackAllocator(StackStorage<N>& storage) : storage_(&storage) {}

template<typename T, size_t N>
template<typename U>
StackAllocator<T, N>::StackAllocator(const StackAllocator<U, N>& stack_allocator)
        : storage_(stack_allocator.storage_) {}

template<typename T, size_t N>
StackAllocator<T, N>& StackAllocator<T, N>::operator=(const StackAllocator<T, N>& stack_allocator) {
  storage_ = stack_allocator.storage_;
  return *this;
}

template<typename T, size_t N>
StackAllocator<T, N>::~StackAllocator() {}

template<typename T, size_t N>
T* StackAllocator<T, N>::allocate(size_t count) {
  T* new_memory = storage_->template allocate<T>(count);
  if (new_memory == nullptr) {
    throw std::bad_alloc();
  }
  return new_memory;
}

template<typename T, size_t N>
void StackAllocator<T, N>::deallocate(T* ptr, size_t count) {
  ptr = nullptr;
  size_t x = count;
  x -= x;
}

template<typename T, size_t N, typename U, size_t C>
bool operator==(const StackAllocator<T, N>& first_allocator, const StackAllocator<U, C>& second_allocator) {
  return first_allocator.storage_ == second_allocator.storage_;
}

template<typename T, size_t N, typename U, size_t C>
bool operator!=(const StackAllocator<T, N>& first_allocator, const StackAllocator<U, C>& second_allocator) {
  return !(first_allocator == second_allocator);
}