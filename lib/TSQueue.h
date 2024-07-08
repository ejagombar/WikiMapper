#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T> class TSQueue {
  public:
    TSQueue() = default;
    ~TSQueue() = default;

    void push(T item);
    T pop();
    bool empty();

  private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};

template <typename T> void TSQueue<T>::push(T item) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_queue.push(item);

    m_cond.notify_one(); // Notify one thread that is waiting
}

template <typename T> T TSQueue<T>::pop() {
    std::unique_lock<std::mutex> lock(m_mutex);

    m_cond.wait(lock, [this]() { return !m_queue.empty(); }); // wait until queue is not empty

    T item = m_queue.front();
    m_queue.pop();

    return item;
}

template <typename T> bool TSQueue<T>::empty() { return m_queue.empty(); }
