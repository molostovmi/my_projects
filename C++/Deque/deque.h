#include <vector>
#include <exception>
#include <type_traits>

template<typename T>
class Deque {
private:
  static const size_t k_inside_arr_size = 32;

  std::vector<T*> out_array_;
  size_t size_ = 0;
  std::pair<size_t, size_t> start_ = {0, 0};
  std::pair<size_t, size_t> end_ = {0, 0};

  void extend(size_t new_size, size_t start = 0);

  void clear(size_t cur_size);

public:
  template<bool is_const>
  class base_iterator {
  private:
    using iter_type = std::conditional_t<is_const, typename std::vector<T*>::const_iterator,
            typename std::vector<T*>::iterator>;
    iter_type inside_iter_;
    size_t index_ = 0;

  public:
    typedef std::random_access_iterator_tag  iterator_category;
    typedef std::conditional_t<is_const, const T, T> value_type;
    typedef long long difference_type;
    typedef std::conditional_t<is_const, const T*, T*> pointer;
    typedef std::conditional_t<is_const, const T&, T&> reference;

    base_iterator() = default;

    base_iterator(const iter_type& iter, size_t index);

    base_iterator(const base_iterator& iter) = default;

    typename Deque<T>::base_iterator<is_const>& operator=(const base_iterator& iter) = default;

    ~base_iterator() = default;

    operator typename Deque<T>::base_iterator<true>() const;

    std::conditional_t<is_const, const T&, T&> operator*();

    std::conditional_t<is_const, const T*, T*> operator->() const;

    base_iterator& operator+=(long long new_index);

    base_iterator operator+(long long new_index) const;

    base_iterator& operator-=(long long new_index);

    base_iterator operator-(long long new_index) const;

    long long operator-(const base_iterator& iter);

    base_iterator& operator++();

    base_iterator& operator--();

    base_iterator operator++(int);

    base_iterator operator--(int);

    bool operator==(const base_iterator& iter) const;

    auto operator<=>(const typename Deque<T>::base_iterator<is_const>& iter) const;
  };

  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Deque() = default;

  explicit Deque(size_t new_size);

  Deque(size_t new_size, const T& data);

  Deque(const Deque<T>& deque);

  Deque<T>& operator=(const Deque<T>& deque);

  ~Deque();

  void push_back(const T& new_data);

  void push_front(const T& new_data);

  void pop_back();

  void pop_front();

  const T& operator[](size_t ind) const;

  T& operator[](size_t ind);

  const T& at(size_t ind) const;

  T& at(size_t ind);

  size_t size() const;

  typename Deque<T>::iterator begin();

  typename Deque<T>::iterator end();

  typename Deque<T>::const_iterator begin() const;

  typename Deque<T>::const_iterator end() const;

  typename Deque<T>::const_iterator cbegin() const;

  typename Deque<T>::const_iterator cend() const;

  typename Deque<T>::reverse_iterator rbegin();

  typename Deque<T>::reverse_iterator rend();

  typename Deque<T>::const_reverse_iterator rbegin() const;

  typename Deque<T>::const_reverse_iterator rend() const;

  typename Deque<T>::const_reverse_iterator crbegin() const;

  typename Deque<T>::const_reverse_iterator crend() const;

  void insert(typename Deque<T>::iterator iter, const T& data);

  void erase(typename Deque<T>::iterator iter);
};

template<typename T>
void Deque<T>::extend(size_t new_size, size_t start) {
  if ((start == 0 && new_size <= out_array_.size()) ||
      (start + out_array_.size() > new_size)) {
    return;
  }

  std::vector<T*> new_array(new_size, nullptr);
  for (size_t i = 0; i < new_size; ++i) {
    if (i < start || i > end_.first + start || new_size == 1) {
      new_array[i] = reinterpret_cast<T*>(new char[sizeof(T) * k_inside_arr_size]);
    } else {
      new_array[i] = out_array_[i - start];
    }
  }

  std::swap(out_array_, new_array);

  start_.first += start;
  end_.first += start;
}

