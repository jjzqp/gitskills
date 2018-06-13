// USBJL.h : main header file for the USBJL DLL
//

#if !defined(AFX_USBJL_H__A8A4D5B9_EFC9_47EC_90AF_155A7ECF84E6__INCLUDED_)
#define AFX_USBJL_H__A8A4D5B9_EFC9_47EC_90AF_155A7ECF84E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "stdio.h" 

/////////////////////////////////////////////////////////////////////////////
// CUSBJLApp
// See USBJL.cpp for the implementation of this class
//

class CUSBJLApp : public CWinApp
{
public:
	CUSBJLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSBJLApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CUSBJLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USBJL_H__A8A4D5B9_EFC9_47EC_90AF_155A7ECF84E6__INCLUDED_)


// -----------  // -----------------------------------------------------------------------


/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    spti.h

Abstract:

    These are the structures and defines that are used in the
    SPTI.C.

Author:

Revision History:

--*/

#pragma pack(1)
typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS
{
    SCSI_PASS_THROUGH spt;
    ULONG             Filler;      // realign buffers to double word boundary
    UCHAR             ucSenseBuf[32];
    UCHAR             ucDataBuf[512];
} SCSI_PASS_THROUGH_WITH_BUFFERS, *PSCSI_PASS_THROUGH_WITH_BUFFERS;

typedef struct _SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER
{
    SCSI_PASS_THROUGH_DIRECT sptd;
    ULONG             Filler;      // realign buffer to double word boundary
    UCHAR             ucSenseBuf[32];
} SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, *PSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER;


typedef struct ZFY_INFO
{
	char cSerial[7];	// ��Ʒ��ţ�����Ϊ��
	char userNo[6];		// ʹ���߾��ţ�����Ϊ��
	char userName[32];	// ʹ��������
	char unitNo[12];	// ʹ���ߵ�λ���
	char unitName[32];	// ʹ���ߵ�λ����
} ZFY_INFO;



BYTE cdb[16];


//VOID PrintError(ULONG);
//VOID PrintDataBuffer(PUCHAR, ULONG);
//VOID PrintInquiryData(PVOID);
//PUCHAR AllocateAlignedBuffer(ULONG, ULONG);
//VOID PrintStatusResults(BOOL, DWORD, PSCSI_PASS_THROUGH_WITH_BUFFERS, ULONG);
//VOID PrintSenseInfo(PSCSI_PASS_THROUGH_WITH_BUFFERS);

//
// Command Descriptor Block constants.
//

#define CDB6GENERIC_LENGTH   6
#define CDB10GENERIC_LENGTH  10

#define SETBITON       1
#define SETBITOFF      0

// һ������
#define READ_CAMERA	   0xF9
#define ADJUST_CAMERA  0xFA
#define WRITE_FLASH    0xfB
#define READ_FLASH     0xfD
#define OTHER_CMD      0xfC


// ��������
enum
{
    CAMERA_PARAM_READ,
	SET_EV_MODE = 1,    // ���
	SET_WHITE_BALANCE,  // ��ƽ��
	SET_MD_LEVEL,       // �˶�
	SET_NR_LEVEL,       // ����
	SET_SHPN_LEVEL,     // ��
    SET_GAMMA,          // ٤��
    SET_EV,             // �ع�
    SET_SATURATION,     // ���Ͷ�

	SET_CCM3x4,         // ��ɫ����

	SET_LSC_X = 10,     // ��Ӱ
	SET_LSC_Y,
	SET_LSC_R,
	SET_LSC_G,
	SET_LSC_B,

	READ_ID = 0x20
};



#define ISD_TOOL_WRITE  0xFB
#define ISD_TOOL_READ   0xFD 

#define USER_CMD_READ   0x81
#define USER_CMD_WRITE  0x82

