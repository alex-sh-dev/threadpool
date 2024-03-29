#ifndef TASKSHOLDER_H
#define TASKSHOLDER_H

#include "common.h"

class CTasksHolder
{
private:
  CTasksHolder() {}
  ~CTasksHolder() {}

public:
  static CTasksHolder & getInstance()
  {
    static CTasksHolder instance;
    return instance;
  }

  CTasksHolder(CTasksHolder const &) = delete;
  void operator=(CTasksHolder const &) = delete;

  void addTask(const CTaskPtr & task, uint64_t taskIdx)
  {
    lock_guard<mutex> lock(_mtx);
    _tasks.insert({taskIdx, task});
  }

  CTaskPtr task(uint64_t taskIdx)
  {
    lock_guard<mutex> lock(_mtx);
    CTaskMap::const_iterator it = _tasks.find(taskIdx);
    if (it != _tasks.cend()) {
      return it->second;
    }

    return nullptr;
  }

  void clearTasks()
  {
    lock_guard<mutex> lock(_mtx);
    _tasks.clear();
  }

  void addResult(const CTaskPtr & task, CTask::ResultType result)
  {
    if (result.empty()) {
      return;
    }

    lock_guard<mutex> lock(_mtx);
    for (CTaskMap::const_iterator it = _tasks.cbegin(); it != _tasks.cend(); ++it) {
      if (it->second.get() != task.get()) {
        continue;
      }

      _results.insert({it->first, result});
      break;
    }
  }

  CTaskResultPair getResult(uint64_t taskIdx)
  {
    unique_lock<mutex> lock(_mtx);
    CTaskResultMap::const_iterator it = _results.find(taskIdx);
    CTask::ResultType result;
    if (it != _results.cend()) {
      result = it->second;
    }
    lock.unlock();

    CTaskPtr t = task(taskIdx);
    if (t.get()) {
      return make_pair(t, result);
    }

    return make_pair(nullptr, 0);
  }

  const CTaskResultMap & getResults()
  {
    lock_guard<mutex> lock(_mtx);
    return _results;
  }

private:
  mutex _mtx;
  CTaskMap _tasks;
  CTaskResultMap _results;
};

#endif // TASKSHOLDER_H
