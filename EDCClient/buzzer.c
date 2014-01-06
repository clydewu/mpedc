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
#include <sys/stat.h>

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

#define MAX_EMP_LIST_SIZE   (4096)
#define MAX_EDC_LIST_SIZE   (2)
#define MAX_PROJ_LIST_SIZE  (128)
#define MAX_MEM_EDC_LOG     (256)
#define MAX_EDC_LOG_LEN     (256)
#define MAX_STATE_LEN       (16)
#define MAX_LOG_TYPE_LEN    (16)
#define MAX_LOG_LEVEL_LEN   (16)

// This is only for syntax highlight of vim in Mac OS
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0x2000
#endif

// log macro Function
// Wht use macro here?
// Cause of exp should be combine to the 2nd argument of frpintf
// directly (Instead of be a %s)
// And please always note THIS IS A MARCO, NOT FUNCTION!!!
#define log0(lv, mod_name, func_name, exp)          \
{                                                   \
    if (lv <= g_log_level)                          \
    {                                               \
        log_lock();                                 \
        fprintf(stderr, "%s [%s][%s] %s: " exp "\n",\
                get_cur_YmdHMS( ),                  \
                mod_name,                           \
                LOGLEVLE2STR[lv],                   \
                func_name                           \
        );                                          \
        log_unlock();                               \
    }                                               \
}                                                   \


#define log1(lv, mod_name, func_name, exp, p1)      \
{                                                   \
    if (lv <= g_log_level)                          \
    {                                               \
        log_lock();                                 \
        fprintf(stderr, "%s [%s][%s] %s: " exp "\n",\
                get_cur_YmdHMS( ),                  \
                mod_name,                           \
                LOGLEVLE2STR[lv],                   \
                func_name,                          \
                p1                                  \
        );                                          \
        log_unlock();                               \
    }                                               \
}                                                   \

#define log2(lv, mod_name, func_name, exp, p1, p2)  \
{                                                   \
    if (lv <= g_log_level)                          \
    {                                               \
        log_lock();                                 \
        fprintf(stderr, "%s [%s][%s] %s: " exp "\n",\
                get_cur_YmdHMS( ),                  \
                mod_name,                           \
                LOGLEVLE2STR[lv],                   \
                func_name,                          \
                p1, p2                              \
        );                                          \
        log_unlock();                               \
    }                                               \
}                                                   \

#define log3(lv, mod_name, func_name, exp, p1, p2, p3)\
{                                                   \
    if (lv <= g_log_level)                          \
    {                                               \
        log_lock();                                 \
        fprintf(stderr, "%s [%s][%s] %s: " exp "\n",\
                get_cur_YmdHMS( ),                  \
                mod_name,                           \
                LOGLEVLE2STR[lv],                   \
                func_name,                          \
                p1, p2, p3                          \
        );                                          \
        log_unlock();                               \
    }                                               \
}                                                   \

const char kModName[] = "EDCClient";
const char kEmpListFile[] = "./employee.list";
const char kEDCListFile[] = "./edc.list";
const char kProjListFile[] = "./projects.list";
const char kEDCLogFile[] = "./edc_tmp.log";
const char kServerIni[] = "./edc_setup.conf";
const char kNetworkIni[] = "./edc_network.conf";
const char kSetupIpCmd[] = "./edc_net_setup.sh ./edc_network.conf";
const char kTempFileSuffix[] = ".tmp";
const char kLogFolder[] = "./log/";
const char kLogSuffix[] = "log";
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
const int kMicroPerSecond = 1000000;
const int kMaxCardSNLen = MAX_CARD_SN_LEN;
const int kMaxBufferlen = 4096;
const int kMaxMemEDCLog = MAX_MEM_EDC_LOG;
const int kMaxEDCLogLen = MAX_EDC_LOG_LEN;
const int kMaxLogTypeLen = MAX_LOG_TYPE_LEN;
const int kMaxStateLen = MAX_STATE_LEN;
const int kMaxHeaderLen= 16;
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
const int kMaxLogLen = 8192;
const int kMaxLogFileSize = 1024 * 1024;
const char kYmdHMS[] = "%.4d-%.2d-%.2d %.2d:%.2d:%.2d";
const char kYmdHMS_NS[] = "%.4d%.2d%.2d%.2d%.2d%.2d";

const float kWaitSecScanKey = 0.2;
const float kWaitSecReadComPort = 0.1;
const float kWaitSecDoubleBuzzer = 0.2;
const float kWaitSecStopPtrCount = 0.2;
const float kBuzzerShort = 0.1;
const float kBuzzerMedium = 0.3;
const float kBuzzerLong = 0.5;

const int kIdleSecInvalidCard = 5;

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
const int kMaxSocketPortMin = 1;
const int kMaxSocketPortMax = 65535;
const int kMaxPasswdLen = MAX_PASSWD_LEN;

const int kMaxReadLineLen = 512;
const int kMaxPathLen = 512;
const char kTab = '\t';
const char kNewLine = '\n';
const char kSep = '|';
const char kPoint = '.';
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
const char STR_MONO_BIG[] = "黑大";
const char STR_MONO_SMALL[] = "黑小";
const char STR_MONO_OTHER[] = "黑特";
const char STR_COLOR_BIG[] = "彩大";
const char STR_COLOR_SMALL[] = "彩小";
const char STR_COLOR_OTHER[] = "彩特";
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
const char STR_SETUP_WAIT[] = "寫入中...";
const char STR_SETUP_CURRENT[] = "目前:";
const char STR_SETUP_NEWSET[] = "設定:";
const char STR_SETUP_ERROR[] = "格式錯誤";
const char STR_SETUP_PASSWD_ERROR[] = "密碼錯誤";
const char STR_SETUP_INPUT_PASSWD[] = "輸入密碼";
const char STR_DISABLE[] = "無法使用";
const char STR_ONLINE[] = "On-Line";
const char STR_OFFLINE[] = "Off-Line";


typedef enum _log_level
{
    FATAL,
    ERROR,
    WARN,
    INFO,
    DEBUG
} LOG_LEVEL; 

const char LOGLEVLE2STR[5][MAX_LOG_LEVEL_LEN] = {
    "FATAL",
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG"
};

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
    SETUP,
    PASSWD
} OP_STATE;

const char STATE2STR[6][MAX_STATE_LEN] = 
{
    "IDLE",
    "INVALID_CARD",
    "QUOTA",
    "SETUP",
    "PASSWD"
};

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
const char PTN_PAPER_GRAY_OTHER[] = "GO:%d ";
const char PTN_PAPER_COLOR_A[] = "CA%d:%d ";
const char PTN_PAPER_COLOR_B[] = "CB%d:%d ";
const char PTN_PAPER_COLOR_OTHER[] = "CO:%d ";

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
    int     only_mono;
    int     enable;
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
    int     enable;
} EDC_DATA;


typedef struct _project_data
{
    char    proj_num[MAX_PROJECT_LEN + 1];
    int     enable;
} PROJ_DATA;

typedef struct _edc_ctx
{
    PLKPCONTEXT     lkp_ctx;
    COM_CTX         com_ctx;
    KEY_CTX         key_ctx;

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

    pthread_t       sync_list_thr;

    pthread_t       sync_log_thr;
    char            edc_tmp_log[MAX_MEM_EDC_LOG][MAX_EDC_LOG_LEN];
    int             edc_log_num;
    pthread_mutex_t edc_log_mutex;

    pthread_t       log_thr;
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
int init(EDC_CTX *p_ctx);
void quit(EDC_CTX*);
int get_current_time_r(struct tm* p_time_info);
int show_datetime(EDC_CTX* p_ctx, struct tm* p_time_info);
int show_line(EDC_CTX*, int, const char*);
int left_right_str(char*, const int, const char*, const char*);
int set_led(EDC_CTX* p_ctx, unsigned int conf);
int read_rfid(EDC_CTX*);
int is_valid_card(EDC_CTX*);
int buzzer(int);
int get_str_from_keypad(EDC_CTX*, const char*, char*, const int, const int);
int get_ipv4_from_keypad(EDC_CTX*, const char*, char*, const int, const int);
int load_server_set(EDC_CTX*, const char*);
int load_network_set(EDC_CTX*, const char*);
int set_backlight(EDC_CTX* p_ctx, unsigned char u8_type);
int show_quota_info(EDC_CTX *p_ctx, int quota, int gb, int gs, int go, int cb, int cs, int co);
int trim_ipv4(char* ipv4, const int len);

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
char* get_cur_YmdHMS(void);
char* get_cur_YmdHMS_r(char *buf, int buf_len, int no_seperate);
int append_edc_log(EDC_CTX*, const EDC_LOG_TYPE, const char*);
int gen_cost_log(EDC_CTX*, const EDC_LOG_TYPE, PRINTERTYPE*);

// Printer Utilities
int usage_dup(PRINTERTYPE* , PRINTERTYPE*);
int usage_same_as(PRINTERTYPE *p1, PRINTERTYPE *p2);
int usage_empty(PRINTERTYPE *p);
int print_printertype(PRINTERTYPE *);
int init_printertype(PRINTERTYPE *usage);
int count2cost(PRINTERCOUNT_V2 *ptr_counter, int paper_size_a, int paper_size_b, 
        int *gray_big, int *gray_small, int *gray_other,
        int *color_big, int *color_small, int *color_other);

// Network Utilities
int connect_server(EDC_CTX*);
size_t sock_write(int, const char*, size_t);
size_t sock_read(int, char*, size_t);
int set_nonblock(int, int);
int dl_remote_list(EDC_CTX*, const char*, const char*);
int get_remote_list(EDC_CTX*,const char*,const char*, pthread_mutex_t*);

// Thread
int stop_sync_thr(EDC_CTX*);
void sync_thr_func(void *ctx);
void sync_log_thr_func(void *ctx);
void log_thr_func();

// Log Utilities
int init_log(EDC_CTX* p_ctx, LOG_LEVEL level);
char* get_log_YmdHMS(void);
int log_lock();
int log_unlock();
static pthread_mutex_t* get_log_mutex();
int get_file_size(const char* file_name);


//Global var for log
LOG_LEVEL g_log_level = DEBUG;
int g_log_pipe[2];
int g_log_fd;

int main(void)
{
    int ret;
    int ret_code = kSuccess;
    EDC_CTX ctx;

    if (init(&ctx))
    {
        log0(FATAL, kModName, __func__, "initializition failure.");
        goto INIT_FAIL;
    }
    log0(INFO, kModName, __func__, "===== EDC Client initialize OK =====");

    if (set_led(&ctx, kLEDNone) < kSuccess)
    {
        log0(FATAL, kModName, __func__, "Set LED down failure.");
        goto INIT_FAIL;
    }

    if(lcd_clean_scr(ctx.lkp_ctx) < kSuccess)
    {
        log0(FATAL, kModName, __func__, "Can not clean screen.");
        goto INIT_FAIL;
    }

    if(lcd_clean(ctx.lkp_ctx) < kSuccess)
    {
        log0(FATAL, kModName, __func__, "Can not clean screen buffer.");
        goto INIT_FAIL;
    }

    log0(INFO, kModName, __func__, "Start main loop");
    while (kTrue)
    {
        log1(INFO, kModName, __func__, "State change: %s", STATE2STR[ctx.state]);
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
        case SETUP:
            ret = setup_state(&ctx);
            break;
        case PASSWD:
            ret = passwd_state(&ctx);
            break;
        default:
            log0(FATAL, kModName, __func__, "Never ever be here");
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
        log0(ERROR, kModName, __func__, "Can not print error msg to screen.");
    }

    if (set_led(&ctx, kLEDNone) < kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not set LED.");
    }

    log0(INFO, kModName, __func__, "Program terminate.");
    quit(&ctx);
    return ret_code;
}

int stop_sync_thr(EDC_CTX* p_ctx)
{
    if (!p_ctx)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if (pthread_cancel(p_ctx->sync_list_thr) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not join sync_list thread.");
        return kFailure;
    }

    if (pthread_cancel(p_ctx->sync_log_thr) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not join sync_list thread.");
        return kFailure;
    }

    if (pthread_join(p_ctx->sync_list_thr, NULL) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not join sync_list thread.");
        return kFailure;
    }

    if (pthread_join(p_ctx->sync_log_thr, NULL) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not join sync_log thread.");
        return kFailure;
    }

    return kSuccess;
}

void sync_list_thr_func(void* ctx)
{
    EDC_CTX* p_ctx;

    if (!p_ctx)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return ;
    }

    p_ctx = (EDC_CTX*)ctx;

    while (kTrue)
    {
        sleep(kSyncListInterval);
        pthread_testcancel();

        if (p_ctx->connected)
        {
            log0(INFO, kModName, __func__, "Sync lists from Server");
            if (sync_lists(p_ctx) != kSuccess)
            {
                log0(ERROR, kModName, __func__, "Sync lists from server fail!");
            }

            log0(INFO, kModName, __func__, "Load downloads lists");
            if (load_local_lists(p_ctx) != kSuccess)
            {
                log0(ERROR, kModName, __func__, "Load lists failure!");
                //TODO should terminate program.
            }
            continue;
        }
        else
        {
            log0(ERROR, kModName, __func__, "Server is disconnected, can't sync lists!");
        }
    }
}

