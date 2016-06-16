// Just a concurrent way to buffer data

#include <thread>
#include <mutex>

template <typename T>
class ConcurrentBuffer {
public:

  T read() {
    std::unique_lock<std::mutex> mlock(mutex);
    newItem = false;
    return front;
  }

  bool hasNext() {
    std::unique_lock<std::mutex> mlock(mutex);
    return newItem;
  }


  void write(T item) {
    std::unique_lock<std::mutex> mlock(mutex);
    front = item;
    newItem = true;
  }

private:
  bool newItem = true;
  T front;
  std::mutex mutex;
};