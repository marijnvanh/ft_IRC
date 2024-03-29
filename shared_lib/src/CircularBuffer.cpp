#include <cstring>
#include "CircularBuffer.h"

template <typename T>
auto IRC::CircularBuffer<T>::Get() -> T
{
	std::lock_guard<std::mutex> lock(mutex_);

	if (this->IsEmpty())
	{
		return T();
	}

	auto val = this->buffer_[this->tail_];
	this->full_ = false;
	this->tail_ = (this->tail_ + 1) % this->max_size_;

	return (val);
}

template <typename T>
auto IRC::CircularBuffer<T>::Put(T item) -> void
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

template <typename T>
auto IRC::CircularBuffer<T>::Flush() -> void
{
	std::lock_guard<std::mutex> lock(mutex_);

	head_ = tail_;
	full_ = false;
}

template <typename T>
bool IRC::CircularBuffer<T>::IsFull() const
{
	return (this->full_);
}

template <typename T>
bool IRC::CircularBuffer<T>::IsEmpty() const
{
	return (this->full_ && this->head_ == this->tail_);
}

template <typename T>
size_t IRC::CircularBuffer<T>::Size() const
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

template <typename T>
size_t IRC::CircularBuffer<T>::Capacity() const
{
	return (this->max_size_);
}