int get_press_key(EDC_CTX* p_ctx, unsigned char* key)
{
    int ret;

    if (!p_ctx || !key)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    // By experience, Sleep a while to keep it success
    usleep(kMicroPerSecond * kWaitSecScanKey);

    if (pthread_mutex_lock(&p_ctx->lkp_ctx_mutex))
    {
        log0(ERROR, kModName, __func__, "Lock lkp_ctx mutex failure!");
        return kFailure;
    }

    ret = kpd_get_keypad(p_ctx->lkp_ctx, key);

    if (pthread_mutex_unlock(&p_ctx->lkp_ctx_mutex))
    {
        log0(ERROR, kModName, __func__, "Unlock lkp_ctx mutex failure!");
        return kFailure;
    }

    return ret;
}

void sync_log_thr_func(void *ctx)
{
    EDC_CTX* p_ctx;
    int sync_success = kFalse;
    int ret;

    if (!p_ctx)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return ;
    }

    p_ctx = (EDC_CTX*)ctx;

    while (kTrue)
    {
        sleep(kSyncLogInterval);
        pthread_testcancel();

        if (p_ctx->connected)
        {
            //log0(INFO, kModName, __func__, "Sync EDC Log to server");
            ret = sync_log(p_ctx);
            if (ret != kSuccess)
            {
                log0(ERROR, kModName, __func__, "Sync EDC log to server fail!");
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
            log0(INFO, kModName, __func__, "Server is disconnected, re-connect to server");
            if (connect_server(p_ctx) != kSuccess)
            {
                log0(ERROR, kModName, __func__, "Re-Connect to agent failure");
                sync_success = kFalse;
            }
            else
            {
                if (sync_log(p_ctx) != kSuccess)
                {
                    log0(ERROR, kModName, __func__, "Sync edc log to server fail!");
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
                log0(ERROR, kModName, __func__, "Save EDC log to file failure!");
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if (get_cur_YmdHMS_r(cur_date, kMaxYmdHMSLen + 1, kFalse) == NULL)
    {
        log0(ERROR, kModName, __func__, "Get current datetime fail");
        return kFailure;
    }

    if (pthread_mutex_lock(&p_ctx->edc_log_mutex))
    {
        log0(ERROR, kModName, __func__, "Lock EDC log mutex failure!");
        return kFailure;
    }

    if (p_ctx->edc_log_num >= kMaxMemEDCLog)
    {
        log0(ERROR, kModName, __func__, "Temporary space of EDC log is not enought");
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
            log0(ERROR, kModName, __func__, "Make log string failure");
            return kFailure;
        }
        log1(INFO, kModName, __func__, "Append EDC Log: %s",
                p_ctx->edc_tmp_log[p_ctx->edc_log_num]);
        p_ctx->edc_log_num++;
    }

    if (pthread_mutex_unlock(&p_ctx->edc_log_mutex))
    {
        log0(ERROR, kModName, __func__, "Unlock EDC log mutex failure!");
        return kFailure;
    }

    return 0;
}

char* get_cur_YmdHMS(void)
{
    struct tm time_info = { 0 };
    static char cur_date[21];
    /* init */
    memset(cur_date, 0, 21);
    get_current_time_r(&time_info);
    snprintf(cur_date, sizeof(cur_date), kYmdHMS,
            time_info.tm_year+1900,
            time_info.tm_mon+1,
            time_info.tm_mday,
            time_info.tm_hour,
            time_info.tm_min,
            time_info.tm_sec);
    return cur_date;
}

char* get_cur_YmdHMS_r(char *buf, int buf_len, int no_seperate)
{
    time_t time_cur;
    struct tm local_time = {0};

    if (!buf)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return NULL;
    }

    strncpy(buf, "0000-00-00 00:00:00", buf_len);

    time_cur = time(NULL);

    localtime_r(&time_cur, &local_time);
    snprintf(buf, buf_len, (no_seperate)?kYmdHMS_NS:kYmdHMS,
            local_time.tm_year+1900,
            local_time.tm_mon+1,
            local_time.tm_mday,
            local_time.tm_hour,
            local_time.tm_min,
            local_time.tm_sec);

    return buf;
}


int save_log_to_local(EDC_CTX *p_ctx, const char *log_tmp_file)
{
    FILE *fp_log;
    int i = 0;

    if (!p_ctx || !log_tmp_file)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if(!(fp_log = fopen(log_tmp_file, "a")))
    {
        log0(ERROR, kModName, __func__, "Can not open log temp file.");
        return kFailure;
    }

    if (pthread_mutex_lock(&p_ctx->edc_log_mutex))
    {
        log0(ERROR, kModName, __func__, "Lock EDC log mutex failure!");
        fclose(fp_log);
        return kFailure;
    }

    // 1. write to files
    while (i < p_ctx->edc_log_num)
    {
        if (fprintf(fp_log, "%s\n", p_ctx->edc_tmp_log[i]) <= 0)
        {
            log0(ERROR, kModName, __func__, "Write to temp log file failure");
            pthread_mutex_unlock(&p_ctx->edc_log_mutex);
            fclose(fp_log);
            return kFailure;
        }
        i++;
    }
    // 2. clean up ctx
    p_ctx->edc_log_num = 0;

    if (pthread_mutex_unlock(&p_ctx->edc_log_mutex))
    {
        log0(ERROR, kModName, __func__, "Unlock EDC log mutex failure!");
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }
    memset(line, 0, kMaxEDCLogLen);
    memset(log_buf, 0, kMaxMemEDCLog * kMaxEDCLogLen);

    //Check local file first
    if (access(kEDCLogFile, F_OK) == kFailure)
    {    
        //log0(DEBUG, kModName, __func__, "EDC log file is not exist.");
    }
    else
    {
        log0(INFO, kModName, __func__, "Send EDC log which in file to server.");
        buf_size = sizeof(log_buf);
        buf_ptr = log_buf;
        if (!(fp_log = fopen(kEDCLogFile, "r")))
        {
            log0(ERROR, kModName, __func__, "Can not open EDC log file.");
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
                    log0(ERROR, kModName, __func__, "Send log to server fail.");
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
            log0(ERROR, kModName, __func__, "Send log to server fail.");
            p_ctx->connected = kFalse;
            return kFailure;
        }

        if (unlink(kEDCLogFile) != kSuccess)
        {
            log0(ERROR, kModName, __func__, "Delete sended EDC log file failure!");
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
        log0(INFO, kModName, __func__, "Send EDC log which in memory to server.");
        if (pthread_mutex_lock(&p_ctx->edc_log_mutex))
        {
            log0(ERROR, kModName, __func__, "Lock EDC log mutex failure!");
            return kFailure;
        }

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
                if (sock_write(p_ctx->server_fd, log_buf, buf_ptr - log_buf) != buf_ptr - log_buf)
                {
                    log0(ERROR, kModName, __func__, "Send memory log to server fail.");
                    p_ctx->connected = kFalse;
                    pthread_mutex_unlock(&p_ctx->edc_log_mutex);
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

        if (sock_write(p_ctx->server_fd, log_buf, buf_ptr - log_buf) != buf_ptr - log_buf)
        {
            p_ctx->connected = kFalse;
            pthread_mutex_unlock(&p_ctx->edc_log_mutex);
            return kFailure;
        }

        p_ctx->edc_log_num = 0;

        if (pthread_mutex_unlock(&p_ctx->edc_log_mutex))
        {
            log0(ERROR, kModName, __func__, "Unlock EDC log mutex failure!");
            return kFailure;
        }
    }

    return kSuccess;
}

int init(EDC_CTX *p_ctx)
{
    if (!p_ctx)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    //log1(INFO, kModName, __func__, "Initialize log setup: %s", LOGLEVLE2STR[INFO]);
    if (init_log(p_ctx, DEBUG) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not create log ctx.");
        return kFailure;
    }
    log0(INFO, kModName, __func__, "===== Log initializition =====");

    p_ctx->state = IDLE;
    p_ctx->connected = kFalse;
    memset(p_ctx->project_code, 0, kMaxProjectCodeLen + 1);

    log1(INFO, kModName, __func__, "Load server setup: %s", kServerIni);
    if (load_server_set(p_ctx, kServerIni) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Load server setup failure.");
        return kFailure;
    }

    log0(INFO, kModName, __func__, "Initialize mutesies.");
    if (pthread_mutex_init(&(p_ctx->emp_mutex), NULL) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not create employee mutex.");
        return kFailure;
    }

    if (pthread_mutex_init(&(p_ctx->edc_mutex), NULL) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not create edc mutex.");
        return kFailure;
    }

    if (pthread_mutex_init(&(p_ctx->proj_mutex), NULL) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not create project mutex.");
        return kFailure;
    }

    if (pthread_mutex_init(&(p_ctx->edc_log_mutex), NULL) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not create EDC log mutex.");
        return kFailure;
    }

    if (pthread_mutex_init(&(p_ctx->lkp_ctx_mutex), NULL) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not create lkp_ctx mutex.");
        return kFailure;
    }

    log0(INFO, kModName, __func__, "Initialize main context");
    p_ctx->lkp_ctx = lkp_create();
    if (!p_ctx->lkp_ctx)
    {
        log0(ERROR, kModName, __func__, "Can not create lkp contexet.");
        return kFailure;
    }

    log0(INFO, kModName, __func__, "Open device");
    if (device_open())
    {
        log0(ERROR, kModName, __func__, "Can not open device.");
        return kFailure;
    }
    
    log1(INFO, kModName, __func__, "Load COM port setup: %s", kDefINIFile);
    if (load_com_setting(&p_ctx->com_ctx,
                kDefINIFile) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not create com contexet.");
        return kFailure;
    }

    if (serOpenCOM(&p_ctx->com_ctx) != kSuccess) 
    {
        log0(ERROR, kModName, __func__, "Can't Open COM Port");
        return kFailure;
    }

    log1(INFO, kModName, __func__, "Load network setup: %s", kNetworkIni);
    if (load_network_set(p_ctx, kNetworkIni) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Load network setup failure.");
        return kFailure;
    }

    log0(INFO, kModName, __func__, "Connect to EDCAgent");
    if (connect_server(p_ctx) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Connect to server failure,"
                "use local files.");
    }
    else
    {
        log0(INFO, kModName, __func__, "Sync list from EDCAgent");
        if (sync_lists(p_ctx) != kSuccess)
        {
            log0(ERROR, kModName, __func__, "Sync list file failure.");
            return kFailure;
        }
    }

    log0(INFO, kModName, __func__, "Load downloaded lists");
    if (load_local_lists(p_ctx) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Load lists failure!");
        return kFailure;
    }

    log0(INFO, kModName, __func__, "Initialize thread to sync list");
    if (pthread_create(&(p_ctx->sync_list_thr), NULL,
                (void*)sync_list_thr_func, (void*)p_ctx) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Create sync thread failure.");
        return kFailure;
    }

    log0(INFO, kModName, __func__, "Initialize thread to sync log");
    if (pthread_create(&(p_ctx->sync_log_thr), NULL,
                (void*)sync_log_thr_func, (void*)p_ctx) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Create sync thread failure.");
        return kFailure;
    }

    p_ctx->edc_log_num = 0;
    memset(p_ctx->edc_tmp_log, 0, kMaxMemEDCLog * kMaxEDCLogLen);

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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    snprintf(tmp_emp_file, kMaxPathLen, "%s%s",
            kEmpListFile, kTempFileSuffix);
    snprintf(tmp_edc_file, kMaxPathLen, "%s%s",
            kEDCListFile, kTempFileSuffix);
    snprintf(tmp_proj_file, kMaxPathLen, "%s%s",
            kProjListFile, kTempFileSuffix);

    // Employee list
    log1(INFO, kModName, __func__, "Download Employee list to temp file: %s",
            tmp_emp_file);
    if (dl_remote_list(p_ctx, kSyncEmpCmd,
                tmp_emp_file) != kSuccess)
    {
        log1(ERROR, kModName, __func__,
            "Download Employee list to local is failure, use local list: %s",
            tmp_emp_file);
    }
    else
    {
        // Download OK, test it.
        if (load_employee_list(tmp_emp_list, MAX_EMP_LIST_SIZE,
                tmp_emp_file, NULL) < 0)
        {
            log1(ERROR, kModName, __func__,
                "Downloaded employee list is malformed, use local list: %s",
                tmp_emp_file);
        }
        else
        {
            if (rename(tmp_emp_file, kEmpListFile) != kSuccess)
            {
                log2(ERROR, kModName, __func__,
                    "Move temp file %s to employee file %s failure"
                    ", use original file\n", tmp_emp_file, kEmpListFile);
            }
        }
    }

    // EDC list
    log1(INFO, kModName, __func__, "Download EDC list to temp file: %s",
            tmp_edc_file);
    if (dl_remote_list(p_ctx, kSyncEDCCmd,
                tmp_edc_file) != kSuccess)
    {
        log1(ERROR, kModName, __func__,
            "Download EDC list to local is failure, use local list: %s",
            tmp_edc_file);
    }
    else
    {
        // Download OK, test it.
        if (load_edc_list(tmp_edc_list, kMaxEDCListSize,
                tmp_edc_file, NULL) < 0)
        {
            log1(ERROR, kModName, __func__,
                "Downloaded EDC list is malformed, use local list: %s",
                tmp_edc_file);
        }
        else
        {
            if (rename(tmp_edc_file, kEDCListFile) != kSuccess)
            {
                log2(ERROR, kModName, __func__,
                    "Move temp file %s to edc file %s failure"
                    ", use original file\n", tmp_edc_file, kEDCListFile);
            }
        }
    }

    // Project list
    log1(INFO, kModName, __func__, "Download Project list to temp file: %s",
            tmp_proj_file);
    if (dl_remote_list(p_ctx, kSyncProjCmd,
                tmp_proj_file) != kSuccess)
    {
        log1(ERROR, kModName, __func__,
            "Download Project list to local is failure, use local list: %s",
            tmp_proj_file);
    }
    else
    {
        // Download OK, test it.
        if (load_proj_list(tmp_proj_list, kMaxProjListSize,
                tmp_proj_file, NULL) < 0)
        {
            log1(ERROR, kModName, __func__,
                "Downloaded Project list is malformed, use local list: %s",
                tmp_proj_file);
        }
        else
        {
            if (rename(tmp_proj_file, kProjListFile) != kSuccess)
            {
                log2(ERROR, kModName, __func__,
                    "Move temp file %s to proj file %s failure"
                    ", use original file\n", tmp_proj_file, kProjListFile);
            }
        }
    }

    return kSuccess;
}

int load_local_lists(EDC_CTX *p_ctx)
{
    if (!p_ctx)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if ((p_ctx->emp_num = load_employee_list(
            p_ctx->emp_list, kMaxEmpListSize,
            kEmpListFile, &p_ctx->emp_mutex)) < 0)
    {
        log0(ERROR, kModName, __func__, "Load Employee list failure.");
        return kFailure;
    }

    if ((p_ctx->edc_num = load_edc_list(
            p_ctx->edc_list, kMaxEDCListSize,
            kEDCListFile, &p_ctx->edc_mutex)) < 0)
    {
        log0(ERROR, kModName, __func__, "Load EDC list failure.");
        return kFailure;
    }

    if ((p_ctx->proj_num = load_proj_list(
            p_ctx->proj_list, kMaxProjListSize,
            kProjListFile, &p_ctx->proj_mutex)) < 0)
    {
        log0(ERROR, kModName, __func__, "Load project list failure.");
        return kFailure;
    }

    return kSuccess;
}

void quit(EDC_CTX *p_ctx)
{
    if (serCloseCOM(&p_ctx->com_ctx) != RET_SUCCESS)
    {
        log0(ERROR, kModName, __func__, "Close COM failure.");
    }

    device_close();

    lkp_release(p_ctx->lkp_ctx);

    stop_sync_thr(p_ctx);

    return;
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(p_ctx->server_port);
    log0(DEBUG, kModName, __func__, "CLD: inet_addr()");
    if ((addr.sin_addr.s_addr = inet_addr(p_ctx->server_ip)) == INADDR_NONE)
    {
        log0(ERROR, kModName, __func__, "Server setup error.");
        return kFailure;
    }

    log0(DEBUG, kModName, __func__, "CLD: open socket()");
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        log0(ERROR, kModName, __func__, "Open socket error.");
        return kFailure;
    }

    log0(DEBUG, kModName, __func__, "CLD: set_nonblock()");
    if (set_nonblock(sock, kTrue) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Set socket non-block error.");
        return kFailure;
    }

    log0(DEBUG, kModName, __func__, "CLD: connect()");
    if ((conn_ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr))) == -1)
    {
        if (errno != EINPROGRESS)
        {
            log2(ERROR, kModName, __func__, 
                "Establish connect failure(%d): %s",
                conn_ret, strerror(errno));
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

                log0(DEBUG, kModName, __func__, "CLD: select()");
                sel_ret = select(sock + 1,
                        NULL, &conn_fds, NULL, &timeout);
                if (sel_ret < 0 && errno != EINTR)
                {
                    log1(ERROR, kModName, __func__, 
                        "Connect failure: %s",
                        strerror(errno));
                    return kFailure;
                }
                else if (sel_ret == 0)
                {
                    log0(ERROR, kModName, __func__, "Connect timeout.");
                    return kFailure;
                }
                else if (FD_ISSET(sock, &conn_fds))
                {
                    int valopt;
                    socklen_t int_size = sizeof(socklen_t);
                    log0(DEBUG, kModName, __func__, "CLD: getsockopt()");
                    if (getsockopt(sock, SOL_SOCKET, SO_ERROR,
                            (void*)&valopt, &int_size) < 0)
                    {
                        log2(ERROR, kModName, __func__, 
                            "Get sockopt failure (%d): %s",
                            errno, strerror(errno));
                        return kFailure;
                    }

                    if (valopt == 0)
                    {
                        break;
                    }

                    log1(ERROR, kModName, __func__, 
                        "Connect failure, valopt: %d", valopt);
                    return kFailure;
                }
            }

        }
    }

    if (set_nonblock(sock, kFalse) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Set socket to block mode failure.");
        return kFailure;
    }

    log0(INFO, kModName, __func__, "Connect to agent success.");
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
        log0(ERROR, kModName, __func__, "Get flag failure.");
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
        log0(ERROR, kModName, __func__, "Set flag failure.");
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if (!(fpSetting = fopen(ini_file, "r")))
    {
        log1(ERROR, kModName, __func__, "Can not open server ini file: %s",
                ini_file);
        return RET_ERR_SETUP_FILE;
    }

    if (!feof(fpSetting))
    {
        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, num_tmp, &len);
        com = strtol(num_tmp, &end_ptr, 10);
        if (*end_ptr != '\0')
        {
            log1(ERROR, kModName, __func__,
                "COM setup in INI file error: %s",
                ini_file);
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
            log0(ERROR, kModName, __func__, "Port setup in INI file error!");
            return kFailure;
        }
        p_ctx->server_port = port;

        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, p_ctx->edc_id, &len);

        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, p_ctx->fn_passwd, &len);
    }
    fclose(fpSetting);

    log1(INFO, kModName, __func__, 
            "Server ip: %s", p_ctx->server_ip);
    log1(INFO, kModName, __func__, 
            "Server port: %d", p_ctx->server_port);
    log1(INFO, kModName, __func__, 
            "id: %s", p_ctx->edc_id);
    log1(INFO, kModName, __func__, 
            "passwd: %s", p_ctx->fn_passwd);

    return kSuccess;
}

