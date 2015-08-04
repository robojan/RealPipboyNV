#include "Scheduler.h"

#include <windows.h>
#include <cassert>

using namespace std;
using namespace chrono;

Scheduler::Scheduler() : 
m_running(false)
{
}

Scheduler::~Scheduler()
{
	stop();
}

void Scheduler::start()
{
	if (m_running)
		return;
	m_running = true;
	m_thread = std::thread(Scheduler::startThread, this);
}

void Scheduler::stop()
{
	if (!m_running)
		return;
	m_running = false;
	if(m_thread.joinable())
		m_thread.join();
}

int Scheduler::addPeriodic(std::function<void(void *)> func, int period, void *user)
{
	assert(period > 0);
	Task &task = Task(func, period, steady_clock::now(), user, true);

	m_tasksMutex.lock();
	m_tasks.push(task);
	m_tasksMutex.unlock();

	return task.getId();
}

int Scheduler::addSingleShot(std::function<void(void *)> func, int time, void *user)
{
	assert(time > 0);
	Task &task = Task(func, time, steady_clock::now(), user);

	m_tasksMutex.lock();
	m_tasks.push(task);
	m_tasksMutex.unlock();

	return task.getId();
}

void Scheduler::removeTask(int id)
{
	m_tasksMutex.lock();
	m_removeIds.insert(id);
	m_tasksMutex.unlock();
}

void Scheduler::startThread(Scheduler *thisptr)
{
	thisptr->threadFunction();
}

void Scheduler::threadFunction()
{
	while (m_running) {
		int sleepTime = 100;
		m_tasksMutex.lock();
		steady_clock::time_point now = steady_clock::now();
		while(m_tasks.size() > 0) {
			steady_clock::time_point resumeTime = m_tasks.top().getResumeTime();
			if (resumeTime > now) {
				sleepTime = (int32_t)duration_cast<milliseconds>(resumeTime - now).count();
				break;
			}
			Task task = m_tasks.top();
			m_tasks.pop();
			if (m_removeIds.find(task.getId()) == m_removeIds.end()) {
				task.call();
				if (task.isPeriodic()) {
					task.updateTime(now);
					m_tasks.push(task);
				}
			}
			else {
				m_removeIds.erase(task.getId());
			}
		}		
		m_tasksMutex.unlock();
		Sleep(sleepTime);
	}
}

Task::Task(std::function<void(void *)> func, int time, steady_clock::time_point now, 
	void *user, bool periodic /*= false*/) :
	m_id(m_idCounter++), m_continueTime(now + milliseconds(time)), m_func(func), 
	m_periodic(periodic), m_deltaTime(time), m_user(user)
{

}

int Task::getId() const
{
	return m_id;
}

void Task::call() const
{
	m_func(m_user);
}

bool Task::isPeriodic() const
{
	return m_periodic;
}

int Task::getDeltaTime() const
{
	return m_deltaTime;
}

steady_clock::time_point Task::getResumeTime() const
{
	return m_continueTime;
}

void Task::updateTime(std::chrono::steady_clock::time_point now)
{
	m_continueTime = now + milliseconds(m_deltaTime);
}

int Task::m_idCounter = 0;
