#ifndef __scaly_task__
#define __scaly_task__

#include "scaly.h"

typedef struct scaly_Task scaly_Task;
struct scaly_Task {
    scaly_Pool* pool;
};

scaly_Task* new_scaly_Task(scaly_Page* _page);
scaly_Page* scaly_Task_getExtensionPage(scaly_Task* this);
void scaly_Task_releaseExtensionPage(scaly_Task* this, scaly_Page* page);
void scaly_Task_dispose(scaly_Task* this);
scaly_Page* allocatePage(scaly_Task* this);

#endif // __scaly_task__