int load_network_set(EDC_CTX *p_ctx, const char *ini_file)
{
    FILE *fpSetting;
    char line[kMaxReadLineLen];
    int len;

    if (!ini_file)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if (!(fpSetting = fopen(ini_file, "r")))
    {
        log1(ERROR, kModName, __func__,
                "Can not open network ini file: %s", ini_file);
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

    log1(INFO, kModName, __func__,
            "EDC ip: %s", p_ctx->edc_ip);
    log1(INFO, kModName, __func__,
            "Submask: %s", p_ctx->submask);
    log1(INFO, kModName, __func__,
            "Gateway: %s", p_ctx->gateway);

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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if (!p_ctx->connected)
    {
        log0(ERROR, kModName, __func__, "Socket disconnect!");
        return kFailure;
    }

    sock = p_ctx->server_fd;
    memset(send_buf, 0, kMaxReadLineLen);

    send_len = snprintf(send_buf, kMaxReadLineLen,
            "%s\t%s\n", sync_cmd, p_ctx->edc_id);

    log0(DEBUG, kModName, __func__, "CLD: Send request to Agent...");
    if (sock_write(sock, send_buf, send_len) != send_len)
    {
        log0(ERROR, kModName, __func__, "Send request of lists to server fail.");
        p_ctx->connected = kFalse;
        return kFailure;
    }

    // If database of server is empty, there will return 0
    // and the list file will be a empty file
    log0(DEBUG, kModName, __func__, "CLD: Read request to Agent...");
    if ((total_recv = sock_read(sock,
            list_buf, kMaxEmpListBuf)) < 0)
    {
        log0(ERROR, kModName, __func__, "Read response from server fail.");
        return kFailure;
    }

    if(!(fp_list = fopen(file_name, "w")))
    {
        log0(ERROR, kModName, __func__, "Can not open local temp list.");
        return kFailure;
    }

    int ret = fprintf(fp_list, "%s", list_buf);
    if (ret < total_recv)
    {
        log1(ERROR, kModName, __func__, "Write to temp list failure:%d", ret);
        fclose(fp_list);
        return kFailure;
    }
    fclose(fp_list);
    return kSuccess;
}

size_t sock_write(int sock, const char *buf, size_t buf_len)
{
    size_t    nleft;
    ssize_t    send_len;

    const char  *ptr;
    char send_buf[kMaxHeaderLen + buf_len];

    int sel_ret;
    struct timeval timeout;
    fd_set write_fds;

    snprintf(send_buf, kMaxHeaderLen + buf_len, "%d|%s", (int)buf_len, buf);
    ptr = (char *)send_buf;
    nleft = strlen(send_buf);

    /*
    while (nleft > 0)
    {
        if ((send_len = send(sock, ptr, nleft, 0)) < 0)
        {
            fprintf(stderr, "Send data failure!\n");
            break;
        }

        nleft -= send_len;
        ptr += send_len;
    }
    return (buf_len - nleft);
    */

    while (nleft > 0)
    {
        timeout.tv_sec = kMaxSockTimeout;
        timeout.tv_usec = 0;
        FD_ZERO(&write_fds);
        FD_SET(sock, &write_fds);
        sel_ret = select(sock + 1, NULL,
                &write_fds, NULL, &timeout);
        if (sel_ret == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                log0(ERROR, kModName, __func__, "Select failure");
                return kFailure;
            }
        }
        else if (sel_ret == 0)
        {
            log0(ERROR, kModName, __func__, "Select timeout");
            return kFailure;
        }
        else if (FD_ISSET(sock, &write_fds))
        {
            send_len = send(sock, ptr, nleft, MSG_NOSIGNAL);
            if (send_len == 0)
            {
                log0(ERROR, kModName, __func__, "Server disconnect.");
                return kFailure;
            }
            else if ( send_len < 0)
            {
                if (errno == EINTR)
                {
                    continue;
                }
                log1(ERROR, kModName, __func__, "Send data failure, ret: %d", (int)send_len);
                break;
            }
            else
            {
                nleft -= send_len;
                ptr += send_len;
            }
        }
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
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
                log0(ERROR, kModName, __func__, "Select failure");
                return kFailure;
            }
        }
        else if (sel_ret == 0)
        {
            log0(ERROR, kModName, __func__, "Select timeout");
            return kFailure;
        }
        else if (FD_ISSET(sock, &recv_fds))
        {
            recv_ptr = recv_buf;
            recv_len = recv(sock, recv_buf, kMaxBufferlen, 0);
            if (recv_len == 0)
            {
                log0(ERROR, kModName, __func__, "Server disconnect.");
                return kFailure;
            }
            else if(recv_len < 0)
            {
                if (errno == EINTR)
                {
                    continue;
                }

                log0(ERROR, kModName, __func__, "Read socket failure.");
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
                    //log0(ERROR, kModName, __func__, "List buffer too small.");
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
            log0(ERROR, kModName, __func__, "Should not here");
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
    int ret;

    // Duplicate edc_num in local to detect modification.
    int cur_edc_num;
    int cur_connected;
    int cur_min;
    int backlight_epoch;
    int backlight_flag;
    struct tm time_info;

    if (!p_ctx)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if (lcd_clean_scr(p_ctx->lkp_ctx) < kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not clean screen.");
        return kFailure;
    }

    if (lcd_clean(p_ctx->lkp_ctx) < kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not clean screen buffer.");
        return kFailure;
    }

    if (set_led(p_ctx, kLEDBlue) < kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not set LED.");
        return kFailure;
    }

    if (set_backlight(p_ctx, 0xff))
    {
        log0(ERROR, kModName, __func__, "Can not open backlight.");
        return kFailure;
    }

    // Initial user data
    memset(&p_ctx->curr_card_sn, 0, kMaxCardSNLen);
    memset(&p_ctx->project_code, 0, kMaxProjectCodeLen + 1);
    p_ctx->curr_emp_idx = 0;

    cur_edc_num = p_ctx->edc_num;
    cur_connected = p_ctx->connected;
    get_current_time_r(&time_info);
    cur_min = time_info.tm_min;
    backlight_epoch = time(NULL);
    backlight_flag = kTrue;

    show_datetime(p_ctx, &time_info);
    show_line(p_ctx, 1, (cur_edc_num==0)?STR_DISABLE:STR_PLEASE_CARD);
    show_line(p_ctx, 3, cur_connected?STR_ONLINE:STR_OFFLINE);

    while (kTrue)
    {
        ret = get_press_key(p_ctx, &in_key);
        if (ret < 0)
        {
            log1(ERROR, kModName, __func__,
                    "Can not get keypad in idle state, ret:%d", ret);
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

                if (set_backlight(p_ctx, 0xff))
                {
                    log0(ERROR, kModName, __func__, "Can not open backlight.");
                    return kFailure;
                }
                backlight_epoch = time(NULL);
                backlight_flag = kTrue;

                show_line(p_ctx, 2, p_ctx->project_code);
            }
        }

        if (backlight_flag == kTrue)
        {
            if (time(NULL) - backlight_epoch > kLCDBackLightTimeout)
            {
                if (set_backlight(p_ctx, 0x00))
                {
                    log0(ERROR, kModName, __func__, "Can not close backlight when timeout.");
                    return kFailure;
                }
                backlight_flag = kFalse;
            }
        }

        if (cur_edc_num != p_ctx->edc_num)
        {
            // edc_num is modified, re-draw line 1
            cur_edc_num = p_ctx->edc_num;
            show_line(p_ctx, 1, (cur_edc_num==0)?STR_DISABLE:STR_PLEASE_CARD);
        }

        if (p_ctx->edc_num > 0)
        {
            // This EDC is registed in server, OK, detect card
            serWriteCOM(&p_ctx->com_ctx, kSendComSignal, sizeof(kSendComSignal));
            usleep(kMicroPerSecond * kWaitSecReadComPort);
            if (read_rfid(p_ctx) == kSuccess)
            {
                if (is_valid_card(p_ctx))
                {
                    p_ctx->state = QUOTA;
                    buzzer(kMicroPerSecond * kBuzzerShort);
                    log1(INFO, kModName, __func__, "Valid Card: %s", p_ctx->curr_card_sn);
                }
                else
                {
                    p_ctx->state = INVALID_CARD;
                    log1(INFO, kModName, __func__, "Get an invalid Card: %s", p_ctx->curr_card_sn);
                }
                break;
            }
        }

        if (cur_connected != p_ctx->connected)
        {
            cur_connected = p_ctx->connected;
            show_line(p_ctx, 3, cur_connected?STR_ONLINE:STR_OFFLINE);
        }

        get_current_time_r(&time_info);
        if (cur_min != time_info.tm_min)
        {
            cur_min = time_info.tm_min;
            if (show_datetime(p_ctx, &time_info) != kSuccess)
            {
                return kFailure;
            }
        }
    }

    return kSuccess;
}

