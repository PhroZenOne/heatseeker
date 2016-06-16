// Ugly hack for dubble buffering, no idea if I have implemented this correct, but it seems to work.

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class ConcurrentBuffer {
public:

  T read() {
    while (buffer1 == null && buffer2 == null) {
      //No image available, lets wait
      cond_.wait(mlock);
    }
    if (firstBufferRead) {
      std::unique_lock<std::mutex> mlock(mutex1);
      auto ret = buffer1;
      buffer1 = null;
      firstBufferRead = false;
      return ret;
    } else {
      std::unique_lock<std::mutex> mlock(mutex2);
      auto ret = buffer2;
      buffer2 = null;
      firstBufferRead = true;
      return ret;
    }
  }


  void write(T&& item) {
    if (!firstBufferRead) {
      std::unique_lock<std::mutex> mlock(mutex1);
      buffer1 = std::move(item);
      mlock.unlock();
    } else {
      std::unique_lock<std::mutex> mlock(mutex2);
      buffer2 = std::move(item);
      mlock.unlock();
    }
    cond_.notify_one();
  }

private:
  std::atomic<bool> firstBufferRead(true);
  std::atomic<T> buffer1;
  std::atomic<T> buffer2;
  std::mutex mutex1;
  std::mutex mutex2;
  std::condition_variable cond;
};