#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>
#include <set>

#include <vector>

#include "task.h"

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

    void suspend();
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

    void p_run();
    void p_cancelTask(CTask* task);
};

#endif // THREADPOOL_H
