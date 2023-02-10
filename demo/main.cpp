#include <iostream>
#include <string>
#include <chrono>
#include <memory>

#include "threadpool.h"

using namespace std;

typedef map<uint64_t, CTask::ResultType> CTaskResultMap;
typedef pair<CTaskPtr, CTask::ResultType> CTaskResultPair;
typedef shared_ptr<CThreadPool> CThreadPoolPtr;

//classes

class CFactTask : public CTask
{
public:
  CFactTask(uint32_t n)
  : CTask(), _n(n) {}

  void run(ResultReceiver rr) override
  {
    uint64_t result = 1;
    uint32_t i = 1;
    while (i <= _n && !cancelled()) {
      result *= i;
      ++i;
    }
    result = cancelled() ? 0 : result;

    if (rr) {
      rr(shared_from_this(), result);
    }
  }

private:
  uint32_t _n;
};

class CSleepTask : public CTask
{
public:
  CSleepTask(int secs)
  : CTask(), _secs(secs) {}

  void run(ResultReceiver rr) override
  {
    _secs = max(1, _secs);
    int i = 0;
    while (i <= _secs && !cancelled()) {
      this_thread::sleep_for(chrono::seconds(1));
      ++i;
    }
  }

private:
  int _secs;
};

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

  void addTask(CTaskPtr task, uint64_t taskIdx)
  {
    lock_guard<mutex> lock(_mtx);
    _tasks.insert({taskIdx, task});
  }

  CTaskPtr task(uint64_t taskIdx)
  {
    lock_guard<mutex> lock(_mtx);
    CTaskMap::iterator it = _tasks.find(taskIdx);
    if (it != _tasks.end()) {
      return it->second;
    }

    return nullptr;
  }

  void clearTasks()
  {
    lock_guard<mutex> lock(_mtx);
    _tasks.clear();
  }

  void addResult(CTaskPtr task, CTask::ResultType result)
  {
    if (result.empty()) {
      return;
    }

    lock_guard<mutex> lock(_mtx);
    for (CTaskMap::iterator it = _tasks.begin(); it != _tasks.end(); ++it) {
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
    CTaskResultMap::iterator it = _results.find(taskIdx);
    CTask::ResultType result;
    if (it != _results.end()) {
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

//helper methods

int isNumber(const string & str)
{
  for (char const & c : str) {
    if (isdigit(c) == 0) {
      return false;
    }
  }

  return true;
}

int input()
{
  string str;
  cin >> str;
  return isNumber(str) ? atoi(str.c_str()) : 0;
}

int pressEnterToContinue()
{
  cout << "Press Enter to continue" << endl;
  cin.ignore();
  cin.get();
}

template<typename Base, typename T>
inline bool instanceof(const T * ptr)
{
  return dynamic_cast<const Base *>(ptr) != nullptr;
}

//callbacks

void taskReceivedResult(CTaskPtr task, CTask::ResultType result)
{
  if (!task.get()) {
    return;
  }

  CTasksHolder::getInstance().addResult(task, result);
}

//main

int main()
{
  int numOfThreads;
  cout << "Set the number of threads: ";
  numOfThreads = input();

  CThreadPoolPtr pTp = make_shared<CThreadPool>(numOfThreads);

  cout << "Info: this thread pool is not suspended by default. In other words, after adding task, a free thread executes it" << endl;

  int choice = 0;
  do {
    cout << "==Menu==" << endl;
    cout << "1 - Add a task(s)" << endl;
    cout << "2 - Thread pool info" << endl;
    cout << "3 - Cancel a task by index" << endl;
    cout << "4 - Suspend the thread pool" << endl;
    cout << "5 - Resume the thread pool" << endl;
    cout << "6 - Cancel all tasks" << endl;
    cout << "7 - Results" << endl;
    cout << "9 - Quit" << endl;
    cout << "> ";

    choice = input();
    switch (choice) {
      case 1: {
          int subChoise = 0;
          do {
            cout << "==Tasks==" << endl;
            cout << "1 - Calclulate the factorial of a number; 2 - Sleep; 3 - Back: ";

            subChoise = input();
            switch (subChoise) {
              case 1: {
                  string nStr;
                  cout << "n = ";
                  cin >> nStr;
                  uint32_t n = isNumber(nStr) ? static_cast<uint32_t>(atoi(nStr.c_str())) : 0;
                  CTaskPtr task = make_shared<CFactTask>(n);
                  task->setResultReceiver(taskReceivedResult);
                  uint64_t idx = pTp->addTask(task);
                  CTasksHolder::getInstance().addTask(task, idx);
                  cout << "task idnex = " << idx << " (n = " << n << ")" << endl;
                  break;
                }
              case 2: {
                  cout << "secs = ";
                  int secs = max(1, input());
                  CTaskPtr task = make_shared<CSleepTask>(secs);
                  uint64_t idx = pTp->addTask(task);
                  CTasksHolder::getInstance().addTask(task, idx);
                  cout << "task idnex = " << idx << " (secs = " << secs << ")" << endl;
                  break;
                }
              default:
                break;
            }
          } while (subChoise != 3);
          break;
        }
      case 2: {
          set<uint64_t> waitingTasks = pTp->waitingTaskIdxs();
          set<uint64_t> runningTasks = pTp->runningTaskIdxs();
          if (waitingTasks.size() > 0) {
            cout << "Waiting tasks = ";
            for (uint64_t idx : waitingTasks) {
              cout << idx << ' ';
            }
            cout << "(count = " << waitingTasks.size() << ")" << endl;
          } else {
            cout << "There are no waiting tasks" << endl;
          }

          if (runningTasks.size() > 0) {
            cout << "Running tasks = ";
            for (uint64_t idx : runningTasks) {
              cout << idx << ' ';
            }
            cout << "(count = " << runningTasks.size() << ")" << endl;
          } else {
            cout << "There are no running tasks" << endl;
          }

          cout << "suspended = " << (pTp->suspended() ? "true" : "false") << endl;

          pressEnterToContinue();
          break;
        }
      case 3:
        cout << "Cancel the task by index = ";
        pTp->cancelTask(input());
        break;
      case 4:
        pTp->suspend();
        cout << "The thread pool was suspended" << endl;
        pressEnterToContinue();
        break;
      case 5:
        pTp->resume();
        cout << "The thread poll was resumed" << endl;
        pressEnterToContinue();
        break;
      case 6:
        pTp->cancelAllTasks();
        break;
      case 7: {
          int subChoise = 0;
          do {
            cout << "==Results==" << endl;
            cout << "1 - Get result of a task by index; 2 - All results; 3 - Back: ";

            subChoise = input();
            switch (subChoise) {
              case 1: {
                  cout << "Get result of a task by index = ";
                  CTaskResultPair result = CTasksHolder::getInstance().getResult(input());
                  bool taskHasNoCalcResult = result.first == nullptr;
                  if (!taskHasNoCalcResult) {
                    if (instanceof<CFactTask>(result.first.get())) {
                      uint64_t value = boost::any_cast<uint64_t>(result.second);
                      if (value > 0) {
                        cout << "Result = " << value << endl;
                      } else {
                        cout << "Result = this task cancelled or big value or something else" <<
                          endl;
                      }
                    } else {
                      taskHasNoCalcResult = true;
                    }
                  }
                  if (taskHasNoCalcResult) {
                    cout << "This task does't have a calculation result or there's no task with this index" << endl;
                  }

                  pressEnterToContinue();
                  break;
                }
              case 2: {
                  const CTaskResultMap & results = CTasksHolder::getInstance().getResults();
                  if (results.size() > 0) {
                    cout << "Results = ";
                    for (CTaskResultMap::const_iterator it = results.cbegin(); it != results.cend(); ++it) {
                      if (typeid(uint64_t) == it->second.type()) {
                        cout << boost::any_cast<uint64_t>(it->second)
                             << "(" << it->first << ")" << ' ';
                      }
                    }
                    cout << endl;
                  } else {
                    cout << "There are no results" << endl;
                  }

                  pressEnterToContinue();
                  break;
                }
              default:
                break;
            }
          } while (subChoise != 3);
          break;
        }
      default:
        break;
    }

  } while (choice != 9);

  CTasksHolder::getInstance().clearTasks();

  cin.get();

  return 0;
}