int invalid_card_state(EDC_CTX *p_ctx)
{
    char edc_log[kMaxEDCLogLen];
    struct tm time_info;

    if (!p_ctx)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if(lcd_clean_scr(p_ctx->lkp_ctx) < kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not clean screen.");
        return kFailure;
    }

    if(lcd_clean(p_ctx->lkp_ctx) < kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not clean screen buffer.");
        return kFailure;
    }

    if (set_led(p_ctx, kLEDRed) < kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not set LED.");
        return kFailure;
    }

    snprintf(edc_log, kMaxEDCLogLen, PTN_CARD_INVALID, p_ctx->curr_card_sn);
    if (append_edc_log(p_ctx, CARD, edc_log) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Append EDC log failure");
        return kFailure;
    }

    get_current_time_r(&time_info);
    show_datetime(p_ctx, &time_info);
    show_line(p_ctx, 1, STR_INVALID_CARD);
    show_line(p_ctx, 2, STR_CONTACT_STAFF);

    memset(&p_ctx->curr_card_sn, 0, kMaxCardSNLen);

    if (set_backlight(p_ctx, 0xff))
    {
        log0(ERROR, kModName, __func__, "Can not set backlight.");
        return kFailure;
    }

    buzzer(kMicroPerSecond * kBuzzerShort);
    usleep(kMicroPerSecond * kIdleSecInvalidCard);
    p_ctx->state = IDLE;
    return kSuccess;
}

