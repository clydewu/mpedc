#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "matrix500.h"
#include "lib/libmpedc.h"

const int kSuccess = 0;
const int kFailure = -1;
const int kTrue = 1;
const int kFalse = 0;
const int kFontWidth = 16;
const int kFontHeight = 16;
const int kScreenWidth = 128;
const int kScreenHeight = 64;
const int kMaxScreenLine = 4;
const int kMaxLineWord = 16;
const int kMaxFailLimit = 10;

typedef struct _ctx
{
    PLKPCONTEXT     lkp_ctx;
} CTX;

int show_line(CTX *p_ctx, int line, const char *string);

int main(int argc, char* argv[])
{
    char str[16];
    CTX ctx;

    if (argc < 2 || argc > 5)
    {
        fprintf(stderr, "Usage: %s line1 [line2 [line3 [line4]]]\n", argv[0]);
        exit(-1);
    }

    ctx.lkp_ctx = lkp_create();
    if (!ctx.lkp_ctx)
    {
        fprintf(stderr, "Can not create lkp contexet.\n");
        return kFailure;
    }

    if (device_open())
    {
        fprintf(stderr, "Can not open device.\n");
        return kFailure;
    }

    switch (argc)
    {
        case 5:
            snprintf(str, 16, "%s", argv[4]);
            show_line(&ctx, 3, str);
        case 4:
            snprintf(str, 16, "%s", argv[3]);
            show_line(&ctx, 2, str);
        case 3:
            snprintf(str, 16, "%s", argv[2]);
            show_line(&ctx, 1, str);
        case 2:
            snprintf(str, 16, "%s", argv[1]);
            show_line(&ctx, 0, str);
    }

    device_close();
    lkp_release(ctx.lkp_ctx);

    return kTrue;
}


int show_line(CTX *p_ctx, int line, const char *string)
{
    int ret;
    int failure_count = 0;

    if (line > kMaxScreenLine - 1)
    {
        fprintf(stderr, "Line exceed limited\n");
        return kFailure;
    }

    if (strlen(string) > kMaxLineWord)
    {
        fprintf(stderr, "Line exceed limited\n");
        return kFailure;
    }
    
    while (kTrue)
    {
        ret = lcd_clean_buffer(p_ctx->lkp_ctx, 0, kFontHeight * line, kScreenWidth, kFontHeight);
        if (ret < 0)
        {
            fprintf(stderr, "Clean buffer error, line=%d, ret=%d\n", line, ret);
            return kFailure;
        }

        ret = lcd_draw_text_16f(p_ctx->lkp_ctx, 0, kFontHeight * line, string, 0);
        if (ret < 0)
        {
            fprintf(stderr, "Draw buffer error, line=%d, ret=%d\n", line, ret);
            return kFailure;
        }

        ret = lcd_print_out(p_ctx->lkp_ctx);

        if (ret >= 0)
        {
            break;
        }

        fprintf(stderr, "Print out LCD error, count=%d, str='%s' line=%d, ret=%d\n",
                failure_count, string, line, ret);

        if (++failure_count > kMaxFailLimit)
        {
            return kFailure;
        }
    }

    return kSuccess;
}

