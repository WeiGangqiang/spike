#ifndef INC_E5B1D1A7FDE34FDB992441E8D3F98FCA_H
#define INC_E5B1D1A7FDE34FDB992441E8D3F98FCA_H

#include <atomic>
#include <new>

namespace  tcc{

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
using std::hardware_destructive_interference_size;
#else
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

template <typename T>
struct MpmcQueue {
  explicit MpmcQueue(uint32_t sizeLog2)
      : size_(1 << sizeLog2), mask_(size_ - 1), items_(new Item[size_]), head_(0), tail_(0) {
    for (uint32_t i = 0; i < size_; i++) {
      items_[i].seq_.store(i, std::memory_order_relaxed);
    }
  }

  ~MpmcQueue() {
    if (!std::is_trivially_destructible<T>::value) {
      size_t head = head_;
      size_t tail = tail_;
      while (head != tail) {
        items_[(head++) & mask_].data_.~T();
      }
    }
    delete[] items_;
  }

  MpmcQueue(MpmcQueue const &) = delete;
  void operator=(MpmcQueue const &) = delete;

  template <class... Args>
  bool Push(Args &&... itemArgs) {
    Item *item = nullptr;
    auto pos = tail_.load(std::memory_order_relaxed);

    while (true) {
      item = &items_[pos & mask_];
      size_t seq = item->seq_.load(std::memory_order_acquire);

      intptr_t diff = (intptr_t)seq - (intptr_t)pos;
      if (diff == 0) {
        if (tail_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) break;
      } else if (diff < 0)
        return false;
      else
        pos = tail_.load(std::memory_order_relaxed);
    }

    new (&(item->data_)) T(std::forward<Args>(itemArgs)...);
    item->seq_.store(pos + 1, std::memory_order_release);
    return true;
  }

  bool Pop(T &itemData) {
    Item *item = nullptr;
    auto pos = head_.load(std::memory_order_relaxed);

    while (true) {
      item = &items_[pos & mask_];
      size_t seq = item->seq_.load(std::memory_order_acquire);

      intptr_t diff = (intptr_t)seq - (intptr_t)(pos + 1);
      if (diff == 0) {
        if (head_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) break;
      }

      else if (diff < 0)
        return false;
      else
        pos = head_.load(std::memory_order_relaxed);
    }

    itemData = item->data_;
    item->seq_.store(pos + mask_ + 1, std::memory_order_release);
    return true;
  }

  bool IsEmpty() const {
    return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
  }

  bool IsFull() const {
    return GetSize() >= GetCapacity();
  }

  size_t GetSize() const {
    return tail_.load(std::memory_order_acquire) - head_.load(std::memory_order_acquire);
  }

  size_t GetCapacity() const {
    return size_;
  }

private:
  using QueueIndex = std::atomic<uint32_t>;

  struct Item {
    QueueIndex seq_;
    T data_;
  };

  char pad_infer_before_[hardware_destructive_interference_size];
  uint32_t const size_;
  uint32_t const mask_;
  Item *const items_;

  // used by consumer
  alignas(hardware_destructive_interference_size) QueueIndex head_;
  // used by producer
  alignas(hardware_destructive_interference_size) QueueIndex tail_;

  char pad_infer_after_[hardware_destructive_interference_size - sizeof(QueueIndex)];
};

}  // namespace gert

#endif // INC_E5B1D1A7FDE34FDB992441E8D3F98FCA_H