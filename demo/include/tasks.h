#ifndef TASKS_H
#define TASKS_H

#include <thread>
#include "task.h"

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

#endif // TASKS_H
