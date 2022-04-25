#ifndef TASK_H
#define TASK_H

#include <atomic>

using namespace std;

class CTask {
public:
    CTask();
    ~CTask();

    virtual void run() = 0;

    void cancel();
    bool cancelled(){ return _cancelled; }

private:
    atomic_bool _cancelled{ false };
};

#endif // TASK_H
