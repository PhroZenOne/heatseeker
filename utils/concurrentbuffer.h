// Just a concurrent way to buffer data
#ifndef CONCURRENTBUFFER_H
#define CONCURRENTBUFFER_H

#include <thread>
#include <mutex>

template <typename T>
class ConcurrentBuffer {
public:

  T read() {
    std::lock_guard<std::mutex> lock(mutex);
    newItem = false;
    return front;
  }

  bool hasNext() {
    std::lock_guard<std::mutex> lock(mutex);
    return newItem;
  }


  void write(T item) {
    std::lock_guard<std::mutex> lock(mutex);
    front = item;
    newItem = true;
  }

private:
  bool newItem = false;
  T front;
  std::mutex mutex;
};

#endif // CONCURRENTBUFFER_H
