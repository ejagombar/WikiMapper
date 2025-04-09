#include <mutex>

// Source: https://www.reddit.com/r/cpp/comments/p132c7/a_c_locking_wrapper/

template <class T, class M = std::mutex, template <class...> class WL = std::unique_lock,
          template <class...> class RL = std::unique_lock>
struct mutex_guarded {
    auto read(auto f) const {
        auto l = lock();
        return f(t);
    }
    auto write(auto f) {
        auto l = lock();
        return f(t);
    }
    mutex_guarded() = default;
    explicit mutex_guarded(T in) : t(std::move(in)) {}

  private:
    mutable M m;
    T t;
    auto lock() const { return RL<M>(m); }
    auto lock() { return WL<M>(m); }
};
