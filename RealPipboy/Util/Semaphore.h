#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

// With thanks to Dave
// http://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads

#include <mutex>
#include <condition_variable>

class Semaphore
{
public:
	using native_handle_type = std::condition_variable::native_handle_type;

	explicit Semaphore(size_t n = 0);
	Semaphore(const Semaphore&) = delete;
	Semaphore& operator=(const Semaphore&) = delete;

	void notify();
	void wait();
	template<class Rep, class Period>
	bool wait_for(const std::chrono::duration<Rep, Period>& d);
	template<class Clock, class Duration>
	bool wait_until(const std::chrono::time_point<Clock, Duration>& t);

	native_handle_type native_handle();

private:
	size_t                  count;
	std::mutex              mutex;
	std::condition_variable cv;
};

inline Semaphore::Semaphore(size_t n)
	: count{ n }
{}

inline void Semaphore::notify()
{
	std::lock_guard<std::mutex> lock{ mutex };
	++count;
	cv.notify_one();
}

inline void Semaphore::wait()
{
	std::unique_lock<std::mutex> lock{ mutex };
	cv.wait(lock, [&]{ return count > 0; });
	--count;
}

template<class Rep, class Period>
bool Semaphore::wait_for(const std::chrono::duration<Rep, Period>& d)
{
	std::unique_lock<std::mutex> lock{ mutex };
	auto finished = cv.wait_for(lock, d, [&]{ return count > 0; });

	if (finished)
		--count;

	return finished;
}

template<class Clock, class Duration>
bool Semaphore::wait_until(const std::chrono::time_point<Clock, Duration>& t)
{
	std::unique_lock<std::mutex> lock{ mutex };
	auto finished = cv.wait_until(lock, t, [&]{ return count > 0; });

	if (finished)
		--count;

	return finished;
}

inline Semaphore::native_handle_type Semaphore::native_handle()
{
	return cv.native_handle();
}

#endif