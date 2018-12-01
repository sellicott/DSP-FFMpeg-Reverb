#ifndef MPF_CIRCULAR_BUFFER
#define MPF_CIRCULAR_BUFFER

#include <memory>
#include <iterator>
#include <algorithm>

//iterator forward declaration
template <typename T> class CircularBufferIter;
template <typename T> class ConstCircularBufferIter;

template<typename T>
class CircularBuffer {

public:
  //stuff for making this an iterable class
  typedef CircularBufferIter<T>      iterator;
  typedef ConstCircularBufferIter<T> const_iterator;

  typedef ptrdiff_t difference_type;
  typedef size_t    size_type;
  typedef T         value_type;
  typedef T*        pointer;
  typedef const T*  const_pointer;
  typedef T&        reference;
  typedef const T&  const_reference;

  CircularBuffer() = delete;
  CircularBuffer(size_t capacity)
  {
    front_idx = 0;
    buff_size = 0;
    buff_capacity = capacity;
    buff = std::make_unique<T[]>(capacity);
    std::fill_n(buff.get(), buff_capacity, 0);
  }
  
  size_t size() const noexcept
  {
    return buff_size;
  }

  bool empty() const noexcept
  {
    return buff_size == 0;
  }

  size_t capacity() const noexcept
  {
    return buff_capacity;
  }

  size_t free_space() const noexcept
  {
    return buff_capacity - buff_size;
  }

  iterator begin() noexcept
  {
    return iterator(*this, front_idx);
  }

  iterator end() noexcept
  {
    return iterator(*this, front_idx + buff_size);
  }

  const_iterator begin() const noexcept
  {
    return const_iterator(*this, front_idx);
  }

  const_iterator end() const noexcept
  {
    return const_iterator(*this, front_idx + buff_size);
  }

  size_t contig_space() const noexcept
  {
    //get the offsets of the front and rear
    auto rear_idx = (front_idx + buff_size) %  buff_capacity;
    if(!buff_size){
      return buff_capacity;
    }
    if(rear_idx > front_idx) {
      return rear_idx - front_idx;
    }
    else {
      return front_idx - rear_idx;
    }
  }

  reference operator[](size_type idx)
  {
    return buff[idx];
  }

  const_reference operator[](size_type idx) const
  {
    return buff[idx];
  }

  reference front()
  {
    return buff[front_idx];
  }

  const_reference front() const
  {
    return buff[front_idx];
  }

  reference back()
  {
    return buff[(front_idx + buff_size) % buff_capacity];
  }

  const_reference back() const
  {
    return buff[(front_idx + buff_size) % buff_capacity];
  }

  void push_back(const value_type& item)
  {
    back() = item;
    if( free_space() ) {
      ++buff_size;
    }
  }

  template <typename InputIterator>
  void push_back(InputIterator first, size_type n)
  {
    std::copy_n(first, n, this->end());
    if(buff_size + n > buff_capacity) buff_size = buff_capacity;
  }

  void pop_front() noexcept
  {
    pop_front(1);
  }

  void pop_front(size_type n)
  {
    if( n >= buff_size ) n = buff_size;

    front_idx = (front_idx + n) % buff_capacity;
    buff_size -= n;
  }

private:
  size_t front_idx;
  size_t buff_size;
  size_t buff_capacity;
  std::unique_ptr<T[]> buff;
};


template <typename T> class CircularBufferIter : public std::iterator<std::output_iterator_tag, T>{

private:
  CircularBuffer<T>& circ_buff;
  size_t offset;
  size_t capacity;
  using iterator = CircularBufferIter<T>;

public:
  CircularBufferIter(CircularBuffer<T>& cb, size_t iter_offset)
    : circ_buff(cb) 
  {
    capacity = circ_buff.capacity();
    offset = iter_offset % capacity;
  }

  bool operator == (const iterator& rhs) const noexcept
  {
    return (offset == rhs.offset && 
            &circ_buff == &rhs.circ_buff) 
              ? true : false;
  }

  bool operator != (const iterator& rhs) const noexcept
  {
    return ! (*this == rhs);
  }

  iterator& operator++ () noexcept
  {
    offset = (offset + 1) % capacity;
    return *this;
  }

  iterator operator++ (int) noexcept
  {
    auto copy = *this;
    offset = (offset + 1) % capacity;
    return copy;
  }

  T& operator* () noexcept
  {
    return circ_buff[offset];
  }
};

template <typename T> class ConstCircularBufferIter {

private:
  CircularBuffer<T>& circ_buff;
  size_t offset;
  size_t capacity;
  using iterator = ConstCircularBufferIter<T>;

public:
  ConstCircularBufferIter(CircularBuffer<T>& cb, size_t iter_offset)
    : circ_buff(cb) 
  {
    capacity = circ_buff.capacity();
    offset = iter_offset % capacity;
  }

  bool operator == (const iterator& rhs) const noexcept
  {
    return (offset == rhs.offset && 
            circ_buff == rhs.circ_buff) 
              ? true : false;
  }

  bool operator != (const iterator& rhs) const noexcept
  {
    return ! (*this == rhs);
  }

  iterator& operator++ () noexcept
  {
    offset = (offset + 1) % capacity;
    return *this;
  }

  iterator operator++ (int) noexcept
  {
    auto copy = *this;
    offset = (offset + 1) % capacity;
    return copy;
  }

  T& operator* () noexcept
  {
    return circ_buff[offset];
  }
};

#endif //MPF_CIRCULAR_BUFFER