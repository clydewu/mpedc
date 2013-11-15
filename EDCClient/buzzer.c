#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <sys/kd.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "matrix500.h"
#include "lib/libmpedc.h"

#define MAX_COM_PORT_LEN    (64)
#define MAX_COM_CMD_BUFFER  (9)
#define MAX_PROJECT_LEN     (4)
#define MAX_CARD_SN_LEN     (16)
#define MAX_DEP_NAME_LEN    (64)
#define MAX_DEP_NO_LEN      (16)
#define MAX_EMP_NAME_LEN    (64)
#define MAX_EMP_NO_LEN      (64)
#define MAX_EDC_ID_LEN      (16)
#define MAX_IP_LEN          (16)
#define MAX_PORT_LEN        (5)
#define MAX_PASSWD_LEN      (16)

#define MAX_EMP_LIST_SIZE   (1024)
#define MAX_EDC_LIST_SIZE   (2)
#define MAX_PROJ_LIST_SIZE  (128)
#define MAX_MEM_EDC_LOG     (256)
#define MAX_EDC_LOG_LEN     (256)
#define MAX_LOG_TYPE_LEN    (16)

const char kEmpListFile[] = "./employee.list";
const char kEDCListFile[] = "./edc.list";
const char kProjListFile[] = "./projects.list";
const char kEDCLogFile[] = "./edc_tmp.log";
const char kServerIni[] = "./edc_setup.conf";
const char kNetworkIni[] = "./edc_network.conf";
const char kSetupIpCmd[] = "./edc_net_setup.sh ./edc_network.conf";
const char kTempFileSuffix[] = ".tmp";
const int kMaxEmpListSize = MAX_EMP_LIST_SIZE;
const int kMaxEDCListSize = MAX_EDC_LIST_SIZE;
const int kMaxProjListSize = MAX_PROJECT_LEN;

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
const char kTimeFormat[] = "%m/%d %R";
const int kMicroPerSecond = 1000000;
const int kMaxCardSNLen = MAX_CARD_SN_LEN;
const int kMaxBufferlen = 4096;
const int kMaxMemEDCLog = MAX_MEM_EDC_LOG;
const int kMaxEDCLogLen = MAX_EDC_LOG_LEN;
const int kMaxLogTypeLen = MAX_LOG_TYPE_LEN;
const int kKeyScanPerSec = 15;
const int kMaxYmdHMSLen = 20;
const int kASCIIFn = 16;
const int kASCIIUp = 17;
const int kASCIIDown = 18;
const int kASCIIPower = 19;
const int kASCIIClear = 20;
const int kASCIICancel = 21;
const int kASCIIEnter = 22;
const int kASCIIEject = 23;
const int kASCIIFirstVisiable = 32;
const int kASCIILastVisiable = 127;
const int kASCII_zero = 48;
const int kASCII_nine = 57;
const int kMaxFailLimit = 10;

const int kMaxPrtPage = 11;

const char kSetupStrServerIP[] = "server";
const char kSetupStrServerPort[] = "port";
const char kSetupStrEDCID[] = "id";
const char kSetupStrFnPasswd[] = "passwd";
const char kSetupStrPRTCON[] = "com";
const char kSetupSelfIP[] = "ip";
const char kSetupSubmask[] = "submask";
const char kSetupGateway[] = "gateway";

const int kMaxProjectCodeLen = MAX_PROJECT_LEN;

const char kDefINIFile[] = "./comtest.ini";
const int kMaxCOMPortLen = MAX_COM_PORT_LEN;
const int kMaxCOMCmdBuffer = MAX_COM_CMD_BUFFER;
const char kSendComSignal[] = {0x80, 0xA4, 0x00, 0x02, 0x09};
const int kMaxReadRFIDLen = 256;
const int kMaxCardReadLen = 12;
const int kMaxEDCIDLen = MAX_EDC_ID_LEN;
const int kMaxConnectTypeLen = 8;
const int kConnectTypeMin = 0;
const int kConnectTypeMax = 4;
const int kMaxPasswdLen = MAX_PASSWD_LEN;

const int kMaxReadLineLen = 512;
const int kMaxPathLen = 512;
const char kTab = '\t';
const char kNewLine = '\n';
const char kSep = '|';
const int kMaxIPLen = MAX_IP_LEN;
const int kMaxPortLen = MAX_PORT_LEN;
const int kMaxSockTimeout = 10;
const int kMaxConLenStrLen = 16;

const int kLCDBackLightTimeout = 5;
const int kSyncListInterval = 180;
const int kSyncLogInterval = 5;

const int kLEDNum = 4;
const unsigned int kLEDNone = 0;
const unsigned int kLEDBlue = 1;
const unsigned int kLEDRed = 2;
const unsigned int kLEDYellow = 4;
const unsigned int kLEDGreen = 8;

const char kSyncEmpCmd[] = "SYNC_EMP";
const char kSyncEDCCmd[] = "SYNC_EDC";
const char kSyncProjCmd[] = "SYNC_PROJ";
const char kSyncLogCmd[] = "SYNC_LOG";
const char kHeartBeat[] = "HEARTBEAT";
const int kMaxEmpListBuf = (1024 * 1024); // 1M

const char STR_EMPTY[] = "";
const char STR_SPACE[] = " ";
const char STR_PLEASE_CARD[] = "請靠卡使用";
const char STR_EMPOLYEE_ID[] = "員編:";
const char STR_INVALID_CARD[] = "無效卡";
const char STR_CONTACT_STAFF[] = "請洽管理人員";
const char STR_PROJECT_CODE[] = "專案代碼:";
const char STR_QUOTA[] = "額度";
const char STR_USING[] = "使用中";
const char STR_MONO_A3[] = "黑大";
const char STR_MONO_A4[] = "黑小";
const char STR_COLOR_A3[] = "彩大";
const char STR_COLOR_A4[] = "彩小";
const char STR_REMAIN_SEC[] = "剩秒";
const char STR_ERROR[] = "發生錯誤";
const char STR_SETUP_PRT_CON_TYPE[] = "設定COM Port";
const char STR_SETUP_EDC_ID[] = "設定EDC ID";
const char STR_SETUP_EDC_IP[] = "設定IP";
const char STR_SETUP_SUBMASK[] = "設定Submask";
const char STR_SETUP_GATEWAY[] = "設定Gateway ";
const char STR_SETUP_SERVER_IP[] = "設定SERVER IP";
const char STR_SETUP_SERVER_PORT[] = "設定SERVER PORT";
const char STR_SETUP_FN_PASSWD[] = "設定管理密碼";
const char STR_SETUP_CONFIRM[] = "確定/取消";
const char STR_SETUP_CURRENT[] = "目前:";
const char STR_SETUP_NEWSET[] = "設定:";
const char STR_SETUP_ERROR[] = "格式錯誤";
const char STR_SETUP_PASSWD_ERROR[] = "密碼錯誤";
const char STR_SETUP_INPUT_PASSWD[] = "輸入密碼";

enum RET{
        RET_SUCCESS = 0,           // 成功。
        RET_ERR_UNKNOW  = -0xFF,   // 未知的錯誤
        RET_ERR_SETUP_FILE,        // 讀取設定檔誤
        RET_ERR_READER_FILE,       // 開啟Reader失敗
        RET_ERR_UNKNOWN_READER     // 未知的Reader錯誤
};

typedef struct
{
        int len;
        unsigned char data[256];
} CMD_DATA, *PCMD_DATA;

typedef enum _op_state
{
    IDLE,
    INVALID_CARD,
    QUOTA,
    SCANNING,
    SETUP,
    PASSWD
} OP_STATE;

typedef enum _fn_state
{
    SET_PRT_TYPE,
    SET_EDC_ID,
    SET_EDC_IP,
    SET_SUBMASK,
    SET_GATEWAY,
    SET_SRV_IP,
    SET_SRV_PORT,
    SET_FN_PASS,
    SET_CONFIRM
} FN_STATE;

typedef enum _edc_log_type
{
    CARD,
    COPY,
    SCAN,
    PRINT,
    FAX
} EDC_LOG_TYPE;

const char TYPE2STR[5][MAX_LOG_TYPE_LEN] = {
    "CARD",
    "COPY",
    "SCAN",
    "PRINT",
    "FAX"
};

typedef enum _prt_con_type
{
    DIO = 0,
    COM = 3
} PRT_CON_TYPE;

const char PTN_CARD_VALID[] = "VALID %s";
const char PTN_CARD_INVALID[] = "INVALID %s";
const char PTN_CARD_ENJECT[] = "ENJECT %s";
const char PTN_CARD_TIMEOUT[] = "TIMEOUT %s";

const char PTN_PAPER_GRAY_A[] = "GA%d:%d ";
const char PTN_PAPER_GRAY_B[] = "GB%d:%d ";
const char PTN_PAPER_COLOR_A[] = "CA%d:%d ";
const char PTN_PAPER_COLOR_B[] = "CB%d:%d ";

typedef struct _com_ctx
{
    int com_handle;
    char port[MAX_COM_PORT_LEN];
    int baud_rate;
    int data_bits;
    int parity;
    int stop_bits;
    CMD_DATA cmd_buffer[MAX_COM_CMD_BUFFER];
} COM_CTX;

typedef struct _key_ctx
{
    char cur_key;
} KEY_CTX;

typedef struct _employee_data
{
    char    dep_name[MAX_DEP_NAME_LEN + 1];
    char    dep_no[MAX_DEP_NO_LEN + 1];
    char    emp_no[MAX_EMP_NO_LEN + 1];
    char    card_sn[MAX_CARD_SN_LEN + 1];
    int     init_quota;
    int     curr_quota;
} EMPLOYEE_DATA;

typedef struct _edc_data
{
    char    edc_id[MAX_EDC_ID_LEN + 1];
    char    edc_ip[MAX_IP_LEN + 1];
    char    printer_ip[MAX_IP_LEN + 1];
    int     limit_time;
    int     show_quota;
    int     mono_a3;
    int     mono_a4;
    int     color_a3;
    int     color_a4;
    int     paper_size_a;
    int     paper_size_b;
} EDC_DATA;


typedef struct _project_data
{
    char    proj_num[MAX_PROJECT_LEN + 1];
} PROJ_DATA;

typedef struct _edc_ctx
{
    PLKPCONTEXT     lkp_ctx;
    COM_CTX         com_ctx;
    KEY_CTX         key_ctx;

    int             backlight_flag;

    pthread_mutex_t lkp_ctx_mutex;

    char            edc_id[MAX_EDC_ID_LEN + 1];
    char            edc_ip[MAX_IP_LEN + 1];
    char            submask[MAX_IP_LEN + 1];
    char            gateway[MAX_IP_LEN + 1];
    char            server_ip[MAX_IP_LEN + 1];
    int             server_port;
    PRT_CON_TYPE    prt_con_type;
    char            fn_passwd[MAX_PASSWD_LEN + 1];

    OP_STATE        state;
    char            project_code[MAX_PROJECT_LEN + 1];
    char            curr_card_sn[MAX_CARD_SN_LEN + 1];

    int             server_fd;
    int             connected;

    int             emp_num;
    EMPLOYEE_DATA   emp_list[MAX_EMP_LIST_SIZE];
    int             curr_emp_idx;
    pthread_mutex_t emp_mutex;

    int             edc_num;
    EDC_DATA        edc_list[MAX_EDC_LIST_SIZE];
    pthread_mutex_t edc_mutex;

    int             proj_num;
    PROJ_DATA       proj_list[MAX_PROJ_LIST_SIZE];
    pthread_mutex_t proj_mutex;


    pthread_t       keypad_thr;       

    pthread_t       sync_list_thr;

    pthread_t       sync_log_thr;
    char            edc_tmp_log[MAX_MEM_EDC_LOG][MAX_EDC_LOG_LEN];
    int             edc_log_num;
    pthread_mutex_t log_mutex;
} EDC_CTX;





// COM Module
int serOpenCOM(COM_CTX*);
int serCloseCOM(COM_CTX*);
int serReadCOM(COM_CTX*, void*, const int);
int serWriteCOM(COM_CTX*, const void*, const int);
int DataConvertCMD(char *buf, char *buf1, int len, PCMD_DATA cmd_buffer);
unsigned int AsciiToUINTForHex(char *Source);
void FileGetValue(char *buf, char *u8Data, int *i32Len);
int load_com_setting(COM_CTX*, const char*);
int setport(COM_CTX*);
int OpenDev(char*);

// State Function
int idle_state(EDC_CTX*);
int invalid_card_state(EDC_CTX*);
int quota_state(EDC_CTX*);
int scanning_state(EDC_CTX*);
int setup_state(EDC_CTX*);
int passwd_state(EDC_CTX*);

// EDC Utilities
int init(EDC_CTX*, const char*);
void quit(EDC_CTX*);
int show_datetime(EDC_CTX*);
int show_line(EDC_CTX*, int, const char*);
int left_right_str(char*, const int, const char*, const char*);
int set_led(unsigned int conf);
int read_rfid(EDC_CTX*);
int is_valid_card(EDC_CTX*);
int buzzer(int);
int get_str_from_keypad(EDC_CTX*, const char*, char*, const int, const int);
int get_ipv4_from_keypad(EDC_CTX*, const char*, char*, const int, const int);
int load_server_set(EDC_CTX*, const char*);
int load_network_set(EDC_CTX*, const char*);
int set_backlight(EDC_CTX* p_ctx, unsigned char u8_type);
int show_quota_info(EDC_CTX *p_ctx, int quota, int gb, int gs, int cb, int cs);

// List Utilities
int sync_lists(EDC_CTX*);
int load_local_lists(EDC_CTX*);
int load_employee_list(EMPLOYEE_DATA*, const int, const char*, pthread_mutex_t*);
int load_edc_list(EDC_DATA*, const int, const char*, pthread_mutex_t*);
int load_proj_list(PROJ_DATA*, const int, const char*, pthread_mutex_t*);
int get_str_before_char(const char*, const char, char*, int);

// EDC log Utilities
int sync_log(EDC_CTX*);
int save_log_to_local(EDC_CTX*, const char*);
int get_cur_YmdHMS_r(char*, int);
int append_edc_log(EDC_CTX*, const EDC_LOG_TYPE, const char*);
int gen_cost_log(EDC_CTX*, const EDC_LOG_TYPE, PRINTERTYPE*);

// Printer Utilities
int usage_dup(PRINTERTYPE* , PRINTERTYPE*);
int usage_same_as(PRINTERTYPE *p1, PRINTERTYPE *p2);
int print_printertype(PRINTERTYPE *);
int init_printertype(PRINTERTYPE *usage);
int count2cost(PRINTERCOUNT_V2 *ptr_counter, int paper_size_a, int paper_size_b, 
        int *gray_big, int *gray_small, int *color_big, int *color_small);