enum _RESULT_FLAG
{
	CONNECT_SUCCESS = 0X01,     // ���ӳɹ�
	CONNECT_FAILED,				// ����ʧ��
	CHECK_PWD_SUCCESS,			// ����У��ɹ�
	CHECK_PWD_FAILED,			// ����У��ʧ��
	SET_SYSTEMTIME_SUCCESS,		// ϵͳʱ��ͬ���ɹ�
	SET_SYSTEM_FAILED,			// ϵͳʱ��ͬ��ʧ��
	MSDC_SUCCESS,				// ת���ƶ�����ģʽ�ɹ�
	MSDC_FAILED					// ת���ƶ�����ģʽʧ��
};
enum
{
    INIT_USB_DEVICE = 0x01,  /* ��ʼ������ */
    GET_ID_CODE,
    GET_DEV_INFO,
    WRITE_DEV_INFO,
    SYNC_DEV_TIME,
    SET_MSDC,
    READ_DEV_RES,
    READ_BATTERY_VAL,
    WRITE_ALL_DEV_INFO,
    SET_PASSWORD,
    SET_ID_CODE,
    GET_SD_STATUS,
	GET_JL_PASSWORD,
	GET_RETURN_STATUS,

}USB_CMD;


// CDB
// 0 1 2 3 4 5 6 7 
// 0 --> һ������
// 1 --> ��������        ==1-13  ==32����Ĭ��ֵ��      ==64
// 2 3 4 5 --> С����    
// 6 7 --> �����ݳ���              дSensor��0��1��2��3��        

//
// Mode Sense/Select page constants.
//

#define MODE_PAGE_ERROR_RECOVERY        0x01
#define MODE_PAGE_DISCONNECT            0x02
#define MODE_PAGE_FORMAT_DEVICE         0x03
#define MODE_PAGE_RIGID_GEOMETRY        0x04
#define MODE_PAGE_FLEXIBILE             0x05
#define MODE_PAGE_VERIFY_ERROR          0x07
#define MODE_PAGE_CACHING               0x08
#define MODE_PAGE_PERIPHERAL            0x09
#define MODE_PAGE_CONTROL               0x0A
#define MODE_PAGE_MEDIUM_TYPES          0x0B
#define MODE_PAGE_NOTCH_PARTITION       0x0C
#define MODE_SENSE_RETURN_ALL           0x3f
#define MODE_SENSE_CURRENT_VALUES       0x00
#define MODE_SENSE_CHANGEABLE_VALUES    0x40
#define MODE_SENSE_DEFAULT_VAULES       0x80
#define MODE_SENSE_SAVED_VALUES         0xc0
#define MODE_PAGE_DEVICE_CONFIG         0x10
#define MODE_PAGE_MEDIUM_PARTITION      0x11
#define MODE_PAGE_DATA_COMPRESS         0x0f

//
// SCSI CDB operation codes
//

#define SCSIOP_TEST_UNIT_READY     0x00
#define SCSIOP_REZERO_UNIT         0x01
#define SCSIOP_REWIND              0x01
#define SCSIOP_REQUEST_BLOCK_ADDR  0x02
#define SCSIOP_REQUEST_SENSE       0x03
#define SCSIOP_FORMAT_UNIT         0x04
#define SCSIOP_READ_BLOCK_LIMITS   0x05
#define SCSIOP_REASSIGN_BLOCKS     0x07
#define SCSIOP_READ6               0x08
#define SCSIOP_RECEIVE             0x08
#define SCSIOP_WRITE6              0x0A
#define SCSIOP_PRINT               0x0A
#define SCSIOP_SEND                0x0A
#define SCSIOP_SEEK6               0x0B
#define SCSIOP_TRACK_SELECT        0x0B
#define SCSIOP_SLEW_PRINT          0x0B
#define SCSIOP_SEEK_BLOCK          0x0C
#define SCSIOP_PARTITION           0x0D
#define SCSIOP_READ_REVERSE        0x0F
#define SCSIOP_WRITE_FILEMARKS     0x10
#define SCSIOP_FLUSH_BUFFER        0x10
#define SCSIOP_SPACE               0x11
#define SCSIOP_INQUIRY             0x12
#define SCSIOP_VERIFY6             0x13
#define SCSIOP_RECOVER_BUF_DATA    0x14
#define SCSIOP_MODE_SELECT         0x15
#define SCSIOP_RESERVE_UNIT        0x16
#define SCSIOP_RELEASE_UNIT        0x17
#define SCSIOP_COPY                0x18
#define SCSIOP_ERASE               0x19
#define SCSIOP_MODE_SENSE          0x1A
#define SCSIOP_START_STOP_UNIT     0x1B
#define SCSIOP_STOP_PRINT          0x1B
#define SCSIOP_LOAD_UNLOAD         0x1B
#define SCSIOP_RECEIVE_DIAGNOSTIC  0x1C
#define SCSIOP_SEND_DIAGNOSTIC     0x1D
#define SCSIOP_MEDIUM_REMOVAL      0x1E
#define SCSIOP_READ_CAPACITY       0x25
#define SCSIOP_READ                0x28
#define SCSIOP_WRITE               0x2A
#define SCSIOP_SEEK                0x2B
#define SCSIOP_LOCATE              0x2B
#define SCSIOP_WRITE_VERIFY        0x2E
#define SCSIOP_VERIFY              0x2F
#define SCSIOP_SEARCH_DATA_HIGH    0x30
#define SCSIOP_SEARCH_DATA_EQUAL   0x31
#define SCSIOP_SEARCH_DATA_LOW     0x32
#define SCSIOP_SET_LIMITS          0x33
#define SCSIOP_READ_POSITION       0x34
#define SCSIOP_SYNCHRONIZE_CACHE   0x35
#define SCSIOP_COMPARE             0x39
#define SCSIOP_COPY_COMPARE        0x3A
#define SCSIOP_WRITE_DATA_BUFF     0x3B
#define SCSIOP_READ_DATA_BUFF      0x3C
#define SCSIOP_CHANGE_DEFINITION   0x40
#define SCSIOP_READ_SUB_CHANNEL    0x42
#define SCSIOP_READ_TOC            0x43
#define SCSIOP_READ_HEADER         0x44
#define SCSIOP_PLAY_AUDIO          0x45
#define SCSIOP_PLAY_AUDIO_MSF      0x47
#define SCSIOP_PLAY_TRACK_INDEX    0x48
#define SCSIOP_PLAY_TRACK_RELATIVE 0x49
#define SCSIOP_PAUSE_RESUME        0x4B
#define SCSIOP_LOG_SELECT          0x4C
#define SCSIOP_LOG_SENSE           0x4D


