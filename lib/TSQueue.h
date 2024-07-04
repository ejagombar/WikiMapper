#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T> class TSQueue {
  public:
    void push(T item);
    T pop();

  private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};