// Network Utilities
int connect_server(EDC_CTX*);
size_t sock_write(int, const void*, size_t);
size_t sock_read(int, char*, size_t);
int set_nonblock(int, int);
int dl_remote_list(EDC_CTX*, const char*, const char*);
int get_remote_list(EDC_CTX*,const char*,const char*, pthread_mutex_t*);

// Thread
int stop_sync_thr(EDC_CTX*);
void sync_thr_func(void*);
void sync_log_thr_func(void*);




int main(void)
{
    int ret;
    int ret_code = kSuccess;
    EDC_CTX ctx;

    if (init(&ctx, kDefINIFile))
    {
        fprintf(stderr, "Can not connect to the device.\n");
        goto INIT_FAIL;
    }

    if (set_led(kLEDNone) != kSuccess)
    {
        fprintf(stderr, "Set LED down failure.\n");
        goto INIT_FAIL;
    }

    if(lcd_clean_scr(ctx.lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Can not clean screen.\n");
        goto INIT_FAIL;
    }

    if(lcd_clean(ctx.lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Can not clean screen buffer.\n");
        goto INIT_FAIL;
    }

    // Main Loop
    while (kTrue)
    {
        switch (ctx.state)
        {
        case IDLE:
            ret = idle_state(&ctx);
            break;
        case INVALID_CARD:
            ret = invalid_card_state(&ctx);
           break;
        case QUOTA:
            ret = quota_state(&ctx);
            break;
        case SCANNING:
            ret = scanning_state(&ctx);
            break;
        case SETUP:
            ret = setup_state(&ctx);
            break;
        case PASSWD:
            ret = passwd_state(&ctx);
            break;
        default:
            fprintf(stderr, "Never ever be here\n");
            ret = kFailure;
            break;
        }

        if (ret != kSuccess)
        {
            break;
        }
    }
INIT_FAIL:
    ret_code = kFailure;
    if (show_line(&ctx, 1, STR_ERROR) != kSuccess)
    {
        fprintf(stderr, "Can not print error msg to screen.\n");
    }

    if (set_led(kLEDNone) != kSuccess)
    {
        fprintf(stderr, "Can not set LED.\n");
    }

    if(lcd_clean_scr(ctx.lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Can not clean screen.\n");
    }
    quit(&ctx);
    fprintf(stderr, "Program terminate.\n");
    return ret_code;
}

int stop_sync_thr(EDC_CTX* p_ctx)
{
    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if (pthread_cancel(p_ctx->sync_list_thr) != kSuccess)
    {
        fprintf(stderr, "Can not join sync_list thread.\n");
        return kFailure;
    }

    if (pthread_cancel(p_ctx->sync_log_thr) != kSuccess)
    {
        fprintf(stderr, "Can not join sync_list thread.\n");
        return kFailure;
    }


    if (pthread_join(p_ctx->sync_list_thr, NULL) != kSuccess)
    {
        fprintf(stderr, "Can not join sync_list thread.\n");
        return kFailure;
    }

    if (pthread_join(p_ctx->sync_log_thr, NULL) != kSuccess)
    {
        fprintf(stderr, "Can not join sync_log thread.\n");
        return kFailure;
    }

    return kSuccess;
}

void sync_list_thr_func(void* ctx)
{
    EDC_CTX* p_ctx;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return ;
    }

    p_ctx = (EDC_CTX*)ctx;

    while (kTrue)
    {
        sleep(kSyncListInterval);
        //sleep(3);
        pthread_testcancel();

        if (p_ctx->connected)
        {
            if (sync_lists(p_ctx) != kSuccess)
            {
                fprintf(stderr, "Sync lists from server fail!\n");
            }

            if (load_local_lists(p_ctx) != kSuccess)
            {
                fprintf(stderr, "Load lists failure!\n");
                //TODO should terminate program.
            }
            continue;
        }
        else
        {
            fprintf(stderr, "Server is disconnected, can't sync lists!\n");
        }
    }
}

void keypad_thr_func(void *ctx)
{
    int ret;
    EDC_CTX* p_ctx;
    unsigned char in_key;
    int backlight_usec;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return ;
    }

    p_ctx = (EDC_CTX*)ctx;
    p_ctx->key_ctx.cur_key = 0;
    backlight_usec = 0;

    while (kTrue)
    {
        //Use mutex to avoid API failure
        if (pthread_mutex_lock(&p_ctx->lkp_ctx_mutex))
        {
            fprintf(stderr, "Lock lkp_ctx mutex failure!\n");
            return;
        }

        ret = kpd_get_keypad(p_ctx->lkp_ctx, &in_key);

        if (pthread_mutex_unlock(&p_ctx->lkp_ctx_mutex))
        {
            fprintf(stderr, "Unlock lkp_ctx mutex failure!\n");
            return;
        }

        if (ret >= 0)
        {
            if (in_key > 0 )
            {
                fprintf(stderr, "input key: %d\n", in_key);
                p_ctx->key_ctx.cur_key = in_key;

                if (set_backlight(p_ctx, 0xff))
                {
                    fprintf(stderr, "Can not open backlight.\n");
                    return;
                }
                backlight_usec = 0;
                p_ctx->backlight_flag = kTrue;
            }
            else if ((char)in_key < 0)
            {
                fprintf(stderr, "Invalid input key: %d\n", in_key);
            }
        }
        else
        {
            fprintf(stderr, "Get keypad failure.\n");
        }

        // If flag enable, increase count and check timeout
        if (p_ctx->backlight_flag)
        {
            if (backlight_usec > kMicroPerSecond * kLCDBackLightTimeout)
            {
                if (set_backlight(p_ctx, 0x00))
                {
                    fprintf(stderr, "Can not close backlight when timeout.\n");
                    return;
                }
                backlight_usec = 0;
                p_ctx->backlight_flag = kFalse;
            }

            backlight_usec += kMicroPerSecond / kKeyScanPerSec;
        }

        usleep(kMicroPerSecond / kKeyScanPerSec);
    }
}

int get_press_key(EDC_CTX* p_ctx, unsigned char* key)
{
    if (!p_ctx || !key)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    *key = p_ctx->key_ctx.cur_key;
    p_ctx->key_ctx.cur_key = 0;
    return kSuccess;
}

void sync_log_thr_func(void *ctx)
{
    EDC_CTX* p_ctx;
    int sync_success = kFalse;
    int ret;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return ;
    }

    p_ctx = (EDC_CTX*)ctx;

    while (kTrue)
    {
        sleep(kSyncLogInterval);
        pthread_testcancel();

        if (p_ctx->connected)
        {
            ret = sync_log(p_ctx);
            if (ret != kSuccess)
            {
                fprintf(stderr, "Sync edc log to server fail!\n");
                sync_success = kFalse;
            }
            else
            {
                sync_success = kTrue;
            }
        }
        else
        {
            // Check connected and reconnect here, due to the interval of this thread is most short
            if (connect_server(p_ctx) != kSuccess)
            {
                fprintf(stderr, "Re-Connect to server failure\n");
                sync_success = kFalse;
            }
            else
            {
                if (sync_log(p_ctx) != kSuccess)
                {
                    fprintf(stderr, "Sync edc log to server fail!\n");
                    sync_success = kFalse;
                }
                else
                {
                    sync_success = kTrue;
                }
            }
        }

        if (sync_success == kFalse) 
        {
            if (save_log_to_local(p_ctx, kEDCLogFile) == kFailure)
            {
                fprintf(stderr, "Save EDC log to file failure!\n");
                // TODO program should dead
            }
        }
    }
}

int append_edc_log(EDC_CTX *p_ctx, const EDC_LOG_TYPE type, const char *content)
{
    char cur_date[kMaxYmdHMSLen + 1];

    if (!p_ctx || !content)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if (get_cur_YmdHMS_r(cur_date, kMaxYmdHMSLen + 1) != kSuccess)
    {
        fprintf(stderr, "Get current datetime fail\n");
        return kFailure;
    }

    if (pthread_mutex_lock(&p_ctx->log_mutex))
    {
        fprintf(stderr, "Lock EDC log mutex failure!\n");
        return kFailure;
    }

    if (p_ctx->edc_log_num >= kMaxMemEDCLog)
    {
        fprintf(stderr, "Temporary space of EDC log is not enought\n");
        return kFailure;
    }
    else
    {
        if (snprintf(p_ctx->edc_tmp_log[p_ctx->edc_log_num], kMaxEDCLogLen, "%s\t%s\t%s\t%s\t%s\t%s\n",
                    TYPE2STR[type],
                    p_ctx->edc_id,
                    p_ctx->project_code,
                    p_ctx->emp_list[p_ctx->curr_emp_idx].emp_no,
                    cur_date,
                    content) < 0)
        {
            fprintf(stderr, "Make log string failure\n");
            return kFailure;
        }
        p_ctx->edc_log_num++;
    }

    if (pthread_mutex_unlock(&p_ctx->log_mutex))
    {
        fprintf(stderr, "Unlock EDC log mutex failure!\n");
        return kFailure;
    }

    return 0;
}

int get_cur_YmdHMS_r(char *buf, int buf_len)
{
    time_t time_cur;
    struct tm local_time = {0};

    if (!buf)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    strncpy(buf, "0000-00-00 00:00:00", buf_len);

    time_cur = time(NULL);

    localtime_r(&time_cur, &local_time);
    snprintf(buf, buf_len, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
            local_time.tm_year+1900,
            local_time.tm_mon+1,
            local_time.tm_mday,
            local_time.tm_hour,
            local_time.tm_min,
            local_time.tm_sec);

    return kSuccess;
}

int save_log_to_local(EDC_CTX *p_ctx, const char *log_tmp_file)
{
    FILE *fp_log;
    int i = 0;

    if (!p_ctx || !log_tmp_file)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if(!(fp_log = fopen(log_tmp_file, "a")))
    {
        fprintf(stderr, "Can not open log temp file.\n");
        return kFailure;
    }

    if (pthread_mutex_lock(&p_ctx->log_mutex))
    {
        fprintf(stderr, "Lock EDC log mutex failure!\n");
        fclose(fp_log);
        return kFailure;
    }

    // 1. write to files
    while (i < p_ctx->edc_log_num)
    {
        if (fprintf(fp_log, "%s\n", p_ctx->edc_tmp_log[i]) <= 0)
        {
            fprintf(stderr, "Write to temp log file failure\n");
            pthread_mutex_unlock(&p_ctx->log_mutex);
            fclose(fp_log);
            return kFailure;
        }
        i++;
    }
    // 2. clean up ctx
    p_ctx->edc_log_num = 0;

    if (pthread_mutex_unlock(&p_ctx->log_mutex))
    {
        fprintf(stderr, "Unlock EDC log mutex failure!\n");
        fclose(fp_log);
        return kFailure;
    }

    fclose(fp_log);
    return kSuccess;
}

int sync_log(EDC_CTX *p_ctx)
{
    // for file log
    FILE *fp_log;
    char line[kMaxEDCLogLen];
    char log_buf[kMaxMemEDCLog * kMaxEDCLogLen];
    char *buf_ptr;
    int buf_size;
    int line_len;

    // for memory log
    int log_count;
    char *cur_edc_log;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }
    memset(line, 0, kMaxEDCLogLen);
    memset(log_buf, 0, kMaxMemEDCLog * kMaxEDCLogLen);

    //Check local file first
    if (access(kEDCLogFile, F_OK) == kFailure)
    {    
        //fprintf(stderr, "CLD: EDC log file is not exist.\n");
    }
    else
    {
        buf_size = sizeof(log_buf);
        buf_ptr = log_buf;
        if (!(fp_log = fopen(kEDCLogFile, "r")))
        {
            fprintf(stderr, "Can not open EDC log file.\n");
            return kFailure;
        }

        line_len = snprintf(buf_ptr, buf_size, "%s\t%s\n", kSyncLogCmd, p_ctx->edc_id);
        buf_size -= line_len;
        buf_ptr += line_len;

        while (fgets(line, kMaxEDCLogLen, fp_log) != NULL)
        {
            if (buf_size > strlen(line) + 1)
            {
                line_len = snprintf(buf_ptr, buf_size, "%s\n", line);
                buf_size -= line_len;
                buf_ptr += line_len;
            }
            else
            {
                // If buffer is not enough, send current log
                if (sock_write(p_ctx->server_fd, log_buf, buf_ptr - log_buf) != buf_ptr - log_buf)
                {
                    fprintf(stderr, "Send log to server fail.\n");
                    p_ctx->connected = kFalse;
                    fclose(fp_log);
                    return kFailure;
                }

                // Reset buffer
                buf_size = sizeof(log_buf);
                buf_ptr = log_buf;
                line_len = snprintf(buf_ptr, buf_size, "%s\t%s\n", kSyncLogCmd, p_ctx->edc_id);
                buf_size -= line_len;
                buf_ptr += line_len;
                // Append current line
                line_len = snprintf(buf_ptr, buf_size, "%s\n", line);
                buf_size -= line_len;
                buf_ptr += line_len;
            }
        }
        fclose(fp_log);

        if (sock_write(p_ctx->server_fd, log_buf, buf_ptr - log_buf) != buf_ptr - log_buf)
        {
            fprintf(stderr, "Send log to server fail.\n");
            p_ctx->connected = kFalse;
            return kFailure;
        }

        if (unlink(kEDCLogFile) != kSuccess)
        {
            fprintf(stderr, "Delete sended EDC log file failure!\n");
            return kFailure;
        }
    }

    //2. Send memory content if exist
    buf_size = sizeof(log_buf);
    buf_ptr = log_buf;
    line_len = snprintf(buf_ptr, buf_size, "%s\t%s\n", kSyncLogCmd, p_ctx->edc_id);
    buf_size -= line_len;
    buf_ptr += line_len;
    if (p_ctx->edc_log_num > 0)
    {
        if (pthread_mutex_lock(&p_ctx->log_mutex))
        {
            fprintf(stderr, "Lock EDC log mutex failure!\n");
            return kFailure;
        }

        fprintf(stderr, "CLD: Sync main lock: %d \n", p_ctx->edc_log_num);
        log_count = 0;
        while (log_count < p_ctx->edc_log_num)
        {
            cur_edc_log = p_ctx->edc_tmp_log[log_count];
            if (buf_size > strlen(cur_edc_log) + 1)
            {
                line_len = snprintf(buf_ptr, buf_size, "%s\n", cur_edc_log);
                buf_size -= line_len;
                buf_ptr += line_len;
            }
            else
            {
                // If buffer is not enough, send current log
                fprintf(stderr, "CLD: %s\n", log_buf);
                if (sock_write(p_ctx->server_fd, log_buf, buf_ptr - log_buf) != buf_ptr - log_buf)
                {
                    fprintf(stderr, "Send memory log to server fail.\n");
                    p_ctx->connected = kFalse;
                    pthread_mutex_unlock(&p_ctx->log_mutex);
                    return kFailure;
                }

                // Reset buffer
                buf_size = sizeof(log_buf);
                buf_ptr = log_buf;
                line_len = snprintf(buf_ptr, buf_size, "%s\t%s\n", kSyncLogCmd, p_ctx->edc_id);
                buf_size -= line_len;
                buf_ptr += line_len;
                // Append current line
                line_len = snprintf(buf_ptr, buf_size, "%s\n", cur_edc_log);
                buf_size -= line_len;
                buf_ptr += line_len;
            }
            log_count++;
        }

        fprintf(stderr, "CLD2: %s\n", log_buf);
        if (sock_write(p_ctx->server_fd, log_buf, buf_ptr - log_buf) != buf_ptr - log_buf)
        {
            p_ctx->connected = kFalse;
            pthread_mutex_unlock(&p_ctx->log_mutex);
            return kFailure;
        }

        p_ctx->edc_log_num = 0;

        if (pthread_mutex_unlock(&p_ctx->log_mutex))
        {
            fprintf(stderr, "Unlock EDC log mutex failure!\n");
            return kFailure;
        }
    }

    return kSuccess;
}

