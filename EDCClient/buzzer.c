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
#define MAX_PROJECT_LEN     (16)
#define MAX_CARD_SN_LEN     (16)
#define MAX_DEP_NAME_LEN    (64)
#define MAX_DEP_NO_LEN      (16)
#define MAX_EMP_NAME_LEN    (64)
#define MAX_EMP_NO_LEN      (64)
#define MAX_EDC_ID_LEN      (16)
#define MAX_IP_LEN          (16)
#define MAX_PORT_LEN        (5)

#define MAX_EMP_LIST_SIZE   (1024)
#define MAX_EDC_LIST_SIZE   (2)
#define MAX_PROJ_LIST_SIZE  (128)
#define MAX_MEM_EDC_LOG     (256)
#define MAX_EDC_LOG_LEN     (128)
#define MAX_LOG_TYPE_LEN    (16)

const char kEmpListFile[] = "./employee.list";
const char kEDCListFile[] = "./edc.list";
const char kProjListFile[] = "./projects.list";
const char kEDCLogFile[] = "./edc_tmp.log";
const char kServerIni[] = "./edc_setup.ini";
const char kNetworkIni[] = "./edc_network.ini";
const char kSetupIpCmd[] = "./edc_setip.sh";
const char kTempFileSuffix[] = ".tmp";
const int kMaxEmpListSize = MAX_EMP_LIST_SIZE;
const int kMaxEDCListSize = MAX_EDC_LIST_SIZE;
const int kMaxProjListSize = MAX_PROJECT_LEN;

const int kSuccess = 0;
const int kFailure = -1;
const int kGetKeyPadCancel = 1;
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
const int kMaxYmdHMSLen = 20;
const int kASCIIFn = 16;
const int kASCIIClear = 20;
const int kASCIICancel = 21;
const int kASCIIEnter = 22;
const int kASCIIEject = 23;
const int kASCIIFirstVisiable = 32;
const int kASCIILastVisiable = 127;
const int kASCII_zero = 48;
const int kASCII_nine = 57;

const char kSetupStrServerIP[] = "server";
const char kSetupStrServerPort[] = "port";
const char kSetupStrEDCID[] = "id";
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

const int kMaxReadLineLen = 512;
const int kMaxPathLen = 512;
const char kTab = '\t';
const char kNewLine = '\n';
const char kSep = '|';
const int kMaxIPLen = MAX_IP_LEN;
const int kMaxPortLen = MAX_PORT_LEN;
const int kMaxSockTimeout = 10;
const int kMaxConLenStrLen = 16;

const int kLCDBackLightTimeout = 20;
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
const char STR_SETUP_EDC_ID[] = "設定EDC ID";
const char STR_SETUP_EDC_IP[] = "設定IP";
const char STR_SETUP_SUBMASK[] = "設定Submask";
const char STR_SETUP_GATEWAY[] = "設定Gateway ";
const char STR_SETUP_SERVER_IP[] = "設定SERVER IP";
const char STR_SETUP_SERVER_PORT[] = "設定SERVER PORT";
const char STR_SETUP_CURRENT[] = "目前:";
const char STR_SETUP_NEWSET[] = "設定:";
const char STR_SETUP_ERROR[] = "格式錯誤";

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
    SETUP
} OP_STATE;

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

const char PTN_CARD_VALID[] = "VALID %s";
const char PTN_CARD_INVALID[] = "INVALID %s";
const char PTN_CARD_ENJECT[] = "ENJECT %s";
const char PTN_CARD_TIMEOUT[] = "TIMEOUT %s";

const char PTN_PAPER_BB[] = "BB %d";
const char PTN_PAPER_BS[] = "BS %d";
const char PTN_PAPER_CB[] = "CB %d";
const char PTN_PAPER_CS[] = "CS %d";

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
} EDC_DATA;

typedef struct _project_data
{
    char    proj_num[MAX_PROJECT_LEN + 1];
} PROJ_DATA;

