#include <iostream>
#include <type_traits>

template<typename T>
class WeakPtr;

template<typename T>
class EnableSharedFromThis;

/*
 * Shared ptr
 */

struct BaseControlBlock {
  size_t counter;
  size_t weak_counter;

  BaseControlBlock(size_t counter, size_t weak_counter) : counter(counter), weak_counter(weak_counter) {}

  virtual ~BaseControlBlock() = default;

  virtual void delete_block() {}

  virtual void dealloc() {}

  virtual void* get_ptr() { return nullptr; }
};

template<typename T>
class SharedPtr {
private:
  template<typename Alloc = std::allocator<T>, typename Deleter = std::default_delete<T>>
  struct ControlBlockRegular : BaseControlBlock {
    using BlockAlloc = typename std::allocator_traits<Alloc>::template
                                rebind_alloc<ControlBlockRegular<Alloc, Deleter>>;
    using AllocRebindTraits = std::allocator_traits<BlockAlloc>;
    [[no_unique_address]] BlockAlloc alloc;
    [[no_unique_address]] Deleter deleter;
    T* ptr;

    ControlBlockRegular(size_t counter, size_t weak_counter, T* ptr,
                        Alloc alloc, Deleter deleter) : BaseControlBlock(counter, weak_counter),
                                                        alloc(alloc),
                                                        deleter(deleter),
                                                        ptr(ptr) {}

    ~ControlBlockRegular() override = default;

    void delete_block() override {
      AllocRebindTraits::deallocate(alloc, this, 1);
    }

    void dealloc() override {
      deleter(ptr);
    }

    void* get_ptr() override { return ptr; }
  };

  template<typename U, typename Alloc = std::allocator<U>>
  struct ControlBlockMakeShared : BaseControlBlock {
    using BlockAlloc = typename std::allocator_traits<Alloc>::template
                                rebind_alloc<ControlBlockMakeShared<U, Alloc>>;
    using AllocRebindTraits = std::allocator_traits<BlockAlloc>;
    U value;
    [[no_unique_address]] BlockAlloc alloc;

    template<typename... Args>
    ControlBlockMakeShared(size_t counter, size_t weak_counter, Alloc alloc,
                           Args&&... args) : BaseControlBlock(counter, weak_counter),
                                             value(std::forward<Args>(args)...),
                                             alloc(alloc) {
    }

    ~ControlBlockMakeShared() override = default;

    void delete_block() override {
      AllocRebindTraits::deallocate(alloc, this, 1);
    }

    void dealloc() override {
      AllocRebindTraits::destroy(alloc, &value);
    }

    void* get_ptr() override { return &value; }
  };

  T* aliasing_ptr_ = nullptr;
  BaseControlBlock* counts_ = nullptr;

  template<typename U, typename... Args>
  friend SharedPtr<U> makeShared(Args&&... args);

  template<typename U, typename Alloc, typename... Args>
  friend SharedPtr<U> allocateShared(Alloc alloc, Args&&... args);

  template<typename U>
  friend class SharedPtr;

  template<typename U>
  friend class WeakPtr;

  template<typename Alloc>
  explicit SharedPtr(ControlBlockMakeShared<T, Alloc>* control_block);

  SharedPtr(const WeakPtr<T>& smart_ptr);

  void clear();

public:
  SharedPtr() = default;

  template<typename Deleter, typename Alloc>
  explicit SharedPtr(T* ptr, Deleter deleter, Alloc alloc);

  explicit SharedPtr(T* ptr);

  template<typename Deleter>
  SharedPtr(T* ptr, Deleter deleter);

  SharedPtr(const SharedPtr& smart_ptr, T* ptr = nullptr);

  template<typename U, std::enable_if_t<std::is_base_of_v<T, U>, bool> = true >
  SharedPtr(const SharedPtr<U>& smart_ptr) : counts_(smart_ptr.counts_) {
    ++counts_->counter;
  }

  template<typename U, std::enable_if_t<std::disjunction_v<std::is_base_of<T, U>, std::is_same<T, U>>, bool> = true >
  SharedPtr(SharedPtr<U>&& smart_ptr) : counts_(smart_ptr.counts_) {
    smart_ptr.counts_ = nullptr;
  }

  SharedPtr& operator=(const SharedPtr& smart_ptr);

  template<typename U, std::enable_if_t<std::is_base_of_v<T, U>, bool> = true >
  SharedPtr& operator=(const SharedPtr<U>& smart_ptr) {
    clear();

    counts_ = smart_ptr.counts_;
    if (counts_) {
      ++counts_->counter;
    }

    return *this;
  }