// ---------------- usb 

// д����
extern   "C"   _declspec(dllexport) DWORD USB_WriteToData(HANDLE devHandle, BYTE Command1, BYTE Command2, int wValue, unsigned short bigDataLen, BYTE *bigData);

// ������
extern   "C"   _declspec(dllexport) DWORD USB_ReadFromData(HANDLE devHandle, BYTE Command1, BYTE Command2, int dataLen, BYTE *bigData);

// ��ȡ�豸���
extern   "C"   _declspec(dllexport) HANDLE USB_GetDeviceHandle(char drv, char *ndevName);

// �ر��豸���
extern   "C"   _declspec(dllexport) DWORD USB_CloseHandle(HANDLE handle);



//  -------------------------  ִ����

// ��ʼ���B��
extern   "C"   _declspec(dllexport) BOOL Init_Device(char *IDCode, u16 *iRet);
//extern   "C"   _declspec(dllexport) BOOL Init_Device(int handle);


// ݶȡ���������뼰��Ʒ�ͺŴ���
extern   "C"   _declspec(dllexport) BOOL GetIDCode(char *IDCode, u16 *iRet);

// ��ȡ��¼����Ϣ
extern   "C"  _declspec(dllexport) BOOL GetZFYInfo(ZFY_INFO *info, char *sPwd, u16 *iRet);

// д���¼����Ϣ
extern   "C"  _declspec(dllexport) BOOL WriteZFYInfo(ZFY_INFO *info, char *sPwd, u16 *iRet);

// ͬ��ִ����¼��ʱ��
extern   "C"  _declspec(dllexport) BOOL SyncDevTime(char *sPwd, u16 *iRet);

// ����Ϊ�ƶ�����ģʽ
extern   "C"  _declspec(dllexport) BOOL SetMSDC(char *sPwd, u16 *iRet);

// ��ȡ��ǰ¼��ֱ���
extern   "C"  _declspec(dllexport) BOOL ReadDeviceResolution(int *Width, int *Height, char *sPwd, u16 *iRet);

// ��ȡ����
extern   "C"  _declspec(dllexport) BOOL ReadDeviceBatteryDumpEnergy(int *Battery, char *sPwd, u16 *iRet);

// ���ù���Ա����
extern   "C"  _declspec(dllexport) BOOL SetPassword(char *sPwd, u16 *iRet);

// �����豸ʶ����
extern   "C"  _declspec(dllexport) BOOL SetIDCode(char *IDCode, u16 *iRet);