template<typename T>
void Deque<T>::clear(size_t cur_size) {
  while (cur_size != 0) {
    pop_front();
    --cur_size;
  }

  for (size_t i = 0; i < out_array_.size(); ++i) {
    delete[] reinterpret_cast<char*>(out_array_[i]);
  }

  size_ = 0;
  start_ = end_ = {0, 0};
  out_array_.clear();
}

template<typename T>
Deque<T>::Deque(size_t new_size)
        : out_array_(new_size / k_inside_arr_size + 1, nullptr),
          size_(new_size), start_ {0, 0},
          end_ {new_size / k_inside_arr_size, new_size % k_inside_arr_size} {
  for (size_t i = 0; i < out_array_.size(); ++i) {
    out_array_[i] = reinterpret_cast<T*>(new char[sizeof(T) * k_inside_arr_size]);
  }

  size_t cur_size = 0;
  try {
    for (size_t i = 0; i <= end_.first; ++i) {
      for (size_t j = 0; j < k_inside_arr_size; ++j) {
        if (i == end_.first && j >= end_.second) {
          break;
        }
        new(out_array_[i] + j) T();
        ++cur_size;
      }
    }
  } catch(...) {
    clear(cur_size);
    throw;
  }
}

template<typename T>
Deque<T>::Deque(size_t new_size, const T& data)
        : out_array_(new_size / k_inside_arr_size + 1, nullptr),
          size_(new_size), start_ {0, 0},
          end_ {new_size / k_inside_arr_size, new_size % k_inside_arr_size} {
  for (size_t i = 0; i < out_array_.size(); ++i) {
    out_array_[i] = reinterpret_cast<T*>(new char[sizeof(T) * k_inside_arr_size]);
  }

  size_t cur_size = 0;
  try {
    for (size_t i = 0; i <= end_.first; ++i) {
      for (size_t j = 0; j < k_inside_arr_size; ++j) {
        if (i == end_.first && j >= end_.second) {
          break;
        }
        new(out_array_[i] + j) T(data);
        ++cur_size;
      }
    }
  } catch(...) {
    clear(cur_size);
    throw;
  }
}

template<typename T>
Deque<T>::Deque(const Deque<T>& deque) : out_array_(deque.out_array_.size()),
                                         size_(deque.size_),
                                         start_(deque.start_), end_(deque.end_) {
  for (size_t i = 0; i < out_array_.size(); ++i) {
    out_array_[i] = reinterpret_cast<T*>(new char[sizeof(T) * k_inside_arr_size]);
  }

  size_t cur_size = 0;
  try {
    for (size_t i = start_.first; i <= end_.first; ++i) {
      for (size_t j = 0; j < k_inside_arr_size; ++j) {
        if ((i == start_.first && j < start_.second) ||
            (i == end_.first && j >= end_.second)) {
          continue;
        }
        new(out_array_[i] + j) T(deque.out_array_[i][j]);
        ++cur_size;
      }
    }
  } catch (...) {
    clear(cur_size);
    throw;
  }
}

template<typename T>
Deque<T>& Deque<T>::operator=(const Deque<T>& deque) {
  if (this == &deque) {
    return *this;
  }

  Deque new_deque(deque);
  std::swap(size_, new_deque.size_);
  std::swap(start_, new_deque.start_);
  std::swap(end_, new_deque.end_);
  std::swap(out_array_, new_deque.out_array_);

  return *this;
}

template<typename T>
Deque<T>::~Deque() {
  clear(size_);
}

template<typename T>
void Deque<T>::push_back(const T& new_data) {
  if (out_array_.size() == 0 || (end_.first + 1 == out_array_.size() && end_.second + 1 == k_inside_arr_size) ||
      (end_.first == out_array_.size() && end_.second == 0)) {
    extend(out_array_.size() == 0 ? 1 : out_array_.size() * 2);
  }

  new(out_array_[end_.first] + end_.second) T(new_data);

  end_.first = (end_.second == k_inside_arr_size - 1 ? end_.first + 1 : end_.first);
  end_.second = (end_.second + 1) % k_inside_arr_size;
  ++size_;
}