  template<typename U, std::enable_if_t<std::disjunction_v<std::is_base_of<T, U>, std::is_same<T, U>>, bool> = true >
  SharedPtr& operator=(SharedPtr<U>&& smart_ptr) {
    clear();

    counts_ = smart_ptr.counts_;
    smart_ptr.counts_ = nullptr;

    return *this;
  }

  ~SharedPtr();

  T& operator*() const;

  T* operator->() const;

  T* get() const;

  size_t use_count() const;

  void reset(T* new_ptr = nullptr);

  void swap(SharedPtr& smart_ptr);
};

template<typename T>
template<typename Alloc>
SharedPtr<T>::SharedPtr(ControlBlockMakeShared<T, Alloc>* control_block) : counts_(control_block) {}

template<typename T>
SharedPtr<T>::SharedPtr(const WeakPtr<T>& smart_ptr) : counts_(smart_ptr.counts_) {
  if (counts_) {
    ++counts_->counter;
  }
}

template<typename T>
void SharedPtr<T>::clear() {
  if (counts_) {
    if (--counts_->counter == 0) {
      if (counts_->weak_counter == 0) {
        counts_->dealloc();
        counts_->delete_block();
      } else {
        counts_->dealloc();
      }
    }
  }
}

template<typename T>
template<typename Deleter, typename Alloc>
SharedPtr<T>::SharedPtr(T* ptr, Deleter deleter, Alloc alloc) {
  using BlockAlloc = typename std::allocator_traits<Alloc>::template
                              rebind_alloc<ControlBlockRegular<Alloc, Deleter>>;
  using AllocRebindTraits = std::allocator_traits<BlockAlloc>;
  BlockAlloc my_alloc = alloc;

  ControlBlockRegular<Alloc, Deleter>* new_counts = AllocRebindTraits::allocate(my_alloc, 1);
  new(new_counts) ControlBlockRegular<Alloc, Deleter>(1, 0, ptr, my_alloc, deleter);
  counts_ = new_counts;
}

template<typename T>
SharedPtr<T>::SharedPtr(T* ptr) {
  if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
    ptr->ptr_ = *this;
  } else {
    using BlockAlloc = typename std::allocator_traits<std::allocator<T>>::template
                                rebind_alloc<ControlBlockRegular<std::allocator<T>>>;
    using AllocRebindTraits = std::allocator_traits<BlockAlloc>;

    BlockAlloc my_alloc;
    ControlBlockRegular<std::allocator<T>>* new_counts = AllocRebindTraits::allocate(my_alloc, 1);
    new(new_counts) ControlBlockRegular<std::allocator<T>>(1, 0, ptr, my_alloc, std::default_delete<T>());
    counts_ = new_counts;
  }
}

template<typename T>
template<typename Deleter>
SharedPtr<T>::SharedPtr(T* ptr, Deleter deleter) : SharedPtr(ptr, deleter, std::allocator<T>())  {}

template<typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& smart_ptr, T* ptr) : counts_(smart_ptr.counts_) {
  if (counts_) {
    ++counts_->counter;
  }
  aliasing_ptr_ = ptr;
}

template<typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& smart_ptr) {
  if (this == &smart_ptr) {
    return *this;
  }

  clear();
  counts_ = smart_ptr.counts_;

  if (counts_) {
    ++counts_->counter;
  }

  return *this;
}

template<typename T>
SharedPtr<T>::~SharedPtr() { clear(); }

template<typename T>
T& SharedPtr<T>::operator*() const { return *get(); }

template<typename T>
T* SharedPtr<T>::operator->() const { return get(); }

template<typename T>
T* SharedPtr<T>::get() const {
  if (aliasing_ptr_ != nullptr) {
    return aliasing_ptr_;
  }

  if (!counts_) {
    return nullptr;
  }

  return reinterpret_cast<T*>(counts_->get_ptr());
}

template<typename T>
size_t SharedPtr<T>::use_count() const {
  if (counts_) {
    return counts_->counter;
  }

  return 0;
}

template<typename T>
void SharedPtr<T>::reset(T* new_ptr) {
  clear();

  if (new_ptr != nullptr) {
    using BlockAlloc = typename std::allocator_traits<std::allocator<T>>::template
    rebind_alloc<ControlBlockRegular<std::allocator<T>>>;
    using AllocRebindTraits = std::allocator_traits<BlockAlloc>;

    BlockAlloc my_alloc;
    ControlBlockRegular<std::allocator<T>>* new_counts = AllocRebindTraits::allocate(my_alloc, 1);
    new(new_counts) ControlBlockRegular<std::allocator<T>>(1, 0, new_ptr, my_alloc, std::default_delete<T>());
    counts_ = new_counts;
  } else {
    counts_ = nullptr;
  }
}

