#include "task.h"

CTask::CTask() {}

CTask::~CTask() {}

//public methods

void CTask::cancel() {
    _cancelled = true;
}
