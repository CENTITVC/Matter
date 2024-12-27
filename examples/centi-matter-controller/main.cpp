#include <sys/ioctl.h>
#include <application/AppTask.h>
#include <lib/support/CodeUtils.h>


// ================================================================================
// Main Code
// ================================================================================

int main(int argc, char * argv[])
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    err = AppTask::Instance().StartApp();
    SuccessOrExit(err);
    
exit:
    if (err != CHIP_NO_ERROR)
    {
        ChipLogProgress(NotSpecified, "iNergy app ended with error: %s ", ErrorStr(err));

        // End the program with non zero error code to indicate a error.
        return -1;
    }

    return 0;    
}
