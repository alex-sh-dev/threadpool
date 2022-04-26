#ifndef TASK_H
#define TASK_H

#include <atomic>
#include <boost/any.hpp>

using namespace std;

class CTask {
public:
    CTask();
    virtual ~CTask();

    typedef boost::any ResultType;
    typedef void(*ResultReceiver)(CTask*, ResultType);

    virtual void run(ResultReceiver rr) = 0;

    void cancel();
    bool cancelled(){ return _cancelled; }

    void setResultReceiver(ResultReceiver rr);
    ResultReceiver resultReceiver();

private:
    atomic_bool _cancelled{ false };
    ResultReceiver _resultReceiver;
};

#endif // TASK_H