int init(EDC_CTX *p_ctx, const char* com_ini_file)
{
    if (!p_ctx || !com_ini_file)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    p_ctx->state = IDLE;
    p_ctx->connected = kFalse;
    memset(p_ctx->project_code, 0, kMaxProjectCodeLen + 1);

    if (pthread_mutex_init(&(p_ctx->emp_mutex), NULL) != kSuccess)
    {
        fprintf(stderr, "Can not create employee mutex.\n");
        return kFailure;
    }

    if (pthread_mutex_init(&(p_ctx->edc_mutex), NULL) != kSuccess)
    {
        fprintf(stderr, "Can not create edc mutex.\n");
        return kFailure;
    }

    if (pthread_mutex_init(&(p_ctx->proj_mutex), NULL) != kSuccess)
    {
        fprintf(stderr, "Can not create project mutex.\n");
        return kFailure;
    }

    if (pthread_mutex_init(&(p_ctx->log_mutex), NULL) != kSuccess)
    {
        fprintf(stderr, "Can not create EDC log mutex.\n");
        return kFailure;
    }

    if (pthread_mutex_init(&(p_ctx->lkp_ctx_mutex), NULL) != kSuccess)
    {
        fprintf(stderr, "Can not create lkp_ctx mutex.\n");
        return kFailure;
    }

    p_ctx->lkp_ctx = lkp_create();
    if (!p_ctx->lkp_ctx)
    {
        fprintf(stderr, "Can not create lkp contexet.\n");
        return kFailure;
    }

    if (device_open())
    {
        fprintf(stderr, "Can not open device.\n");
        return kFailure;
    }
    
    if (load_com_setting(&p_ctx->com_ctx,
                com_ini_file) != kSuccess)
    {
        fprintf(stderr, "Can not create com contexet.\n");
        return kFailure;
    }

    if (serOpenCOM(&p_ctx->com_ctx) != kSuccess) 
    {
        fprintf(stderr, "Can't Open COM Port\n");
        return kFailure;
    }

    if (load_server_set(p_ctx, kServerIni) != kSuccess)
    {
        fprintf(stderr, "Load server setup failure.\n");
        return kFailure;
    }

    if (load_network_set(p_ctx, kNetworkIni) != kSuccess)
    {
        fprintf(stderr, "Load network setup failure.\n");
        return kFailure;
    }

    if (connect_server(p_ctx) != kSuccess)
    {
        fprintf(stderr, "Connect to server failure,"
                "use local files.\n");
    }
    else
    {
        if (sync_lists(p_ctx) != kSuccess)
        {
            fprintf(stderr, "Sync list file failure.\n");
            return kFailure;
        }
    }

    if (load_local_lists(p_ctx) != kSuccess)
    {
        fprintf(stderr, "Load lists failure!\n");
        return kFailure;
    }

    if (pthread_create(&(p_ctx->keypad_thr), NULL,
                (void*)keypad_thr_func, (void*)p_ctx) != kSuccess)
    {
        fprintf(stderr, "Create keypad thread failure.\n");
        return kFailure;
    }

    if (pthread_create(&(p_ctx->sync_list_thr), NULL,
                (void*)sync_list_thr_func, (void*)p_ctx) != kSuccess)
    {
        fprintf(stderr, "Create sync thread failure.\n");
        return kFailure;
    }

    p_ctx->edc_log_num = 0;
    memset(p_ctx->edc_tmp_log, 0, kMaxMemEDCLog * kMaxEDCLogLen);

    if (pthread_create(&(p_ctx->sync_log_thr), NULL,
                (void*)sync_log_thr_func, (void*)p_ctx) != kSuccess)
    {
        fprintf(stderr, "Create sync thread failure.\n");
        return kFailure;
    }

    return kSuccess;
}

int sync_lists(EDC_CTX *p_ctx)
{
    char            tmp_emp_file[kMaxPathLen];
    char            tmp_edc_file[kMaxPathLen];
    char            tmp_proj_file[kMaxPathLen];
    EMPLOYEE_DATA   tmp_emp_list[kMaxEmpListSize];
    EDC_DATA        tmp_edc_list[kMaxEDCListSize];
    PROJ_DATA       tmp_proj_list[kMaxProjListSize];

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    snprintf(tmp_emp_file, kMaxPathLen, "%s%s",
            kEmpListFile, kTempFileSuffix);
    snprintf(tmp_edc_file, kMaxPathLen, "%s%s",
            kEDCListFile, kTempFileSuffix);
    snprintf(tmp_proj_file, kMaxPathLen, "%s%s",
            kProjListFile, kTempFileSuffix);

    // Employee list
    if (dl_remote_list(p_ctx, kSyncEmpCmd,
                tmp_emp_file) != kSuccess)
    {
        fprintf(stderr, "Download Employee list failure,"
                " use local list.\n");
    }
    else
    {
        // Download OK, test it.
        if (load_employee_list(tmp_emp_list, MAX_EMP_LIST_SIZE,
                tmp_emp_file, NULL) < 0)
        {
            fprintf(stderr, "Downloaded employee list"
                    " malformed, use local list\n");
        }
        else
        {
            if (rename(tmp_emp_file, kEmpListFile) != kSuccess)
            {
                fprintf(stderr, "Move employee file failure, "
                        "use original file\n");
            }
        }
    }

    // EDC list
    if (dl_remote_list(p_ctx, kSyncEDCCmd,
                tmp_edc_file) != kSuccess)
    {
        fprintf(stderr, "Download EDC list failure, "
                "use local list.\n");
    }
    else
    {
        // Download OK, test it.
        if (load_edc_list(tmp_edc_list, kMaxEDCListSize,
                tmp_edc_file, NULL) < 0)
        {
            fprintf(stderr, "Downloaded EDC list "
                    "malformed, use local list\n");
        }
        else
        {
            if (rename(tmp_edc_file, kEDCListFile) != kSuccess)
            {
                fprintf(stderr, "Move EDC file failure, "
                        "use original file\n");
            }
        }
    }

    // Project list
    if (dl_remote_list(p_ctx, kSyncProjCmd,
                tmp_proj_file) != kSuccess)
    {
        fprintf(stderr, "Download project list failure, "
                "use local list.\n");
    }
    else
    {
        // Download OK, test it.
        if (load_proj_list(tmp_proj_list, kMaxProjListSize,
                tmp_proj_file, NULL) < 0)
        {
            fprintf(stderr, "Downloaded project list "
                    "malformed, use local list\n");
        }
        else
        {
            if (rename(tmp_proj_file, kProjListFile) != kSuccess)
            {
                fprintf(stderr, "Move project file failure, "
                        "use original file\n");
            }
        }
    }

    return kSuccess;
}

int load_local_lists(EDC_CTX *p_ctx)
{
    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if ((p_ctx->emp_num = load_employee_list(
            p_ctx->emp_list, kMaxEmpListSize,
            kEmpListFile, &p_ctx->emp_mutex)) < 0)
    {
        fprintf(stderr, "Load Employee list failure.\n");
        return kFailure;
    }

    if ((p_ctx->edc_num = load_edc_list(
            p_ctx->edc_list, kMaxEDCListSize,
            kEDCListFile, &p_ctx->edc_mutex)) < 0)
    {
        fprintf(stderr, "Load EDC list failure.\n");
        return kFailure;
    }

    if ((p_ctx->proj_num = load_proj_list(
            p_ctx->proj_list, kMaxProjListSize,
            kProjListFile, &p_ctx->proj_mutex)) < 0)
    {
        fprintf(stderr, "Load project list failure.\n");
        return kFailure;
    }

    return kSuccess;
}

void quit(EDC_CTX *p_ctx)
{
    if (serCloseCOM(&p_ctx->com_ctx) != RET_SUCCESS)
    {
        fprintf(stderr, "Close COM failure.\n");
    }

    device_close();

    lkp_release(p_ctx->lkp_ctx);

    stop_sync_thr(p_ctx);

    if (pthread_cancel(p_ctx->keypad_thr) != kSuccess)
    {
        fprintf(stderr, "Can not join sync_list thread.\n");
    }

    if (pthread_join(p_ctx->keypad_thr, NULL) != kSuccess)
    {
        fprintf(stderr, "Can not join keypad thread.\n");
    }

}

int connect_server(EDC_CTX* p_ctx)
{
    int conn_ret;
    int sel_ret;
    int sock;
    struct sockaddr_in addr;

    struct timeval timeout;
    fd_set conn_fds;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(p_ctx->server_port);
    if ((addr.sin_addr.s_addr = inet_addr(p_ctx->server_ip)) == INADDR_NONE)
    {
        fprintf(stderr, "Server setup error.\n");
        return kFailure;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "Open socket error.\n");
        return kFailure;
    }

    if (set_nonblock(sock, kTrue) != kSuccess)
    {
        fprintf(stderr, "Set socket non-block error.\n");
        return kFailure;
    }

    if ((conn_ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr))) == -1)
    {
        if (errno != EINPROGRESS)
        {
            fprintf(stderr, "Establish connect failure(%d):\
                    %s.\n", conn_ret, strerror(errno));
            return kFailure;
        }
        else
        {
            while (kTrue)
            {
                FD_ZERO(&conn_fds);
                FD_SET(sock, &conn_fds);
                timeout.tv_sec = 5;
                timeout.tv_usec = 0;

                sel_ret = select(sock + 1,
                        NULL, &conn_fds, NULL, &timeout);
                if (sel_ret < 0 && errno != EINTR)
                {
                    fprintf(stderr, "Connect failure: %s.\n",\
                            strerror(errno));
                    return kFailure;
                }
                else if (sel_ret == 0)
                {
                    fprintf(stderr, "Connect timeout.\n");
                    return kFailure;
                }
                else if (FD_ISSET(sock, &conn_fds))
                {
                    int valopt;
                    socklen_t int_size = sizeof(socklen_t);
                    if (getsockopt(sock, SOL_SOCKET, SO_ERROR,
                            (void*)&valopt, &int_size) < 0)
                    {
                        fprintf(stderr, "Get sockopt failure\
                            (%d): %s.\n", errno, strerror(errno));
                        return kFailure;
                    }

                    if (valopt == 0)
                    {
                        break;
                    }

                    fprintf(stderr, "Connect failure, valopt:%d.\n", valopt);
                    return kFailure;
                }
            }

        }
    }

    if (set_nonblock(sock, kFalse) != kSuccess)
    {
        fprintf(stderr, "Set socket to block mode failure.\n");
        return kFailure;
    }

    fprintf(stderr, "Connect to server success.\n");
    p_ctx->server_fd = sock;
    p_ctx->connected = kTrue;

    return kSuccess;
}

int disconnect_server(EDC_CTX *p_ctx)
{
    shutdown(p_ctx->server_fd, 2);
    p_ctx->connected = kFalse;

    return kSuccess;
}

int set_nonblock(int sock, int nonblock)
{
    int flags;
    flags = fcntl(sock, F_GETFL, 0);

    if (flags == -1)
    {
        fprintf(stderr, "Get flag failure.\n");
        return kFailure;
    }

    if (nonblock)
    {
        flags |= O_NONBLOCK;
    }
    else
    {
        flags ^= O_NONBLOCK;
    }

    if (fcntl(sock, F_SETFL, flags) == -1)
    {
        fprintf(stderr, "Set flag failure.\n");
        return kFailure;
    }

    return kSuccess;
}

int load_server_set(EDC_CTX *p_ctx, const char *ini_file)
{
    FILE *fpSetting;
    char line[kMaxReadLineLen];
    int len;
    //char ipv4[kMaxIPLen + 1];
    char num_tmp[kMaxPortLen + 1];
    //char edc_id[kMaxEDCIDLen + 1];
    int port;
    int com;
    char *end_ptr;

    if (!ini_file)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if (!(fpSetting = fopen(ini_file, "r")))
    {
        fprintf(stderr, "Can not open server ini file: %s.\n", ini_file);
        return RET_ERR_SETUP_FILE;
    }

    if (!feof(fpSetting))
    {
        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, num_tmp, &len);
        com = strtol(num_tmp, &end_ptr, 10);
        if (*end_ptr != '\0')
        {
            fprintf(stderr, "COM setup in INI file error: %s\n", ini_file);
            return kFailure;
        }
        p_ctx->prt_con_type = com;

        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, p_ctx->server_ip, &len);

        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, num_tmp, &len);
        port = strtol(num_tmp, &end_ptr, 10);
        if (*end_ptr != '\0')
        {
            fprintf(stderr, "Port setup in INI file error!\n");
            return kFailure;
        }
        p_ctx->server_port = port;

        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, p_ctx->edc_id, &len);

        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, p_ctx->fn_passwd, &len);
    }
    fclose(fpSetting);

    fprintf(stderr, "Server ip: %s\n", p_ctx->server_ip);
    fprintf(stderr, "Server port: %d\n", p_ctx->server_port);
    fprintf(stderr, "id: %s\n", p_ctx->edc_id);
    fprintf(stderr, "passwd: %s\n", p_ctx->fn_passwd);

    return kSuccess;
}

int load_network_set(EDC_CTX *p_ctx, const char *ini_file)
{
    FILE *fpSetting;
    char line[kMaxReadLineLen];
    int len;

    if (!ini_file)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if (!(fpSetting = fopen(ini_file, "r")))
    {
        fprintf(stderr, "Can not open network ini file: %s.\n", ini_file);
        return RET_ERR_SETUP_FILE;
    }

    if (!feof(fpSetting))
    {
        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, p_ctx->edc_ip, &len);

        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, p_ctx->submask, &len);

        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, p_ctx->gateway, &len);
    }
    fclose(fpSetting);

    fprintf(stderr, "EDC ip: %s\n", p_ctx->edc_ip);
    fprintf(stderr, "Submask: %s\n", p_ctx->submask);
    fprintf(stderr, "Gateway: %s\n", p_ctx->gateway);

    return kSuccess;
}

