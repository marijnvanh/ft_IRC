#include <cstring>
#include "CircularBuffer.h"

template <class T>
auto Shared::CircularBuffer<T>::Get() -> T
{
	std::lock_guard<std::mutex> lock(mutex_);

	if (this->IsEmpty())
	{
		return T();
	}

	auto val = this->buffer_[->tail_];
	this->full_ = false;
	this->tail_ = (this->tail_ + 1) % this->max_size_;

	return (val);
}

template <class T>
auto Shared::CircularBuffer<T>::Put(T item) -> void
{
	std::lock_guard<std::mutex> lock(mutex_);

	this->buffer_[this->head_] = item;

	if (this->full_)
	{
		this->tail_ = (this->tail_ + 1) % this->max_size_;
	}

	this->head_ = (this->head_ + 1) % this->max_size_;

	this->full_ = (this->head_ == this->tail_);
}

template <class T>
auto Shared::CircularBuffer<T>::Flush() -> void
{
	std::lock_guard<std::mutex> lock(mutex_);

	head_ = tail_;
	full_ = false;
}

template <class T>
bool Shared::CircularBuffer<T>::IsFull() const
{
	return (this->full_);
}

template <class T>
bool Shared::CircularBuffer<T>::IsEmpty() const
{
	return (this->full_ && this->head_ == this->tail_);
}

template <class T>
size_t Shared::CircularBuffer<class T>::Size() const
{
	size_t size = this->max_size_;

	if (this->full_ == false)
	{
		if (head_ >= tail_)
		{
			size = head_ - tail_;
		}
		else
		{
			size = max_size_ + head_ - tail_;
		}		
	}

	return (size);
}

template <class T>
size_t Shared::CircularBuffer<T>::Capacity() const
{
	return (this->max_size_);
}

