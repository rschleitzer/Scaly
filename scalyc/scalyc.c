#include "scalyc.h"

scaly_Page* __CurrentPage;
scaly_Task* __CurrentTask;

int main(int argc, char **argv) {

    // Allocate the root page for the main thread
    posix_memalign((void**)&__CurrentPage, scaly_pageSize, scaly_pageSize * scaly_maxStackPages);
    if (!__CurrentPage)
        return -1;
    scaly_Page_reset(__CurrentPage);

    scaly_Task* task = new_scaly_Task(__CurrentPage);
    __CurrentTask = task;

    int ret = 0;

	return ret;
}
