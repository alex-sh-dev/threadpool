#include "threadpool/task.h"

CTask::CTask()
: _resultReceiver(nullptr) {}

CTask::~CTask() {}

//public methods

void CTask::cancel()
{
  _cancelled = true;
}

void CTask::setResultReceiver(ResultReceiver rr)
{
  _resultReceiver = rr;
}

CTask::ResultReceiver CTask::resultReceiver()
{
  return _resultReceiver;
}
