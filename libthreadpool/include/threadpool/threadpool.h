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

typedef map<uint64_t, CTaskPtr> CTaskMap;
typedef deque<pair<CTaskPtr, uint64_t>> CTaskDeque;

class CThreadPool
{
public:
  CThreadPool(uint32_t numOfThreads);
  ~CThreadPool();

  uint64_t addTask(const CTaskPtr & task);

  set<uint64_t> runningTaskIdxs();
  set<uint64_t> waitingTaskIdxs();

  void cancelTask(uint64_t taskIdx);
  void cancelAllTasks();

  void suspend();
  void resume();

  bool suspended();

  void shutdown();

private:
  CTaskDeque _queue;
  mutex _queueMtx;
  condition_variable _queueCV;

  CTaskMap _runningTasks;
  mutex _runningTasksMtx;

  vector<thread> _threads;

  atomic_bool _cancelled{false};
  atomic_bool _suspended{false};
  uint64_t _lastTaskIdx = 0;

  void run();
  void cancelTask(const CTaskPtr & task);
};

#endif // THREADPOOL_H