typedef struct _edc_ctx
{
    PLKPCONTEXT     lkp_ctx;
    COM_CTX         com_ctx;
    char            edc_id[MAX_EDC_ID_LEN + 1];
    char            edc_ip[MAX_IP_LEN + 1];
    char            submask[MAX_IP_LEN + 1];
    char            gateway[MAX_IP_LEN + 1];
    char            server_ip[MAX_IP_LEN + 1];
    int             server_port;

    OP_STATE        state;
    char            project_code[MAX_PROJECT_LEN + 1];
    char            curr_card_sn[MAX_CARD_SN_LEN + 1];
    int             curr_emp;

    int             server_fd;
    int             connected;

    int             emp_num;
    EMPLOYEE_DATA   emp_list[MAX_EMP_LIST_SIZE];
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

// EDC Utilities
int init(EDC_CTX*, const char*);
void quit(EDC_CTX*);
int show_datetime(EDC_CTX*);
int show_line(EDC_CTX*, int, const char*);
int set_led(unsigned int conf);
int read_rfid(EDC_CTX*);
int is_valid_card(EDC_CTX*);
int buzzer(int);
int get_str_from_keypad(EDC_CTX*, const char*, char*, const int, const int);
int get_ipv4_from_keypad(EDC_CTX*, const char*, char*, const int, const int);

// List Utilities
int sync_lists(EDC_CTX*);
int load_local_lists(EDC_CTX*);
int load_server_set(EDC_CTX*, const char*);
int load_employee_list(EMPLOYEE_DATA*, const int, const char*, pthread_mutex_t*);
int load_edc_list(EDC_DATA*, const int, const char*, pthread_mutex_t*);
int load_proj_list(PROJ_DATA*, const int, const char*, pthread_mutex_t*);
int get_str_before_char(const char*, const char, char*, int);

// EDC log Utilities
int sync_log(EDC_CTX*);
int save_log_to_local(EDC_CTX*, const char*);
int get_cur_YmdHMS_r(char*, int);
int append_edc_log(EDC_CTX*, const EDC_LOG_TYPE, const char*);

// Network Utilities
int connect_server(EDC_CTX*);
size_t sock_write(int, const void*, size_t);
size_t sock_read(int, char*, size_t);
int set_nonblock(int, int);
int dl_remote_list(EDC_CTX*, const char*, const char*);
int get_remote_list(EDC_CTX*,const char*,const char*, pthread_mutex_t*);

// Thread
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
        //sleep(kSyncListInterval);
        sleep(3);
        if (p_ctx->connected)
        {
            if (sync_lists(p_ctx) != kSuccess)
            {
                fprintf(stderr, "Sync lists from server fail!\n");
            }
            continue;
        }
        else
        {
            if (connect_server(p_ctx) != kSuccess)
            {
                fprintf(stderr, "Re-Connect to server failure, "
                        "skip sync list.\n");
                continue;
            }
            else
            {
                if (sync_lists(p_ctx) != kSuccess)
                {
                    fprintf(stderr, "Sync lists from server fail!\n");
                }
                continue;
            }
        }
    }
}

