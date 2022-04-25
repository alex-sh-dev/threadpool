#include <iostream>
#include <string>
#include <chrono>
#include "threadpool.h"

using namespace std;

class CFactTask : public CTask {
public:
    CFactTask(uint32_t n) : CTask(), _n(n) {}
    void run() override {
        uint64_t result = 1;
        uint32_t i = 1;
        while (i <= _n && !cancelled()) {
            result *= i;
            ++i;
        }
        result = cancelled() ? 0 : result;
    }

private:
    uint32_t _n;

};

class CSleepTask : public CTask {
public:
    CSleepTask(int secs) : CTask(), _secs(secs) {}
    void run() override {
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

int isNumber(const string& str) {
    for (char const &c : str) {
        if (isdigit(c) == 0)
            return false;
    }

    return true;
}

int input() {
    string str;
    cin >> str;
    return isNumber(str) ? atoi(str.c_str()) : 0;
}

int pressEnterToContinue() {
    cout << "Press Enter to continue" << endl;
    cin.ignore();
    cin.get();
}

int main() {
    int numOfThreads;
    cout << "Set the number of threads:";
    numOfThreads = input();

    CThreadPool *tp = new CThreadPool(numOfThreads);
    deque<CTask *> tasks;

    cout << "Info: this thread pool is not suspended by default. In other words, after adding task, a free thread executes it" << endl;

    int choice = 0;
    do {
        cout << "==Menu==" << endl;
        cout << "Please make your selection" << endl;
        cout << "1 - Add a task(s)" << endl;
        cout << "2 - Thread pool info" << endl;
        cout << "3 - Cancel a task by index" << endl;
        cout << "4 - Suspend the thread pool" << endl;
        cout << "5 - Resume the thread pool" << endl;
        cout << "6 - Cancel all tasks" << endl;
        cout << "9 - Quit" << endl;
        cout << "Selection: ";

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
                            uint32_t n =isNumber(nStr) ? (uint32_t)atoi(nStr.c_str()) : 0;
                            CFactTask *task = new CFactTask(n);
                            tasks.push_back(task);
                            uint64_t idx = tp->addTask(task);
                            cout << "task idnex = " << idx << " (n = " << n << ")" << endl;
                            break;
                        }
                        case 2: {
                            cout << "secs = ";
                            int secs = max(1, input());
                            CSleepTask *task = new CSleepTask(secs);
                            tasks.push_back(task);
                            uint64_t idx = tp->addTask(task);
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
                set<uint64_t> waitingTasks = tp->waitingTaskIdxs();
                set<uint64_t> runningTasks = tp->runningTaskIdxs();
                if (waitingTasks.size() > 0) {
                    cout << "Waiting tasks = ";
                    for (uint64_t idx : waitingTasks)
                        cout << idx << ' ';
                    cout << "(count = " << waitingTasks.size() << ")" << endl;
                } else
                    cout << "There are no waiting tasks" << endl;

                if (runningTasks.size() > 0) {
                    cout << "Running tasks = ";
                    for (uint64_t idx : runningTasks)
                        cout << idx << ' ';
                    cout << "(count = " << runningTasks.size() << ")" << endl;
                } else
                    cout << "There are no running tasks" << endl;

                cout << "suspended = " << (tp->suspended() ? "true" : "false") << endl;

                pressEnterToContinue();
                break;
            }
            case 3:
                cout << "Cancel the task by index = ";
                tp->cancelTask(input());
                break;
            case 4:
                tp->suspend();
                cout << "The thread pool was suspended" << endl;
                pressEnterToContinue();
                break;
            case 5:
                tp->resume();
                cout << "The thread poll was resumed" << endl;
                pressEnterToContinue();
                break;
            case 6:
                tp->cancelAllTasks();
                break;
            default:
                break;
        }

    } while (choice != 9);

    delete tp;
    for (int i = 0; i < tasks.size(); ++i)
        delete tasks[i];
    tasks.clear();

    cin.get();

    return 0;
}
