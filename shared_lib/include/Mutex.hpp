#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <memory>
#include <mutex>

namespace ft_irc {

    template<typename T>
    class MutexHandle {
        std::mutex* _lock;
        T* _ref;

    public:
        MutexHandle(std::mutex* lock, T* ref) : _lock(lock), _ref(ref) {}

        T& operator*() {
            return *_ref;
        }

        T* operator->() {
            return _ref;
        }

        ~MutexHandle() {
            _lock->unlock();
        }
    };

    template<typename T>
    class Mutex {
        std::mutex _lock;
        T* _ref;

    public:
        /*
         * This forces ownernship to be guaranteed.
         * _ref will always be owned by Mutex.
         */
        Mutex(std::unique_ptr<T> wrap) : _ref(wrap.release()) {}; 

        ~Mutex() {
            _lock.lock();
            _lock.unlock();
            delete _ref;
        }

        auto Take() -> MutexHandle<T> {
            _lock.lock();
            return MutexHandle(&_lock, _ref);
        }

        auto TryTake() -> std::optional<MutexHandle<T>> {
            if (_lock.try_lock()) {
                return std::make_optional<MutexHandle>(&_lock, _ref);
            } else {
                return std::nullopt;
            }
        }

        /*
         * Checks if the thread is currently locked.
         * This is relatively expensive because it performs a lock
         * by itself.
         */
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
        return Mutex<T>(std::make_unique<T>(std::forward<Args>(args)...));
    }


}

#endif // MUTEX_HPP