void sync_log_thr_func(void *ctx)
{
    EDC_CTX* p_ctx;
    int sync_success = kFalse;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return ;
    }

    p_ctx = (EDC_CTX*)ctx;

    while (kTrue)
    {
        sleep(kSyncLogInterval);
        sync_success = kFalse;

        if (p_ctx->connected)
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
        else
        {
            if (connect_server(p_ctx) != kSuccess)
            {
                fprintf(stderr, "Re-Connect to server failure, "
                        "store to local disk.\n");
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
        if (snprintf(p_ctx->edc_tmp_log[p_ctx->edc_log_num], kMaxEDCLogLen, "%s\t%s\t%s\t%d\%s\t%s",
                    TYPE2STR[type],
                    p_ctx->edc_id,
                    p_ctx->project_code,
                    p_ctx->curr_emp,
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

    if(!(fp_log = fopen(log_tmp_file, "w")))
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
                if (sock_write(p_ctx->server_fd, log_buf, buf_ptr - log_buf) == 0)
                {
                    fprintf(stderr, "Send request to server fail.\n");
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

        if (sock_write(p_ctx->server_fd, log_buf, buf_ptr - log_buf) == 0)
        {
            fprintf(stderr, "Send request to server fail.\n");
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
                if (sock_write(p_ctx->server_fd, log_buf, buf_ptr - log_buf) == 0)
                {
                    fprintf(stderr, "Send request to server fail.\n");
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
                line_len = snprintf(buf_ptr, buf_size, "%s\n", cur_edc_log);
                buf_size -= line_len;
                buf_ptr += line_len;
            }
            log_count++;
        }
        p_ctx->edc_log_num = 0;

        fprintf(stderr, "CLD: %s\n", log_buf);
        if (sock_write(p_ctx->server_fd, log_buf, buf_ptr - log_buf) == 0)
        {
            fprintf(stderr, "Send request to server fail.\n");
            fclose(fp_log);
            return kFailure;
        }

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
    memset(p_ctx->project_code, 0, kMaxProjectCodeLen);

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

    //TODO set ip/server_ip/edc_id from ini file
    if (load_server_set(p_ctx, kServerIni) != kSuccess)
    {
        fprintf(stderr, "Load server setup failure.\n");
        return kFailure;
    }

    if (connect_server(p_ctx) != kSuccess)
    {
        fprintf(stderr, "Connect to server failure,\
                use local files.\n");
        if (load_local_lists(p_ctx))
        {
            fprintf(stderr, "Load local list file failure.\n");
            return kFailure;
        }
    }

    if (sync_lists(p_ctx) != kSuccess)
    {
        fprintf(stderr, "Sync list file failure.\n");
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
    /*
    int             tmp_emp_num;
    int             tmp_edc_num;
    int             tmp_proj_num;
    */

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

    return load_local_lists(p_ctx);
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
            while (1)
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
                    fprintf(stderr, "Connect failure.\n");
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
    char port_tmp[kMaxPortLen + 1];
    //char edc_id[kMaxEDCIDLen + 1];
    int port;
    char *end_ptr;

    if (!ini_file)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if (!(fpSetting = fopen(ini_file, "r")))
    {
        fprintf(stderr, "Can not open ini file.\n");
        return RET_ERR_SETUP_FILE;
    }

    if (!feof(fpSetting))
    {
        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, p_ctx->server_ip, &len);

        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, port_tmp, &len);
        port = strtol(port_tmp, &end_ptr, 10);
        if (*end_ptr != '\0')
        {
            fprintf(stderr, "Port setup in INI file error!\n");
            return kFailure;
        }
        p_ctx->server_port = port;

        fgets(line, kMaxReadLineLen, fpSetting);
        FileGetValue(line, p_ctx->edc_id, &len);
    }
    fclose(fpSetting);


    fprintf(stderr, "Server ip: %s\n", p_ctx->server_ip);
    fprintf(stderr, "Server port: %d\n", p_ctx->server_port);
    fprintf(stderr, "id: %s\n", p_ctx->edc_id);

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

    sock = p_ctx->server_fd;
    memset(send_buf, 0, kMaxReadLineLen);

    send_len = snprintf(send_buf, kMaxReadLineLen,
            "%s\t%s\n", sync_cmd, p_ctx->edc_id);

    if (sock_write(sock, send_buf, send_len) == 0)
    {
        fprintf(stderr, "Send request to server fail.\n");
        return kFailure;
    }

    if ((total_recv = sock_read(sock,
            list_buf, kMaxEmpListBuf)) <= 0)
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

size_t sock_write(int fd, const void *vptr, size_t n)
{
    size_t    nleft;
    ssize_t    nwritten;
    const char  *ptr;

    ptr = (char *)vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ((nwritten = send(fd, ptr, nleft,0 )) < 0 )
        {
            fprintf(stderr, "Error sending data!\n");
            break;
        }

        nleft -= nwritten;
        ptr   += nwritten;
    }
    return (n);
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
                    fprintf(stderr, "List buffer too small.\n");
                    return kFailure;
                }
                strncpy(curr, recv_ptr, recv_len);
                curr += recv_len;
                *curr = '\0';

                //fprintf(stderr, "CLD:total_recv:%d, content-length: %d\n", total_recv, content_len);
                if (content_len > 0 && total_recv >= content_len)
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
    //int ret;
    //int remain_sec;
    char *in_pos = p_ctx->project_code;
    unsigned char in_key;

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

    if (set_led(kLEDBlue) != kSuccess)
    {
        fprintf(stderr, "Can not set LED.\n");
        return kFailure;
    }

    show_line(p_ctx, 1, STR_PLEASE_CARD);
    //show_line(p_ctx, 3, STR_EMPOLYEE_ID);

    // Initial user data
    memset(&p_ctx->curr_card_sn, 0, kMaxCardSNLen);
    memset(&p_ctx->project_code, 0, kMaxProjectCodeLen);
    p_ctx->curr_emp = 0;
    while (kTrue)
    {   
        //catch input and update screen
        if (kpd_get_keypad(p_ctx->lkp_ctx, &in_key) < 0)
        {
            fprintf(stderr, "Can not get keypad.\n");
            return kFailure;
        }
        else
        {
            if (in_key != 0 )
            {
                if (in_key == kASCIIFn)
                {
                    p_ctx->state = SETUP;
                    break;
                }

                *in_pos++ = in_key;
                if (in_pos != p_ctx->project_code)
                {
                    show_line(p_ctx, 2, p_ctx->project_code);
                }

                // Enable all backlight
                if (lkp_set_backlight(p_ctx->lkp_ctx, 0xff))
                {
                    fprintf(stderr, "Can not set backlight.\n");
                    return kFailure;
                }
            }
        }

        if (show_datetime(p_ctx) != kSuccess)
        {
            return kFailure;
        }

        //catch carding
        serWriteCOM(&p_ctx->com_ctx, kSendComSignal, sizeof(kSendComSignal));
        usleep(kMicroPerSecond / 10);
        if (read_rfid(p_ctx) == kSuccess)
        {
            if (is_valid_card(p_ctx))
            {
                p_ctx->state = QUOTA;
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

    buzzer((kMicroPerSecond / 10) * 3);
    usleep(kMicroPerSecond * 5);
    p_ctx->state = IDLE;
    return kSuccess;
}

int quota_state(EDC_CTX* p_ctx)
{
    char quota_line[kMaxLineWord];
    char mono_line[kMaxLineWord];
    char color_line[kMaxLineWord];
    char remain_line[kMaxLineWord];
    EMPLOYEE_DATA *curr_emp;
    EDC_DATA *curr_edc;
    int time_remain;

    unsigned char in_key;

    char edc_log[kMaxEDCLogLen];

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    if (p_ctx->project_code[0] == '\0')
    {
        snprintf(p_ctx->project_code, kMaxProjectCodeLen, "%s", "0000");
    }

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
    //TODO
    // Init print
    /*
    if (ptr_count_init(p_ctx->lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Initial print counter failure\n");
        return kFailure;
    }
    */

    // Ref is_valid_card()
    curr_emp = &(p_ctx->emp_list[p_ctx->curr_emp - 1]);
    curr_edc = &(p_ctx->edc_list[0]);
    time_remain = curr_edc->limit_time;

    snprintf(quota_line, kMaxLineWord, "%s:%d",
            STR_QUOTA, curr_emp->init_quota);

    snprintf(mono_line, kMaxLineWord, "%s%d %s%d",
            STR_MONO_A3, curr_edc->mono_a3,
            STR_MONO_A4, curr_edc->mono_a4);

    snprintf(color_line, kMaxLineWord, "%s%d %s%d",
            STR_COLOR_A3, curr_edc->color_a3,
            STR_COLOR_A4, curr_edc->color_a4);

    show_line(p_ctx, 0, quota_line);
    show_line(p_ctx, 1, mono_line);
    show_line(p_ctx, 2, color_line);

    while (kTrue)
    {
        //catch input and update screen
        if (kpd_get_keypad(p_ctx->lkp_ctx, &in_key) < 0)
        {
            fprintf(stderr, "Can not get keypad.\n");
            return kFailure;
        }
        else
        {
            if (in_key != 17 )
            {
                //User input enject
                break;
            }
        }
        snprintf(remain_line, kMaxLineWord, "%s: %d",
                STR_REMAIN_SEC, time_remain);
        show_line(p_ctx, 3, remain_line);
        usleep(kMicroPerSecond);
        if (time_remain-- <= 0)
        {
            p_ctx->state = IDLE;
            break;
        }
    }

    //TODO
    //Get statistic info from print
    /*
    if (ptr_count_get(p_ctx->lkp_ctx, prt_counter) != kSuccess)
    {
        fprintf(stderr, "Get print counter failure\n");
        return kFailure;
    }

    // Get use status
    if ( (prt_count.u8_work_status & 0x01) == 1 )
    {
        // Operation Finish
        if (gen_cost_log(p_ctx, prt_count))
        {
            fprintf(stderr, "Generate print EDC log failure\n");
            return kFailure;

        }

    }

    // Get paper-track status
    if ( (prt_count.u8_work_status & 0x02) == 1)
    {
        // Lack paper
    }

    //release print
    if (ptr_count_stop(p_ctx->lkp_ctx) != kSuccess)
    {
        fprintf(stderr, "Stop print counter failure\n");
        return kFailure;
    }
    */
    return kSuccess;
}

/*
int gen_cost_log(EDC_CTX* p_ctx, PRINTERCOUNT *p_prt)
{
    int mono_a3;
    int mono_a4;
    int color_a3;
    int color_a4;
    char edc_log_content[kMaxEDCLogLen];

    if (!p_ctx || !p_prt)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    mono_a3 = p_prt->u16_gray_scale_a + p_prt->u16_double_gray_scale_a * 2;
    mono_a4 = p_prt->u16_gray_scale_b + p_prt->u16_double_gray_scale_b * 2;
    color_a3 = p_prt->u16_color_a + p_prt->u16_double_color_a * 2;
    color_a4 = p_prt->u16_color_b + p_prt->u16_double_color_b * 2;

    if (mono_a3 != 0)
    {
        snprintf(edc_log_content, kMaxEDCLogLen, PTN_PAPER_BB, mono_a3);
        //TODO type may need change
        if (append_edc_log(p_ctx, COPY, edc_log_content))
        {
            fprintf(stderr, "Append EDC log failure\n");
            return kFailure;
        }
    }

}
*/

int scanning_state(EDC_CTX* p_ctx)
{
    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    return kSuccess;
}

int setup_state(EDC_CTX* p_ctx)
{
    char new_edc_id[kMaxEDCIDLen + 1];
    char new_edc_ip[kMaxIPLen + 1];
    char new_submask[kMaxIPLen + 1];
    char new_gateway[kMaxIPLen + 1];
    char new_server_ip[kMaxIPLen + 1];
    char new_server_port[kMaxPortLen + 1];
    int new_server_port_num;
    char *end_ptr;
    char cur_line[kMaxLineWord + 1];

    FILE *fp_setup;
    FILE *fp_network;
    int ret;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    // Stop Threads
    if (pthread_cancel(p_ctx->sync_list_thr) != kSuccess)
    {
        fprintf(stderr, "Can not cancel sync_list thread.\n");
        return kFailure;
    }

    if (pthread_cancel(p_ctx->sync_log_thr) != kSuccess)
    {
        fprintf(stderr, "Can not cancel sync_log thread.\n");
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

    memset(new_edc_id, 0, kMaxEDCIDLen + 1);
    memset(new_edc_ip, 0, kMaxIPLen + 1);
    memset(new_submask, 0, kMaxIPLen + 1);
    memset(new_gateway, 0, kMaxIPLen + 1);
    memset(new_server_ip, 0, kMaxIPLen + 1);

    // Set EDC ID
    snprintf(cur_line, kMaxLineWord + 1, "%s%s", STR_SETUP_CURRENT, p_ctx->edc_id);
    show_line(p_ctx, 0, STR_SETUP_EDC_ID);
    show_line(p_ctx, 1, cur_line);
    if (get_str_from_keypad(p_ctx, STR_SETUP_NEWSET,
                new_edc_id, kMaxEDCIDLen + 1, 2) == kGetKeyPadCancel)
    {
        strncpy(new_edc_id , p_ctx->edc_id, kMaxEDCIDLen + 1);
    }

    // Set EDC IP
    while (kTrue)
    {
        snprintf(cur_line, kMaxLineWord + 1, "%s", p_ctx->edc_ip);
        show_line(p_ctx, 0, STR_SETUP_EDC_IP);
        show_line(p_ctx, 1, cur_line);
        show_line(p_ctx, 3, STR_EMPTY);
        if (get_ipv4_from_keypad(p_ctx, NULL,
                    new_edc_ip, kMaxIPLen + 1, 2) == kGetKeyPadCancel)
        {
            strncpy(new_edc_ip , p_ctx->edc_ip, kMaxIPLen + 1);
        }

        if (inet_addr(new_edc_ip) == -1)
        {
            memset(new_edc_ip, 0, kMaxIPLen + 1);
            show_line(p_ctx, 3, STR_SETUP_ERROR);
            usleep(kMicroPerSecond);
            continue;
        }

        break;
    }

    // Set Submask
    while (kTrue)
    {
        snprintf(cur_line, kMaxLineWord + 1, "%s", p_ctx->submask);
        show_line(p_ctx, 0, STR_SETUP_SUBMASK);
        show_line(p_ctx, 1, cur_line);
        show_line(p_ctx, 3, STR_EMPTY);
        if (get_ipv4_from_keypad(p_ctx, NULL,
                    new_submask, kMaxIPLen + 1, 2) == kGetKeyPadCancel)
        {
            strncpy(new_submask , p_ctx->submask, kMaxIPLen + 1);
        }

        if (inet_addr(new_submask) == -1)
        {
            memset(new_submask, 0, kMaxIPLen + 1);
            show_line(p_ctx, 3, STR_SETUP_ERROR);
            usleep(kMicroPerSecond);
            continue;
        }

        break;
    }

    // Set Gateway
    while (kTrue)
    {
        snprintf(cur_line, kMaxLineWord + 1, "%s", p_ctx->gateway);
        show_line(p_ctx, 0, STR_SETUP_GATEWAY);
        show_line(p_ctx, 1, cur_line);
        show_line(p_ctx, 3, STR_EMPTY);
        if (get_ipv4_from_keypad(p_ctx, NULL,
                    new_gateway, kMaxIPLen + 1, 2) == kGetKeyPadCancel)
        {
            strncpy(new_gateway, p_ctx->gateway, kMaxIPLen + 1);
        }

        if (inet_addr(new_gateway) == -1)
        {
            memset(new_gateway, 0, kMaxIPLen + 1);
            show_line(p_ctx, 3, STR_SETUP_ERROR);
            usleep(kMicroPerSecond);
            continue;
        }

        break;
    }

    // Set Server IP
    while (kTrue)
    {
        snprintf(cur_line, kMaxLineWord + 1, "%s", p_ctx->server_ip);
        show_line(p_ctx, 0, STR_SETUP_SERVER_IP);
        show_line(p_ctx, 1, cur_line);
        show_line(p_ctx, 3, STR_EMPTY);
        if (get_ipv4_from_keypad(p_ctx, NULL,
                    new_server_ip, kMaxIPLen + 1, 2) == kGetKeyPadCancel)
        {
            strncpy(new_server_ip, p_ctx->server_ip, kMaxIPLen + 1);
        }

        if (inet_addr(new_server_ip) == -1)
        {
            memset(new_server_ip, 0, kMaxIPLen + 1);
            show_line(p_ctx, 3, STR_SETUP_ERROR);
            usleep(kMicroPerSecond);
            continue;
        }

        break;
    }

    // Set Server Port
    while (kTrue)
    {
        snprintf(cur_line, kMaxLineWord + 1, "%d", p_ctx->server_port);
        show_line(p_ctx, 0, STR_SETUP_SERVER_PORT);
        show_line(p_ctx, 1, cur_line);
        show_line(p_ctx, 3, STR_EMPTY);
        if (get_str_from_keypad(p_ctx, STR_SETUP_SERVER_PORT,
                    new_server_port, kMaxPortLen + 1, 2) == kGetKeyPadCancel)
        {
            //strncpy(new_server_port , p_ctx->server_port, kMaxPortLen + 1);
            snprintf(new_server_port, kMaxPortLen + 1, "%d", p_ctx->server_port);
        }

        new_server_port_num = (int)strtol(new_server_port, &end_ptr, 10);
        if (*end_ptr != '\0')
        {
            memset(new_server_port, 0, kMaxPortLen + 1);
            show_line(p_ctx, 3, STR_SETUP_ERROR);
            usleep(kMicroPerSecond);
            continue;
        }
        break;
    }

    strncpy(p_ctx->edc_id, new_edc_id, kMaxEDCIDLen+1);
    strncpy(p_ctx->edc_ip, new_edc_ip, kMaxIPLen+1);
    strncpy(p_ctx->submask, new_submask, kMaxIPLen+1);
    strncpy(p_ctx->gateway, new_gateway, kMaxIPLen+1);
    strncpy(p_ctx->server_ip, new_server_ip, kMaxIPLen+1);
    p_ctx->server_port = new_server_port_num;
    /*
    fprintf(stderr, "CLD: EDC_ID: %s.\n", new_edc_id);
    fprintf(stderr, "CLD: EDC_IP: %s.\n", new_edc_ip);
    fprintf(stderr, "CLD: SUBMASK: %s.\n", new_submask);
    fprintf(stderr, "CLD: GATEWAY: %s.\n", new_gateway);
    fprintf(stderr, "CLD: SERVER_IP: %s.\n", new_server_ip);
    fprintf(stderr, "CLD: SERVER_PORT: %s.\n", new_server_port);
    */

    // Write back to file
    if(!(fp_setup = fopen(kServerIni, "w")))
    {
        fprintf(stderr, "Can not open %s.\n", kServerIni);
        return kFailure;
    }
    ret = fprintf(fp_setup, "%s=%s\n%s=%s\n%s=%s\n",
            kSetupStrServerIP, new_server_ip,
            kSetupStrServerPort, new_server_port,
            kSetupStrEDCID, new_edc_id);
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
    if (disconnect_server(p_ctx) != kSuccess)
    {
        fprintf(stderr, "Connect to server failure");
        return kFailure;
    }

    if (system(kSetupIpCmd) != kSuccess)
    {
        fprintf(stderr, "Reset network setting fail.");
        return kFailure;
    }

    if (connect_server(p_ctx) != kSuccess)
    {
        fprintf(stderr, "Connect to server failure");
        return kFailure;
    }

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

    ptr_buf = buf;
    while (kTrue)
    {   
        //catch input and update screen
        if (kpd_get_keypad(p_ctx->lkp_ctx, &in_key) < 0)
        {
            fprintf(stderr, "Can not get keypad.\n");
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
                *--ptr_buf = '\0';
            }
            else if (in_key == kASCIICancel)
            {
                memset(buf, 0, buf_len);
                ptr_buf = buf;
                return kGetKeyPadCancel;
            }
            else if (in_key == kASCIIEnter)
            {
                *ptr_buf = '\0';
                break;
            }
        }
        snprintf(scr_line_str, kMaxLineWord + 1, "%s%s", prompt, buf);
        show_line(p_ctx, scr_line, scr_line_str);
        usleep(kMicroPerSecond / 10);
    }

    return kSuccess;
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

    ptr_buf = buf;
    while (kTrue)
    {   
        //catch input and update screen
        if (kpd_get_keypad(p_ctx->lkp_ctx, &in_key) < 0)
        {
            fprintf(stderr, "Can not get keypad.\n");
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
                if (*--ptr_buf == '.')
                {
                    --ptr_buf;
                }
                *ptr_buf = '\0';
            }
            else if (in_key == kASCIICancel)
            {
                memset(buf, 0, buf_len);
                ptr_buf = buf;
                return kGetKeyPadCancel;
            }
            else if (in_key == kASCIIEnter)
            {
                *ptr_buf = '\0';
                break;
            }
        }
        snprintf(scr_line_str, kMaxLineWord + 1, "%s", buf);
        show_line(p_ctx, scr_line, scr_line_str);
        usleep(kMicroPerSecond / 10);
    }

    return kSuccess;
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
    struct tm * time_info;
    char time_str[kMaxScreenLine];

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    cur_time = time(NULL);
    time_info = localtime(&cur_time);
    if (strftime(time_str, kMaxLineWord,
            kTimeFormat, time_info) == 0)
    {
        fprintf(stderr, "Can not get time string.\n");
        return kFailure;
    }

    return show_line(p_ctx, 0, time_str);
}

int set_led(unsigned int conf)
{
    int i;
    const int lcd_offset = 20;

    for (i = 0; i < kLEDNum; i++)
    {
        if (device_power(i + lcd_offset, (conf >> i & 0x01)))
        {
            fprintf(stderr, "Can not power on LED device: %d.\n", i + lcd_offset);
            return kFailure;
        }
    }

    return kSuccess;
}

int show_line(EDC_CTX *p_ctx, int line, const char *string)
{
    int ret;

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

    ret = lcd_print_out(p_ctx->lkp_ctx);
    if (ret != 0)
    {
        fprintf(stderr, "Print out LCD error, line=%d, ret=%d\n", line, ret);
        return kFailure;
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

    if( len > 0 && (pcData[0] == 0xA4 && pcData[1] == 0x01) )
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
        show_line(p_ctx, 3, p_ctx->curr_card_sn);
        return kSuccess;
    }

    return kFailure;
}

int is_valid_card(EDC_CTX *p_ctx)
{
    int i;
    EMPLOYEE_DATA* emp_list;

    if (!p_ctx)
    {
        fprintf(stderr, "Parameter Fail!\n");
        return kFailure;
    }

    emp_list = p_ctx->emp_list;

    if (pthread_mutex_lock(&p_ctx->emp_mutex))
    {
        fprintf(stderr, "Lock Employee mutex failure!\n");
        return kFailure;
    }

    for (i = 0; i < p_ctx->emp_num; i++)
    {
        if (strncmp(emp_list->card_sn, p_ctx->curr_card_sn, kMaxCardSNLen) == 0)
        {
            // i.e. index 0 is first employee
            p_ctx->curr_emp = i + 1;
            return kTrue;
        }
        emp_list++;
    }

    if (pthread_mutex_unlock(&p_ctx->emp_mutex))
    {
        fprintf(stderr, "Unlock Employee mutex failure!\n");
        return kFailure;
    }

    return kFalse;
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
        if ((get_len = get_str_before_char(cur_ptr, kNewLine,
                        temp, kMaxReadLineLen)) == kFailure)
        {
            goto LOAD_EDC_FAIL_LINE;
        }
        list_ptr->color_a4 = (int)strtol(temp, NULL, 10);

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
        printf("edc_id: %s\n", list_ptr->edc_id);
        printf("edc_ip: %s\n", list_ptr->edc_ip);
        printf("printer: %s\n", list_ptr->printer_ip);
        printf("limit_time: %d\n", list_ptr->limit_time);
        printf("show_quota: %d\n", list_ptr->show_quota);
        printf("mono a3: %d\n", list_ptr->mono_a3);
        printf("mono a4: %d\n", list_ptr->mono_a4);
        printf("color a3: %d\n", list_ptr->color_a3);
        printf("color a4: %d\n", list_ptr->color_a4);
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
