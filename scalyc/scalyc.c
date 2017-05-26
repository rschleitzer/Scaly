#include "scalyc.h"

scaly_Page* __CurrentPage;
scaly_Task* __CurrentTask;

int main(int argc, char **argv) {

    // Allocate the root page for the main thread
    posix_memalign((void**)&__CurrentPage, scaly_pageSize, scaly_pageSize * scaly_maxStackPages);
    if (!__CurrentPage)
        return -1;
    scaly_Page_reset(__CurrentPage);

    scaly_Task* task = scaly_Task_new(__CurrentPage);
    __CurrentTask = task;

    // Collect the arguments into a string Array
    scaly_Array* arguments = scaly_Array_newWithCapacity(__CurrentPage, argc - 1);
    for (int i = 1; i < argc; i++)
        scaly_Array_push(arguments, scaly_string_fromRawString(__CurrentPage, argv[i]));

    int ret = 0;

	return ret;
}
