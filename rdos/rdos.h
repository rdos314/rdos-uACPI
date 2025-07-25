
#ifndef _RDOS_H
#define _RDOS_H

#pragma pack( __push, 1 )

#define RDOSAPI

#ifdef __WATCOMC__
#include "machtype.h"
#define real_math   xreal
#endif

#ifdef __GNUC__
#define real_math   long double
#endif

#include <stdarg.h>
#include "rdoshdr.h"
#include "rdu.h"

#define FILE_ATTRIBUTE_READONLY         0x1
#define FILE_ATTRIBUTE_HIDDEN           0x2
#define FILE_ATTRIBUTE_SYSTEM           0x4
#define FILE_ATTRIBUTE_DIRECTORY        0x10
#define FILE_ATTRIBUTE_ARCHIVE          0x20
#define FILE_ATTRIBUTE_NORMAL           0x80

#ifndef O_RDONLY

#define O_RDONLY        0x0000  /* open for read only */
#define O_WRONLY        0x0001  /* open for write only */
#define O_RDWR          0x0002  /* open for read and write */
#define O_APPEND        0x0010  /* writes done at end of file */
#define O_CREAT         0x0020  /* create new file */
#define O_TRUNC         0x0040  /* truncate existing file */
#define O_NOINHERIT     0x0080  /* file is not inherited by child process */
#define O_TEXT          0x0100  /* text file */
#define O_BINARY        0x0200  /* binary file */
#define O_EXCL          0x0400  /* exclusive open */

#endif

#define LGOP_NULL  0
#define LGOP_NONE  1
#define LGOP_OR  2
#define LGOP_AND  3
#define LGOP_XOR  4
#define LGOP_INVERT  5
#define LGOP_INVERT_OR  6
#define LGOP_INVERT_AND  7
#define LGOP_INVERT_XOR  8
#define LGOP_ADD  9
#define LGOP_SUBTRACT  10
#define LGOP_MULTIPLY  11

#define getred(pgc)       (((pgc)>>16)&0xFF)
#define getgreen(pgc)     (((pgc)>>8)&0xFF)
#define getblue(pgc)      ((pgc)&0xFF)
#define mkcolor(r,g,b)    (((r)<<16)|((g)<<8)|(b))

#define AUDIO_WIDGET_TYPE_OUTPUT      1
#define AUDIO_WIDGET_TYPE_INPUT       2
#define AUDIO_WIDGET_TYPE_MIXER       3
#define AUDIO_WIDGET_TYPE_SELECTOR    4
#define AUDIO_WIDGET_TYPE_PIN         5
#define AUDIO_WIDGET_TYPE_POWER       6
#define AUDIO_WIDGET_TYPE_BEEP        7

typedef void (TRdosCallback)(void *param, char ch);

struct RdosThreadEntryPoint
{
    long long Offset;
    short int Sel;
};

struct RdosThreadState
{
     short int ID;
     char Name[32];
     unsigned long MsbTime;
     unsigned long LsbTime;
     char List[32];
     long Offset;
     short int Sel;
};

struct RdosThreadActionState
{
     short int ID;
     char Name[32];
     unsigned long MsbTime;
     unsigned long LsbTime;
     char List[32];
     char Action[32];
     struct RdosThreadEntryPoint Pos;
     short int UserCount;
     struct RdosThreadEntryPoint UserCall[64];
};

struct RdosTss
{
    long cr3;
    long eip;
    long eflags;
    long eax;
    long ecx;
    long edx;
    long ebx;
    long esp;
    long ebp;
    long esi;
    long edi;
    short int es;
    short int cs;
    short int ss;
    short int ds;
    short int fs;
    short int gs;
    short int ldt;
    long dr[4];
    long dr7;
    long MathControl;
    long MathStatus;
    long MathTag;
    long MathEip;
    short int MathCs;
    long MathDataOffs;
    short int MathDataSel;
    real_math st[8];
    char WcSpace[16];
};

struct RdosUsbEvent
{
    short int Event;
    short int Controller;
    short int Port;
    char Pipe;
};

struct RdosFutex
{
    int Handle;
    int Counter;
    short int Val;
    short int Owner;
    char *Name;
};

struct RdosDirEntry
{
    long long Inode;
    long long Size;
    unsigned long long CreateTime;
    unsigned long long AccessTime;
    unsigned long long ModifyTime;
    int Attrib;
    int Flags;
    int Uid;
    int Gid;
    int Pos;

    short int PathNameSize;
    char PathName[];
};

struct RdosDirInfo
{
    struct RdosDirEntry *Entry;
    int HeaderSize;
    int Count;
};

struct RdosFileInfo
{
    long long SectorCount;
    long long DiscSize;
    long long CurrSize;
    long long CreateTime;
    long long AccessTime;
    long long ModifyTime;
    int Attrib;
    int Flags;
    int Uid;
    int Gid;
    int ServHandle;
    int BytesPerSector;
    char Name[1];
};

struct RdosFileMapEntry
{
    long long Pos;
    int Size;
    char *Base;
};

struct RdosFileHandleInfo
{
    long long ReqSize;
    long long PosArr[480];
    int Bitmap[15];
    struct RdosFutex Futex;
};

struct RdosFileMap
{
    unsigned char SortedArr[241];
    unsigned short int Resv;
    char Update;
    int Count;
    struct RdosFileHandleInfo *Handle;
    struct RdosFileInfo *Info;
    struct RdosFileMapEntry MapArr[240];
};

#define USB_EVENT_ATTACH                1
#define USB_EVENT_DETACH                2
#define USB_EVENT_CONTROLLER_ERROR      3
#define USB_EVENT_CRC_ERROR             4
#define USB_EVENT_BIT_STUFFING_ERROR    5
#define USB_EVENT_DATA_TOGGLE_ERROR     6
#define USB_EVENT_STALL                 7
#define USB_EVENT_NOT_RESPONDING        8
#define USB_EVENT_PID_FAILURE           9
#define USB_EVENT_UNEXPECTED_PID       10
#define USB_EVENT_DATA_OVERRUN         11
#define USB_EVENT_DATA_UNDERRUN        12
#define USB_EVENT_BUFFER_OVERRUN       13
#define USB_EVENT_BUFFER_UNDERRUN      14
#define USB_EVENT_DATA_BUFFER_ERROR    15
#define USB_EVENT_BABBLE               16
#define USB_EVENT_TRANS_ERROR          17
#define USB_EVENT_MISSED_MICROFRAME    18
#define USB_EVENT_HALTED               19
#define USB_EVENT_TRB_ERROR            20
#define USB_EVENT_UNKNOWN              21
#define USB_EVENT_BANDWIDTH_ERROR      22
#define USB_EVENT_NO_SLOTS             23
#define USB_EVENT_SLOT_NOT_ENABLED     24
#define USB_EVENT_PIPE_NOT_ENABLED     25
#define USB_EVENT_NO_PING              26
#define USB_EVENT_RESET                27
#define USB_EVENT_OVER_CURRENT         28


#define uss_handle  0
#define uss_counter 4
#define uss_val 8
#define uss_owner 10
#define uss_size 12

// Exception handling prototypes

#define STATUS_BREAKPOINT               0x80000003L
#define STATUS_SINGLE_STEP              0x80000004L
#define STATUS_ACCESS_VIOLATION         0xC0000005L
#define STATUS_IN_PAGE_ERROR            0xC0000006L
#define STATUS_INVALID_HANDLE           0xC0000008L
#define STATUS_NO_MEMORY                0xC0000017L
#define STATUS_ILLEGAL_INSTRUCTION      0xC000001DL
#define STATUS_ARRAY_BOUNDS_EXCEEDED    0xC000008CL
#define STATUS_FLOAT_DENORMAL_OPERAND   0xC000008DL
#define STATUS_FLOAT_DIVIDE_BY_ZERO     0xC000008EL
#define STATUS_FLOAT_INEXACT_RESULT     0xC000008FL
#define STATUS_FLOAT_INVALID_OPERATION  0xC0000090L
#define STATUS_FLOAT_OVERFLOW           0xC0000091L
#define STATUS_FLOAT_STACK_CHECK        0xC0000092L
#define STATUS_FLOAT_UNDERFLOW          0xC0000093L
#define STATUS_INTEGER_DIVIDE_BY_ZERO   0xC0000094L
#define STATUS_INTEGER_OVERFLOW         0xC0000095L
#define STATUS_PRIVILEGED_INSTRUCTION   0xC0000096L
#define STATUS_STACK_OVERFLOW           0xC00000FDL
#define STATUS_CONTROL_C_EXIT           0xC000013AL