int dl_remote_list(EDC_CTX *p_ctx, const char* sync_cmd, const char* file_name)
{
    int sock;

    char send_buf[kMaxReadLineLen];
    int send_len;

    char list_buf[kMaxEmpListBuf];
    int total_recv;

    FILE *fp_list;

    if (!p_ctx || !sync_cmd || !file_name)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if (!p_ctx->connected)
    {
        fprintf(stderr, "Socket disconnect!\n");
        return kFailure;
    }

    sock = p_ctx->server_fd;
    memset(send_buf, 0, kMaxReadLineLen);

    send_len = snprintf(send_buf, kMaxReadLineLen,
            "%s\t%s\n", sync_cmd, p_ctx->edc_id);

    if (sock_write(sock, send_buf, send_len) != send_len)
    {
        fprintf(stderr, "Send request of lists to server fail.\n");
        p_ctx->connected = kFalse;
        return kFailure;
    }

    // If database of server is empty, there will return 0
    // and the list file will be a empty file
    if ((total_recv = sock_read(sock,
            list_buf, kMaxEmpListBuf)) < 0)
    {
        fprintf(stderr, "Read response from server fail.\n");
        return kFailure;
    }

    if(!(fp_list = fopen(file_name, "w")))
    {
        fprintf(stderr, "Can not open local temp list.\n");
        return kFailure;
    }

    int ret = fprintf(fp_list, "%s", list_buf);
    if (ret < total_recv)
    {
        fprintf(stderr, "Write to temp list failure:%d.\n", ret);
        fclose(fp_list);
        return kFailure;
    }
    fclose(fp_list);
    return kSuccess;
}

size_t sock_write(int sock, const void *buf, size_t buf_len)
{
    size_t    nleft;
    ssize_t    send_len;
    const char  *ptr;

    ptr = (char *)buf;
    nleft = buf_len;
    while (nleft > 0)
    {
        if ((send_len = send(sock, ptr, nleft, 0)) < 0)
        {
            fprintf(stderr, "Send data failure!\n");
            break;
        }

        nleft -= send_len;
        ptr   += send_len;
    }

    return (buf_len - nleft);
}

size_t sock_read(int sock, char* buffer, size_t buf_len)
{
    int sel_ret;
    char recv_buf[kMaxBufferlen];
    int recv_len;
    char *recv_ptr;     // data start point (no header)
    char *curr = buffer; //write point of buffer

    int total_recv = 0;
    int head_offset = 0;
    int content_len = 0;
    char content_len_str[kMaxConLenStrLen];
    struct timeval timeout;
    fd_set recv_fds;

    if (!sock || !buffer || !buf_len)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    memset(recv_buf, 0, kMaxBufferlen);
    memset(content_len_str, 0, kMaxConLenStrLen);

    while (kTrue)
    {
        timeout.tv_sec = kMaxSockTimeout;
        timeout.tv_usec = 0;
        FD_ZERO(&recv_fds);
        FD_SET(sock, &recv_fds);
        sel_ret = select(sock + 1, &recv_fds,
                NULL, NULL, &timeout);
        if (sel_ret == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                fprintf(stderr, "select failure");
                return kFailure;
            }
        }
        else if (sel_ret == 0)
        {
            fprintf(stderr, "Timeout\n");
            return kFailure;

        }
        else if (FD_ISSET(sock, &recv_fds))
        {
            recv_ptr = recv_buf;
            recv_len = recv(sock, recv_buf, kMaxBufferlen, 0);
            if (recv_len == 0)
            {
                // Disconnect
                fprintf(stderr, "Server disconnect.\n");
                return kFailure;
            }
            else if(recv_len < 0)
            {
                if (errno == EINTR)
                {
                    continue;
                }

                fprintf(stderr, "Read socket failure.\n");
                return kFailure;
            }
            else
            {
                //fprintf(stderr, "CLD: recv: %s(%d)\n", recv_buf, recv_len);
                if (content_len == 0)
                {
                    // 1st read
                    head_offset = get_str_before_char(
                        (const char*)recv_buf,
                        kSep, content_len_str, kMaxConLenStrLen);
                    //fprintf(stderr, "CLD: content-length_str: %s\n", content_len_str);
                    content_len =
                        (int)strtol(content_len_str, NULL, 10);
                    recv_ptr += (head_offset + 1);
                    recv_len -= head_offset + 1;
                }

                total_recv += recv_len;
                if (total_recv > buf_len - 1)
                {
                    recv_len = total_recv - (buf_len -1);
                    //fprintf(stderr, "List buffer too small.\n");
                    return kFailure;
                }
                strncpy(curr, recv_ptr, recv_len);
                curr += recv_len;
                *curr = '\0';

                //fprintf(stderr, "CLD: total_recv:%d, content-length: %d\n", total_recv, content_len);
                if (content_len >= 0 && total_recv >= content_len)
                {
                    //Get all data due to exceed contetn_length
                    break;
                }
            }
        }
        else
        {
            fprintf(stderr, "Should not here\n");
            return kFailure;
        }
    }

    return total_recv;
}

