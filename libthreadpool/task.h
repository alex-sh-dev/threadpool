#ifndef TASK_H
#define TASK_H

#include <atomic>
#include <boost/any.hpp>
#include <memory>

using namespace std;

class CTask
  : public enable_shared_from_this<CTask>
{
public:
  typedef boost::any ResultType;
  typedef void (* ResultReceiver)(shared_ptr<CTask>, ResultType);

public:
  CTask();
  virtual ~CTask();

  virtual void run(ResultReceiver rr) = 0;

  void cancel();
  bool cancelled() {return _cancelled;}

  void setResultReceiver(ResultReceiver rr);
  ResultReceiver resultReceiver();

private:
  atomic_bool _cancelled{false};
  ResultReceiver _resultReceiver;
};

typedef shared_ptr<CTask> CTaskPtr;

#endif // TASK_H
