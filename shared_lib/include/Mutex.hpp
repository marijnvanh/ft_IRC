#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <memory>
#include <mutex>

namespace ft_irc {

    template<typename T>
    class MutexHandle {
        std::function<void()> _release;
        T* _ref;

    public:
        MutexHandle(std::function<void()> release, T* ref) {
            _release = release;
            _ref = ref;
        }

        T& operator*() {
            return *_ref;
        }

        T* operator->() {
            return _ref;
        }

        ~MutexHandle() {
            _release();
        }
    };

    template<typename T>
    class Mutex {
        std::mutex _lock;
        T* _ref;

    public:
        Mutex(T* wrap) : _ref(wrap) {};

        ~Mutex() {
            _lock.lock();
            _lock.unlock();
        }

        auto Take() -> MutexHandle<T> {
            _lock.lock();
            return MutexHandle([&]() {
                _lock.unlock();
            }, _ref);
        }

        auto TryTake() -> std::optional<MutexHandle<T>> {
            if (_lock.try_lock()) {
                return std::optional{MutexHandle([&]() {
                    _lock.unlock();
                }, _ref)};
            } else {
                return std::nullopt;
            }
        }

        auto IsLocked() -> bool {
            auto was_locked = !_lock.try_lock();
            if (!was_locked)
                _lock.unlock();
            return was_locked;
        }

    };

    template<typename T, typename... Args>
    auto MakeMutex(Args&&... args) -> Mutex<T>
    {
        return Mutex<T>(new T(std::forward<Args>(args)...));
    }


}

#endif // MUTEX_HPP