int idle_state(EDC_CTX *p_ctx)
{
    //int remain_sec;
    char *in_pos = p_ctx->project_code;
    unsigned char in_key;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if (lcd_clean_scr(p_ctx->lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Can not clean screen.\n");
        return kFailure;
    }

    if (lcd_clean(p_ctx->lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Can not clean screen buffer.\n");
        return kFailure;
    }

    if (set_led(kLEDBlue) != kSuccess)
    {
        fprintf(stderr, "Can not set LED.\n");
        return kFailure;
    }

    show_line(p_ctx, 1, STR_PLEASE_CARD);
    //show_line(p_ctx, 3, STR_EMPOLYEE_ID);

    // Initial user data
    memset(&p_ctx->curr_card_sn, 0, kMaxCardSNLen);
    memset(&p_ctx->project_code, 0, kMaxProjectCodeLen + 1);
    p_ctx->curr_emp_idx = 0;
    while (kTrue)
    {   
        if (show_datetime(p_ctx) != kSuccess)
        {
            return kFailure;
        }

        //catch input and update screen
        if (get_press_key(p_ctx, &in_key) != kSuccess)
        {
            //fprintf(stderr, "Can not get keypad.\n");
            return kFailure;
        }
        else
        {
            if (in_key != 0 )
            {
                if (in_key == kASCIIFn)
                {
                    p_ctx->state = PASSWD;
                    break;
                }
                else if (in_key == kASCIIClear 
                    && in_pos > p_ctx->project_code)
                {
                    *--in_pos = '\0';
                }
                else if (in_key == kASCIICancel)
                {
                    in_pos = p_ctx->project_code;
                    *in_pos = '\0';
                }
                else if (in_key >= kASCII_zero && in_key <= kASCII_nine
                        && in_pos - p_ctx->project_code < kMaxProjectCodeLen)
                {
                    *in_pos++ = in_key;
                    *in_pos = '\0';
                }

                //fprintf(stderr, "project code: %s\n", p_ctx->project_code);

                show_line(p_ctx, 2, p_ctx->project_code);
            }
        }

        //catch carding
        serWriteCOM(&p_ctx->com_ctx, kSendComSignal, sizeof(kSendComSignal));
        usleep(kMicroPerSecond / 10);
        if (read_rfid(p_ctx) == kSuccess)
        {
            fprintf(stderr, "Card: %s\n", p_ctx->curr_card_sn);
            if (is_valid_card(p_ctx))
            {
                p_ctx->state = QUOTA;
                buzzer((kMicroPerSecond / 10) * 1);
            }
            else
            {
                p_ctx->state = INVALID_CARD;
            }
            break;
        }
        usleep(kMicroPerSecond / 10);

    }
    return kSuccess;
}

int invalid_card_state(EDC_CTX *p_ctx)
{
    char edc_log[kMaxEDCLogLen];

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if(lcd_clean_scr(p_ctx->lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Can not clean screen.\n");
        return kFailure;
    }

    if(lcd_clean(p_ctx->lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Can not clean screen buffer.\n");
        return kFailure;
    }

    if (set_led(kLEDRed) != kSuccess)
    {
        fprintf(stderr, "Can not set LED.\n");
        return kFailure;
    }

    snprintf(edc_log, kMaxEDCLogLen, PTN_CARD_INVALID, p_ctx->curr_card_sn);
    if (append_edc_log(p_ctx, CARD, edc_log) != kSuccess)
    {
        fprintf(stderr, "Append EDC log failure\n");
        return kFailure;
    }

    show_datetime(p_ctx);
    show_line(p_ctx, 1, STR_INVALID_CARD);
    show_line(p_ctx, 2, STR_CONTACT_STAFF);

    memset(&p_ctx->curr_card_sn, 0, kMaxCardSNLen);

    if (set_backlight(p_ctx, 0xff))
    {
        fprintf(stderr, "Can not set backlight.\n");
        return kFailure;
    }

    buzzer((kMicroPerSecond / 10) * 1);
    buzzer((kMicroPerSecond / 10) * 1);
    usleep(kMicroPerSecond * 5);

    if (set_backlight(p_ctx, 0x00))
    {
        fprintf(stderr, "Can not set backlight.\n");
        return kFailure;
    }

    p_ctx->state = IDLE;
    return kSuccess;
}

int quota_state(EDC_CTX* p_ctx)
{
    char remain_sec[kMaxLineWord + 1];
    char remain_line[kMaxLineWord + 1];
    char action_type[kMaxLineWord + 1];
    EMPLOYEE_DATA *curr_emp;
    EDC_DATA *curr_edc;
    int utime_remain;
    PRINTERCOUNT_V2 ptr_counter;
    PRINTERTYPE print_usage;
    PRINTERTYPE photocopy_usage;
    PRINTERTYPE empty_usage;
    int cur_quota;
    int enject_or_timeout = 0;
    int stage = 0; //0: stop, 1: start
    int scan_flag = kFalse;

    int gb = 0;
    int gs = 0;
    int cb = 0;
    int cs = 0;

    unsigned char in_key;

    char edc_log[kMaxEDCLogLen];

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if (p_ctx->project_code[0] == '\0')
    {
        snprintf(p_ctx->project_code, kMaxProjectCodeLen + 1, "%s", "0000");
    }
    fprintf(stderr, "Project code: %s.\n", p_ctx->project_code);

    if (set_led(kLEDGreen) != kSuccess)
    {
        fprintf(stderr, "Can not set LED.\n");
        return kFailure;
    }

    snprintf(edc_log, kMaxEDCLogLen, PTN_CARD_VALID, p_ctx->curr_card_sn);
    if (append_edc_log(p_ctx, CARD, edc_log) != kSuccess)
    {
        fprintf(stderr, "Append EDC log failure\n");
        return kFailure;
    }

    if (p_ctx->prt_con_type != 0)
    {
        if (ptr_select(p_ctx->prt_con_type) != kSuccess)
        {
            fprintf(stderr, "Set COM port printer failure.\n");
        }
    }

    // is_valid_card() will prepare curr_emp_idx
    curr_emp = &(p_ctx->emp_list[p_ctx->curr_emp_idx]);
    curr_edc = &(p_ctx->edc_list[0]);
    utime_remain = curr_edc->limit_time * kMicroPerSecond;

    // Enable all backlight
    if (set_backlight(p_ctx, 0xff))
    {
        fprintf(stderr, "Can not open backlight when touch card.\n");
        return kFailure;
    }
    p_ctx->backlight_flag = kTrue;

    // Init print, start to statistic
    if (ptr_count_init(p_ctx->lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Initial print counter failure\n");
        return kFailure;
    }

    init_printertype(&(ptr_counter.photocopy));
    init_printertype(&(ptr_counter.print));
    init_printertype(&(ptr_counter.fax));
    init_printertype(&(ptr_counter.scan));
    init_printertype(&photocopy_usage);
    init_printertype(&print_usage);
    init_printertype(&empty_usage);

    cur_quota = curr_emp->curr_quota;
    if (show_quota_info(p_ctx, cur_quota, gb, gs, cb, cs) != kSuccess)
    {
        fprintf(stderr, "Show quota screen failure\n");
        return kFailure;
    }

    while (kTrue)
    {
        //catch input and update screen
        if (get_press_key(p_ctx, &in_key) < 0)
        {
            //fprintf(stderr, "Can not get keypad.\n");
            return kFailure;
        }
        else
        {
            if (in_key == kASCIIEject )
            {
                //User input ENJECT, end statistic
                enject_or_timeout = 1;
                break;
            }
        }

        //Get statistic info from printer
        if (ptr_count_get(p_ctx->lkp_ctx, &ptr_counter) != kSuccess)
        {
            fprintf(stderr, "Get print counter failure\n");
            return kFailure;
        }


        /*
         * How to detect scan?
         * Due to ptr_count_get() can only provide correct status of scan
         * Use below algorithm to detect scan occurring:
         * 1. If u8_work_status change from 0 to 1,
         *    This mean some action is done.
         *    Check every counter, if neither of them be changed,
         *    We can judge this action is scan.
         * 2. If counter of scan is changed,
         *    Simpliy we know scan is occured.
         */
        if ((ptr_counter.u8_work_status & 0x01) == 1)
        {
            if (stage == 1
                && usage_same_as(&(ptr_counter.photocopy), &empty_usage) == kFalse
                && usage_same_as(&(ptr_counter.print), &empty_usage) == kFalse
                && usage_same_as(&(ptr_counter.scan), &empty_usage) == kFalse)
            {
                scan_flag = kTrue;
                stage = 0;
            }

            // Operation end, continue countdown
            utime_remain -= kMicroPerSecond / 10;
            if (utime_remain <= 0)
            {
                p_ctx->state = IDLE;
                break;
            }
        }
        else if ((ptr_counter.u8_work_status & 0x01) == 0)
        {
            // Operating
            utime_remain = curr_edc->limit_time * kMicroPerSecond;

            count2cost(&ptr_counter, curr_edc->paper_size_a, curr_edc->paper_size_b,
                    &gb, &gs, &cb, &cs);
            if (usage_same_as(&(ptr_counter.scan), &empty_usage) == kTrue)
            {
                scan_flag = kTrue;
                snprintf(action_type, kMaxLineWord + 1, "S");
            }

            if (!usage_same_as(&(ptr_counter.photocopy), &photocopy_usage) == kTrue)
            {
                snprintf(action_type, kMaxLineWord + 1, "C");
            }
            usage_dup(&(ptr_counter.photocopy), &photocopy_usage);

            if (!usage_same_as(&(ptr_counter.print), &print_usage) == kTrue)
            {
                snprintf(action_type, kMaxLineWord + 1, "P");
            }
            usage_dup(&(ptr_counter.print), &print_usage);

            cur_quota = curr_emp->curr_quota
                        - gb * curr_edc->mono_a3
                        - gs * curr_edc->mono_a4
                        - cb * curr_edc->color_a3
                        - cs * curr_edc->color_a4;
            fprintf(stderr, "CLD: cur_quota: %d, gb:%d, gs:%d, cb:%d, cs:%d\n",
                    cur_quota, gb, gs, cb ,cs);
            if (show_quota_info(p_ctx, cur_quota, gb, gs, cb, cs) != kSuccess)
            {
                fprintf(stderr, "Show quota screen failure\n");
                return kFailure;
            }
        }
        
        usleep(kMicroPerSecond / 10);
        snprintf(remain_sec, kMaxLineWord + 1, "%s: %d",
                STR_REMAIN_SEC, (int)(utime_remain / kMicroPerSecond));

        left_right_str(remain_line, kMaxLineWord + 1, remain_sec, action_type);
        show_line(p_ctx, 3, remain_line);
    }

    // Get use status
    //if ( (ptr_counter.u8_work_status & 0x01) == 1 ) {}
    // Get paper-track status
    //if ( (ptr_counter.u8_work_status & 0x02) == 1) {}

    //print_printertype(&(ptr_counter.photocopy));
    
    init_printertype(&empty_usage);
    if (!usage_same_as(&ptr_counter.photocopy, &empty_usage) 
            && gen_cost_log(p_ctx, COPY, &ptr_counter.photocopy) != kSuccess)
    {
        fprintf(stderr, "Generate EDC log of copy failure\n");
        return kFailure;
    }

    init_printertype(&empty_usage);
    if (!usage_same_as(&ptr_counter.print, &empty_usage) 
            && gen_cost_log(p_ctx, PRINT, &ptr_counter.print) != kSuccess)
    {
        fprintf(stderr, "Generate EDC log of print failure\n");
        return kFailure;
    }

    if (scan_flag == kTrue
            && gen_cost_log(p_ctx, SCAN, &ptr_counter.scan) != kSuccess)
    {
        fprintf(stderr, "Generate EDC log of scan failure\n");
        return kFailure;
    }

    //release print
    if (ptr_count_stop(p_ctx->lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Stop print counter failure\n");
        return kFailure;
    }

    if (enject_or_timeout == 1)
    {
        snprintf(edc_log, kMaxEDCLogLen, PTN_CARD_ENJECT, p_ctx->curr_card_sn);
    }
    else
    {
        snprintf(edc_log, kMaxEDCLogLen, PTN_CARD_TIMEOUT, p_ctx->curr_card_sn);
    }

    if (append_edc_log(p_ctx, CARD, edc_log) != kSuccess)
    {
        fprintf(stderr, "Append EDC log failure\n");
        return kFailure;
    }

    p_ctx->state = IDLE;
    return kSuccess;
}

int show_quota_info(EDC_CTX *p_ctx, int quota, int gb, int gs, int cb, int cs)
{
    char quota_str[kMaxLineWord + 1];
    char quota_line[kMaxLineWord + 1];
    char mono_line[kMaxLineWord + 1];
    char color_line[kMaxLineWord + 1];

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }


    snprintf(quota_str, kMaxLineWord + 1, "%s:%d",
            STR_QUOTA, quota);

    if (left_right_str(quota_line, kMaxLineWord + 1, quota_str, p_ctx->edc_id) != kSuccess)
    {
        fprintf(stderr, "Buffer too small\n");
        return kFailure;
    }

    snprintf(mono_line, kMaxLineWord, "%s%d %s%d",
            STR_MONO_A3, gb,
            STR_MONO_A4, gs);

    snprintf(color_line, kMaxLineWord, "%s%d %s%d",
            STR_COLOR_A3, cb,
            STR_COLOR_A4, cs);

    show_line(p_ctx, 0, quota_line);
    show_line(p_ctx, 1, mono_line);
    show_line(p_ctx, 2, color_line);

    return kSuccess;
}

int print_printertype(PRINTERTYPE *usage)
{
    int i;

    for(i = 0; i < kMaxPrtPage; i++)
    {
        fprintf(stderr, "%d %d %d %d %d %d %d %d\n",
                usage->u16_gray_scale_a[i],
                usage->u16_gray_scale_b[i],
                usage->u16_color_a[i],
                usage->u16_color_b[i],
                usage->u16_double_gray_scale_a[i],
                usage->u16_double_gray_scale_b[i],
                usage->u16_double_color_a[i],
                usage->u16_double_color_b[i]);
    }

    return kSuccess;
}

int init_printertype(PRINTERTYPE *usage)
{
    int i;
    for(i = 0; i < kMaxPrtPage; i++)
    {
        usage->u16_gray_scale_a[i] = 0;
        usage->u16_gray_scale_b[i] = 0;
        usage->u16_color_a[i] = 0;
        usage->u16_color_b[i] = 0;
        usage->u16_double_gray_scale_a[i] = 0;
        usage->u16_double_gray_scale_b[i] = 0;
        usage->u16_double_color_a[i] = 0;
        usage->u16_double_color_b[i] = 0;
    }

    return 0;
}

int count2cost(PRINTERCOUNT_V2 *ptr_counter, int paper_size_a, int paper_size_b, 
        int *gray_big, int *gray_small, int *color_big, int *color_small)
{
    PRINTERTYPE *print;
    PRINTERTYPE *copy;
    int i;

    if (!ptr_counter || !gray_big || !gray_small || !color_big || !color_small)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    print = &(ptr_counter->print);
    copy = &(ptr_counter->photocopy);
    *gray_big = 0;
    *gray_small = 0;
    *color_big = 0;
    *color_small = 0;

    // Count print cost
    for (i = 0; i < kMaxPrtPage; i++)
    {
        //fprintf(stderr, "- %d - \n", i);
        if (i < paper_size_a)
        {
            //fprintf(stderr, "--- print a big ---\n");
            *gray_big += print->u16_gray_scale_a[i] + print->u16_double_gray_scale_a[i] * 2;
            *color_big += print->u16_color_a[i] + print->u16_double_color_a[i] * 2;
        }
        else
        {
            //fprintf(stderr, "--- print a small ---\n");
            *gray_small += print->u16_gray_scale_a[i] + print->u16_double_gray_scale_a[i] * 2;
            *color_small += print->u16_color_a[i] + print->u16_double_color_a[i] * 2;
        }

        if (i < paper_size_b)
        {
            //fprintf(stderr, "--- print b small ---\n");
            *gray_big += print->u16_gray_scale_b[i] + print->u16_double_gray_scale_b[i] * 2;
            *color_big += print->u16_color_b[i] + print->u16_double_color_b[i] * 2;
        }
        else
        {
            //fprintf(stderr, "--- print b big ---\n");
            *gray_small += print->u16_gray_scale_b[i] + print->u16_double_gray_scale_b[i] * 2;
            *color_small += print->u16_color_b[i] + print->u16_double_color_b[i] * 2;
        }
    }

    // Count copy cost
    for (i=0; i < kMaxPrtPage; i++)
    {
        //fprintf(stderr, "- %d - \n", i);
        if (i < paper_size_a)
        {
            //fprintf(stderr, "--- copy a big ---\n");
            *gray_big += copy->u16_gray_scale_a[i] + copy->u16_double_gray_scale_a[i] * 2;
            *color_big += copy->u16_color_a[i] + copy->u16_double_color_a[i] * 2;
        }
        else
        {
            //fprintf(stderr, "--- copy a small ---\n");
            *gray_small += copy->u16_gray_scale_a[i] + copy->u16_double_gray_scale_a[i] * 2;
            *color_small += copy->u16_color_a[i] + copy->u16_double_color_a[i] * 2;
        }

        if (i < paper_size_b)
        {
            //fprintf(stderr, "--- copy b big ---\n");
            *gray_big += copy->u16_gray_scale_b[i] + copy->u16_double_gray_scale_b[i] * 2;
            *color_big += copy->u16_color_b[i] + copy->u16_double_color_b[i] * 2;
        }
        else
        {
            //fprintf(stderr, "--- copy b small ---\n");
            *gray_small += copy->u16_gray_scale_b[i] + copy->u16_double_gray_scale_b[i] * 2;
            *color_small += copy->u16_color_b[i] + copy->u16_double_color_b[i] * 2;
        }
    }

    return kSuccess;
}

int usage_dup(PRINTERTYPE *p_src, PRINTERTYPE *p_dest)
{
    int i;

    if (!p_src || ! p_dest)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    for (i = 0; i < kMaxPrtPage; i++)
    {
        p_dest->u16_gray_scale_a[i] = p_src->u16_gray_scale_a[i];
        p_dest->u16_gray_scale_b[i] = p_src->u16_gray_scale_b[i];
        p_dest->u16_color_a[i] = p_src->u16_color_a[i];
        p_dest->u16_color_b[i] = p_src->u16_color_b[i];
        p_dest->u16_double_gray_scale_a[i] = p_src->u16_double_gray_scale_a[i];
        p_dest->u16_double_gray_scale_b[i] = p_src->u16_double_gray_scale_b[i];
        p_dest->u16_double_color_a[i] = p_src->u16_double_color_a[i];
        p_dest->u16_double_color_b[i] = p_src->u16_double_gray_scale_b[i];
    }

    return kTrue;
}

int usage_same_as(PRINTERTYPE *p1, PRINTERTYPE *p2)
{
    int i;

    if (!p1 || ! p2)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    for (i = 0; i < kMaxPrtPage; i++)
    {
        if (p1->u16_gray_scale_a[i] != p2->u16_gray_scale_a[i] ||
            p1->u16_gray_scale_b[i] != p2->u16_gray_scale_b[i] ||
            p1->u16_color_a[i] != p2->u16_color_a[i] ||
            p1->u16_color_b[i] != p2->u16_color_b[i] ||
            p1->u16_double_gray_scale_a[i] != p2->u16_double_gray_scale_a[i] ||
            p1->u16_double_gray_scale_b[i] != p2->u16_double_gray_scale_b[i] ||
            p1->u16_double_color_a[i] != p2->u16_double_color_a[i] ||
            p1->u16_double_color_b[i] != p2->u16_double_gray_scale_b[i] )
        {
            return kFalse;
        }
    }

    return kTrue;
}

int gen_cost_log(EDC_CTX* p_ctx, const EDC_LOG_TYPE log_type, PRINTERTYPE *usage)
{
    //NOTE! kMaxEDCLogLen maybe not long enough when many pages is not 0
    char edc_log_content[kMaxEDCLogLen];
    char temp_str[kMaxEDCLogLen];
    int gray_a_sum = 0;
    int gray_b_sum = 0;
    int color_a_sum = 0;
    int color_b_sum = 0;
    int i;

    if (!p_ctx || !usage)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    memset(edc_log_content, 0, kMaxEDCLogLen);
    memset(temp_str, 0, kMaxEDCLogLen);

    for (i = 0; i < kMaxPrtPage; i++)
    {
        gray_a_sum = usage->u16_gray_scale_a[i] + usage->u16_double_gray_scale_a[i] * 2;
        gray_b_sum = usage->u16_gray_scale_b[i] + usage->u16_double_gray_scale_b[i] * 2;
        color_a_sum = usage->u16_color_a[i] + usage->u16_double_color_a[i] * 2;
        color_b_sum = usage->u16_color_b[i] + usage->u16_double_color_b[i] * 2;

        if (gray_a_sum != 0)
        {
            snprintf(temp_str, kMaxEDCLogLen, PTN_PAPER_GRAY_A, i, gray_a_sum);
            strncat(edc_log_content, temp_str, kMaxEDCLogLen);
        }

        if (gray_b_sum != 0)
        {
            snprintf(temp_str, kMaxEDCLogLen, PTN_PAPER_GRAY_B, i, gray_b_sum);
            strncat(edc_log_content, temp_str, kMaxEDCLogLen);
        }

        if (color_a_sum != 0)
        {
            snprintf(temp_str, kMaxEDCLogLen, PTN_PAPER_COLOR_A, i, color_a_sum);
            strncat(edc_log_content, temp_str, kMaxEDCLogLen);
        }

        if (color_b_sum != 0)
        {
            snprintf(temp_str, kMaxEDCLogLen, PTN_PAPER_COLOR_B, i, color_b_sum);
            strncat(edc_log_content, temp_str, kMaxEDCLogLen);
        }
    }

    if (append_edc_log(p_ctx, log_type, edc_log_content))
    {
        fprintf(stderr, "Append EDC log failure\n");
        return kFailure;
    }

    return kSuccess;
}

int scanning_state(EDC_CTX* p_ctx)
{
    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    return kSuccess;
}


int passwd_state(EDC_CTX* p_ctx)
{
    unsigned char in_key;
    char in_passwd[kMaxPasswdLen + 1];
    char *ptr = in_passwd;

    int bl_usec = 0;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if (lcd_clean_scr(p_ctx->lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Can not clean screen.\n");
        return kFailure;
    }

    if (lcd_clean(p_ctx->lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Can not clean screen buffer.\n");
        return kFailure;
    }

    memset(in_passwd, 0, kMaxPasswdLen + 1);

    show_line(p_ctx, 0, STR_SETUP_INPUT_PASSWD);

    while (kTrue)
    {
        //catch input and update screen
        if (get_press_key(p_ctx, &in_key) < 0)
        {
            return kFailure;
        }
        else
        {
            if (in_key != 0)
            {
                if (in_key >= kASCIIFirstVisiable && in_key <= kASCIILastVisiable
                        && ptr - in_passwd < kMaxPasswdLen)
                {
                    *ptr++ = in_key;
                    *ptr = '\0';
                }
                else if (in_key == kASCIIClear && ptr > in_passwd)
                {
                    *--ptr = '\0';
                }
                else if (in_key == kASCIICancel)
                {
                    p_ctx->state = IDLE;
                    return kSuccess;
                }
                else if (in_key == kASCIIEnter)
                {
                    break;
                }
                show_line(p_ctx, 2, in_passwd);

                // Enable all backlight
                if (set_backlight(p_ctx, 0xff))
                {
                    fprintf(stderr, "Can not open backlight in idle.\n");
                    return kFailure;
                }
                bl_usec = 0;
            }
        }

        if (bl_usec > kMicroPerSecond * kLCDBackLightTimeout)
        {
            if (set_backlight(p_ctx, 0x00))
            {
                fprintf(stderr, "Can not close backlight in idle when timeout.\n");
                return kFailure;
            }
            bl_usec = 0;
        }
        bl_usec += kMicroPerSecond / 10;

        usleep(kMicroPerSecond / 10);
    }

    if (strncmp(in_passwd, p_ctx->fn_passwd, kMaxPasswdLen) == 0)
    {
        p_ctx->state = SETUP;
    }
    else
    {
        show_line(p_ctx, 2, STR_SETUP_PASSWD_ERROR);
        buzzer(kMicroPerSecond / 10);
        usleep(kMicroPerSecond / 2);
        p_ctx->state = IDLE;
    }

    return kSuccess;
}

int setup_state(EDC_CTX* p_ctx)
{
    int new_prt_con_type;
    char new_prt_con_type_str[kMaxConnectTypeLen + 1];
    char new_edc_id[kMaxEDCIDLen + 1];
    char new_edc_ip[kMaxIPLen + 1];
    char new_submask[kMaxIPLen + 1];
    char new_gateway[kMaxIPLen + 1];
    char new_server_ip[kMaxIPLen + 1];
    char new_server_port_str[kMaxPortLen + 1];
    char new_fn_passwd[kMaxPasswdLen + 1];
    int new_server_port;
    char *end_ptr;

    FILE *fp_setup;
    FILE *fp_network;
    int ret;

    FN_STATE state = SET_PRT_TYPE;
    int state_ret;
    unsigned char in_key;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if (stop_sync_thr(p_ctx) != kSuccess)
    {
        fprintf(stderr, "Can't stop threads.\n");
        return kFailure;
    }

    // Clean screen and set LED
    if(lcd_clean_scr(p_ctx->lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Can not clean screen.\n");
        return kFailure;
    }

    if(lcd_clean(p_ctx->lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Can not clean screen buffer.\n");
        return kFailure;
    }

    if (set_led(kLEDBlue) != kSuccess)
    {
        fprintf(stderr, "Can not set LED.\n");
        return kFailure;
    }

    snprintf(new_prt_con_type_str, kMaxConnectTypeLen + 1, "%d", p_ctx->prt_con_type);
    new_prt_con_type = p_ctx->prt_con_type;
    strncpy(new_edc_id, p_ctx->edc_id, kMaxEDCIDLen + 1);
    strncpy(new_edc_ip, p_ctx->edc_ip, kMaxIPLen + 1);
    strncpy(new_submask, p_ctx->submask, kMaxIPLen + 1);
    strncpy(new_gateway, p_ctx->gateway, kMaxIPLen + 1);
    strncpy(new_server_ip, p_ctx->server_ip, kMaxIPLen + 1);
    snprintf(new_server_port_str, kMaxPortLen + 1, "%d", p_ctx->server_port);
    strncpy(new_fn_passwd, p_ctx->fn_passwd, kMaxPasswdLen + 1);
    new_server_port = p_ctx->server_port;

    state = SET_PRT_TYPE;
    //TODO so dirty here...
    while (kTrue)
    {
        fprintf(stderr, "state: %d\n", state);
        switch (state)
        {
            case SET_PRT_TYPE:
                while (kTrue)
                {
                    show_line(p_ctx, 0, STR_SETUP_PRT_CON_TYPE);
                    show_line(p_ctx, 3, STR_EMPTY);
                    state_ret = get_str_from_keypad(p_ctx, STR_EMPTY,
                            new_prt_con_type_str, kMaxConnectTypeLen + 1, 1);
                    //fprintf(stderr, "ret: %d, value: %s\n", state_ret, new_prt_con_type_str);
                    if (state_ret != kFailure)
                    {
                        new_prt_con_type = (int)strtol(new_prt_con_type_str, &end_ptr, 10);
                        if (*end_ptr != '\0'
                                || new_prt_con_type < kConnectTypeMin
                                || new_prt_con_type > kConnectTypeMax)
                        {
                            show_line(p_ctx, 3, STR_SETUP_ERROR);
                            buzzer((kMicroPerSecond / 10) * 1);
                            usleep(kMicroPerSecond / 2);
                            continue;
                        }
                        break;
                    }
                }
                break;
            case SET_EDC_ID:
                show_line(p_ctx, 0, STR_SETUP_EDC_ID);
                state_ret = get_str_from_keypad(p_ctx, STR_EMPTY,
                        new_edc_id, kMaxEDCIDLen + 1, 1);
                //fprintf(stderr, "ret: %d, value: %s\n", state_ret, new_edc_id);
                break;
            case SET_EDC_IP:
                while (kTrue)
                {
                    show_line(p_ctx, 0, STR_SETUP_EDC_IP);
                    show_line(p_ctx, 3, STR_EMPTY);
                    state_ret = get_ipv4_from_keypad(p_ctx, STR_EMPTY,
                            new_edc_ip, kMaxIPLen + 1, 1);
                    //fprintf(stderr, "ret: %d, value: %s\n", state_ret, new_edc_ip);
                    if (state_ret != kFailure)
                    {
                        if (inet_addr(new_edc_ip) == kFailure)
                        {
                            show_line(p_ctx, 3, STR_SETUP_ERROR);
                            buzzer((kMicroPerSecond / 10) * 1);
                            usleep(kMicroPerSecond / 2);
                            continue;
                        }
                        break;
                    }
                }
                break;
            case SET_SUBMASK:
                while (kTrue)
                {
                    show_line(p_ctx, 0, STR_SETUP_SUBMASK);
                    show_line(p_ctx, 3, STR_EMPTY);
                    state_ret = get_ipv4_from_keypad(p_ctx, STR_EMPTY,
                                new_submask, kMaxIPLen + 1, 1);
                    //fprintf(stderr, "ret: %d, value: %s\n", state_ret, new_submask);
                    if (state_ret != kFailure)
                    {
                        if (inet_addr(new_submask) == kFailure)
                        {
                            show_line(p_ctx, 3, STR_SETUP_ERROR);
                            buzzer((kMicroPerSecond / 10) * 1);
                            usleep(kMicroPerSecond / 2);
                            continue;
                        }
                        break;
                    }
                }
                break;
            case SET_GATEWAY:
                while (kTrue)
                {
                    show_line(p_ctx, 0, STR_SETUP_GATEWAY);
                    show_line(p_ctx, 3, STR_EMPTY);
                    state_ret = get_ipv4_from_keypad(p_ctx, STR_EMPTY,
                                new_gateway, kMaxIPLen + 1, 1);
                    //fprintf(stderr, "ret: %d, value: %s\n", state_ret, new_gateway);
                    if (state_ret != kFailure)
                    {
                        if (inet_addr(new_gateway) == kFailure)
                        {
                            show_line(p_ctx, 3, STR_SETUP_ERROR);
                            buzzer((kMicroPerSecond / 10) * 1);
                            usleep(kMicroPerSecond);
                            continue;
                        }
                        break;
                    }
                }
                break;
            case SET_SRV_IP:
                while (kTrue)
                {
                    show_line(p_ctx, 0, STR_SETUP_SERVER_IP);
                    show_line(p_ctx, 3, STR_EMPTY);
                    state_ret = get_ipv4_from_keypad(p_ctx, STR_EMPTY,
                                new_server_ip, kMaxIPLen + 1, 1);
                    //fprintf(stderr, "ret: %d, value: %s\n", state_ret, new_server_ip);
                    if (state_ret != kFailure)
                    {
                        if (inet_addr(new_server_ip) == -1)
                        {
                            show_line(p_ctx, 3, STR_SETUP_ERROR);
                            buzzer((kMicroPerSecond / 10) * 1);
                            usleep(kMicroPerSecond);
                            continue;
                        }
                        break;
                    }
                }
                break;
            case SET_SRV_PORT:
                while (kTrue)
                {
                    show_line(p_ctx, 0, STR_SETUP_SERVER_PORT);
                    show_line(p_ctx, 3, STR_EMPTY);
                    state_ret = get_str_from_keypad(p_ctx, STR_EMPTY,
                                new_server_port_str, kMaxPortLen + 1, 1);
                    //fprintf(stderr, "ret: %d, value: %s\n", state_ret, new_server_port_str);
                    if (state_ret != kFailure)
                    {
                        new_server_port = (int)strtol(new_server_port_str, &end_ptr, 10);
                        if (*end_ptr != '\0'
                                || new_prt_con_type < kConnectTypeMin
                                || new_prt_con_type > kConnectTypeMax)
                        {
                            show_line(p_ctx, 3, STR_SETUP_ERROR);
                            buzzer((kMicroPerSecond / 10) * 1);
                            usleep(kMicroPerSecond);
                            continue;
                        }
                        break;
                    }
                }
                break;
            case SET_FN_PASS:
                show_line(p_ctx, 0, STR_SETUP_FN_PASSWD);
                state_ret = get_str_from_keypad(p_ctx, STR_EMPTY,
                        new_fn_passwd, kMaxPasswdLen + 1, 1);
                //fprintf(stderr, "ret: %d, value: %s\n", state_ret, new_edc_id);
                break;
            case SET_CONFIRM:
                show_line(p_ctx, 0, STR_SETUP_CONFIRM);
                show_line(p_ctx, 1, STR_EMPTY);
                while (kTrue)
                {   
                    if (get_press_key(p_ctx, &in_key) < 0)
                    {
                        return kFailure;
                    }
                    else if (in_key == kASCIIEnter)
                    {
                        state_ret = kASCIIDown;
                        break;
                    }
                    else if (in_key == kASCIICancel)
                    {
                        state_ret = kASCIICancel;
                        break;
                    }
                    else if (in_key == kASCIIUp)
                    {
                        state_ret = kASCIIUp;
                        break;
                    }
                    usleep(kMicroPerSecond / 10);
                }
                fprintf(stderr, "ret: %d\n", state_ret);
                break;
        }

        if (state_ret == kFailure)
        {
            fprintf(stderr, "Fetch setup failure in state: %d\n", state);
            return kFailure;
        }

        // Second level action 
        if (state != SET_CONFIRM)
        {
            if (state_ret == kASCIIUp && state != SET_PRT_TYPE)
            {
                state--;
            }
            else if (state_ret == kASCIIDown)
            {
                state++;
            }
        }
        else
        {
            if (state_ret == kASCIIUp)
            {
                state--;
            }
            else if (state_ret == kASCIIDown)
            {
                fprintf(stderr, "Write to ini: %s\n", kServerIni);
                strncpy(p_ctx->edc_id, new_edc_id, kMaxEDCIDLen+1);
                strncpy(p_ctx->edc_ip, new_edc_ip, kMaxIPLen+1);
                strncpy(p_ctx->submask, new_submask, kMaxIPLen+1);
                strncpy(p_ctx->gateway, new_gateway, kMaxIPLen+1);
                strncpy(p_ctx->server_ip, new_server_ip, kMaxIPLen+1);
                strncpy(p_ctx->fn_passwd, new_fn_passwd, kMaxPasswdLen+1);
                p_ctx->server_port = new_server_port;
                p_ctx->prt_con_type = new_prt_con_type;

                // Write back to file
                if(!(fp_setup = fopen(kServerIni, "w")))
                {
                    fprintf(stderr, "Can not open %s.\n", kServerIni);
                    return kFailure;
                }
                ret = fprintf(fp_setup, "%s=%d\n%s=%s\n%s=%d\n%s=%s\n%s=%s\n",
                        kSetupStrPRTCON, new_prt_con_type,
                        kSetupStrServerIP, new_server_ip,
                        kSetupStrServerPort, new_server_port,
                        kSetupStrEDCID, new_edc_id,
                        kSetupStrFnPasswd, new_fn_passwd);
                if (ret < 0)
                {
                    fprintf(stderr, "Write to %s failure.\n", kServerIni);
                    fclose(fp_setup);
                    return kFailure;
                }
                fclose(fp_setup);

                if(!(fp_network = fopen(kNetworkIni, "w")))
                {
                    fprintf(stderr, "Can not open %s.\n", kNetworkIni);
                    return kFailure;
                }
                ret = fprintf(fp_network, "%s=%s\n%s=%s\n%s=%s\n",
                        kSetupSelfIP, new_edc_ip,
                        kSetupSubmask, new_submask,
                        kSetupGateway, new_gateway);
                if (ret < 0)
                {
                    fprintf(stderr, "Write to %s failure.\n", kServerIni);
                    fclose(fp_network);
                    return kFailure;
                }
                fclose(fp_network);

                // Re-connect to server
                if (p_ctx->connected == kTrue)
                {
                    fprintf(stderr, "Disconnect from server...\n");
                    if (disconnect_server(p_ctx) != kSuccess)
                    {
                        fprintf(stderr, "Disconnect from server failure.\n");
                        return kFailure;
                    }
                }

                if (system(kSetupIpCmd) != kSuccess)
                {
                    fprintf(stderr, "Reset network setting failure.\n");
                    return kFailure;
                }

                if (connect_server(p_ctx) != kSuccess)
                {
                    fprintf(stderr, "Connect to server failure.\n");
                }

                break;
            }
            else if (state_ret == kASCIICancel)
            {
                // do nothing
                break;
            }
        }
    }

    /*
    fprintf(stderr, "CLD: EDC_ID: %s.\n", new_edc_id);
    fprintf(stderr, "CLD: EDC_IP: %s.\n", new_edc_ip);
    fprintf(stderr, "CLD: SUBMASK: %s.\n", new_submask);
    fprintf(stderr, "CLD: GATEWAY: %s.\n", new_gateway);
    fprintf(stderr, "CLD: SERVER_IP: %s.\n", new_server_ip);
    fprintf(stderr, "CLD: SERVER_PORT: %s.\n", new_server_port);
    */

    // Restart threads
    if (pthread_create(&(p_ctx->sync_list_thr), NULL,
                (void*)sync_list_thr_func, (void*)p_ctx) != kSuccess)
    {
        fprintf(stderr, "Create sync thread failure.\n");
        return kFailure;
    }

    if (pthread_create(&(p_ctx->sync_log_thr), NULL,
                (void*)sync_log_thr_func, (void*)p_ctx) != kSuccess)
    {
        fprintf(stderr, "Create sync thread failure.\n");
        return kFailure;
    }

    p_ctx->state = IDLE;
    return kSuccess;
}

int get_str_from_keypad(EDC_CTX *p_ctx, const char* prompt,
        char *buf, const int buf_len, const int scr_line)
{
    unsigned char in_key;
    char *ptr_buf;
    char scr_line_str[kMaxLineWord + 1];

    if (!p_ctx || !buf)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    ptr_buf = buf + strlen(buf);
    while (kTrue)
    {   
        //catch input and update screen
        if (get_press_key(p_ctx, &in_key) < 0)
        {
            //fprintf(stderr, "Can not get keypad.\n");
            return kFailure;
        }
        else if (in_key != 0)
        {
            if (in_key >= kASCIIFirstVisiable &&
                    in_key <= kASCIILastVisiable)
            {
                if (ptr_buf - buf >= buf_len - 1 - 1)
                {
                    //Full, do nothing
                    continue;
                }
                *ptr_buf++ = in_key;
                *ptr_buf = '\0';
            }
            else if (in_key == kASCIIClear)
            {
                if (ptr_buf > buf)
                {
                    *--ptr_buf = '\0';
                }
            }
            else if (in_key == kASCIICancel)
            {
                memset(buf, 0, buf_len);
                ptr_buf = buf;
                //return kASCIICancel;
            }
            else if (in_key == kASCIIEnter ||
                     in_key == kASCIIDown)
            {
                //*ptr_buf = '\0';
                break;
            }
            else if (in_key == kASCIIUp)
            {
                return kASCIIUp;
            }
        }
        snprintf(scr_line_str, kMaxLineWord + 1, "%s%s", prompt, buf);
        show_line(p_ctx, scr_line, scr_line_str);
        usleep(kMicroPerSecond / 10);
    }

    return kASCIIDown;
}

int get_ipv4_from_keypad(EDC_CTX *p_ctx, const char* prompt,
        char *buf, const int buf_len, const int scr_line)
{
    unsigned char in_key;
    char *ptr_buf;
    int char_idx;
    char scr_line_str[kMaxLineWord + 1];

    if (!p_ctx || !buf)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if (buf_len < kMaxIPLen + 1)
    {
        fprintf(stderr, "Buffer too short for IPv4!\n");
        return kFailure;
    }

    ptr_buf = buf + strlen(buf);
    while (kTrue)
    {   
        //catch input and update screen
        if (get_press_key(p_ctx, &in_key) < 0)
        {
            //fprintf(stderr, "Can not get keypad.\n");
            return kFailure;
        }
        else if (in_key != 0)
        {
            if (in_key >= kASCII_zero &&
                    in_key <= kASCII_nine)
            {
                if (ptr_buf - buf >= kMaxIPLen)
                {
                    //Full, do nothing
                    continue;
                }
                *ptr_buf++ = in_key;
                *ptr_buf = '\0';
                char_idx = ptr_buf - buf;
                if (char_idx == 3 || char_idx == 7 || char_idx == 11)
                {
                    *ptr_buf++ = '.';
                    *ptr_buf = '\0';
                }
            }
            else if (in_key == kASCIIClear)
            {
                if (ptr_buf > buf)
                {
                    if (*--ptr_buf == '.')
                    {
                        --ptr_buf;
                    }
                    *ptr_buf = '\0';
                }
            }
            else if (in_key == kASCIICancel)
            {
                memset(buf, 0, buf_len);
                ptr_buf = buf;
                //return kASCIICancel;
            }
            else if (in_key == kASCIIEnter
                    || in_key == kASCIIDown)
            {
                //*ptr_buf = '\0';
                break;
            }
            else if (in_key == kASCIIUp)
            {
                return kASCIIUp;
            }
        }
        snprintf(scr_line_str, kMaxLineWord + 1, "%s%s", prompt, buf);
        show_line(p_ctx, scr_line, scr_line_str);
        usleep(kMicroPerSecond / 10);
    }
    snprintf(scr_line_str, kMaxLineWord + 1, "%s%s", prompt, buf);
    show_line(p_ctx, scr_line, scr_line_str);

    return kASCIIDown;
}


int buzzer(int msec)
{
    if (device_power(DEVICE_BUZZER_2, 1))
    {
        fprintf(stderr, "Can't open buzzer!\n");
        return kFailure;
    }
    usleep(msec);
    if (device_power(DEVICE_BUZZER_2, 0))
    {
        fprintf(stderr, "Can't close buzzer!\n");
        return kFailure;
    }

    return kSuccess;
}

int show_datetime(EDC_CTX* p_ctx)
{
    time_t cur_time;
    struct tm time_info;
    char time_str[kMaxLineWord + 1];
    char line_str[kMaxLineWord + 1];

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    cur_time = time(NULL);
    localtime_r(&cur_time, &time_info);
    snprintf(time_str, kMaxLineWord + 1, "%.2d/%.2d %.2d:%.2d",
            time_info.tm_mon + 1,
            time_info.tm_mday,
            time_info.tm_hour,
            time_info.tm_min);

    if (left_right_str(line_str, kMaxLineWord + 1, time_str, p_ctx->edc_id))
    {
        fprintf(stderr, "EDC id is too long in line of datetime.\n");
        return kFailure;
    }

    return show_line(p_ctx, 0, line_str);
}

int set_led(unsigned int conf)
{
    int i;
    int failure_count = 0;
    const int lcd_offset = 20;

    for (i = 0; i < kLEDNum; i++)
    {
        while (kTrue)
        {
            if (device_power(i + lcd_offset, (conf >> i & 0x01)) == kSuccess)
            {
                break;
            }

            fprintf(stderr, "Can not power on LED device: %d.\n", i + lcd_offset);
            if (++failure_count > kMaxFailLimit)
            {
                return kFailure;
            }
            usleep(kMicroPerSecond / 10);
        }
    }

    return kSuccess;
}

int set_backlight(EDC_CTX* p_ctx, unsigned char u8_type)
{
    int ret;
    int failure_count = 0;

    while (kTrue)
    {

        if (pthread_mutex_lock(&p_ctx->lkp_ctx_mutex))
        {
            fprintf(stderr, "Lock lkp_ctx mutex failure!\n");
            return kFailure;
        }

        ret = lkp_set_backlight(p_ctx->lkp_ctx, u8_type);

        if (pthread_mutex_unlock(&p_ctx->lkp_ctx_mutex))
        {
            fprintf(stderr, "Unlock lkp_ctx mutex failure!\n");
            return kFailure;
        }

        if ( ret == kSuccess)
        {
            break;
        }

        fprintf(stderr, "Can not set backlight: %d.\n", u8_type);
        if (++failure_count > kMaxFailLimit)
        {
            return kFailure;
        }
        usleep(kMicroPerSecond / 10);
    }

    return kSuccess;
}

int left_right_str(char *buf, const int buf_size, const char *left, const char * right)
{
    int spaces = buf_size - strlen(left) - strlen(right) - 1;
    int i;
    char *ptr;

    if (!buf)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }
    ptr = buf;

    //fprintf(stderr, "Spaces=%d, left=%d, right=%d.\n", spaces, strlen(left), strlen(right));
    //fprintf(stderr, "left=%s, right=%s.\n", left, right);
    if (spaces < 0)
    {
        fprintf(stderr, "Bufer too small\n");
        return kFailure;
    }

    strncpy(ptr, left, strlen(left));
    //fprintf(stderr, "Buf: %s\n", buf);
    ptr = buf + strlen(left);
    for (i = 0; i < spaces; i++)
    {
        *ptr++ = ' ';
        //fprintf(stderr, "Buf: %s\n", buf);
    }
    strncpy(ptr, right, strlen(right));
    //fprintf(stderr, "Buf: %s\n", buf);
    *(ptr + strlen(right)) = '\0';
    //fprintf(stderr, "Buf: %s\n", buf);
    //fprintf(stderr, "buf: %s(%d)\n", buf, strlen(buf));

    return kSuccess;
}

int show_line(EDC_CTX *p_ctx, int line, const char *string)
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
        if (pthread_mutex_lock(&p_ctx->lkp_ctx_mutex))
        {
            fprintf(stderr, "Lock lkp_ctx mutex failure!\n");
            return kFailure;
        }

        ret = lcd_clean_buffer(p_ctx->lkp_ctx, 0, kFontHeight * line, kScreenWidth, kFontHeight);
        if (ret != 0)
        {
            fprintf(stderr, "Clean buffer error, line=%d, ret=%d\n", line, ret);
            return kFailure;
        }

        ret = lcd_draw_text_16f(p_ctx->lkp_ctx, 0, kFontHeight * line, string, 0);
        if (ret != 0)
        {
            fprintf(stderr, "Draw buffer error, line=%d, ret=%d\n", line, ret);
            return kFailure;
        }

        //usleep(kMicroPerSecond / 10);
        ret = lcd_print_out(p_ctx->lkp_ctx);

        if (pthread_mutex_unlock(&p_ctx->lkp_ctx_mutex))
        {
            fprintf(stderr, "Unlock lkp_ctx mutex failure!\n");
            return kFailure;
        }

        if (ret == 0)
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

int read_rfid(EDC_CTX *p_ctx)
{
    int i;
    int len = 0;
    char *pos;
    unsigned char pcData[kMaxReadRFIDLen];
    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    pos = p_ctx->curr_card_sn;

    memset(pcData, '\0', kMaxReadRFIDLen);
    len = serReadCOM(&p_ctx->com_ctx, pcData, kMaxReadRFIDLen);
    if (len > kMaxCardReadLen)
    {
        fprintf(stderr, "Read from com too long: %d\n", len);
        return kFailure;
    }

    if (len > 0 && (pcData[0] == 0xA4 && pcData[1] == 0x01))
    {
        for(i = 2; i < kMaxCardReadLen - 2; i++)
        {
            unsigned char data = ((pcData[i] >> 4) & 0x0F);
            if(data < 0x0A)
                *(pos++) = data + 0x30;
            else
                *(pos++) = data + 0x37;

            data = pcData[i] & 0x0F;
            if(data < 0x0A)
                *(pos++) = (pcData[i] & 0x0F) + 0x30;
            else
                *(pos++) = (pcData[i] & 0x0F) + 0x37;

        }
        //show_line(p_ctx, 3, p_ctx->curr_card_sn);
        return kSuccess;
    }

    return kFailure;
}

int is_valid_card(EDC_CTX *p_ctx)
{
    int i;
    int result = kFalse;
    EMPLOYEE_DATA* emp_ptr;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    emp_ptr = p_ctx->emp_list;

    if (pthread_mutex_lock(&p_ctx->emp_mutex))
    {
        fprintf(stderr, "Lock Employee mutex failure!\n");
        return kFailure;
    }

    for (i = 0; i < p_ctx->emp_num; i++)
    {
        if (strncmp(emp_ptr->card_sn, p_ctx->curr_card_sn, kMaxCardSNLen) == 0)
        {
            p_ctx->curr_emp_idx = i;
            result = kTrue;
            break;
        }
        emp_ptr++;
    }

    if (pthread_mutex_unlock(&p_ctx->emp_mutex))
    {
        fprintf(stderr, "Unlock Employee mutex failure!\n");
        return kFailure;
    }

    return result;
}


void FileGetValue(char *buf, char *u8Data, int *i32Len)
{
    int i, j;
    int EqualFlag = 0;

    j = 0;
    for( i = 0; i < strlen(buf); i++)
    {
        if( EqualFlag == 0 )
        {
            if( buf[i]  != '=')
                continue;

            EqualFlag = 1;
            continue;
        }

        if( ('\r' == buf[i]) || ('\n' == buf[i]) ) 
        {
            u8Data[j++] = (char) NULL;
            break;
        }
        u8Data[j++] = buf[i];
    }

    *i32Len = j;
    return;
}

unsigned int AsciiToUINTForHex(char *Source)
{
    int i;
    unsigned int ui = 0L;

    for(i = 0; i< (int)strlen(Source); i++)
    {
        ui <<= 4;
        if(Source[i] >= 'A' && Source[i] <= 'F')
            ui += (unsigned int)(Source[i]-'A'+0x0A);
        else if(Source[i] >= 'a' && Source[i] <= 'f')
            ui += (unsigned int)(Source[i]-'a'+0x0A);
        else if(Source[i] >= '0' && Source[i] <= '9')
            ui += (unsigned int)(Source[i]-'0');
        else
            return 0L;
    }

    return ui;
}

int DataConvertCMD(char *buf, char *buf1, int len, PCMD_DATA cmd_buffer)
{
    int i;
    char temp[3];

    cmd_buffer->len = len/2;
    for(i = 0; i < len/2; i++)
    {
        memcpy(temp, &((char *)buf1)[i * 2], 2);
        temp[2] = (char)NULL;
        cmd_buffer->data[i] = AsciiToUINTForHex(temp);
    }

    return 0;
}

int get_str_before_char(const char* line, const char chr, char* dest, int dest_len)
{
    char *ptr;

    if (!line || !dest)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    ptr = strchr(line, chr);
    if (!ptr)
    {
        return kFailure;
    }

    if (dest_len < ptr - line + 1)
    {
        fprintf(stderr, "Buffer too small.\n");
        return kFailure;
    }

    strncpy(dest, line, ptr - line);
    dest[ptr - line] = '\0';
    //fprintf(stderr, "CLD: line: %s, dest: %s, p-l: %d\n",line, dest, ptr-line);

    return ptr - line;
}

int load_employee_list(EMPLOYEE_DATA *p_list, const int list_size,
        const char *file_name, pthread_mutex_t *p_mutex)
{
    FILE *fList;
    int line_count = 0;
    char line[kMaxReadLineLen];
    char temp[kMaxReadLineLen];
    char *cur_ptr;
    int get_len;
    EMPLOYEE_DATA *list_ptr;

    if (!p_list)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    list_ptr = p_list;

    if (p_mutex && pthread_mutex_lock(p_mutex))
    {
        fprintf(stderr, "Lock Employee mutex failure!\n");
        return kFailure;
    }

    if (!(fList = fopen(file_name, "r")))
    {
        fprintf(stderr, "Can not open ini file.\n");
        return kFailure;
    }

    while (++line_count <= list_size &&
            fgets(line, kMaxReadLineLen, fList))
    {
        cur_ptr = line;
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        list_ptr->dep_name, MAX_DEP_NAME_LEN + 1)) == kFailure)
        {
            goto LOAD_EMP_FAIL_LINE;
        }

        cur_ptr += (get_len + 1);   // Include tab
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        list_ptr->dep_no, MAX_DEP_NO_LEN + 1)) == kFailure)
        {
            goto LOAD_EMP_FAIL_LINE;
        }

        cur_ptr += (get_len + 1);
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        list_ptr->emp_no, MAX_EMP_NO_LEN + 1)) == kFailure)
        {
            goto LOAD_EMP_FAIL_LINE;
        }

        cur_ptr += (get_len + 1);
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        list_ptr->card_sn, MAX_CARD_SN_LEN + 1)) == kFailure)
        {
            goto LOAD_EMP_FAIL_LINE;
        }

        cur_ptr += (get_len + 1);
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        temp, kMaxReadLineLen)) == kFailure)
        {
            goto LOAD_EMP_FAIL_LINE;
        }
        list_ptr->init_quota = (int)strtol(temp, NULL, 10);

        cur_ptr += (get_len + 1);
        if ((get_len = get_str_before_char(cur_ptr, kNewLine,
                        temp, kMaxReadLineLen)) == kFailure)
        {
            goto LOAD_EMP_FAIL_LINE;
        }
        list_ptr->curr_quota = (int)strtol(temp, NULL, 10);

        list_ptr++;
    }
    fclose(fList);

    if (p_mutex && pthread_mutex_unlock(p_mutex))
    {
        fprintf(stderr, "Unlock Employee mutex failure!\n");
        return kFailure;
    }

    /*
    for (list_ptr = p_list;
            list_ptr - p_list < line_count - 1; list_ptr++)
    {
        printf("dep_name: %s\n", list_ptr->dep_name);
        printf("dep_no: %s\n", list_ptr->dep_no);
        printf("emp_no: %s\n", list_ptr->emp_no);
        printf("card_sn: %s (%d)\n", list_ptr->card_sn, strlen(list_ptr->card_sn));
        printf("init_quota: %d\n", list_ptr->init_quota);
        printf("curr_quota: %d\n", list_ptr->curr_quota);
    }
    */

    return line_count;

