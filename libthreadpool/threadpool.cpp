#include "threadpool.h"

const int defaultNumOfThreads = 2;

CThreadPool::CThreadPool(uint32_t numOfThreads)
{
  numOfThreads = numOfThreads > 0
    ? numOfThreads
    : max(thread::hardware_concurrency(), static_cast<unsigned int>(defaultNumOfThreads));
  _threads.reserve(numOfThreads);
  for (uint32_t i = 0; i < numOfThreads; ++i) {
    _threads.push_back(thread(&CThreadPool::run, this));
  }
}

CThreadPool::~CThreadPool()
{
  if (!_cancelled) {
    shutdown();
  }
}

//public methods

uint64_t CThreadPool::addTask(CTaskPtr task)
{
  if (!task.get()) {
    return 0;
  }

  _lastTaskIdx += 1;
  uint64_t taskIdx = _lastTaskIdx;

  unique_lock<mutex> queueLock(_queueMtx);
  _queue.push_back(make_pair(task, taskIdx));
  queueLock.unlock();

  _queueCV.notify_one();

  return taskIdx;
}

set<uint64_t> CThreadPool::runningTaskIdxs()
{
  set<uint64_t> idxs;
  lock_guard<mutex> runningTasksLock(_runningTasksMtx);
  for (CTaskMap::const_iterator it = _runningTasks.cbegin(); it != _runningTasks.cend(); ++it) {
    idxs.insert(it->first);
  }

  return idxs;
}

set<uint64_t> CThreadPool::waitingTaskIdxs()
{
  set<uint64_t> idxs;
  lock_guard<mutex> queueLock(_queueMtx);
  for (CTaskDeque::const_iterator it = _queue.cbegin(); it != _queue.cend(); ++it) {
    idxs.insert((*it).second);
  }

  return idxs;
}

void CThreadPool::cancelTask(uint64_t taskIdx)
{
  unique_lock<mutex> queueLock(_queueMtx);

  for (CTaskDeque::const_iterator it = _queue.cbegin(); it != _queue.cend(); ) {
    if ((*it).second == taskIdx) {
      cancelTask((*it).first);
      _queue.erase(it);
      break;
    } else {
      ++it;
    }
  }
  queueLock.unlock();

  unique_lock<mutex> runningTasksLock(_runningTasksMtx);
  CTaskMap::const_iterator it = _runningTasks.find(taskIdx);
  if (it != _runningTasks.cend()) {
    cancelTask(it->second);
  }
  runningTasksLock.unlock();
}

void CThreadPool::cancelAllTasks()
{
  unique_lock<mutex> queueLock(_queueMtx);
  while (!_queue.empty()) {
    cancelTask(_queue.front().first);
    _queue.pop_front();
  }
  queueLock.unlock();

  unique_lock<mutex> runningTasksLock(_runningTasksMtx);
  for (CTaskMap::const_iterator it = _runningTasks.cbegin(); it != _runningTasks.cend(); ++it) {
    cancelTask(it->second);
  }
  runningTasksLock.unlock();
}

void CThreadPool::suspend()
{
  _suspended = true;
}

void CThreadPool::resume()
{
  if (!_suspended) {
    return;
  }

  _suspended = false;
  _queueCV.notify_all();
}

bool CThreadPool::suspended()
{
  return _suspended;
}

void CThreadPool::shutdown()
{
  _cancelled = true;
  _queueCV.notify_all();

  cancelAllTasks();
  for (uint32_t i = 0; i < _threads.size(); ++i) {
    _threads[i].join();
  }
  _threads.clear();
}

//private methods

void CThreadPool::run()
{
  while (!_cancelled) {
    unique_lock<mutex> lock(_queueMtx);
    _queueCV.wait(lock, [this]() -> bool {
        return !_queue.empty() || _cancelled || !_suspended;
      });

    if (!_queue.empty() && !_suspended) {
      CTaskPtr task = _queue.front().first;
      uint64_t idx = _queue.front().second;
      _queue.pop_front();
      lock.unlock();

      unique_lock<mutex> lock1(_runningTasksMtx);
      _runningTasks.insert({idx, task});
      lock1.unlock();

      if (!task->cancelled()) {
        task->run(task->resultReceiver());
      }

      unique_lock<mutex> lock2(_runningTasksMtx);
      _runningTasks.erase(idx);
      lock2.unlock();
    }
  }
}

inline void CThreadPool::cancelTask(CTaskPtr task)
{
  if (!task.get()) {
    return;
  }

  if (!task->cancelled()) {
    task->cancel();
  }
}
