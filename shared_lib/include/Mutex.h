#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <optional>
#include <memory>
#include <mutex>
#include <iostream>

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

        // Prevent duplication of a handle, as to disallow 
        MutexHandle(const MutexHandle& h2) = delete;
        MutexHandle& operator=(const MutexHandle& h2) = delete;

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

        auto TryTake() -> std::unique_ptr<MutexHandle<T>> {
            if (_lock.try_lock()) {
                return std::make_unique<MutexHandle<T>>(&_lock, _ref);
            } else {
                return std::unique_ptr<MutexHandle<T>>(nullptr);
            }
        }

        auto Access(std::function<void(T&)> modify) -> void {
            auto h = Take();
            modify(*h);
        }
    };

    template<typename T>
    std::ostream& operator<<(std::ostream& os, Mutex<T>& mutex)
    {
        auto h = mutex.TryTake();
        if (h) {
            os << "Mutex(" << **h << ")";
        } else {
            os << "Mutex(<<locked>>)";
        }
        return os;
    }

    template<typename T, typename... Args>
    auto MakeMutex(Args&&... args) -> Mutex<T>
    {
        return Mutex<T>(std::make_unique<T>(std::forward<Args>(args)...));
    }

}

#endif // MUTEX_HPP