LOAD_EMP_FAIL_LINE:
    fprintf(stderr, "Employee list malformed in line %d.\n", line_count);
    if (p_mutex && pthread_mutex_unlock(p_mutex))
    {
        fprintf(stderr, "Unlock Employee mutex failure!\n");
    }
    fclose(fList);
    return kFailure;
}

int load_edc_list(EDC_DATA *p_list, const int list_size, const char *file_name, pthread_mutex_t *p_mutex)
{
    FILE *fList;
    int line_count = 1;
    char line[kMaxReadLineLen];
    char temp[kMaxReadLineLen];
    char *cur_ptr;
    int get_len;
    EDC_DATA *list_ptr;

    if (!p_list)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    list_ptr = p_list;

    if (p_mutex && pthread_mutex_lock(p_mutex))
    {
        fprintf(stderr, "Lock EDC mutex failure!\n");
        return kFailure;
    }

    if (!(fList = fopen(file_name, "r")))
    {
        fprintf(stderr, "Can not open ini file.\n");
        return kFailure;
    }

    //<EDC_ID>\t<EDC_IP>\t<PRT_IP>\t<TIMEOUT>\t<SHOW_QUOTA>\t<MonoA3>\t<MonoA4>\t<ColorA3>\t<ColorA4>\n
    while ( ++line_count <= list_size &&
            fgets(line, kMaxReadLineLen, fList))
    {
        cur_ptr = line;
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        list_ptr->edc_id, MAX_EDC_ID_LEN + 1)) == kFailure)
        {
            goto LOAD_EDC_FAIL_LINE;
        }

        cur_ptr += (get_len +1);
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        list_ptr->edc_ip, MAX_IP_LEN + 1)) == kFailure)
        {
            goto LOAD_EDC_FAIL_LINE;
        }

        cur_ptr += (get_len +1);
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        list_ptr->printer_ip, MAX_IP_LEN + 1)) == kFailure)
        {
            goto LOAD_EDC_FAIL_LINE;
        }

        cur_ptr += (get_len +1);
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        temp, kMaxReadLineLen)) == kFailure)
        {
            goto LOAD_EDC_FAIL_LINE;
        }
        list_ptr->limit_time = (int)strtol(temp, NULL, 10);

        cur_ptr += (get_len +1);
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        temp, kMaxReadLineLen)) == kFailure)
        {
            goto LOAD_EDC_FAIL_LINE;
        }
        list_ptr->show_quota = (int)strtol(temp, NULL, 10);

        cur_ptr += (get_len +1);
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        temp, kMaxReadLineLen)) == kFailure)
        {
            goto LOAD_EDC_FAIL_LINE;
        }
        list_ptr->mono_a3 = (int)strtol(temp, NULL, 10);

        cur_ptr += (get_len +1);
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        temp, kMaxReadLineLen)) == kFailure)
        {
            goto LOAD_EDC_FAIL_LINE;
        }
        list_ptr->mono_a4 = (int)strtol(temp, NULL, 10);

        cur_ptr += (get_len +1);
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        temp, kMaxReadLineLen)) == kFailure)
        {
            goto LOAD_EDC_FAIL_LINE;
        }
        list_ptr->color_a3 = (int)strtol(temp, NULL, 10);

        cur_ptr += (get_len +1);
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        temp, kMaxReadLineLen)) == kFailure)
        {
            goto LOAD_EDC_FAIL_LINE;
        }
        list_ptr->color_a4 = (int)strtol(temp, NULL, 10);

        cur_ptr += (get_len +1);
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        temp, kMaxReadLineLen)) == kFailure)
        {
            goto LOAD_EDC_FAIL_LINE;
        }
        list_ptr->paper_size_a = (int)strtol(temp, NULL, 10);

        cur_ptr += (get_len +1);
        if ((get_len = get_str_before_char(cur_ptr, kNewLine,
                        temp, kMaxReadLineLen)) == kFailure)
        {
            goto LOAD_EDC_FAIL_LINE;
        }
        list_ptr->paper_size_b = (int)strtol(temp, NULL, 10);

        list_ptr++;
    }
    fclose(fList);
    if (p_mutex && pthread_mutex_unlock(p_mutex))
    {
        fprintf(stderr, "Unlock EDC mutex failure!\n");
        return kFailure;
    }

    /*
    for (list_ptr = (EDC_DATA*)p_list;
            list_ptr -p_list < line_count - 1; list_ptr++)
    {
        fprintf(stderr, "edc_id: %s\n", list_ptr->edc_id);
        fprintf(stderr, "edc_ip: %s\n", list_ptr->edc_ip);
        fprintf(stderr, "printer: %s\n", list_ptr->printer_ip);
        fprintf(stderr, "limit_time: %d\n", list_ptr->limit_time);
        fprintf(stderr, "show_quota: %d\n", list_ptr->show_quota);
        fprintf(stderr, "mono a3: %d\n", list_ptr->mono_a3);
        fprintf(stderr, "mono a4: %d\n", list_ptr->mono_a4);
        fprintf(stderr, "color a3: %d\n", list_ptr->color_a3);
        fprintf(stderr, "color a4: %d\n", list_ptr->color_a4);
        fprintf(stderr, "paper size a: %d\n", list_ptr->paper_size_a);
        fprintf(stderr, "paper size b: %d\n", list_ptr->paper_size_b);
    }
    */

    return line_count;

