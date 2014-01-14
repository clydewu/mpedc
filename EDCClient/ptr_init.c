#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "matrix500.h"
#include "lib/libmpedc.h"

const int kSuccess = 0;
const int kFailure = -1;
const int kTrue = 1;
const int kFalse = 0;

const int kMaxInitArgNum = 4;
const int kMaxStopArgNum = 2;

const int kPosNameArg = 0;
const int kPosActArg = 1;
const int kPosComArg = 2;
const int kPosMonoArg = 3;

const int kMinComPort = 0;
const int kMaxComPort = 4;

const char kActArgInit[] = "-init";
const char kActArgStop[] = "-stop";

int show_usage(char *name);

int main(int argc, char* argv[])
{
    char *name;
    PLKPCONTEXT ctx;
    char *action;
    char *com_port_str;
    char *only_mono_str;
    char *check;
    int com_port;
    int only_mono;

    name = argv[kPosNameArg];
    if (argc <= 1)
    {
        show_usage(name);
        return kFailure;
    }
    else if (argc == kMaxStopArgNum)
    {
        action = argv[kPosActArg];
    }
    else if (argc == kMaxInitArgNum)
    {
        action = argv[kPosActArg];
        com_port_str = argv[kPosComArg];
        only_mono_str = argv[kPosMonoArg];
        com_port = (int)strtol(com_port_str, &check, 10);
        if (*check != '\0' || com_port < kMinComPort || com_port > kMaxComPort)
        {
            fprintf(stderr, "Com port error\n");
            return kFailure;
        }
        only_mono = (int)strtol(only_mono_str, &check, 10);
        if (*check != '\0' || only_mono < kMinComPort || only_mono > kMaxComPort)
        {
            fprintf(stderr, "Gray limitation error\n");
            return kFailure;
        }
    }
    else
    {
        show_usage(name);
        return kFailure;
    }

    ctx = lkp_create();
    if (ctx == NULL)
    {
        fprintf(stderr, "Can not create lkp context\n");
        return kFailure;
    }

    if (strcmp(action, kActArgInit) == 0)
    {
        //Init it
        if (ptr_select(com_port, only_mono) < kSuccess)
        {
            fprintf(stderr, "Set COM port printer failure: COM%d, only_mono: %d",
                    com_port, only_mono);
        }

        // Init print, start to statistic
        if (ptr_count_init(ctx) < kSuccess)
        {
            fprintf(stderr, "Initial print counter failure\n");
            goto END;
        }
        fprintf(stderr, "Initial print counter success\n");
    }
    else if (strcmp(action, kActArgStop) == 0)
    {
        if (ptr_count_stop(ctx) < kSuccess)
        {
            fprintf(stderr, "Stop print counter failure\n");
            goto END;
        }
        fprintf(stderr, "Stop print counter success\n");
    }
    else
    {
        show_usage(name);
    }

END:
    lkp_release(ctx);
    return kSuccess;
}

int show_usage(char *name)
{
    fprintf(stderr, "Usage: %s -init:  Initialize printer count.\n", name);
    fprintf(stderr, "       %s -stop:  Stop printer count.\n", name);
    return kSuccess;
}