int quota_state(EDC_CTX* p_ctx)
{
    int ret;
    char remain_sec_str[kMaxLineWord + 1];
    char remain_line[kMaxLineWord + 1];
    char action_type[kMaxLineWord + 1];
    char edc_log[kMaxEDCLogLen];
    EMPLOYEE_DATA *curr_emp;
    EDC_DATA *curr_edc;
    int start_epoch;
    int curr_epoch;
    PRINTERCOUNT_V2 ptr_counter;
    PRINTERTYPE print_usage;
    PRINTERTYPE photocopy_usage;
    PRINTERTYPE continue_print_usage;
    PRINTERTYPE continue_photocopy_usage;
    PRINTERTYPE empty_usage;
    int curr_quota;
    int enject_or_timeout = 0;
    int stage = 0; //0: stop, 1: start
    int scan_flag = kFalse;
    int status_action_flag;
    int usage_modified_flag;

    int gb = 0;
    int gs = 0;
    int go = 0;
    int cb = 0;
    int cs = 0;
    int co = 0;

    unsigned char in_key;


    if (!p_ctx)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if (p_ctx->project_code[0] == '\0')
    {
        snprintf(p_ctx->project_code, kMaxProjectCodeLen + 1, "%s", "0000");
    }

    if (set_led(p_ctx, kLEDGreen) < kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not set LED.");
        return kFailure;
    }

    // Enable all backlight
    if (set_backlight(p_ctx, 0xff))
    {
        log0(ERROR, kModName, __func__, "Can not open backlight in quota state.");
        return kFailure;
    }

    snprintf(edc_log, kMaxEDCLogLen, PTN_CARD_VALID, p_ctx->curr_card_sn);
    if (append_edc_log(p_ctx, CARD, edc_log) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Append EDC log failure");
        return kFailure;
    }

    // is_valid_card() will prepare curr_emp_idx
    curr_emp = &(p_ctx->emp_list[p_ctx->curr_emp_idx]);
    curr_edc = &(p_ctx->edc_list[0]);
    curr_quota = curr_emp->curr_quota;

    init_printertype(&(ptr_counter.photocopy));
    init_printertype(&(ptr_counter.print));
    init_printertype(&(ptr_counter.fax));
    init_printertype(&(ptr_counter.scan));
    init_printertype(&photocopy_usage);
    init_printertype(&print_usage);
    init_printertype(&empty_usage);
    init_printertype(&continue_print_usage);
    init_printertype(&continue_photocopy_usage);
    memset(action_type, 0, kMaxLineWord + 1);

    if (show_quota_info(p_ctx, curr_quota, gb, gs, go, cb, cs, co) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Show quota screen failure");
        return kFailure;
    }

    log2(DEBUG, kModName, __func__, "Set COM%d, only mono:%d",
                p_ctx->prt_con_type, curr_emp->only_mono);
    if (ptr_select(p_ctx->prt_con_type, curr_emp->only_mono) != kSuccess)
    {
        log2(ERROR, kModName, __func__,
                "Set COM port printer failure: COM%d, only_mono: %d",
                p_ctx->prt_con_type, curr_emp->only_mono);
    }

    // Init print, start to statistic
    if (ptr_count_init(p_ctx->lkp_ctx) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Initial print counter failure");
        return kFailure;
    }

    start_epoch = time(NULL);
    curr_epoch = time(NULL);
    while (kTrue)
    {
        ret = get_press_key(p_ctx, &in_key);
        if (ret < 0)
        {
            log1(ERROR, kModName, __func__,
                    "Can not get keypad in quota state, ret:%d", ret);
            return kFailure;
        }
        else
        {
            if (in_key == kASCIIEject  || 
                in_key == kASCIIClear  ||
                in_key == kASCIICancel ||
                in_key == kASCIIEnter  ||
                (in_key >= kASCIIFirstVisiable && in_key <= kASCIILastVisiable))
            {
                enject_or_timeout = 1;
                break;
            }
        }

        if (ptr_count_get(p_ctx->lkp_ctx, &ptr_counter) < kSuccess)
        {
            log0(ERROR, kModName, __func__, "Get print counter failure");
            return kFailure;
        }


        //CLD
        log0(ERROR, kModName, __func__, "----- print count -----");
        print_printertype(&(ptr_counter.print));
        log0(ERROR, kModName, __func__, "----- copy count -----");
        print_printertype(&(ptr_counter.photocopy));
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
         * 3. BTW, old machine which interface is GPIO can't
         *    return status correct. So we keep check counter to
         *    verify it's running.
         */
        status_action_flag = ((ptr_counter.u8_work_status & 0x01) == 0)?kTrue:kFalse;
        usage_modified_flag = (!usage_same_as(&(ptr_counter.photocopy), &continue_photocopy_usage)
                          || !usage_same_as(&(ptr_counter.print), &continue_print_usage));

        log2(INFO, kModName, __func__, "Status flag: %d, Modified flag: %d",
                status_action_flag, usage_modified_flag);

        if (status_action_flag || usage_modified_flag)
        {
            // Action
            stage = 1;
            start_epoch = time(NULL);
            usage_dup(&(ptr_counter.photocopy), &continue_photocopy_usage);
            usage_dup(&(ptr_counter.print), &continue_print_usage);
            count2cost(&ptr_counter, curr_edc->paper_size_a, curr_edc->paper_size_b,
                    &gb, &gs, &go, &cb, &cs, &co);
            if (!usage_same_as(&(ptr_counter.scan), &empty_usage))
            {
                scan_flag = kTrue;
                log0(DEBUG, kModName, __func__,
                        "Detect SCAN1 due to counter be modified");
            }

            curr_quota = curr_emp->curr_quota
                        - gb * curr_edc->mono_a3
                        - gs * curr_edc->mono_a4
                        - cb * curr_edc->color_a3
                        - cs * curr_edc->color_a4;
            //fprintf(stderr, "CLD: cur_quota: %d, gb:%d, gs:%d, cb:%d, cs:%d\n",
            //        curr_quota, gb, gs, cb ,cs);
            if (usage_modified_flag == kTrue)
            {
                if (show_quota_info(p_ctx, curr_quota, gb, gs, go, cb, cs, co) != kSuccess)
                {
                    log0(ERROR, kModName, __func__, "Show quota screen failure");
                    return kFailure;
                }
            }
        }
        else
        {
            if (stage == 1)
            {
                log0(INFO, kModName, __func__, "Action finish.");
                if (usage_same_as(&(ptr_counter.photocopy), &photocopy_usage)
                    && usage_same_as(&(ptr_counter.print), &print_usage))
                {
                    log0(DEBUG, kModName, __func__,
                            "Dectect SCAN2 due to no counters are modified.");
                    scan_flag = kTrue;
                }

                if (!usage_same_as(&(ptr_counter.scan), &empty_usage))
                {
                    scan_flag = kTrue;
                    log0(DEBUG, kModName, __func__,
                            "Detect SCAN3 due to counter be modified after scan");
                    init_printertype(&(ptr_counter.scan));
                }

                usage_dup(&(ptr_counter.photocopy), &photocopy_usage);
                usage_dup(&(ptr_counter.print), &print_usage);
                stage = 0;
            }


            if (time(NULL)- start_epoch >= curr_edc->limit_time)
            {
                p_ctx->state = IDLE;
                break;
            }
        }

        if (curr_epoch != time(NULL))
        {
            curr_epoch = time(NULL);
            snprintf(remain_sec_str, kMaxLineWord + 1, "%s: %d",
                    STR_REMAIN_SEC, curr_edc->limit_time - (curr_epoch - start_epoch));
            left_right_str(remain_line, kMaxLineWord + 1, remain_sec_str, p_ctx->edc_id);
            show_line(p_ctx, 0, remain_line);
        }

        usleep(kMicroPerSecond / 2);
    }

    if (!usage_empty(&ptr_counter.photocopy)
            && gen_cost_log(p_ctx, COPY, &ptr_counter.photocopy) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Generate EDC log of copy failure");
        return kFailure;
    }

    if (!usage_empty(&ptr_counter.print)
            && gen_cost_log(p_ctx, PRINT, &ptr_counter.print) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Generate EDC log of print failure");
        return kFailure;
    }

    if (scan_flag == kTrue
            && gen_cost_log(p_ctx, SCAN, &ptr_counter.scan) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Generate EDC log of scan failure");
        return kFailure;
    }

    //release print
    buzzer(kMicroPerSecond * kBuzzerShort);
    usleep(kMicroPerSecond * kWaitSecDoubleBuzzer);
    buzzer(kMicroPerSecond * kBuzzerShort); 
    usleep(kMicroPerSecond * kWaitSecStopPtrCount);
    if (ptr_count_stop(p_ctx->lkp_ctx) < kSuccess)
    {
        log0(ERROR, kModName, __func__, "Stop print counter failure");
        return kFailure;
    }

    snprintf(edc_log, kMaxEDCLogLen, (enject_or_timeout==1)?PTN_CARD_ENJECT:PTN_CARD_TIMEOUT,
            p_ctx->curr_card_sn);
    if (append_edc_log(p_ctx, CARD, edc_log) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Append EDC log failure");
        return kFailure;
    }

    p_ctx->state = IDLE;
    return kSuccess;
}

int show_quota_info(EDC_CTX *p_ctx, int quota, int gb, int gs, int go, int cb, int cs, int co)
{
    char quota_str[kMaxLineWord + 1];
    char quota_line[kMaxLineWord + 1];
    char big_line[kMaxLineWord + 1];
    char small_line[kMaxLineWord + 1];
    char other_line[kMaxLineWord + 1];

    if (!p_ctx)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }


    snprintf(quota_str, kMaxLineWord + 1, "%s:%d",
            STR_QUOTA, quota);

    if (left_right_str(quota_line, kMaxLineWord + 1, quota_str, p_ctx->edc_id) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Buffer too small");
        return kFailure;
    }

    snprintf(big_line, kMaxLineWord, "%s%d %s%d",
            STR_MONO_BIG, gb,
            STR_COLOR_BIG, cb);

    snprintf(small_line, kMaxLineWord, "%s%d %s%d",
            STR_MONO_SMALL, gs,
            STR_COLOR_SMALL, cs);

    snprintf(other_line, kMaxLineWord, "%s%d %s%d",
            STR_MONO_OTHER, go,
            STR_COLOR_OTHER, co);

    //show_line(p_ctx, 0, quota_line);
    show_line(p_ctx, 1, big_line);
    show_line(p_ctx, 2, small_line);
    show_line(p_ctx, 3, other_line);

    return kSuccess;
}

int print_printertype(PRINTERTYPE *usage)
{
    int i;

    for (i = 0; i < kMaxPrtPage; i++)
    {
        fprintf(stderr, "S%d\t|%d %d %d %d %d %d %d %d\n",
                i,
                usage->u16_gray_scale_a[i],
                usage->u16_gray_scale_b[i],
                usage->u16_color_a[i],
                usage->u16_color_b[i],
                usage->u16_double_gray_scale_a[i],
                usage->u16_double_gray_scale_b[i],
                usage->u16_double_color_a[i],
                usage->u16_double_color_b[i]
                );
    }
    fprintf(stderr, "Other: %d %d\n", 
            usage->u16_gray_scale_other,
            usage->u16_color_other);

    return kSuccess;
}

int init_printertype(PRINTERTYPE *usage)
{
    int i;
    for (i = 0; i < kMaxPrtPage; i++)
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
    usage->u16_gray_scale_other = 0;
    usage->u16_color_other = 0;

    return kSuccess;
}