template<typename T>
void Deque<T>::push_front(const T& new_data) {
  bool is_first_push = false;

  if (out_array_.size() == 0) {
    extend(1);
    end_ = {0, 1};
    is_first_push = true;
  } else {
    if (start_.first == 0 && start_.second == 0) {
      extend(2 * out_array_.size(), out_array_.size());
    }

    start_.first = (start_.second == 0 ? start_.first - 1 : start_.first);
    start_.second = (start_.second == 0 ? k_inside_arr_size - 1 : start_.second - 1);
  }

  try {
    new(out_array_[start_.first] + start_.second) T(new_data);
  } catch(...) {
    if (is_first_push) {
      end_ = {0, 0};
    } else {
      start_.first = (start_.second == k_inside_arr_size - 1 ? start_.first + 1 : start_.first);
      start_.second = (start_.second == k_inside_arr_size - 1 ? 0 : start_.second + 1);
    }

    throw;
  }
  ++size_;
}

template<typename T>
void Deque<T>::pop_back() {
  end_.first = (end_.second == 0 ? end_.first - 1 : end_.first);
  end_.second = (end_.second == 0 ? k_inside_arr_size - 1 : end_.second - 1);
  (end() - 1)->~T();
  --size_;
}

template<typename T>
void Deque<T>::pop_front() {
  begin()->~T();
  start_.first = (start_.second == k_inside_arr_size - 1 ? start_.first + 1 : start_.first);
  start_.second = (start_.second + 1) % k_inside_arr_size;
  --size_;
}

template<typename T>
const T& Deque<T>::operator[](size_t ind) const {
  return *(begin() + ind);
}

template<typename T>
T& Deque<T>::operator[](size_t ind) {
  return *(begin() + ind);
}

template<typename T>
const T& Deque<T>::at(size_t ind) const {
  if (ind >= size_) {
    throw std::out_of_range("deque");
  }

  return operator[](ind);
}

template<typename T>
T& Deque<T>::at(size_t ind) {
  if (ind >= size_) {
    throw std::out_of_range("deque");
  }

  return operator[](ind);
}

template<typename T>
size_t Deque<T>::size() const { return size_; }

template<typename T>
typename Deque<T>::iterator Deque<T>::begin() {
  return iterator(out_array_.begin() + start_.first, start_.second);
}

template<typename T>
typename Deque<T>::iterator Deque<T>::end() {
  return iterator(out_array_.begin() + end_.first, end_.second);
}

template<typename T>
typename Deque<T>::const_iterator Deque<T>::begin() const {
  return cbegin();
}

template<typename T>
typename Deque<T>::const_iterator Deque<T>::end() const {
  return cend();
}

template<typename T>
typename Deque<T>::const_iterator Deque<T>::cbegin() const {
  return const_iterator(out_array_.cbegin() + start_.first, start_.second);
}

template<typename T>
typename Deque<T>::const_iterator Deque<T>::cend() const {
  return const_iterator(out_array_.cbegin() + end_.first, end_.second);
}

template<typename T>
typename Deque<T>::reverse_iterator Deque<T>::rbegin() {
  return reverse_iterator(end());
}

template<typename T>
typename Deque<T>::reverse_iterator Deque<T>::rend() {
  return reverse_iterator(begin());
}

template<typename T>
typename Deque<T>::const_reverse_iterator Deque<T>::rbegin() const {
  return crbegin();
}

template<typename T>
typename Deque<T>::const_reverse_iterator Deque<T>::rend() const {
  return crend();
}

template<typename T>
typename Deque<T>::const_reverse_iterator Deque<T>::crbegin() const {
  return const_reverse_iterator(cend());
}

template<typename T>
typename Deque<T>::const_reverse_iterator Deque<T>::crend() const {
  return const_reverse_iterator(cbegin());
}

template<typename T>
void Deque<T>::insert(typename Deque<T>::iterator iter, const T& data) {
  if (iter == end()) {
    push_back(data);
    return;
  }

  T old_value = *iter;
  *iter = data;

  while (iter != end() - 1) {
    ++iter;
    std::swap(old_value, *iter);
  }

  push_back(old_value);
}