LOAD_EDC_FAIL_LINE:
    fprintf(stderr, "EDC list malformed in line %d.\n", line_count);
    if (p_mutex && pthread_mutex_unlock(p_mutex))
    {
        fprintf(stderr, "Unlock EDC mutex failure!\n");
    }
    fclose(fList);
    return kFailure;
}

int load_proj_list(PROJ_DATA *p_list, const int list_size, const char *file_name, pthread_mutex_t *p_mutex)
{
    FILE *fList;
    int line_count = 1;
    char line[kMaxReadLineLen];
    char *cur_ptr;
    int get_len;
    PROJ_DATA *list_ptr;

    if (!p_list)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    list_ptr = p_list;

    if (p_mutex && pthread_mutex_lock(p_mutex))
    {
        fprintf(stderr, "Lock project mutex failure!\n");
        return kFailure;
    }

    if(!(fList = fopen(file_name, "r")))
    {
        fprintf(stderr, "Can not open ini file.\n");
        return kFailure;
    }

    while ( ++line_count <= list_size &&
            fgets(line, kMaxReadLineLen, fList))
    {
        cur_ptr = line;
        if ((get_len = get_str_before_char(cur_ptr, kNewLine,
                        list_ptr->proj_num, kMaxProjectCodeLen + 1)) == kFailure)
        {
            goto LOAD_PROJ_FAIL_LINE;
        }
        list_ptr++;
    }
    fclose(fList);
    if (p_mutex && pthread_mutex_unlock(p_mutex))
    {
        fprintf(stderr, "Unlock project mutex failure!\n");
        return kFailure;
    }

    return line_count;

LOAD_PROJ_FAIL_LINE:
    fprintf(stderr, "Project list malformed in line %d.\n", line_count);
    if (p_mutex && pthread_mutex_unlock(p_mutex))
    {
        fprintf(stderr, "Unlock project mutex failure!\n");
    }
    fclose(fList);
    return kFailure;
}

