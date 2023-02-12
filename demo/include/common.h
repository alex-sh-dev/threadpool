#ifndef COMMON_H
#define COMMON_H

#include "threadpool/threadpool.h"

using namespace std;

typedef map<uint64_t, CTask::ResultType> CTaskResultMap;
typedef pair<CTaskPtr, CTask::ResultType> CTaskResultPair;
typedef shared_ptr<CThreadPool> CThreadPoolPtr;

#endif // COMMON_H