template<typename T>
void Deque<T>::erase(typename Deque<T>::iterator iter) {
  while (iter < end() - 1) {
    std::swap(*iter, *(iter + 1));
    ++iter;
  }

  pop_back();
}

template<typename T>
template<bool is_const>
Deque<T>::base_iterator<is_const>::base_iterator(const iter_type& iter, size_t index)
        : inside_iter_(iter), index_(index) {}

template<typename T>
template<bool is_const>
Deque<T>::base_iterator<is_const>::operator typename Deque<T>::base_iterator<true>() const {
  return base_iterator<true>(inside_iter_, index_);
}

template<typename T>
template<bool is_const>
std::conditional_t<is_const, const T&, T&> Deque<T>::base_iterator<is_const>::operator*() {
  return *(*inside_iter_ + index_);
}

template<typename T>
template<bool is_const>
std::conditional_t<is_const, const T*, T*> Deque<T>::base_iterator<is_const>::operator->() const {
  return *inside_iter_ + index_;
}

template<typename T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const>& Deque<T>::base_iterator<is_const>::operator+=(long long new_index) {
  if (new_index >= 0) {
    inside_iter_ += (index_ + static_cast<size_t>(new_index)) / k_inside_arr_size;
    index_ = (index_ + static_cast<size_t>(new_index)) % k_inside_arr_size;
  } else {
    if (static_cast<long long>(index_) + new_index >= 0) {
      index_ = static_cast<size_t>(static_cast<long long>(index_) + new_index);
    } else {
      long long in_delta_index = (static_cast<long long>(index_) + new_index) / static_cast<long long>(k_inside_arr_size);
      inside_iter_ += in_delta_index;
      long long delta_index = (static_cast<long long>(index_) + new_index) % static_cast<long long>(k_inside_arr_size);

      if (delta_index == 0) {
        index_ = 0;
      } else {
        --inside_iter_;
        index_ = static_cast<size_t>(static_cast<long long>(k_inside_arr_size) + delta_index);
      }
    }
  }

  return *this;
}

template<typename T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const> Deque<T>::base_iterator<is_const>::operator+(long long new_index) const {
  typename Deque<T>::base_iterator<is_const> new_iter = *this;
  return new_iter += new_index;
}

template<typename T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const>& Deque<T>::base_iterator<is_const>::operator-=(long long new_index) {
  return *this += -new_index;
}

template<typename T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const> Deque<T>::base_iterator<is_const>::operator-(long long new_index) const {
  typename Deque<T>::base_iterator<is_const> new_iter = *this;
  return new_iter -= new_index;
}

template<typename T>
template<bool is_const>
long long Deque<T>::template base_iterator<is_const>::operator-(const base_iterator& iter) {
  return (inside_iter_ - iter.inside_iter_) * k_inside_arr_size +
         (static_cast<long>(index_) - static_cast<long>(iter.index_));
}

template<typename T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const>& Deque<T>::base_iterator<is_const>::operator++() {
  return *this += 1;
}

template<typename T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const>& Deque<T>::base_iterator<is_const>::operator--() {
  return *this -= 1;
}

template<typename T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const> Deque<T>::base_iterator<is_const>::operator++(int) {
  typename Deque<T>::base_iterator<is_const> old = *this;
  ++*this;
  return old;
}

template<typename T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const> Deque<T>::base_iterator<is_const>::operator--(int) {
  typename Deque<T>::base_iterator<is_const> old = *this;
  --*this;
  return old;
}

template<typename T>
template<bool is_const>
bool Deque<T>::base_iterator<is_const>::operator==(const base_iterator& iter) const {
  return inside_iter_ == iter.inside_iter_ && index_ == iter.index_;
}

template<typename T>
template<bool is_const>
auto Deque<T>::base_iterator<is_const>::operator<=>(const typename Deque<T>::base_iterator<is_const>& iter) const {
  if (inside_iter_ == iter.inside_iter_) {
    return index_ <=> iter.index_;;
  }
  if (inside_iter_ < iter.inside_iter_) {
    return std::strong_ordering::less;
  }

  return std::strong_ordering::greater;
}