template<typename T>
void SharedPtr<T>::swap(SharedPtr<T>& smart_ptr) {
  std::swap(smart_ptr.counts_, counts_);
}

/*
 * Weak ptr
 */

template<typename T>
class WeakPtr {
private:
  template<typename U>
  friend class SharedPtr;

  template<typename U>
  friend class WeakPtr;

  BaseControlBlock* counts_ = nullptr;
  T* aliasing_ptr_ = nullptr;

  void clear();

public:
  WeakPtr() = default;

  WeakPtr(const SharedPtr<T>& smart_ptr);

  template<typename U, std::enable_if_t<std::is_base_of_v<T, U>, bool> = true >
  WeakPtr(const SharedPtr<U>& smart_ptr) : counts_(smart_ptr.counts_) {
    if (counts_) {
      ++counts_->weak_counter;
    }
  }

  WeakPtr(const WeakPtr& smart_ptr);

  template<typename U, std::enable_if_t<std::is_base_of_v<T, U>, bool> = true >
  WeakPtr(const WeakPtr<U>& smart_ptr) : counts_(smart_ptr.counts_) {
    if (counts_) {
      ++counts_->weak_counter;
    }
  }

  WeakPtr(WeakPtr&& smart_ptr);

  WeakPtr& operator=(const WeakPtr& smart_ptr);

  WeakPtr& operator=(WeakPtr&& smart_ptr);

  ~WeakPtr();

  bool expired() const;

  SharedPtr<T> lock() const;

  size_t use_count() const;
};

template<typename T>
void WeakPtr<T>::clear() {
  if (counts_) {
    if (--counts_->weak_counter == 0) {
      if (counts_->counter == 0) {
        counts_->delete_block();
      }
    }
  }
}

template<typename T>
WeakPtr<T>::WeakPtr(const SharedPtr<T>& smart_ptr) : counts_(smart_ptr.counts_) {
  if (counts_) {
    ++counts_->weak_counter;
  }
}

template<typename T>
WeakPtr<T>::WeakPtr(const WeakPtr<T>& smart_ptr) : counts_(smart_ptr.counts_) {
  if (counts_) {
    ++counts_->weak_counter;
  }
}

template<typename T>
WeakPtr<T>::WeakPtr(WeakPtr<T>&& smart_ptr) : counts_(smart_ptr.counts_) {
  smart_ptr.counts_ = nullptr;
}

template<typename T>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr<T>& smart_ptr) {
  if (this == &smart_ptr) {
    return *this;
  }
  clear();

  counts_ = smart_ptr.counts_;

  if (counts_) {
    ++counts_->weak_counter;
  }

  return *this;
}

template<typename T>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<T>&& smart_ptr) {
  clear();

  counts_ = smart_ptr.counts_;
  smart_ptr.counts_ = nullptr;

  return *this;
}

template<typename T>
WeakPtr<T>::~WeakPtr() { clear(); }

template<typename T>
bool WeakPtr<T>::expired() const {
  if (!counts_) {
    return true;
  }

  return counts_->counter == 0;
}

template<typename T>
SharedPtr<T> WeakPtr<T>::lock() const {
  return *this;
}

template<typename T>
size_t WeakPtr<T>::use_count() const {
  if (counts_) {
    return counts_->counter;
  }

  return 0;
}

/*
 * Some functions for ptrs
 */

template<typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(Alloc alloc, Args&&... args) {
  using BlockAlloc = typename std::allocator_traits<Alloc>::template
                              rebind_alloc<typename SharedPtr<T>::template ControlBlockMakeShared<T, Alloc>>;
  using AllocRebindTraits = typename std::allocator_traits<Alloc>::template
                                     rebind_traits<typename SharedPtr<T>::template ControlBlockMakeShared<T, Alloc>>;
  BlockAlloc my_alloc = alloc;
  auto control_block = AllocRebindTraits::allocate(my_alloc, 1);
  AllocRebindTraits::construct(my_alloc, control_block, 1, 0, my_alloc, std::forward<Args>(args)...);

  return SharedPtr<T>(control_block);
}

template<typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
  return allocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}

/*
 * Enable shared from this
 */

template<typename T>
class EnableSharedFromThis {
private:
  WeakPtr<T> ptr_;

  template<typename U>
  friend class SharedPtr;

protected:
  EnableSharedFromThis() = default;

public:
  SharedPtr<T> shared_form_this() {
    if (ptr_.expired()) {
      return SharedPtr<T>();
    }

    return ptr_.lock();
  }
};