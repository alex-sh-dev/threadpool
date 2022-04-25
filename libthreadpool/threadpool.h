#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "task.h"

#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>
#include <set>

#include <vector>

using namespace std;

class CThreadPool {
public:
    CThreadPool(uint32_t numOfThreads);
    ~CThreadPool();

    uint64_t addTask(CTask* task);

    set<uint64_t> runningTaskIdxs();
    set<uint64_t> waitingTaskIdxs();

    void cancelTask(uint64_t taskIdx);
    void cancelAllTasks();

    /**
     * @brief Suspend the execution of the thread pool. New tasks will be added to the queue, but will not be executed until resume() is called
     */
    void suspend();
    /**
     * @brief Cancell suspension (see suspend())
     */
    void resume();

    bool suspended();

    void shutdown();

private:
    deque<pair<CTask*, uint64_t>> _queue;
    mutex _queueMtx;
    condition_variable _queueCV;

    map<uint64_t, CTask*> _runningTasks;
    mutex _runningTasksMtx;

    vector<thread> _threads;

    atomic_bool _cancelled{ false };
    atomic_bool _suspended{ false };
    uint64_t _lastTaskIdx = 0;

    void run();
    void cancelTask(CTask* task);
};

#endif // THREADPOOL_H
