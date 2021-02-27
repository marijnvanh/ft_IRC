#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <optional>
#include <memory>
#include <mutex>
#include <iostream>
#include <functional>

namespace IRC {

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

        MutexHandle(MutexHandle&& other) {
            _ref = other._ref;
            _lock = other._lock;
            other._ref = nullptr;
            other._lock = nullptr;
        }

        auto operator=(MutexHandle&& other) -> MutexHandle& {
            if (this != &other) {
                _ref = other._ref;
                _lock = other._lock;
                other._ref = nullptr;
                other._lock = nullptr;
            }
            return *this;
        }

        // Prevent duplication of a handle, as to disallow mutex copying
        MutexHandle(const MutexHandle& h2) = delete;
        MutexHandle& operator=(const MutexHandle& h2) = delete;

        ~MutexHandle() {
            _lock->unlock();
        }
    };

    template<typename T>
    class Mutex {
        std::unique_ptr<std::mutex> _lock;
        T* _ref;

    public:
        /*
         * This forces ownernship to be guaranteed.
         * _ref will always be owned by Mutex.
         */
        Mutex(std::unique_ptr<T> wrap) : 
            _lock(std::make_unique<std::mutex>()),
            _ref(wrap.release()) {}; 

        ~Mutex() {
            if (_lock) {
                // If we own a lock, we should try to lock it before we get destroyed
                // A handle might be currently owning a Handle into it, deleting would not be a good idea :)
                _lock->lock();
                _lock->unlock();
            }
            delete _ref;
        }

        Mutex(Mutex&& other) {
            other._lock->lock();
            _ref = other._ref;
            _lock = std::move(other._lock);
            other._ref = nullptr;
            _lock->unlock();
        }

        auto operator=(Mutex&& other) -> Mutex& {
            // Now for the tricky bit.
            if (this != &other) {
                auto old_lock = std::move(_lock);
                old_lock->lock();
                other._lock->lock();
                delete _ref;
                _ref = other._ref;
                _lock = std::move(other._lock);
                other._ref = nullptr;
                _lock->unlock();
                old_lock->unlock();
            }
            return *this;
        }

        auto Take() -> MutexHandle<T> {
            _lock->lock();
            // JUSTIFICATION: .get is valid here because we never delete locks without locking them first
            // which would block until a MutexHandle doesn't hold it.
            // Locks also are atomic in their access, so two threads accessing at same time is safe.
            return MutexHandle(_lock.get(), _ref); 
        }

        auto TryTake() -> std::unique_ptr<MutexHandle<T>> {
            if (_lock->try_lock()) {
                // SEE ABOVE
                return std::make_unique<MutexHandle<T>>(_lock.get(), _ref);
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
