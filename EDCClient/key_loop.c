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
const int kMaxHeaderLen= 16;
const int kKeyScanPerSec = 20;
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


enum RET{
        RET_SUCCESS = 0,           // ???????€?
        RET_ERR_UNKNOW  = -0xFF,   // ??ªç?¥ç????¯èª¤
        RET_ERR_SETUP_FILE,        // è®€???è¨­å??æª?èª?
        RET_ERR_READER_FILE,       // ??????Readerå¤±æ??
        RET_ERR_UNKNOWN_READER     // ??ªç?¥ç??Reader??¯èª¤
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

} EDC_CTX;

int print_printertype(PRINTERTYPE *usage);
int init_printertype(PRINTERTYPE *usage);

int main()
{
    EDC_CTX ctx;
    EDC_CTX* p_ctx = &ctx;
    unsigned char in_key;

    fprintf(stderr, "Create Context .\n");
    p_ctx->lkp_ctx = lkp_create();
    if (!p_ctx->lkp_ctx)
    {
        fprintf(stderr, "Can not create lkp contexet.\n");
        return kFailure;
    }
    sleep(1);

    fprintf(stderr, "Open Device.\n");
    if (device_open())
    {
        fprintf(stderr, "Can not open device.\n");
        return kFailure;
    }
    sleep(1);



    while (kTrue)
    {
        if (kpd_get_keypad(p_ctx->lkp_ctx, &in_key) >= 0)
        {
            if (in_key > 0 )
            {
                fprintf(stderr, "input key: %d\n", in_key);
            }
        }
        else
        {
            fprintf(stderr, "Get keypad failure.\n");
        }

        fprintf(stderr, "Sleep 1s....\n");
        usleep(1000000);
    }

    return kTrue;
}

