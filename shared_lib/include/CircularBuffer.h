#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__

#include <mutex>
#include <memory>

namespace IRC::TCP
{
	template <typename T>
	class CircularBuffer
	{
	public:
		explicit CircularBuffer(size_t size) :
			buffer_(std::unique_ptr<T[]>(new T[size])),
			max_(size)
		{}
		~CircularBuffer();

		auto Get() -> T;
		auto Put(T item) -> void;

		auto Flush() -> void;

		bool IsFull() const;
		bool IsEmpty() const;

		size_t Size() const;
		size_t Capacity() const;

	private:
		std::mutex mutex_;

		std::unique_ptr<T[]> buffer_;

		size_t max_;
		size_t head_ = 0;
		size_t tail_ = 0;

		const size_t max_size_;

		bool full_ = 0;

	};

} // namespace IRC::TCP

#endif // __CIRCULAR_BUFFER_H__