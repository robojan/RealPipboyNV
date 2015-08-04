#pragma once

#include <queue>
#include <chrono>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <set>

class Task
{
public:
	Task(std::function<void(void *)> func, int time, std::chrono::steady_clock::time_point now,
		void *user, bool periodic = false);

	int getId() const;
	void call() const;
	bool isPeriodic() const;
	int getDeltaTime() const;

	std::chrono::steady_clock::time_point getResumeTime() const;

	void updateTime(std::chrono::steady_clock::time_point now);

	inline bool operator<(const Task &rhs) const {
		return m_continueTime < rhs.m_continueTime;
	}
	inline bool operator>(const Task &rhs) const { return rhs < *this; }
	inline bool operator<=(const Task &rhs) const { return !(*this > rhs); }
	inline bool operator>=(const Task &rhs) const { return !(*this < rhs); }
	/*static inline bool operator<(const Task &lhs, const Task &rhs) { lhs.operator<(rhs); }
	static inline bool operator>(const Task &lhs, const Task &rhs) { lhs.operator>(rhs); }
	static inline bool operator<=(const Task &lhs, const Task &rhs) { lhs.operator<=(rhs); }
	static inline bool operator>=(const Task &lhs, const Task &rhs) { lhs.operator>=(rhs); }
*/
private:
	static int m_idCounter;

	int m_id;
	int m_deltaTime;
	bool m_periodic;
	void *m_user;
	std::function<void(void *)> m_func;
	std::chrono::steady_clock::time_point m_continueTime;
};

class Scheduler
{
public:
	Scheduler();
	~Scheduler();

	void start();
	void stop();

	int addPeriodic(std::function<void(void *)> func, int period, void *user = NULL);
	int addSingleShot(std::function<void(void *)> func, int time, void *user = NULL);
	void removeTask(int id);

private:
	static void startThread(Scheduler *thisptr);
	void threadFunction();

	std::thread m_thread;
	std::atomic<bool> m_running;

	std::mutex m_tasksMutex;
	std::priority_queue<Task, std::vector<Task>, std::greater<Task>> m_tasks;
	std::set<int> m_removeIds;
};