typedef struct _FLOATING_SAVE_AREA {
    int ControlWord;
    int StatusWord;
    int TagWord;
    int ErrorOffset;
    int ErrorSelector;
    int DataOffset;
    int DataSelector;
    char RegisterArea[80];
    int   Cr0NpxState;
} FLOATING_SAVE_AREA;

typedef struct _CONTEXT
{
    long ContextFlags;
    long Dr0;
    long Dr1;
    long Dr2;
    long Dr3;
    long Dr6;
    long Dr7;
    FLOATING_SAVE_AREA FloatSave;
    long SegGs;
    long SegFs;
    long SegEs;
    long SegDs;
    long Edi;
    long Esi;
    long Ebx;
    long Edx;
    long Ecx;
    long Eax;
    long Ebp;
    long Eip;
    long SegCs;
    long EFlags;
    long Esp;
    long SegSs;
} CONTEXT;

typedef struct _EXCEPTION_RECORD
{
    long ExceptionCode;
    long ExceptionFlags;
    struct _EXCEPTION_RECORD *ExceptionRecord;
    void *ExceptionAddress;
} EXCEPTION_RECORD;

typedef struct _EXCEPTION_POINTERS {
    EXCEPTION_RECORD *ExceptionRecord;
    CONTEXT *ContextRecord;
} EXCEPTION_POINTERS;

#pragma pack( __pop )


// API functions

#ifndef __GNUC__