int count2cost(PRINTERCOUNT_V2 *ptr_counter, int paper_size_a, int paper_size_b, 
        int *gray_big, int *gray_small, int *gray_other,
        int *color_big, int *color_small, int *color_other)
{
    PRINTERTYPE *print;
    PRINTERTYPE *copy;
    int i;

    if (!ptr_counter || !gray_big || !gray_small || !color_big || !color_small)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    print = &(ptr_counter->print);
    copy = &(ptr_counter->photocopy);
    *gray_big = 0;
    *gray_small = 0;
    *gray_other = print->u16_gray_scale_other + copy->u16_gray_scale_other;
    *color_big = 0;
    *color_small = 0;
    *color_other = print->u16_gray_scale_other + copy->u16_color_other;

    // Count print cost
    for (i = 1; i < kMaxPrtPage; i++)
    {
        if (i < paper_size_a)
        {
            //log0(ERROR, kModName, __func__, "--- print a big ---");
            *gray_big += print->u16_gray_scale_a[i] +
                print->u16_double_gray_scale_a[i] * 2 +
                copy->u16_gray_scale_a[i] +
                copy->u16_double_gray_scale_a[i] * 2;
            *color_big += print->u16_color_a[i] +
                print->u16_double_color_a[i] * 2 +
                copy->u16_color_a[i] +
                copy->u16_double_color_a[i] * 2;
        }
        else
        {
            //log0(ERROR, kModName, __func__, "--- print a small ---");
            *gray_small += print->u16_gray_scale_a[i] +
                print->u16_double_gray_scale_a[i] * 2 +
                copy->u16_gray_scale_a[i] +
                copy->u16_double_gray_scale_a[i] * 2;
            *color_small += print->u16_color_a[i] +
                print->u16_double_color_a[i] * 2 +
                copy->u16_color_a[i] +
                copy->u16_double_color_a[i] * 2;
        }

        if (i < paper_size_b)
        {
            //log0(ERROR, kModName, __func__, "--- print b small ---");
            *gray_big += print->u16_gray_scale_b[i] +
                print->u16_double_gray_scale_b[i] * 2 +
                copy->u16_gray_scale_b[i] +
                copy->u16_double_gray_scale_b[i] * 2;
            *color_big += print->u16_color_b[i] +
                print->u16_double_color_b[i] * 2 +
                copy->u16_color_b[i] +
                copy->u16_double_color_b[i] * 2;
        }
        else
        {
            //log0(ERROR, kModName, __func__, "--- print b big ---");
            *gray_small += print->u16_gray_scale_b[i] +
                print->u16_double_gray_scale_b[i] * 2 +
                copy->u16_gray_scale_b[i] +
                copy->u16_double_gray_scale_b[i] * 2;
            *color_small += print->u16_color_b[i] +
                print->u16_double_color_b[i] * 2 +
                copy->u16_color_b[i] +
                copy->u16_double_color_b[i] * 2;
        }
    }

    return kSuccess;
}

int usage_dup(PRINTERTYPE *p_src, PRINTERTYPE *p_dest)
{
    int i;

    if (!p_src || ! p_dest)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
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
    p_dest->u16_gray_scale_other = p_src->u16_gray_scale_other;
    p_dest->u16_color_other = p_src->u16_color_other;

    return kTrue;
}

int usage_same_as(PRINTERTYPE *p1, PRINTERTYPE *p2)
{
    int i;

    if (!p1 || ! p2)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
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

    if (p1->u16_gray_scale_other != p2->u16_gray_scale_other ||
        p1->u16_color_other != p2->u16_color_other)
    {
        return kFalse;
    }

    return kTrue;
}