int load_com_setting(COM_CTX *p_ctx, const char* ini_file)
{
    int i;
    int len;
    char line[kMaxReadLineLen];
    char buf1[256];
    char temp[40] = {0};
    FILE *fpSetting;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if(!(fpSetting = fopen(ini_file, "r")))
    {
        fprintf(stderr, "Can not open ini file.\n");
        return RET_ERR_SETUP_FILE;
    }

    if( !feof(fpSetting) )
    {
        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, p_ctx->port, &len);

        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, temp, &len);
        sscanf(temp, "%d", &(p_ctx->baud_rate));

        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, temp, &len);
        sscanf(temp, "%d", &(p_ctx->data_bits));

        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, temp, &len);
        sscanf(temp, "%d", &(p_ctx->parity));

        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, temp, &len);
        sscanf(temp, "%d", &(p_ctx->stop_bits));

        //fprintf(stderr, "port: %s, baud: %d, data: %d, stop: %d, parity: %d\n", p_ctx->port, p_ctx->baud_rate, p_ctx->data_bits, p_ctx->stop_bits, p_ctx->parity);

        for(i = 0; i < kMaxCOMCmdBuffer; i++)
        {
            memset(&(p_ctx->cmd_buffer[i]), 0, sizeof(CMD_DATA));
            fgets(line, kMaxReadLineLen, fpSetting);
            FileGetValue(line, buf1, &len);
            DataConvertCMD(line, buf1,
                    len, &(p_ctx->cmd_buffer[i]));
        }
    }
    fclose(fpSetting);

    return RET_SUCCESS;
}

int setport(COM_CTX *p_ctx)
{
    int Interface, baudrate;
    int DATABITS;
    int STOPBITS;
    int PARITYON;
    int PARITY;
    struct termios T_new;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    int fd = p_ctx->com_handle;

    //fprintf(stderr, "port: %s, baud: %d, data: %d, stop: %d, parity: %d\n", p_ctx->port, p_ctx->baud_rate, p_ctx->data_bits, p_ctx->stop_bits, p_ctx->parity);
    switch(p_ctx->baud_rate)
    {
        case 300:
            baudrate = B300;
            break;
        case 600:
            baudrate = B600;
            break;
        case 1200:
            baudrate = B1200;
            break;
        case 2400:
            baudrate = B2400;
            break;
        case 4800:
            baudrate = B4800;
            break;
        case 9600:
            baudrate = B9600;
            break;
        case 19200:
            baudrate = B19200;
            break;
        case 38400:
            baudrate = B38400;
            break;
        case 57600:
            baudrate = B57600;
            break;
        case 115200:
            baudrate = B115200;
            break;
        default :
            baudrate = B38400;  
            break;
    }

    switch (p_ctx->data_bits)
    {
        case 8:
        default:
            DATABITS = CS8;
            break;
        case 7:
            DATABITS = CS7;
            break;
        case 6:
            DATABITS = CS6;
            break;
        case 5:
            DATABITS = CS5;
            break;
    }

    switch (p_ctx->stop_bits)
    {
        case 1:
        default:
            STOPBITS = 0;
            break;
        case 2:
            STOPBITS = CSTOPB;
            break;
    }

    switch (p_ctx->parity)
    {
        case 0:
        default:                       //none
            PARITYON = 0;
            PARITY = 0;
            break;
        case 1:                        //odd
            PARITYON = PARENB;
            PARITY = PARODD;
            break;
        case 2:                        //even
            PARITYON = PARENB;
            PARITY = 0;
            break;
    }  //end of switch parity


    /*set serial interface: RS-232*/
    Interface = UC500_UART_TYPE_232;
    /*set serial interface: RS-485*/
    //Interface = UC500_UART_TYPE_485;
    if(ioctl(fd, MATRIX_SET_UART_TYPE, &Interface) != 0) 
    {
        fprintf(stderr, "ioctl() Fail!, fd: %d\n", fd);
        close(fd);
        return RET_ERR_READER_FILE;
    }

    /*termios functions use to control asynchronous communications ports*/
    if (tcgetattr(fd, &T_new) != 0) 
    {       /*fetch tty state*/
        fprintf(stderr, "tcgetattr() Fail!\n");
        close(fd);
        return RET_ERR_READER_FILE; 
    }

    /*set   19200bps, n81, RTS/CTS flow control, 
      ignore modem status lines, 
      hang up on last close, 
      and disable other flags*/
    //T_new.c_cflag = (B19200 | CS8 | CREAD | CLOCAL | HUPCL | CRTSCTS);    
    T_new.c_cflag = (baudrate | DATABITS | STOPBITS | PARITYON | PARITY | CREAD | CLOCAL);  
    T_new.c_oflag = 0;
    T_new.c_iflag = 0;
    T_new.c_lflag = 0;
    if (tcsetattr(fd, TCSANOW, &T_new) != 0) 
    {
        fprintf(stderr, "tcsetattr() Fail!\n");
        close(fd);
        return RET_ERR_READER_FILE; 
    }

    return RET_SUCCESS;
}

/*
   功能：打開串口
   */
int OpenDev(char *Dev)
{
    int fd = open( Dev, O_RDWR | O_NOCTTY | O_NDELAY);

    if (-1 == fd)
    { /*設置數據位元數*/
#ifdef _DEBUG
        printf("SerialIO---Can't Open Serial Port\n");
#endif
        return RET_ERR_READER_FILE;
    }
    else
        return fd;

}

/*
   功能：開啟COMPort
   回傳：參照enum RET
   */
int serOpenCOM(COM_CTX* p_ctx)
{
    int r;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    r = OpenDev(p_ctx->port);
    if( r < 0)
    {
        fprintf(stderr, "Opem COM port Fail!\n");
        return kFailure;
    }

    p_ctx->com_handle = r;

    if( setport(p_ctx))
    {
        fprintf(stderr, "Set COM port Fail!\n");
        return kFailure;
    }

    return kSuccess;
}

/*
   功能：關閉COMPort。
   回傳：參照enum RET。
   */
int serCloseCOM(COM_CTX *p_ctx)
{
    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if( p_ctx->com_handle < 0 )
    {
        fprintf(stderr, "Parameter Fail!\n");
        return RET_ERR_UNKNOW;
    }

    close(p_ctx->com_handle);

    p_ctx->com_handle = 0;

    return RET_SUCCESS;
}

/*
   功能：接受COMPort上的資料。
   回傳：參照enum RET。
   */
int serReadCOM(COM_CTX *p_ctx, void *pData, const int ci32Len)
{
    int i = 0;
    int j;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    do
    {
        j = read(p_ctx->com_handle, pData + i, ci32Len - i);
        if( j > 0 ) 
            i += j;
    }while((ci32Len > i) && (j > 0));

    return i;

}

/*
   功能：發送COMPort上的資料。
   回傳：參照enum RET。
   */
int serWriteCOM(COM_CTX *p_ctx, const void *pcData, const int ci32Len)
{
    int i = 0, j;
    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    do
    {
        j = write(p_ctx->com_handle, pcData + i, ci32Len - i);
        if( j > 0 )
            i += j;
    }while((ci32Len > i) && (j > 0));

    return i;
}