#ifdef __cplusplus
extern "C" {
#endif

int RDOSAPI RdosTestGate(char *par);

void RDOSAPI RdosLoad32();

short int RDOSAPI RdosSwapShort(short int val);
long RDOSAPI RdosSwapLong(long val);

int RDOSAPI RdosXchg(int *pos, int val);
int RDOSAPI RdosAllocateTls();
void RDOSAPI RdosFreeTls(int index);
void *RDOSAPI RdosGetTls(int index);
void RDOSAPI RdosSetTls(int index, void *val);

int RDOSAPI RdosGetCharSize(const char *str);

long RDOSAPI RdosGetLongRandom(void);
long RDOSAPI RdosGetRandom(long range);

int RDOSAPI RdosGetMac(char *mac);
void RDOSAPI RdosCreateUuid(char *uuid);

void RDOSAPI RdosWaitAnio(int Irq);

int RDOSAPI RdosGetPciBus(int ReqBus, int *Bus, int *Device, int *Function);
int RDOSAPI RdosIsPciFunctionUsed(int Bus, int Dev, int Func);
int RDOSAPI RdosGetPciIrq(int Bus, int Dev, int Func);
int RDOSAPI RdosGetPciClass(int Bus, int Dev, int Func, int *Class, int *Subclass);
int RDOSAPI RdosGetPciInterface(int Bus, int Dev, int Func);
int RDOSAPI RdosGetPciMsi(int Bus, int Dev, int Func, int *Reg, int *IrqCount);
int RDOSAPI RdosGetPciMsiX(int Bus, int Dev, int Func, int *Reg, int *IrqCount);
int RDOSAPI RdosGetPciDeviceName(int Bus, int Dev, int Func, char *AcpiName);
int RDOSAPI RdosGetPciDeviceVendor(int Bus, int Dev, int Func, int *Vendor, int *Device);

long RDOSAPI RdosGetAcpiStatus();
int RDOSAPI RdosGetAcpiObject(int Index, char *AcpiName);
int RDOSAPI RdosGetAcpiMethod(int Object, int Index, char *AcpiName);
int RDOSAPI RdosGetAcpiDevice(int Index, char *AcpiName);
int RDOSAPI RdosGetAcpiDeviceIrq(int Device, int Index, int *Share, int *Polarity, int *TriggerMode);
int RDOSAPI RdosGetAcpiDeviceIo(int Device, int Index, int *Start, int *End);
int RDOSAPI RdosGetAcpiDeviceMem(int Device, int Index, int *Start, int *End);
int RDOSAPI RdosGetCpuTemperature();

int RDOSAPI RdosGetHidDevice(int Device, int *UsbController, int *UsbPort);
int RDOSAPI RdosGetHidReportItem(int Device, int Index, char *Name);
int RDOSAPI RdosGetHidReportInputData(int Device, int Report, int Index, char *Name);
int RDOSAPI RdosGetHidReportOutputData(int Device, int Report, int Index, char *Name);
int RDOSAPI RdosGetHidReportFeatureData(int Device, int Report, int Index, char *Name);

int RDOSAPI RdosGetAudioDeviceCount();
int RDOSAPI RdosGetAudioCodecCount(int Device);
int RDOSAPI RdosGetAudioCodecVersion(int Device, int Codec, int *VendorID, int *DeviceID);
int RDOSAPI RdosGetAudioWidgetInfo(int Device, int Codec, int Node, char *Info);
int RDOSAPI RdosGetAudioWidgetConnectionList(int Device, int Codec, int Node, int *ConnectionList);
int RDOSAPI RdosGetSelectedAudioConnection(int Device, int Codec, int Node);
void RDOSAPI RdosGetAudioInputAmpCap(int Device, int Codec, int Node, int *min, int *max);
void RDOSAPI RdosGetAudioOutputAmpCap(int Device, int Codec, int Node, int *min, int *max);
int RDOSAPI RdosHasAudioInputMute(int Device, int Codec, int Node);
int RDOSAPI RdosHasAudioOutputMute(int Device, int Codec, int Node);
int RDOSAPI RdosReadAudioInputAmp(int Device, int Codec, int Node, int Channel, int Input);
int RDOSAPI RdosReadAudioOutputAmp(int Device, int Codec, int Node, int Channel);
int RDOSAPI RdosIsAudioInputAmpMuted(int Device, int Codec, int Node, int Channel, int Input);
int RDOSAPI RdosIsAudioOutputAmpMuted(int Device, int Codec, int Node, int Channel);
int RDOSAPI RdosGetFixedAudioOutput(int *Device, int *Codec, int *Node);
int RDOSAPI RdosGetJackAudioOutput(int Num, int *Device, int *Codec, int *Node);
int RDOSAPI RdosGetJackAudioInput(int Num, int *Device, int *Codec, int *Node);

int RDOSAPI RdosQueryVideoMode(int Mode, int *BitsPerPixel, int *xres, int *yres);
void RDOSAPI RdosSetTextMode();
int RDOSAPI RdosSetVideoMode(int *BitsPerPixel, int *xres, int *yres, int *linesize, void **buffer);
void RDOSAPI RdosSetClipRect(int handle, int xmin, int ymin, int xmax, int ymax);
void RDOSAPI RdosClearClipRect(int handle);
void RDOSAPI RdosSetDrawColor(int handle, int color);
void RDOSAPI RdosSetLGOP(int handle, int lgop);
void RDOSAPI RdosSetHollowStyle(int handle);
void RDOSAPI RdosSetFilledStyle(int handle);

int RDOSAPI RdosAnsiToUtf8(const char *AnsiStr, char *Utf8Str, int BufferSize);
int RDOSAPI RdosUtf8ToAnsi(const char *Utf8Str, char *AnsiStr, int BufferSize);

int RDOSAPI RdosAnsiToUtf16(const char *AnsiStr, short int *Utf16Str, int BufferSize);
int RDOSAPI RdosUtf16ToAnsi(const short int *Utf16Str, char *AnsiStr, int BufferSize);

int RDOSAPI RdosOpenFont(int id, int height);
void RDOSAPI RdosCloseFont(int font);
void RDOSAPI RdosGetStringMetrics(int font, const char *str, int *width, int *height);
void RDOSAPI RdosSetFont(int handle, int font);
int RDOSAPI RdosGetPixel(int handle, int x, int y);
void RDOSAPI RdosSetPixel(int handle, int x, int y);
void RDOSAPI RdosBlit(int SrcHandle, int DestHandle, int width, int height,
                                int SrcX, int SrcY, int DestX, int DestY);
void RDOSAPI RdosDrawMask(int handle, void *mask, int RowSize, int width, int height,
                                int SrcX, int SrcY, int DestX, int DestY);
void RDOSAPI RdosDrawLine(int handle, int x1, int y1, int x2, int y2);
void RDOSAPI RdosDrawString(int handle, int x, int y, const char *str);
void RDOSAPI RdosDrawRect(int handle, int x, int y, int width, int height);
void RDOSAPI RdosDrawEllipse(int handle, int x, int y, int width, int height);
int RDOSAPI RdosCreateBitmap(int BitsPerPixel, int width, int height);
int RDOSAPI RdosCreateAlphaBitmap(int width, int height);
int RDOSAPI RdosExtractValidBitmapMask(int handle);
int RDOSAPI RdosExtractInvalidBitmapMask(int handle);
int RDOSAPI RdosExtractAlphaBitmap(int handle);
int RDOSAPI RdosDuplicateBitmapHandle(int handle);
void RDOSAPI RdosCloseBitmap(int handle);
int RDOSAPI RdosCreateStringBitmap(int font, const char *str);
void RDOSAPI RdosGetBitmapInfo(int handle, int *BitPerPixel, int *width, int *height,
                                           int *linesize, void **buffer);

int RDOSAPI RdosCreateSprite(int DestHandle, int BitmapHandle, int MaskHandle, int lgop);
void RDOSAPI RdosCloseSprite(int handle);
void RDOSAPI RdosShowSprite(int handle);
void RDOSAPI RdosHideSprite(int handle);
void RDOSAPI RdosMoveSprite(int handle, int x, int y);

void RDOSAPI RdosSetForeColor(int color);
void RDOSAPI RdosSetBackColor(int color);

#ifdef __RDOS__     // these are only available in user-mode

void *RDOSAPI RdosAllocateMem(int Size);
void RDOSAPI RdosFreeMem(void *ptr);

void *RDOSAPI RdosAllocateDebugMem(int Size);
void RDOSAPI RdosFreeDebugMem(void *ptr);

long RDOSAPI RdosGetThreadLinear(int Thread, int Sel, long Offset);
int RDOSAPI RdosReadThreadMem(int Thread, int Sel, long Offset, char *Buf, int Size);
int RDOSAPI RdosWriteThreadMem(int Thread, int Sel, long Offset, char *Buf, int Size);
int RDOSAPI RdosGetDebugThread(void);
void RDOSAPI RdosGetThreadTss(int Thread, struct RdosTss *tss);
void RDOSAPI RdosSetThreadTss(int Thread, struct RdosTss *tss);

int RDOSAPI RdosSetCodeBreak(int Thread, int Reg, int Sel, long Offset);
int RDOSAPI RdosSetReadDataBreak(int Thread, int Reg, int Sel, long Offset, int Size);
int RDOSAPI RdosSetWriteDataBreak(int Thread, int Reg, int Sel, long Offset, int Size);
void RDOSAPI RdosClearBreak(int Thread, int Reg);

void RDOSAPI RdosDebugTrace(void);
void RDOSAPI RdosDebugPace();
void RDOSAPI RdosDebugGo();
void RDOSAPI RdosDebugRun(void);
void RDOSAPI RdosDebugNext(void);

int RDOSAPI RdosPrintf(TRdosCallback *outproc, void *param, const char *format, va_list args);

#endif

void *RDOSAPI RdosAllocateCircularBuffer(int size);
void RDOSAPI RdosFreeCircularBuffer(void *ptr, int size);

int RDOSAPI RdosHasPhysical64();
int RDOSAPI RdosUsesPae();

long long RDOSAPI RdosGetFreePhysical();
int RDOSAPI RdosGetPhysicalEntryType(int Entry);
long long RDOSAPI RdosGetPhysicalEntryBase(int Entry);
long long RDOSAPI RdosGetPhysicalEntrySize(int Entry);
int RDOSAPI RdosGetFreeGdt();
int RDOSAPI RdosGetFreeLdt();
int RDOSAPI RdosGetFreeSmallKernelLinear();
int RDOSAPI RdosGetFreeBigKernelLinear();
int RDOSAPI RdosGetFreeSmallLocalLinear();
int RDOSAPI RdosGetFreeBigLocalLinear();

int RDOSAPI RdosGetFreeHandles();
int RDOSAPI RdosGetFreeHandleMem();

int RDOSAPI RdosGetMaxComPort(void);
int RDOSAPI RdosIsComAvailable(char ID);
int RDOSAPI RdosGetStdComPar(char ID, int *Irq, int *Io, int *Baud);
int RDOSAPI RdosGetUsbComPar(char ID, int *Type);
int RDOSAPI RdosGetUsbComDevice(char ID, int *Controller, int *Device);
int RDOSAPI RdosGetUsbCdcComPar(char ID, int *Vendor, int *Product);
int RDOSAPI RdosGetUsbCdcComDevice(char ID, int *Controller, int *Device);
int RDOSAPI RdosGetUsbBusPar(char ID);
int RDOSAPI RdosOpenCom(char ID, long BaudRate, char Parity, char DataBits, char StopBits, int SendBufSize, int RecBufSize);
void RDOSAPI RdosCloseCom(int Handle);
void RDOSAPI RdosFlushCom(int Handle);
char RDOSAPI RdosReadCom(int Handle);
int RDOSAPI RdosWriteCom(int Handle, char Val);
void RDOSAPI RdosEnableCts(int Handle);
void RDOSAPI RdosDisableCts(int Handle);
void RDOSAPI RdosEnableAutoRts(int Handle);
void RDOSAPI RdosDisableAutoRts(int Handle);
int RDOSAPI RdosIsAutoRtsOn(int Handle);
int RDOSAPI RdosGetCts(int Handle);
int RDOSAPI RdosGetDsr(int Handle);
void RDOSAPI RdosSetDtr(int Handle);
void RDOSAPI RdosResetDtr(int Handle);
void RDOSAPI RdosSetRts(int Handle);
void RDOSAPI RdosResetRts(int Handle);
int RDOSAPI RdosGetReceiveBufferSpace(int Handle);
int RDOSAPI RdosGetSendBufferSpace(int Handle);
void RDOSAPI RdosWaitForSendCompletedCom(int Handle);
void RDOSAPI RdosResetCom(int Handle);
int RDOSAPI RdosGetComRecCount(int Handle);
int RDOSAPI RdosSupportsFullDuplex(int Handle);
int RDOSAPI RdosSendComBreak(int Handle, char CharCount);

int RDOSAPI RdosGetMaxPrinters();
int RDOSAPI RdosOpenPrinter(char ID);
void RDOSAPI RdosClosePrinter(int Handle);
int RDOSAPI RdosGetPrinterName(int Handle, char *NameBuf);
int RDOSAPI RdosIsPrinterJammed(int Handle);
int RDOSAPI RdosIsPrinterPaperLow(int Handle);
int RDOSAPI RdosIsPrinterPaperEnd(int Handle);
int RDOSAPI RdosIsPrinterCutterJammed(int Handle);
int RDOSAPI RdosIsPrinterOk(int Handle);
int RDOSAPI RdosIsPrinterHeadLifted(int Handle);
int RDOSAPI RdosHasPrinterPaperInPresenter(int Handle);
int RDOSAPI RdosHasPrinterTemperatureError(int Handle);
int RDOSAPI RdosHasPrinterFeedError(int Handle);
void RDOSAPI RdosPrintTest(int Handle);
int RDOSAPI RdosCreatePrinterBitmap(int Handle, int Height);
void RDOSAPI RdosPrintBitmap(int Handle, int Bitmap);
void RDOSAPI RdosPresentPrinterMedia(int Handle, int Length);
void RDOSAPI RdosEjectPrinterMedia(int Handle);
void RDOSAPI RdosWaitForPrint(int Handle);
void RDOSAPI RdosResetPrinter(int Handle);

int RDOSAPI RdosGetLonModules();
void RDOSAPI RdosResetLonModule(char ID);
int RDOSAPI RdosOpenLonModule(char ID, int SendBuffers, int ReceiveBuffers);
void RDOSAPI RdosCloseLonModule(int Handle);
void RDOSAPI RdosAddWaitForLonModule(int Handle, int LonHandle, int ID);
void RDOSAPI RdosSendLonModuleMsg(int Handle, const char *Buf, int Size);
int RDOSAPI RdosHasLonModuleMsg(int Handle);
int RDOSAPI RdosReceiveLonModuleMsg(int Handle, char *Buf);

int RDOSAPI RdosGetMaxCardDev();
int RDOSAPI RdosOpenCardDev(char ID);
void RDOSAPI RdosCloseCardDev(int Handle);
int RDOSAPI RdosGetCardDevName(int Handle, char *NameBuf);
int RDOSAPI RdosIsCardDevOk(int Handle);
int RDOSAPI RdosIsCardDevBusy(int Handle);
int RDOSAPI RdosIsCardDevInserted(int Handle);
int RDOSAPI RdosHadCardDevInserted(int Handle);
void RDOSAPI RdosClearCardDevInserted(int Handle);
int RDOSAPI RdosWaitForCard(int Handle, char *Strip);
int RDOSAPI RdosGetCardDevTrack1(int Handle, char *Track1);

int RDOSAPI RdosIsCanOnline();
int RDOSAPI RdosCreateCanModuleBitmap();
int RDOSAPI RdosGetCanModuleInfo(int Module, int *ComCount, int *Id);
int RDOSAPI RdosGetCanModuleVersion(int Module, int *MajorVersion, int *MinorVersion, int *SubVersion);
int RDOSAPI RdosGetCanLoaderVersion(int Module, int *MajorVersion, int *MinorVersion, int *SubVersion);
void RDOSAPI RdosGetCanSerialNumber(int Module, char *Buf);
int RDOSAPI RdosGetCanModuleRestarts(int Module);
int RDOSAPI RdosCheckCanSerialPort(int ComPort, int *ModuleId, int *PortNr);
int RDOSAPI RdosProgramCanModule(int Module, const char *ProgramName);
int RDOSAPI RdosWaitForCanModuleProgramming(int Module, int *ErrorCode, int *Position);
int RDOSAPI RdosIsCanModuleOnline(int Module);

int RDOSAPI RdosGetCanBridgeVersion(int *MajorVersion, int *MinorVersion, int *SubVersion);
int RDOSAPI RdosProgramCanBridge(const char *ProgramName);
int RDOSAPI RdosWaitForCanBridgeProgramming(int *ErrorCode, int *Position);

int RDOSAPI RdosGetHandleCount();
int RDOSAPI RdosOpenHandle(const char *Name, int Mode);
int RDOSAPI RdosCloseHandle(int Handle);
int RDOSAPI RdosDeleteHandle(int Handle);
struct RdosFileMap *RDOSAPI RdosGetHandleMap(int Handle, int *HandleId);
void RDOSAPI RdosUpdateHandle(int Handle);
int RDOSAPI RdosMapHandle(int Handle, long long Pos, int Size);
int RDOSAPI RdosGrowHandle(int Handle, long long Pos, int Size);
int RDOSAPI RdosPollHandle(int Handle, void *Buf, int Size);
int RDOSAPI RdosReadHandle(int Handle, void *Buf, int Size);
int RDOSAPI RdosWriteHandle(int Handle, const void *Buf, int Size);
int RDOSAPI RdosDupHandle(int Handle);
int RDOSAPI RdosDup2Handle(int Src, int Dest);
long long RDOSAPI RdosGetHandleSize(int Handle);
long long RDOSAPI RdosSetHandleSize(int Handle, long long Size);
int RDOSAPI RdosGetHandleMode(int Handle);
int RDOSAPI RdosSetHandleMode(int Handle, int Mode);
long long RDOSAPI RdosGetHandlePos(int Handle);
long long RDOSAPI RdosSetHandlePos(int Handle, long long Size);
int RDOSAPI RdosEofHandle(int Handle);
int RDOSAPI RdosIsHandleDevice(int Handle);
int RDOSAPI RdosGetHandleCreateTime(int Handle, unsigned long *MsbTime, unsigned long *LsbTime);
int RDOSAPI RdosGetHandleModifyTime(int Handle, unsigned long *MsbTime, unsigned long *LsbTime);
int RDOSAPI RdosGetHandleAccessTime(int Handle, unsigned long *MsbTime, unsigned long *LsbTime);
int RDOSAPI RdosSetHandleModifyTime(int Handle, unsigned long MsbTime, unsigned long LsbTime);
int RDOSAPI RdosGetHandleReadBufferCount(int Handle);
int RDOSAPI RdosGetHandleWriteBufferSpace(int Handle);
int RDOSAPI RdosHasHandleException(int Handle);
int RDOSAPI RdosAddWaitForHandleRead(int WaitHandle, int Handle, void *ID);
int RDOSAPI RdosAddWaitForHandleWrite(int WaitHandle, int Handle, void *ID);
int RDOSAPI RdosAddWaitForHandleException(int WaitHandle, int Handle, void *ID);
int RDOSAPI RdosSelect(void *HandleMaskArr, int HandleCount, int Timeout);
int RDOSAPI RdosSetHandleBlockingMode(int Handle);
int RDOSAPI RdosSetHandleNonblockingMode(int Handle);

long RDOSAPI RdosOpenFile(const char *FileName, char Access);
long RDOSAPI RdosCreateFile(const char *FileName, int Attrib);
void RDOSAPI RdosCloseFile(long Handle);
long RDOSAPI RdosDuplFile(long Handle);
long long RDOSAPI RdosGetFileSize(long Handle);
void RDOSAPI RdosSetFileSize(long Handle, long long Size);
long long RDOSAPI RdosGetFilePos(long Handle);
void RDOSAPI RdosSetFilePos(long Handle, long long Pos);
int RDOSAPI RdosReadFile(long Handle, void *Buf, int Size);
int RDOSAPI RdosWriteFile(long Handle, const void *Buf, int Size);
void RDOSAPI RdosGetFileTime(long Handle, unsigned long *MsbTime, unsigned long *LsbTime);
void RDOSAPI RdosSetFileTime(long Handle, unsigned long MsbTime, unsigned long LsbTime);

int RDOSAPI RdosCreateMapping(int Size);
int RDOSAPI RdosCreateNamedMapping(const char *Name, int Size);
int RDOSAPI RdosCreateNamedFileMapping(const char *Name, int Size, long FileHandle);
int RDOSAPI RdosOpenNamedMapping(const char *Name);
void RDOSAPI RdosSyncMapping(int Handle);
void RDOSAPI RdosCloseMapping(int Handle);
void RDOSAPI RdosMapView(int Handle, int Offset, void *Base, int Size);
void RDOSAPI RdosUnmapView(int Handle);

int RDOSAPI RdosSetCurDrive(int Drive);
int RDOSAPI RdosGetCurDrive(void);
int RDOSAPI RdosSetCurDir(const char *PathName);
int RDOSAPI RdosGetCurDir(int Drive, char *PathName);
int RDOSAPI RdosMakeDir(const char *PathName);
int RDOSAPI RdosRemoveDir(const char *PathName);
int RDOSAPI RdosRenameFile(const char *ToName, const char *FromName);
int RDOSAPI RdosDeleteFile(const char *PathName);
int RDOSAPI RdosGetFileAttribute(const char *PathName, int *Attribute);
int RDOSAPI RdosSetFileAttribute(const char *PathName, int Attribute);

int RDOSAPI RdosCreateVfsDiscCmd(int DiscNr, const char *Cmd);
void RDOSAPI RdosCloseVfsCmd(int Handle);
void RDOSAPI RdosAddWaitForVfsCmd(int WaitHandle, int CmdHandle, int ID);
int RDOSAPI RdosIsVfsCmdDone(int Handle);
int RDOSAPI RdosGetVfsResponseSize(int Handle);
int RDOSAPI RdosGetVfsResponseData(int Handle, char *Buf, int MaxSize);

void RDOSAPI RdosDefineFaultSave(int DiscNr, long StartSector, long Sectors);
void RDOSAPI RdosClearFaultSave();
int RDOSAPI RdosGetFaultThreadState(int ThreadNr, struct RdosThreadActionState *State);
int RDOSAPI RdosGetFaultThreadTss(int ThreadNr, struct RdosTss *tss);

int RDOSAPI RdosHasCrashInfo();
int RDOSAPI RdosGetCrashCoreInfo(int Core, char *CrashBuf);

void RDOSAPI RdosSetThreadAction(const char *ActionStr);

int RDOSAPI RdosGetThreadCount();
int RDOSAPI RdosGetThreadState(int ThreadNr, struct RdosThreadState *State);
int RDOSAPI RdosGetThreadActionState(int ThreadNr, struct RdosThreadActionState *State);
int RDOSAPI RdosSuspendThread(int Thread);
int RDOSAPI RdosSuspendAndSignalThread(int Thread);
void RDOSAPI RdosMoveToCore(int Core);
void RDOSAPI RdosMoveThreadToCore(int Core, int ThreadNr);
void RDOSAPI RdosMoveToNewCore();

int RDOSAPI RdosGetModuleCount();
int RDOSAPI RdosGetModuleInfo(int Index, int *ID, char *NameBuf, int NameSize);
int RDOSAPI RdosGetModuleSel(int ID);
long long RDOSAPI RdosGetModuleBase(int ID);
long long RDOSAPI RdosGetModuleSize(int ID);

int RDOSAPI RdosGetProcessCount();
int RDOSAPI RdosGetProcessInfo(int Index, int *ID, char *NameBuf, int NameSize);
int RDOSAPI RdosIsProcessRunning(int pid);
int RDOSAPI RdosGetProcessThreads(int pid, unsigned short int *IdArr, int MaxThreads);

int RDOSAPI RdosGetProgramCount();
int RDOSAPI RdosGetProgramInfo(int Index, int *ID, char *NameBuf, int NameSize);
int RDOSAPI RdosGetProgramModules(int Index, unsigned short int *IdArr, int MaxModules);
int RDOSAPI RdosGetProgramProcesses(int Index, unsigned short int *IdArr, int MaxProcesses);
int RDOSAPI RdosGetProcessModules(int Index, unsigned short int *IdArr, int MaxModules);
int RDOSAPI RdosGetProcessModuleUsage(int Index, int ID);

int RDOSAPI RdosGetImageHeader(int Adapter, int Entry, TRdosObjectHeader *Header);
int RDOSAPI RdosGetImageData(int Adapter, int Entry, void *Buf);
int RDOSAPI RdosGetDeviceInfo(int CodeSel, char *Name, unsigned int *CodeSize, unsigned short int *DataSel, unsigned int *DataSize);

int RDOSAPI RdosGetSelectorInfo(int CodeSel, int *Limit, int *Bitness);

int RDOSAPI RdosHasPowerCard();
int RDOSAPI RdosHasHardReset();
void RDOSAPI RdosSoftReset();
void RDOSAPI RdosHardReset();
int RDOSAPI RdosPowerFailure();
int RDOSAPI RdosIsEmergencyStopped();
int RDOSAPI RdosGetCpuVersion(char *VendorStr, int *FeatureFlags, int *freq);
void RDOSAPI RdosGetVersion(int *Major, int *Minor, int *Release);
void RDOSAPI RdosCreateThread(void (*Start)(void *Param), const char *Name, void *Param, int StackSize);
void RDOSAPI RdosCreatePrioThread(void (*Start)(void *Param), int Prio, const char *Name, void *Param, int StackSize);
void RDOSAPI RdosTerminateThread();
int RDOSAPI RdosGetThreadHandle(void);
int RDOSAPI RdosGetProcessHandle(void);

int RDOSAPI RdosStartAppTimer(void (*Start)(void *Param), void *Param, int Ms);
int RDOSAPI RdosStopAppTimer(int index);
int RDOSAPI RdosRestartCurrentAppTimer(int Ms);
int RDOSAPI RdosResetAppTimer(int index, int Ms);

int RDOSAPI RdosHasGlobalTimer();
int RDOSAPI RdosGetActiveCores();
int RDOSAPI RdosGetCoreLoad(int Core, long long *NullTics, long long *CoreTics);
int RDOSAPI RdosGetCoreDuty(int Core, long long *CoreTics, long long *TotalTics);

int RDOSAPI RdosIs64BitExe(const char *prog);

#ifdef __RDOS__     // these are only available in user-mode

int RDOSAPI RdosExec(const char *prog, const char *param, const char *startdir, const char *env);
int RDOSAPI RdosSpawn(const char *prog, const char *param, const char *startdir, const char *env, int *thread);
int RDOSAPI RdosSpawnDebug(const char *prog, const char *param, const char *startdir, const char *env, int *thread);
int RDOSAPI RdosAttachDebugger(int pid);
int RDOSAPI RdosFork(void);
void RDOSAPI RdosFatalErrorExit(void);
void RDOSAPI RdosUnloadExe(int ExitCode);
int RDOSAPI RdosGetExitCode(void);
int RDOSAPI RdosGetProcessExitCode(int handle);
void RDOSAPI RdosAddWaitForProcessEnd(int Handle, int ProcessHandle, void *ID);
int RDOSAPI RdosShowExceptionText();

#endif

void RDOSAPI RdosWaitMilli(int ms);
void RDOSAPI RdosWaitMicro(int us);
void RDOSAPI RdosWaitUntil(unsigned long msb, unsigned long lsb);
void RDOSAPI RdosWaitRealUntil(unsigned long msb, unsigned long lsb);

void RDOSAPI RdosGetSysTime(unsigned long *msb, unsigned long *lsb);
long long RDOSAPI RdosGetLongSysTime(void);
void RDOSAPI RdosGetTime(unsigned long *msb, unsigned long *lsb);
long long RDOSAPI RdosGetLongTime(void);

void RDOSAPI RdosUserGetSysTime(unsigned long *msb, unsigned long *lsb);
long long RDOSAPI RdosUserGetLongSysTime(void);
void RDOSAPI RdosUserGetTime(unsigned long *msb, unsigned long *lsb);
long long RDOSAPI RdosUserGetLongTime(void);

void RDOSAPI RdosSetTime(unsigned long msb, unsigned long lsb);
int RDOSAPI RdosDayOfWeek(int year, int month, int day);

void RDOSAPI RdosDosTimeDateToTics(unsigned short date, unsigned short time, unsigned long *msb, unsigned long *lsb);
void RDOSAPI RdosTicsToDosTimeDate(unsigned long msb, unsigned long lsb, unsigned short *date, unsigned short *time);

void RDOSAPI RdosDecodeTicsBase(unsigned long msb, unsigned long lsb);
void RDOSAPI RdosDecodeMsbTics(unsigned long msb, int *year, int *month, int *day, int *hour);
void RDOSAPI RdosDecodeLsbTics(unsigned long lsb, int *min, int *sec, int *milli, int *micro);

unsigned long RDOSAPI RdosCodeMsbTics(int year, int month, int day, int hour);
unsigned long RDOSAPI RdosCodeLsbTics(int min, int sec, int milli, int micro);

void RDOSAPI RdosAddTics(unsigned long *msb, unsigned long *lsb, long tics);
void RDOSAPI RdosAddMicro(unsigned long *msb, unsigned long *lsb, long us);
void RDOSAPI RdosAddMilli(unsigned long *msb, unsigned long *lsb, long ms);
void RDOSAPI RdosAddSec(unsigned long *msb, unsigned long *lsb, long sec);
void RDOSAPI RdosAddMin(unsigned long *msb, unsigned long *lsb, long min);
void RDOSAPI RdosAddHour(unsigned long *msb, unsigned long *lsb, long hour);
void RDOSAPI RdosAddDay(unsigned long *msb, unsigned long *lsb, long day);
int RDOSAPI RdosSyncTime(long IP);

void RDOSAPI RdosInitFutex(struct RdosFutex *f, const char *n);
void RDOSAPI RdosEnterFutex(const struct RdosFutex *f);
void RDOSAPI RdosLeaveFutex(const struct RdosFutex *f);
void RDOSAPI RdosResetFutex(struct RdosFutex *f);

int RDOSAPI RdosCreateWait(void);
void RDOSAPI RdosCloseWait(int Handle);
int RDOSAPI RdosCheckWait(int Handle);
int RDOSAPI RdosWaitForever(int Handle);
int RDOSAPI RdosWaitTimeout(int Handle, int MillSec);
int RDOSAPI RdosWaitUntilTimeout(int Handle, unsigned long msb, unsigned long lsb);
void RDOSAPI RdosStopWait(int Handle);
void RDOSAPI RdosRemoveWait(int Handle, int ID);
void RDOSAPI RdosAddWaitForKeyboard(int Handle, int ID);
void RDOSAPI RdosAddWaitForMouse(int Handle, int ID);
void RDOSAPI RdosAddWaitForCom(int Handle, int ComHandle, int ID);
void RDOSAPI RdosAddWaitForSysLog(int Handle, int SyslogHandle, int ID);

int RDOSAPI RdosCreateSignal(void);
void RDOSAPI RdosResetSignal(int Handle);
int RDOSAPI RdosIsSignalled(int Handle);
void RDOSAPI RdosSetSignal(int Handle);
void RDOSAPI RdosFreeSignal(int Handle);
void RDOSAPI RdosAddWaitForSignal(int Handle, int SignalHandle, int ID);

int RDOSAPI RdosCreateThreadBlock(const char *Name);
void RDOSAPI RdosWaitThreadBlock(int Handle);
void RDOSAPI RdosCloseThreadBlock(int Handle);

int RDOSAPI RdosGetNetHwId(int Index);
long RDOSAPI RdosGetIp();
long RDOSAPI RdosGetGateway();
int RDOSAPI RdosIpToMac(int Ip, char *mac);
long RDOSAPI RdosGetDhcpEntry(int Index, unsigned long *MsbTime, unsigned long *LsbTime);
int RDOSAPI RdosNameToIp(const char *HostName);
int RDOSAPI RdosIpToName(int Ip, char *HostName, int MaxSize);

void RDOSAPI RdosSendUdp(long RemoteIp, int LocalPort, int RemotePort, const char *Buf, int Size);
void RDOSAPI RdosBroadcastUdp(int LocalPort, int RemotePort, const char *Buf, int Size);
int RDOSAPI RdosBroadcastQueryUdp(const char *ReqBuf, int ReqSize, char *ReplyBuf, int *ReplySize, int DestPort, long Timeout);

int RDOSAPI RdosOpenUdpConnection(int RemoteIp, int LocalPort, int RemotePort);
void RDOSAPI RdosCloseUdpConnection(int Handle);
void RDOSAPI RdosSendUdpConnection(int Handle, const char *Buf, int Size);
int RDOSAPI RdosPeekUdpConnection(int Handle);
int RDOSAPI RdosReadUdpConnection(int Handle, void *Buf, int Size);
void RDOSAPI RdosAddWaitForUdpConnection(int Handle, int ConHandle, int ID);

int RDOSAPI RdosCreateTcpListen(int Port, int MaxConnections, int BufferSize);
int RDOSAPI RdosGetTcpListen(int Handle);
void RDOSAPI RdosCloseTcpListen(int Handle);
void RDOSAPI RdosAddWaitForTcpListen(int Handle, int ConHandle, int ID);

int RDOSAPI RdosCreateSecureListen(int Port, int MaxConnections, int BufferSize);
int RDOSAPI RdosGetSecureListen(int Handle);
void RDOSAPI RdosCloseSecureListen(int Handle);
void RDOSAPI RdosAddWaitForSecureListen(int Handle, int ConHandle, int ID);
void RDOSAPI RdosSetSecureCertificate(int Handle, const char *CertFileName, const char *PrivateKeyFileName, const char *ChainFileName);

int RDOSAPI RdosCreateUdpListen(int Port, int MaxBufferedMessages);
int RDOSAPI RdosGetUdpListenSize(int Handle);
long RDOSAPI RdosGetUdpListenIp(int Handle);
int RDOSAPI RdosGetUdpListenPort(int Handle);
int RDOSAPI RdosGetUdpListenData(int Handle, char *Buf, int Size);
int RDOSAPI RdosClearUdpListen(int Handle);
void RDOSAPI RdosCloseUdpListen(int Handle);
void RDOSAPI RdosAddWaitForUdpListen(int Handle, int ConHandle, int ID);

int RDOSAPI RdosCreateTcpSocket(void);
int RDOSAPI RdosCreateUdpSocket(void);
int RDOSAPI RdosIsIpv4Socket(int handle);
int RDOSAPI RdosConnectIpv4Socket(int handle, long ip, short int port);
int RDOSAPI RdosBindIpv4Socket(int handle, short int port);
int RDOSAPI RdosListenSocket(int handle, int maxconn);
int RDOSAPI RdosAcceptIpv4Socket(int handle, long *ip, short int *port);

int RDOSAPI RdosOpenTcpConnection(int RemoteIp, int LocalPort, int RemotePort, int Timeout, int BufferSize);
int RDOSAPI RdosWaitForTcpConnection(int Handle, long Timeout);
void RDOSAPI RdosAddWaitForTcpConnection(int Handle, int ConHandle, int ID);
void RDOSAPI RdosCloseTcpConnection(int Handle);
void RDOSAPI RdosDeleteTcpConnection(int Handle);
void RDOSAPI RdosAbortTcpConnection(int Handle);
void RDOSAPI RdosPushTcpConnection(int Handle);
int RDOSAPI RdosIsTcpConnectionClosed(int Handle);
int RDOSAPI RdosIsTcpConnectionIdle(int Handle);
long RDOSAPI RdosGetRemoteTcpConnectionIP(int Handle);
int RDOSAPI RdosGetRemoteTcpConnectionPort(int Handle);
int RDOSAPI RdosGetLocalTcpConnectionPort(int Handle);
int RDOSAPI RdosReadTcpConnection(int Handle, void *Buf, int Size);
int RDOSAPI RdosWriteTcpConnection(int Handle, const void *Buf, int Size);
int RDOSAPI RdosPollTcpConnection(int Handle);
int RDOSAPI RdosGetTcpConnectionWriteSpace(int Handle);
void RDOSAPI RdosWaitForTcpConnectionWriteSpace(int Handle);

int RDOSAPI RdosCreateSecureSession();
int RDOSAPI RdosCloseSecureSession(int Handle);

int RDOSAPI RdosCreateSecureConnection(int Session, int RemoteIp, int LocalPort, int RemotePort, int Timeout, int BufferSize);
void RDOSAPI RdosCloseSecureConnection(int Handle);
int RDOSAPI RdosGetSecureTcpHandle(int Handle);
int RDOSAPI RdosWaitForSecureConnection(int Handle, long Timeout);

void RDOSAPI RdosAddWaitForSecureConnection(int Handle, int ConHandle, int ID);
int RDOSAPI RdosIsSecureConnectionClosed(int Handle);
void RDOSAPI RdosPushSecureConnection(int Handle);
int RDOSAPI RdosIsSecuConnectionClosed(int Handle);
int RDOSAPI RdosIsSecureConnectionIdle(int Handle);
long RDOSAPI RdosGetRemoteSecureConnectionIP(int Handle);
int RDOSAPI RdosGetRemoteSecureConnectionPort(int Handle);
int RDOSAPI RdosGetLocalSecureConnectionPort(int Handle);
int RDOSAPI RdosReadSecureConnection(int Handle, void *Buf, int Size);
int RDOSAPI RdosWriteSecureConnection(int Handle, const void *Buf, int Size);
int RDOSAPI RdosPollSecureConnection(int Handle);
int RDOSAPI RdosGetSecureConnectionCertificate(int Handle, void *Buf, int MaxSize);
int RDOSAPI RdosGetCertificateJson(const char *FileName, void *Buf, int MaxSize);

int RDOSAPI RdosGetSecureConnectionWriteSpace(int Handle);
int RDOSAPI RdosGetLocalMailslot(const char *Name);
int RDOSAPI RdosGetRemoteMailslot(long Ip, const char *Name);
void RDOSAPI RdosFreeMailslot(int Handle);
int RDOSAPI RdosSendMailslot(int Handle, const void *Msg, int Size, void *ReplyBuf, int MaxReplySize);

void RDOSAPI RdosDefineMailslot(const char *Name, int MaxSize);
int RDOSAPI RdosReceiveMailslot(void *Msg);
void RDOSAPI RdosReplyMailslot(const void *Msg, int Size);

void RDOSAPI RdosSetFocus(char FocusKey);

int RDOSAPI RdosSetKeyMap(const char *MapName);
void RDOSAPI RdosGetKeyMap(char *MapName);
void RDOSAPI RdosClearKeyboard(void);
int RDOSAPI RdosPollKeyboard(void);
int RDOSAPI RdosReadKeyboard(void);
int RDOSAPI RdosGetKeyboardState(void);
void RDOSAPI RdosPutKeyboard(int KeyCode, int VirtualKey, int ScanCode);
int RDOSAPI RdosPeekKeyEvent(int *ExtKey, int *KeyState, int *VirtualKey, int *ScanCode);
int RDOSAPI RdosReadKeyEvent(int *ExtKey, int *KeyState, int *VirtualKey, int *ScanCode);

void RDOSAPI RdosHideMouse(void);
void RDOSAPI RdosShowMouse(void);
void RDOSAPI RdosGetMousePosition(int *x, int *y);
void RDOSAPI RdosSetMousePosition(int x, int y);
void RDOSAPI RdosSetMouseWindow(int StartX, int StartY, int EndX, int EndY);
void RDOSAPI RdosSetMouseMickey(int x, int y);
int RDOSAPI RdosGetLeftButton(void);
int RDOSAPI RdosGetRightButton(void);
void RDOSAPI RdosGetLeftButtonPressPosition(int *x, int *y);
void RDOSAPI RdosGetRightButtonPressPosition(int *x, int *y);
void RDOSAPI RdosGetLeftButtonReleasePosition(int *x, int *y);
void RDOSAPI RdosGetRightButtonReleasePosition(int *x, int *y);

void RDOSAPI RdosClearText();
void RDOSAPI RdosGetTextSize(int *Rows, int *Cols);
void RDOSAPI RdosGetCursorPosition(int *Row, int *Col);
void RDOSAPI RdosSetCursorPosition(int Row, int Col);
void RDOSAPI RdosGetConsoleCursorPosition(int *Row, int *Col);
void RDOSAPI RdosSetConsoleCursorPosition(int Row, int Col);
void RDOSAPI RdosWriteChar(char ch);
void RDOSAPI RdosWriteSizeString(const char *Buf, int Size);
void RDOSAPI RdosWriteAttributeString(int Row, int Col, const short int *Buf, int Size);
void RDOSAPI RdosWriteString(const char *Buf);

int RDOSAPI RdosPing(long Node, long Timeout);

int RDOSAPI RdosGetIdeDisc(int UnitNr);
int RDOSAPI RdosGetFloppyDisc(int UnitNr);
void RDOSAPI RdosResetDisc(int UnitNr);

int RDOSAPI RdosGetFileCacheSize();
void RDOSAPI RdosSyncDiscPart(int DiscNr);
int RDOSAPI RdosSetDiscInfo(int DiscNr, int SectorSize, long Sectors, int BiosSectorsPerCyl, int BiosHeads);
int RDOSAPI RdosGetDiscInfo(int DiscNr, int *SectorSize, long long *Sectors, int *BiosSectorsPerCyl, int *BiosHeads);
void RDOSAPI RdosGetDiscVendorInfo(int DiscNr, char *Buf, int Size);

int RDOSAPI RdosIsVfsDisc(int DiscNr);
int RDOSAPI RdosGetDiscCacheSize(int DiscNr);
long long RDOSAPI RdosGetDiscCache(int DiscNr);
long long RDOSAPI RdosGetDiscLocked(int DiscNr);
int RDOSAPI RdosReadDisc(int DiscNr, long long Sector, char *Buf, int Size);
int RDOSAPI RdosWriteDisc(int DiscNr, long long Sector, const char *Buf, int Size);
int RDOSAPI RdosIsDiscIdle(int DiscNr);

void RDOSAPI RdosGetRdfsInfo(void *CryptTab, void *KeyTab, void *ExtentSizeTab);
void RDOSAPI RdosDemandLoadDrive(int DriveNr);
int RDOSAPI RdosFormatDrive(int DiscNr, long StartSector, int Size, const char *FsName);
void RDOSAPI RdosRemoveDrive(int DriveNr);

int RDOSAPI RdosAllocateFixedDrive(int DriveNr);

int RDOSAPI RdosGetDriveInfo(int DriveNr, long *FreeUnits, int *BytesPerUnit, long *TotalUnits);
int RDOSAPI RdosGetDriveDiscParam(int DriveNr, int *DiscNr, long *StartSector, long *TotalSectors);

int RDOSAPI RdosGetVfsDriveDisc(int DriveNr);
long long RDOSAPI RdosGetVfsDriveStart(int DriveNr);
long long RDOSAPI RdosGetVfsDriveSize(int DriveNr);
long long RDOSAPI RdosGetVfsDriveFree(int DriveNr);
int RDOSAPI RdosIsVfsPath(const char *PathName);

int RDOSAPI RdosOpenDir(const char *PathName, struct RdosDirInfo *Info);
void RDOSAPI RdosCloseDir(int Handle);

int RDOSAPI RdosCreateCrc(unsigned short int CrcPoly);
void RDOSAPI RdosCloseCrc(int Handle);
unsigned short int RDOSAPI RdosCalcCrc(int Handle, unsigned short int CrcVal, const char *Buf, int Size);
unsigned int RDOSAPI RdosCalcCrc32(unsigned int CrcVal, const char *Buf, int Size);

#ifdef __RDOS__     // these are only available in user-mode

int RDOSAPI RdosGetCurrentDllHandle();
int RDOSAPI RdosGetModuleHandle(void);
const char *RDOSAPI RdosGetExeName(void);
const char *RDOSAPI RdosGetCmdLine(void);
int RDOSAPI RdosLoadDll(const char *Name);
void RDOSAPI RdosFreeDll(int handle);
long RDOSAPI RdosDuplModuleFileHandle(long handle);
int RDOSAPI RdosGetModuleName(int handle, char *Buf, int Size);
int RDOSAPI RdosReadResource(int handle, int ID, char *Buf, int Size);
int RDOSAPI RdosReadBinaryResource(int handle, int ID, char *Buf, int Size);

void * RDOSAPI RdosGetModuleProc(int handle, const char *ProcName);
char RDOSAPI RdosGetModuleFocusKey(int handle);

void RDOSAPI RdosAddWaitForDebugEvent(int Handle, int ProcessHandle, int ID);
char RDOSAPI RdosGetDebugEvent(int handle, int *thread);
void RDOSAPI RdosGetDebugEventData(int handle, void *buf);
void RDOSAPI RdosClearDebugEvent(int handle);
void RDOSAPI RdosContinueDebugEvent(int handle, int thread);
void RDOSAPI RdosAbortDebug(int Handle);

#endif

void RDOSAPI RdosRemoteDebug(long IP);
void RDOSAPI RdosRemoteGui(long IP);

int RDOSAPI RdosOpenSyslog();
void RDOSAPI RdosCloseSyslog(int handle);
int RDOSAPI RdosGetSyslog(int handle, int *severity, unsigned long *msb, unsigned long *lsb, char *buf, int size);

int RDOSAPI RdosConfigAdcChannel(int device, int Channel, int Freq, int Periods);
void RDOSAPI RdosSetAdcAmpLimit(int device, int MinAmp);
void RDOSAPI RdosSetAdcPhaseLimit(int device, int PhaseDiff);
void RDOSAPI RdosSetAdcChannels(int device, int MinChannels);
void RDOSAPI RdosStartAdcCFreq(int device);
void *RDOSAPI RdosReadAdcFreq(int device);

void RDOSAPI RdosSetupAdc(char TestMode, char Speed, int BufCount);
int RDOSAPI RdosStartAdc();
void RDOSAPI RdosStopAdc();
int RDOSAPI RdosMapAdcBlock(int Index, void *Buf);
void RDOSAPI RdosSetAdcTrigger(int PhaseIncr, char WindowBits);

int RDOSAPI RdosReadSerialLines(int device, int *val);
int RDOSAPI RdosToggleSerialLine(int device, int line);
int RDOSAPI RdosSetSerialLine(int device, int line);
int RDOSAPI RdosResetSerialLine(int device, int line);
int RDOSAPI RdosReadSerialVal(int device, int line, int *val);
int RDOSAPI RdosWriteSerialVal(int device, int line, int val);
int RDOSAPI RdosReadSerialRaw(int device, int line, int *val);
int RDOSAPI RdosWriteSerialRaw(int device, int line, int val);

int RDOSAPI RdosOpenSysEnv(void);
int RDOSAPI RdosOpenProcessEnv(void);
void RDOSAPI RdosCloseEnv(int handle);
void RDOSAPI RdosAddEnvVar(int handle, const char *var, const char *value);
void RDOSAPI RdosDeleteEnvVar(int handle, const char *var);
int RDOSAPI RdosFindEnvVar(int handle, const char *var, char *value);
int RDOSAPI RdosGetEnvSize(int handle);
void RDOSAPI RdosGetEnvData(int handle, char *buf);
void RDOSAPI RdosSetEnvData(int handle, const char *buf);

int RDOSAPI RdosOpenSysIni();
int RDOSAPI RdosOpenIni(const char *filename);
void RDOSAPI RdosCloseIni(int handle);
int RDOSAPI RdosDupIni(int handle);
int RDOSAPI RdosGotoIniSection(int handle, const char *name);
int RDOSAPI RdosRemoveIniSection(int handle, const char *name);
int RDOSAPI RdosReadIni(int handle, const char *var, char *str, int maxsize);
int RDOSAPI RdosWriteIni(int handle, const char *var, const char *str);
int RDOSAPI RdosDeleteIni(int handle, const char *var);
int RDOSAPI RdosGotoFirstIniVar(int handle);
int RDOSAPI RdosGotoNextIniVar(int handle);
int RDOSAPI RdosGetCurrIniVar(int handle, char *var, int maxsize);

void RDOSAPI RdosEnableStatusLED();
void RDOSAPI RdosDisableStatusLED();

void RDOSAPI RdosStartWatchdog(int timeout);
void RDOSAPI RdosKickWatchdog();
void RDOSAPI RdosStopWatchdog();

void RDOSAPI RdosStartDebugger(int timeout);
void RDOSAPI RdosKickDebugger();
void RDOSAPI RdosStopDebugger();

void RDOSAPI RdosStartNetCapture(long FileHandle);
void RDOSAPI RdosStopNetCapture();

void RDOSAPI RdosStartCanCapture(long FileHandle);
void RDOSAPI RdosStopCanCapture();

void RDOSAPI RdosStartLonCapture(long FileHandle);
void RDOSAPI RdosStopLonCapture();

int RDOSAPI RdosGetUsbConfig(int Controller, int Device, int Config, void *ptr, int maxsize);
int RDOSAPI RdosHasUsbCardReaderError();
int RDOSAPI RdosHasUsbCardDevReset();
int RDOSAPI RdosHasUsbCardUsbReset();
int RDOSAPI RdosGetUsbDevice(int Controller, int Port, char *Buffer, int Size);
char RDOSAPI RdosGetUsbAddress(int Controller, int Port);
int RDOSAPI RdosOpenUsbDevice(int Controller, int Port);
void RDOSAPI RdosCloseUsbDevice(int handle);
int RDOSAPI RdosSendUsbDeviceControlMsg(int handle, char Type, char Req, int Value, int Index, char *Buf, int Size);
int RDOSAPI RdosIsUsbDeviceConnected(int handle);
void RDOSAPI RdosResetUsbDevice(int handle);
int RDOSAPI RdosOpenUsbPacketPipe(int handle, char pipe, int PacketCount);
int RDOSAPI RdosGetUsbPacketPipe(int handle, char pipe, char *Buf);
void RDOSAPI RdosCloseUsbPipe(int handle, char pipe);
int RDOSAPI RdosGetUsedUsbBuffers(int handle, char pipe);
int RDOSAPI RdosGetFreeUsbBuffers(int handle, char pipe);
int RDOSAPI RdosGetUsbBufferSize(int handle, char pipe);
void RDOSAPI RdosAddWaitForUsbPipe(int whandle, int devhandle, char pipe, int ID);
int RDOSAPI RdosOpenUsbEvent(int QueueSize);
void RDOSAPI RdosCloseUsbEvent(int handle);
void RDOSAPI RdosAddWaitForUsbEvent(int whandle, int evhandle, int ID);
int RDOSAPI RdosGetUsbEvent(int handle, struct RdosUsbEvent *event);

int RDOSAPI RdosGetAllocatedUsbBlocks();
int RDOSAPI RdosGetUsbCloseCount();

int RDOSAPI RdosHasICSP();
int RDOSAPI RdosOpenICSP(int DeviceID);
void RDOSAPI RdosCloseICSP(int Handle);
void RDOSAPI RdosResetICSP(int Handle);
int RDOSAPI RdosWriteICSPCommand(int Handle, int Cmd);
int RDOSAPI RdosWriteICSPData(int Handle, int Data);
int RDOSAPI RdosReadICSPData(int Handle, int *Data);

void RDOSAPI RdosSetCodecGpio0(int Value);

void RDOSAPI RdosGetOutputVolume(int *Left, int *Right);
void RDOSAPI RdosSetOutputVolume(int Left, int Right);

void RDOSAPI RdosGetMasterVolume(int *Left, int *Right);
void RDOSAPI RdosSetMasterVolume(int Left, int Right);

void RDOSAPI RdosGetLineOutVolume(int *Left, int *Right);
void RDOSAPI RdosSetLineOutVolume(int Left, int Right);

int RDOSAPI RdosHasAudio();

int RDOSAPI RdosCreateAudioOutChannel(int SampleRate, int Bits, int Volume);
void RDOSAPI RdosCloseAudioOutChannel(int Handle);
void RDOSAPI RdosWriteAudio(int Handle, int Size, const int *LSamples, const int*RSamples);

int RDOSAPI RdosOpenFm(int SampleRate);
void RDOSAPI RdosCloseFm(int FmHandle);
void RDOSAPI RdosFmWait(int FmHandle, int Samples);

int RDOSAPI RdosCreateFmInstrument(int FmHandle, int C, int M, long double Beta);
void RDOSAPI RdosFreeFmInstrument(int Handle);
void RDOSAPI RdosSetFmAttack(int Handle, int Samples);
void RDOSAPI RdosSetFmSustain(int Handle, int VolumeHalf, int BetaHalf);
void RDOSAPI RdosSetFmRelease(int Handle, int VolumeHalf, int BetaHalf);
void RDOSAPI RdosPlayFmNote(int Handle, long double Freq, int PeakLeftVolume, int PeakRightVolume, int SustainSamples);

int RDOSAPI RdosHasTouch();
void RDOSAPI RdosResetTouchCalibrate();
void RDOSAPI RdosSetTouchCalibrateDividend(int div);
void RDOSAPI RdosSetTouchCalibrateX(int xx, int xy, int xoffset);
void RDOSAPI RdosSetTouchCalibrateY(int yx, int yy, int yoffset);

int RDOSAPI RdosCreateBigNum();
void RDOSAPI RdosDeleteBigNum(int handle);
void RDOSAPI RdosLoadSignedBigNum(int handle, const char *buf, int size);
void RDOSAPI RdosLoadUnsignedBigNum(int handle, const char *buf, int size);
void RDOSAPI RdosSaveSignedBigNum(int handle, char *buf, int size);
void RDOSAPI RdosSaveUnsignedBigNum(int handle, char *buf, int size);

void RDOSAPI RdosLoadDecStrBigNum(int handle, const char *str);
int RDOSAPI RdosGetDecStrSizeBigNum(int handle);
void RDOSAPI RdosSaveDecStrBigNum(int handle, char *buf, int bufsize);

void RDOSAPI RdosLoadHexStrBigNum(int handle, const char *str);
int RDOSAPI RdosGetHexStrSizeBigNum(int handle);
void RDOSAPI RdosSaveHexStrBigNum(int handle, char *buf, int bufsize);

void RDOSAPI RdosAddSignedBigNum(int handle, const char *buf, int size);
void RDOSAPI RdosAddUnsignedBigNum(int handle, const char *buf, int size);
void RDOSAPI RdosSubSignedBigNum(int handle, const char *buf, int size);
void RDOSAPI RdosSubUnsignedBigNum(int handle, const char *buf, int size);

int RDOSAPI RdosAddBigNum(int handle1, int handle2);
int RDOSAPI RdosSubBigNum(int handle1, int handle2);
int RDOSAPI RdosMulBigNum(int handle1, int handle2);
int RDOSAPI RdosDivBigNum(int handle1, int handle2);
int RDOSAPI RdosModBigNum(int handle1, int handle2);
int RDOSAPI RdosPowModBigNum(int base, int exp, int mod);
int RDOSAPI RdosCreateRandomBigNum(int bits);
int RDOSAPI RdosCreateRandomOddBigNum(int bits);
int RDOSAPI RdosFactorPow2BigNum(int handle, int *exponent);

void RDOSAPI RdosWaitAcMeassure();
int RDOSAPI RdosGetAcVoltage(char Phase);
int RDOSAPI RdosGetAcCurrent(char Phase);
int RDOSAPI RdosGetAcConsumePower(char Phase);
int RDOSAPI RdosGetAcProducePower(char Phase);
long long RDOSAPI RdosGetAcConsumeEnergy(char Phase);
long long RDOSAPI RdosGetAcProduceEnergy(char Phase);

int RDOSAPI RdosCreateRealtime();
int RDOSAPI RdosAddRealtimeCore(int handle, const char *exename);
void RDOSAPI RdosWaitForRealtimeSignal(int handle);
int RDOSAPI RdosGetRealtimeSignal(int handle, int *core, int *signal);

int RDOSAPI RdosAllocateRealtimeBuffer(int handle, long long size);
char *RDOSAPI RdosMapRealtimeBuffer(int handle, long long offset, int size);
void RDOSAPI RdosUnmapRealtimeBuffer(int handle);

#ifdef __cplusplus
}
#endif

#endif

#ifdef __GNUC__
#include "rdosgcc.h"
#endif

#ifdef __WATCOMC__

#ifdef __FLAT__
#include "owflat.h"
#endif

#ifdef __COMPACT__
#include "owcomp.h"
#endif

#endif

#endif