int usage_empty(PRINTERTYPE *p)
{
    int i;

    if (!p)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    for (i = 0; i < kMaxPrtPage; i++)
    {
        if (p->u16_gray_scale_a[i] != 0 ||
            p->u16_gray_scale_b[i] != 0 ||
            p->u16_color_a[i] != 0 ||
            p->u16_color_b[i] != 0 ||
            p->u16_double_gray_scale_a[i] != 0 ||
            p->u16_double_gray_scale_b[i] != 0 ||
            p->u16_double_color_a[i] != 0 ||
            p->u16_double_color_b[i] != 0)
        {
            return kFalse;
        }
    }
    if (p->u16_gray_scale_other != 0 ||
        p->u16_color_other != 0)
    {
        return kFalse;
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
    int gray_other = 0;
    int color_a_sum = 0;
    int color_b_sum = 0;
    int color_other = 0;
    int i;

    if (!p_ctx || !usage)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
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

    gray_other = usage->u16_gray_scale_other;
    color_other = usage->u16_color_other;
    if (gray_other != 0)
    {
        snprintf(temp_str, kMaxEDCLogLen, PTN_PAPER_GRAY_OTHER, gray_other);
        strncat(edc_log_content, temp_str, kMaxEDCLogLen);
    }

    if (color_other != 0)
    {
        snprintf(temp_str, kMaxEDCLogLen, PTN_PAPER_COLOR_OTHER, color_other);
        strncat(edc_log_content, temp_str, kMaxEDCLogLen);
    }

    if (append_edc_log(p_ctx, log_type, edc_log_content))
    {
        log0(ERROR, kModName, __func__, "Append EDC log failure");
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if (lcd_clean_scr(p_ctx->lkp_ctx) < kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not clean screen.");
        return kFailure;
    }

    if (lcd_clean(p_ctx->lkp_ctx) < kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not clean screen buffer.");
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
                    log0(ERROR, kModName, __func__, "Can not open backlight in idle.");
                    return kFailure;
                }
                bl_usec = 0;
            }
        }

        if (bl_usec > kMicroPerSecond * kLCDBackLightTimeout)
        {
            if (set_backlight(p_ctx, 0x00))
            {
                log0(ERROR, kModName, __func__, "Can not close backlight in idle when timeout.");
                return kFailure;
            }
            bl_usec = 0;
        }
        bl_usec += kMicroPerSecond / 10;

        usleep(kMicroPerSecond / 10);
    }

    if (strncmp(in_passwd, p_ctx->fn_passwd, kMaxPasswdLen) == 0)
    {
        log0(INFO, kModName, __func__, "Password correct");
        p_ctx->state = SETUP;
    }
    else
    {
        log0(INFO, kModName, __func__, "Password incorrect");
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if (stop_sync_thr(p_ctx) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can't stop threads.");
        return kFailure;
    }

    // Clean screen and set LED
    if(lcd_clean_scr(p_ctx->lkp_ctx) < kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not clean screen.");
        return kFailure;
    }

    if(lcd_clean(p_ctx->lkp_ctx) < kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not clean screen buffer.");
        return kFailure;
    }

    if (set_led(p_ctx, kLEDBlue) < kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not set LED.");
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
        switch (state)
        {
            case SET_PRT_TYPE:
                show_line(p_ctx, 0, STR_SETUP_PRT_CON_TYPE);
                show_line(p_ctx, 3, STR_EMPTY);
                while (kTrue)
                {
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
                show_line(p_ctx, 0, STR_SETUP_EDC_IP);
                show_line(p_ctx, 3, STR_EMPTY);
                while (kTrue)
                {
                    state_ret = get_ipv4_from_keypad(p_ctx, STR_EMPTY,
                            new_edc_ip, kMaxIPLen + 1, 1);
                    //fprintf(stderr, "ret: %d, value: %s\n", state_ret, new_edc_ip);
                    if (state_ret != kFailure)
                    {
                        if (trim_ipv4(new_edc_ip, kMaxIPLen + 1) == kFailure)
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
                show_line(p_ctx, 0, STR_SETUP_SUBMASK);
                show_line(p_ctx, 3, STR_EMPTY);
                while (kTrue)
                {
                    state_ret = get_ipv4_from_keypad(p_ctx, STR_EMPTY,
                                new_submask, kMaxIPLen + 1, 1);
                    //fprintf(stderr, "ret: %d, value: %s\n", state_ret, new_submask);
                    if (state_ret != kFailure)
                    {
                        if (trim_ipv4(new_edc_ip, kMaxIPLen + 1) == kFailure)
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
                show_line(p_ctx, 0, STR_SETUP_GATEWAY);
                show_line(p_ctx, 3, STR_EMPTY);
                while (kTrue)
                {
                    state_ret = get_ipv4_from_keypad(p_ctx, STR_EMPTY,
                                new_gateway, kMaxIPLen + 1, 1);
                    //fprintf(stderr, "ret: %d, value: %s\n", state_ret, new_gateway);
                    if (state_ret != kFailure)
                    {
                        if (trim_ipv4(new_gateway, kMaxIPLen + 1) == kFailure)
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
                show_line(p_ctx, 0, STR_SETUP_SERVER_IP);
                show_line(p_ctx, 3, STR_EMPTY);
                while (kTrue)
                {
                    state_ret = get_ipv4_from_keypad(p_ctx, STR_EMPTY,
                                new_server_ip, kMaxIPLen + 1, 1);
                    //fprintf(stderr, "ret: %d, value: %s\n", state_ret, new_server_ip);
                    if (state_ret != kFailure)
                    {
                        if (trim_ipv4(new_server_ip, kMaxIPLen + 1) == kFailure)
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
                show_line(p_ctx, 0, STR_SETUP_SERVER_PORT);
                show_line(p_ctx, 3, STR_EMPTY);
                while (kTrue)
                {
                    state_ret = get_str_from_keypad(p_ctx, STR_EMPTY,
                                new_server_port_str, kMaxPortLen + 1, 1);
                    //fprintf(stderr, "ret: %d, value: %s\n", state_ret, new_server_port_str);
                    if (state_ret != kFailure)
                    {
                        new_server_port = (int)strtol(new_server_port_str, &end_ptr, 10);
                        if (*end_ptr != '\0'
                                || new_server_port < kMaxSocketPortMin
                                || new_server_port > kMaxSocketPortMax)
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
                break;
        }

        if (state_ret == kFailure)
        {
            log1(ERROR, kModName, __func__,
                    "Fetch setup failure in state: %d", state);
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
                show_line(p_ctx, 2, STR_SETUP_WAIT);
                log1(INFO, kModName, __func__,
                        "Write to ini: %s", kServerIni);
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
                    log1(ERROR, kModName, __func__,
                            "Can not open %s.", kServerIni);
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
                    log1(ERROR, kModName, __func__,
                            "Write to %s failure", kServerIni);
                    fclose(fp_setup);
                    return kFailure;
                }
                fclose(fp_setup);

                if(!(fp_network = fopen(kNetworkIni, "w")))
                {
                    log1(ERROR, kModName, __func__,
                            "Can not open %s", kNetworkIni);
                    return kFailure;
                }
                ret = fprintf(fp_network, "%s=%s\n%s=%s\n%s=%s\n",
                        kSetupSelfIP, new_edc_ip,
                        kSetupSubmask, new_submask,
                        kSetupGateway, new_gateway);
                if (ret < 0)
                {
                    log1(ERROR, kModName, __func__,
                            "Write to %s failure", kServerIni);
                    fclose(fp_network);
                    return kFailure;
                }
                fclose(fp_network);

                // Re-connect to server
                if (p_ctx->connected == kTrue)
                {
                    log0(ERROR, kModName, __func__, "Disconnect from server...");
                    if (disconnect_server(p_ctx) != kSuccess)
                    {
                        log0(ERROR, kModName, __func__, "Disconnect from server failure.");
                        return kFailure;
                    }
                }

                if (system(kSetupIpCmd) != kSuccess)
                {
                    log0(ERROR, kModName, __func__, "Reset network setting failure.");
                    return kFailure;
                }

                if (connect_server(p_ctx) != kSuccess)
                {
                    log0(ERROR, kModName, __func__, "Connect to EDCAgent failure.");
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

    if (p_ctx->connected)
    {
        log0(INFO, kModName, __func__, "Sync lists from Server");
        if (sync_lists(p_ctx) != kSuccess)
        {
            log0(ERROR, kModName, __func__, "Sync lists from server fail!");
        }

        log0(INFO, kModName, __func__, "Load downloads lists");
        if (load_local_lists(p_ctx) != kSuccess)
        {
            log0(ERROR, kModName, __func__, "Load lists failure!");
            //TODO should terminate program.
        }
    }
    else
    {
        log0(ERROR, kModName, __func__, "Server is disconnected, can't sync lists!");
    }

    // Restart threads
    if (pthread_create(&(p_ctx->sync_list_thr), NULL,
                (void*)sync_list_thr_func, (void*)p_ctx) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Create sync thread failure.");
        return kFailure;
    }

    if (pthread_create(&(p_ctx->sync_log_thr), NULL,
                (void*)sync_log_thr_func, (void*)p_ctx) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Create sync thread failure.");
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    snprintf(scr_line_str, kMaxLineWord + 1, "%s%s", prompt, buf);
    show_line(p_ctx, scr_line, scr_line_str);

    ptr_buf = buf + strlen(buf);
    while (kTrue)
    {   
        //catch input and update screen
        if (get_press_key(p_ctx, &in_key) < 0)
        {
            //log0(ERROR, kModName, __func__, "Can not get keypad.");
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

            snprintf(scr_line_str, kMaxLineWord + 1, "%s%s", prompt, buf);
            show_line(p_ctx, scr_line, scr_line_str);
        }
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if (buf_len < kMaxIPLen + 1)
    {
        log0(ERROR, kModName, __func__, "Buffer too short for IPv4!");
        return kFailure;
    }

    snprintf(scr_line_str, kMaxLineWord + 1, "%s%s", prompt, buf);
    show_line(p_ctx, scr_line, scr_line_str);
    ptr_buf = buf + strlen(buf);
    while (kTrue)
    {   
        //catch input and update screen
        if (get_press_key(p_ctx, &in_key) < 0)
        {
            //log0(ERROR, kModName, __func__, "Can not get keypad.");
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
            snprintf(scr_line_str, kMaxLineWord + 1, "%s%s", prompt, buf);
            show_line(p_ctx, scr_line, scr_line_str);
        }
    }

    return kASCIIDown;
}


int buzzer(int msec)
{
    if (device_power(DEVICE_BUZZER_2, 1))
    {
        log0(ERROR, kModName, __func__, "Can't open buzzer!");
        return kFailure;
    }
    usleep(msec);
    if (device_power(DEVICE_BUZZER_2, 0))
    {
        log0(ERROR, kModName, __func__, "Can't close buzzer!");
        return kFailure;
    }

    return kSuccess;
}

int get_current_time_r(struct tm* p_time_info)
{
    time_t cur_time;

    if (!p_time_info)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    cur_time = time(NULL);
    localtime_r(&cur_time, p_time_info);

    return kSuccess;
}

int show_datetime(EDC_CTX* p_ctx, struct tm* p_time_info)
{
    char time_str[kMaxLineWord + 1];
    char line_str[kMaxLineWord + 1];

    if (!p_ctx)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    snprintf(time_str, kMaxLineWord + 1, "%.2d/%.2d %.2d:%.2d",
            p_time_info->tm_mon + 1,
            p_time_info->tm_mday,
            p_time_info->tm_hour,
            p_time_info->tm_min);

    if (left_right_str(line_str, kMaxLineWord + 1, time_str, p_ctx->edc_id))
    {
        log0(ERROR, kModName, __func__, "EDC id is too long in line of datetime.");
        return kFailure;
    }

    return show_line(p_ctx, 0, line_str);
}

int set_led(EDC_CTX* p_ctx, unsigned int conf)
{
    int i;
    int ret;
    int result = kSuccess;
    int failure_count = 0;
    const int lcd_offset = 20;

    if (pthread_mutex_lock(&p_ctx->lkp_ctx_mutex))
    {
        log0(ERROR, kModName, __func__, "Lock lkp_ctx mutex failure!");
        return kFailure;
    }

    for (i = 0; i < kLEDNum; i++)
    {
        while (kTrue)
        {
            ret = device_power(i + lcd_offset, (conf >> i & 0x01));
            if (ret < kSuccess)
            {
                log2(ERROR, kModName, __func__,
                        "Set LED failure, led: %d, ret: %d", i, ret);
                if (++failure_count > kMaxFailLimit)
                {
                    result = kFailure;
                    break;
                }
                continue;
            }

            usleep(kMicroPerSecond / 20);
            break;
        }
    }

    if (pthread_mutex_unlock(&p_ctx->lkp_ctx_mutex))
    {
        log0(ERROR, kModName, __func__, "Unlock lkp_ctx mutex failure!");
        return kFailure;
    }

    return kSuccess;
}

int set_backlight(EDC_CTX* p_ctx, unsigned char u8_type)
{
    int ret;
    int failure_count = 0;

    usleep(kMicroPerSecond / 10);
    while (kTrue)
    {
        if (pthread_mutex_lock(&p_ctx->lkp_ctx_mutex))
        {
            log0(ERROR, kModName, __func__, "Lock lkp_ctx mutex failure!");
            return kFailure;
        }

        ret = lkp_set_backlight(p_ctx->lkp_ctx, u8_type);

        if (pthread_mutex_unlock(&p_ctx->lkp_ctx_mutex))
        {
            log0(ERROR, kModName, __func__, "Unlock lkp_ctx mutex failure!");
            return kFailure;
        }

        if ( ret >= kSuccess)
        {
            break;
        }

        log1(ERROR, kModName, __func__,
                "Can not set backlight: %d", u8_type);
        if (++failure_count > kMaxFailLimit)
        {
            return kFailure;
        }
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }
    ptr = buf;

    //fprintf(stderr, "Spaces=%d, left=%d, right=%d.\n", spaces, strlen(left), strlen(right));
    //fprintf(stderr, "left=%s, right=%s.\n", left, right);
    if (spaces < 0)
    {
        log0(ERROR, kModName, __func__, "Bufer too small");
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
    int ret = 0;
    int result = kSuccess;
    int failure_count = 0;

    if (line > kMaxScreenLine - 1)
    {
        log0(ERROR, kModName, __func__, "Line exceed limited");
        return kFailure;
    }

    if (strlen(string) > kMaxLineWord)
    {
        log0(ERROR, kModName, __func__, "Line exceed limited");
        return kFailure;
    }
    
    if (pthread_mutex_lock(&p_ctx->lkp_ctx_mutex))
    {
        log0(ERROR, kModName, __func__, "Lock lkp_ctx mutex failure!");
        return kFailure;
    }

    while (kTrue)
    {
        ret = lcd_clean_buffer(p_ctx->lkp_ctx, 0,
                kFontHeight * line, kScreenWidth, kFontHeight);
        if (ret < kSuccess)
        {
            log2(ERROR, kModName, __func__,
                    "Clean buffer error, line: %d, ret: %d", line, ret);
            if (++failure_count > kMaxFailLimit)
            {
                result = kFailure;
                break;
            }
            continue;
        }

        ret = lcd_draw_text_16f(p_ctx->lkp_ctx, 0, kFontHeight * line, string, 0);
        if (ret < kSuccess)
        {
            log2(ERROR, kModName, __func__,
                    "Draw buffer error, line: %d, ret: %d", line, ret);
            if (++failure_count > kMaxFailLimit)
            {
                result = kFailure;
                break;
            }
            continue;
        }

        ret = lcd_print_out(p_ctx->lkp_ctx);
        if (ret < kSuccess)
        {
            log2(ERROR, kModName, __func__,
                    "Print out LCD error, str: '%s', ret: %d", string, ret);
            if (++failure_count > kMaxFailLimit)
            {
                result = kFailure;
                break;
            }
            continue;
        }

        result = kSuccess;
        break;
    }

    if (pthread_mutex_unlock(&p_ctx->lkp_ctx_mutex))
    {
        log0(ERROR, kModName, __func__, "Unlock lkp_ctx mutex failure!");
        return kFailure;
    }

    return result;
}

int read_rfid(EDC_CTX *p_ctx)
{
    int i;
    int len = 0;
    char *pos;
    unsigned char pcData[kMaxReadRFIDLen];

    unsigned char temp[kMaxReadRFIDLen];
    unsigned char *t;

    if (!p_ctx)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    pos = p_ctx->curr_card_sn;

    memset(pcData, '\0', kMaxReadRFIDLen);
    len = serReadCOM(&p_ctx->com_ctx, pcData, kMaxReadRFIDLen);
    if (len > kMaxCardReadLen)
    {
        log1(ERROR, kModName, __func__,
                "Read from com too long: %d", len);
        return kFailure;
    }

    //CLD test card number
    /*
    t = temp;
    log0(DEBUG, kModName, __func__, "Read data from RFID hex");
    for (i = 0; i < len; i++)
    {
        *t++ = pcData[i];
        fprintf(stderr, "%02X ", pcData[i]);
    }
    fprintf(stderr, "\n");
    *t = '\0';
    log2(DEBUG, kModName, __func__, "Read data from RFID char: '%s'(%d)", temp, len);
    */

    // Here is Hex to ASCII
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    emp_ptr = p_ctx->emp_list;

    if (pthread_mutex_lock(&p_ctx->emp_mutex))
    {
        log0(ERROR, kModName, __func__, "Lock Employee mutex failure!");
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
        log0(ERROR, kModName, __func__, "Unlock Employee mutex failure!");
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

/*
 * Fetch string before character chr into dest.
 * return length of the string.
 */
int get_str_before_char(const char* line, const char chr, char* dest, int dest_len)
{
    char *ptr;

    if (!line || !dest)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    ptr = strchr(line, chr);
    if (!ptr)
    {
        return kFailure;
    }

    if (dest_len < ptr - line + 1)
    {
        log0(ERROR, kModName, __func__, "Buffer too small.");
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    list_ptr = p_list;

    if (p_mutex && pthread_mutex_lock(p_mutex))
    {
        log0(ERROR, kModName, __func__, "Lock Employee mutex failure!");
        return kFailure;
    }

    if (!(fList = fopen(file_name, "r")))
    {
        log0(ERROR, kModName, __func__, "Can not open ini file.");
        return kFailure;
    }

    while (line_count < list_size &&
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
        if ((get_len = get_str_before_char(cur_ptr, kTab,
                        temp, kMaxReadLineLen)) == kFailure)
        {
            goto LOAD_EMP_FAIL_LINE;
        }
        list_ptr->curr_quota = (int)strtol(temp, NULL, 10);

        cur_ptr += (get_len + 1);
        if ((get_len = get_str_before_char(cur_ptr, kNewLine,
                        temp, kMaxReadLineLen)) == kFailure)
        {
            goto LOAD_EMP_FAIL_LINE;
        }
        list_ptr->only_mono = (int)((*temp == '0')?kTrue:kFalse);

        // Finally, Enable this item.
        list_ptr->enable = kTrue;

        list_ptr++;
        line_count++;
    }
    fclose(fList);

    if (p_mutex && pthread_mutex_unlock(p_mutex))
    {
        log0(ERROR, kModName, __func__, "Unlock Employee mutex failure!");
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
    log1(ERROR, kModName, __func__,
            "Employee list malformed in line: %d", line_count);
    if (p_mutex && pthread_mutex_unlock(p_mutex))
    {
        log0(ERROR, kModName, __func__, "Unlock Employee mutex failure!");
    }
    fclose(fList);
    return kFailure;
}

int load_edc_list(EDC_DATA *p_list, const int list_size, const char *file_name, pthread_mutex_t *p_mutex)
{
    FILE *fList;
    int line_count = 0;
    char line[kMaxReadLineLen];
    char temp[kMaxReadLineLen];
    char *cur_ptr;
    int get_len;
    EDC_DATA *list_ptr;

    if (!p_list)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    list_ptr = p_list;

    if (p_mutex && pthread_mutex_lock(p_mutex))
    {
        log0(ERROR, kModName, __func__, "Lock EDC mutex failure!");
        return kFailure;
    }

    if (!(fList = fopen(file_name, "r")))
    {
        log0(ERROR, kModName, __func__, "Can not open ini file.");
        return kFailure;
    }

    //<EDC_ID>\t<EDC_IP>\t<PRT_IP>\t<TIMEOUT>\t<SHOW_QUOTA>\t<MonoA3>\t<MonoA4>\t<ColorA3>\t<ColorA4>\n
    while ( line_count < list_size &&
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
        line_count++;
    }

    fclose(fList);
    if (p_mutex && pthread_mutex_unlock(p_mutex))
    {
        log0(ERROR, kModName, __func__, "Unlock EDC mutex failure!");
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
    log1(ERROR, kModName, __func__,
            "EDC list malformed in line %d", line_count);
    if (p_mutex && pthread_mutex_unlock(p_mutex))
    {
        log0(ERROR, kModName, __func__, "Unlock EDC mutex failure!");
    }
    fclose(fList);
    return kFailure;
}

int load_proj_list(PROJ_DATA *p_list, const int list_size, const char *file_name, pthread_mutex_t *p_mutex)
{
    FILE *fList;
    int line_count = 0;
    char line[kMaxReadLineLen];
    char *cur_ptr;
    int get_len;
    PROJ_DATA *list_ptr;

    if (!p_list)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    list_ptr = p_list;

    if (p_mutex && pthread_mutex_lock(p_mutex))
    {
        log0(ERROR, kModName, __func__, "Lock project mutex failure!");
        return kFailure;
    }

    if(!(fList = fopen(file_name, "r")))
    {
        log0(ERROR, kModName, __func__, "Can not open ini file.");
        return kFailure;
    }

    while ( line_count < list_size &&
            fgets(line, kMaxReadLineLen, fList))
    {
        cur_ptr = line;
        if ((get_len = get_str_before_char(cur_ptr, kNewLine,
                        list_ptr->proj_num, kMaxProjectCodeLen + 1)) == kFailure)
        {
            goto LOAD_PROJ_FAIL_LINE;
        }
        list_ptr++;
        line_count++;
    }
    
    fclose(fList);
    if (p_mutex && pthread_mutex_unlock(p_mutex))
    {
        log0(ERROR, kModName, __func__, "Unlock project mutex failure!");
        return kFailure;
    }

    return line_count;

LOAD_PROJ_FAIL_LINE:
    log1(ERROR, kModName, __func__,
            "Project list malformed in line %d", line_count);
    if (p_mutex && pthread_mutex_unlock(p_mutex))
    {
        log0(ERROR, kModName, __func__, "Unlock project mutex failure!");
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if(!(fpSetting = fopen(ini_file, "r")))
    {
        log0(ERROR, kModName, __func__, "Can not open ini file.");
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
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
        log1(ERROR, kModName, __func__,
            "ioctl() Fail!, fd: %d", fd);
        close(fd);
        return RET_ERR_READER_FILE;
    }

    /*termios functions use to control asynchronous communications ports*/
    if (tcgetattr(fd, &T_new) != 0) 
    {       /*fetch tty state*/
        log0(ERROR, kModName, __func__, "tcgetattr() Fail!");
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
        log0(ERROR, kModName, __func__, "tcsetattr() Fail!");
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    r = OpenDev(p_ctx->port);
    if( r < 0)
    {
        log0(ERROR, kModName, __func__, "Opem COM port Fail!");
        return kFailure;
    }

    p_ctx->com_handle = r;

    if( setport(p_ctx))
    {
        log0(ERROR, kModName, __func__, "Set COM port Fail!");
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if( p_ctx->com_handle < 0 )
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
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
        log0(ERROR, kModName, __func__, "Parameter Fail!");
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

int init_log(EDC_CTX* p_ctx, LOG_LEVEL level)
{
    g_log_level = level;
    char log_path[kMaxPathLen + 1]; 

    log0(INFO, kModName, __func__, "Open log file");
    memset(log_path, 0, kMaxPathLen + 1);
    snprintf(log_path, kMaxPathLen + 1, "./%s.%s", kModName, kLogSuffix);
    if ((g_log_fd = open(log_path, O_APPEND|O_RDWR|O_CREAT, 0666)) == kFailure)
    {
        log1(ERROR, kModName, __func__, "Can not open log file: %s", log_path);
        return kFailure;
    }

    log0(INFO, kModName, __func__, "Create pipe for log");
    if (pipe(g_log_pipe) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Can not create pipe for log");
        return kFailure;
    }

    if (pthread_create(&(p_ctx->log_thr), NULL,
                (void*)log_thr_func, NULL) != kSuccess)
    {
        log0(ERROR, kModName, __func__, "Create log thread failure.");
        return kFailure;
    }

    log2(INFO, kModName, __func__, "Duplicate stderr to pipe: %d -> %d", STDERR_FILENO, g_log_pipe[1]);
    if (dup2(g_log_pipe[1], STDERR_FILENO) == kFailure)
    {
        log0(ERROR, kModName, __func__, "Can not duplicate stderr");
        return kFailure;
    }

    log0(INFO, kModName, __func__, "Make sure baklog folder is exist");
    if (access(kLogFolder, F_OK) == kFailure)
    {
        if (mkdir(kLogFolder, 775) == kFailure)
        {
            log1(ERROR, kModName, __func__, "Can not make folder: %s", kLogFolder);
            return kFailure;
        }
    }

    return kSuccess;
}


int log_lock()
{
    pthread_mutex_t *mutex = get_log_mutex();
    if (mutex && !pthread_mutex_lock(mutex))
    {
        return kSuccess;
    }
    return kFailure;
}

int log_unlock()
{
    pthread_mutex_t *mutex = get_log_mutex();
    if (mutex && !pthread_mutex_unlock(mutex))
    {
        return kSuccess;
    }
    return kFailure;
}

static pthread_mutex_t* get_log_mutex()
{
    static pthread_mutex_t *mutex = NULL;

    if (mutex)
    {
        return mutex;
    }

    mutex = malloc(sizeof(pthread_mutex_t));

    if (!mutex)
    {
        return NULL;
    }

    if (pthread_mutex_init(mutex, NULL) != kSuccess)
    {
        free(mutex);
        mutex = NULL;
    }

    return mutex;
}

int get_file_size(const char* file_name)
{
    struct stat stat_buf;

    if (!file_name)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return kFailure;
    }

    if (stat(file_name, &stat_buf) == kFailure)
    {
        return kFailure;
    }

    return (int)stat_buf.st_size;
}


void log_thr_func(void)
{
    //EDC_CTX *edc_ctx;
    fd_set log_fds;
    char log_buf[kMaxLogLen];
    int read_len;
    int log_file_size;
    struct timeval timeout;
    int sel_ret;
    char log_path[kMaxPathLen + 1]; 
    char baklog_path[kMaxPathLen + 1]; 
    char cur_date[kMaxYmdHMSLen + 1];

    /*
    if (!ctx)
    {
        log0(ERROR, kModName, __func__, "Parameter Fail!");
        return;
    }
    edc_ctx = (EDC_CTX*)ctx;
    */
    snprintf(log_path, kMaxPathLen + 1, "./%s.%s", kModName, kLogSuffix);

    log0(INFO, kModName, __func__, "Log rotation thread start");
    while (kTrue) 
    {    
        //TODO: Need a stop signal
        if (kFalse)
        {
        }    

        // check if need rotate
        log_file_size = get_file_size(log_path);
        if (log_file_size == kFailure)
        {
            log1(ERROR, kModName, __func__, "Can not fetch log file size: %s", log_path);
        }
        else if (log_file_size > kMaxLogFileSize)
        {
            if (get_cur_YmdHMS_r(cur_date, kMaxYmdHMSLen + 1, kTrue) == NULL)
            {
                log0(FATAL, kModName, __func__, "Get current datetime fail");
                return;
            }

            snprintf(baklog_path, kMaxPathLen + 1,
                    "%s%s%s.%s", kLogFolder, kModName, cur_date, kLogSuffix);

            close(g_log_fd);
            log1(INFO, kModName, __func__, "Backup log file: %s", baklog_path);
            if (rename(log_path, baklog_path) == kFailure)
            {
                log2(FATAL, kModName, __func__,
                        "Backup log file failure. %s -> %s", log_path, baklog_path);
            }

            log1(INFO, kModName, __func__, "Reopen log file: %s", log_path);
            if ((g_log_fd = open(log_path, O_APPEND|O_RDWR|O_CREAT, 0666)) == kFailure)
            {
                log1(FATAL, kModName, __func__, "Can not open log file: %s", log_path);
                return;
            }
        }
        

        FD_ZERO(&log_fds);
        FD_SET(g_log_pipe[0], &log_fds);
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        sel_ret = select(g_log_pipe[0] + 1, &log_fds, NULL, NULL, &timeout);
        if (sel_ret < 0 && errno != EINTR)
        {
            log1(FATAL, kModName, __func__,
                    "Read stderr failure: %s", strerror(errno));
            return;
        }
        else if (sel_ret == 0)
        {
            //log0(DEBUG, kModName, __func__, "Read stderr timeout.");
        }
        else if (FD_ISSET(g_log_pipe[0], &log_fds))
        {
            memset(log_buf, 0, kMaxLogLen);
            if ((read_len = read(g_log_pipe[0], log_buf, kMaxLogLen)) < 0)
            {
                log0(ERROR, kModName, __func__, "Read log fd failure");
            }
            write(g_log_fd, log_buf, read_len);
        }
    }
    return;
}


int trim_ipv4(char* ipv4, const int len)
{
    char tmp_ip[MAX_IP_LEN + 1] = {0};
    char *ptr;
    char *new_ptr;
    int section_flag = kTrue;

    if (!ipv4)
    {
        log0(ERROR, kModName, __func__, "Paramater failure");
        return kFailure;
    }

    if (len > kMaxIPLen + 1)
    {
        log0(ERROR, kModName, __func__, "Buffer too small");
        return kFailure;
    }

    ptr = ipv4;
    memset(tmp_ip, 0, kMaxIPLen + 1);
    new_ptr = tmp_ip;

    while (ptr < ipv4 + len -1)
    {
        if (section_flag && *ptr == '0')
        {
            ptr++;
            continue;
        }

        if (*ptr == kPoint)
        {
            if (section_flag == kTrue)
            {
                *new_ptr++ = '0';
            }
            section_flag = kTrue;
        }
        else
        {
            section_flag = kFalse;
        }

        *new_ptr++ = *ptr;
        ptr++;
    }

    strncpy(ipv4, tmp_ip, len);
    if (inet_addr(tmp_ip) == kFailure)
    {
        return kFailure;
    }

    return kSuccess;
}
