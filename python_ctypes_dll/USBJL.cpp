// USBJL.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "USBJL.h"


// -----------------------

#include <windows.h>
#include <iostream.h>

//HANDLE hMutex;

//------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CUSBJLApp

BEGIN_MESSAGE_MAP(CUSBJLApp, CWinApp)
	//{{AFX_MSG_MAP(CUSBJLApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBJLApp construction

CUSBJLApp::CUSBJLApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUSBJLApp object

CUSBJLApp theApp;

volatile int  JL_Lock = 0;

// -------------------- usb -------------------------

#define  set_addr(addr) cdb[2] = ((unsigned char *)&(addr))[3];\
    cdb[3] = ((unsigned char *)&(addr))[2];\
    cdb[4] = ((unsigned char *)&(addr))[1];\
    cdb[5] = ((unsigned char *)&(addr))[0];

// ��һ��32λ�����зֵ� CDB
#define  set_wValue(aValue) cdb[2] = ((unsigned char *)&(aValue))[3];\
    cdb[3] = ((unsigned char *)&(aValue))[2];\
    cdb[4] = ((unsigned char *)&(aValue))[1];\
    cdb[5] = ((unsigned char *)&(aValue))[0];


#define  set_len(len)  cdb[6] = HIBYTE(len);\
    cdb[7] = LOBYTE(len);

// ��һ��16λ�����зֵ� CDB[6][7]
#define  set_BigDataLen(len)  cdb[6] = HIBYTE(len);\
    cdb[7] = LOBYTE(len);

// ��ȡ����ͷ�����ṹ

struct camera_param_st
{
	u8 ev_mode;
	u8 white_balance;
	u8 md_level;
	u8 nr_level;
	u8 sharpness_level;

	u8 gamma[256];         // ٤��
	u8 ev;
	u16 saturation;
	s16 ccm3x4[12];        // ��ɫ����

	s16 lsc_x;             // ��ͷ��Ӱ
	s16 lsc_y;
	u32 lsc_r[1024];
	u32 lsc_g[1024];
	u32 lsc_b[1024];
};

// ��ʾ���ܵĴ���

DWORD DisplayError()
{
	DWORD rError = GetLastError();
	CString sError;

	sError.Format("�豸����������ţ�%d", rError);
	if (rError = 0x55)  sError.Format("�豸�����ڣ����飡");
	if (rError = 0x79)  sError.Format("�豸�򿪳�ʱ�������´��豸��");
	//AfxMessageBox(sError);   
	return 0;
}


// USB����

DWORD GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
{
    STORAGE_PROPERTY_QUERY	Query;
    DWORD dwOutBytes;
    DWORD status;
    Query.PropertyId = StorageDeviceProperty;
    Query.QueryType = PropertyStandardQuery;

//	CString sDis;  /// -- 
//	sDis.Format("���ţ�%d", IOCTL_STORAGE_QUERY_PROPERTY);
//	AfxMessageBox(sDis);  


    status = DeviceIoControl(hDevice,
                             IOCTL_STORAGE_QUERY_PROPERTY,
                             &Query, sizeof(STORAGE_PROPERTY_QUERY),
                             pDevDesc, pDevDesc->Size,
                             &dwOutBytes,
                             (LPOVERLAPPED)NULL);

    if(status == 0)
    {
        DisplayError();
        return status;
    }

    return 0;
}


// ������

BOOL ISP_ReadFromScsi(HANDLE fileHandle, int cdbLen, void *cdb, int dataLen, BYTE *data)
{
    SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER sptdwb;
	BOOL	status;
	ULONG	length = 0,  returned = 0;

    sptdwb.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    sptdwb.sptd.PathId = 0;
    sptdwb.sptd.TargetId = 1;
    sptdwb.sptd.Lun = 0;
    sptdwb.sptd.CdbLength = cdbLen;		// CDB����ĳ���
    sptdwb.sptd.SenseInfoLength = 26;	// 24;
    sptdwb.sptd.DataIn = SCSI_IOCTL_DATA_IN;	// ������
    sptdwb.sptd.DataTransferLength = dataLen;   //sectorSize;	// ��ȡ���ݵĳ���
	sptdwb.sptd.TimeOutValue = 2;
    sptdwb.sptd.DataBuffer = data;	    	    // ��ȡ�����ݵĴ��ָ��
    sptdwb.sptd.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, ucSenseBuf);
	memcpy(sptdwb.sptd.Cdb, cdb, cdbLen);

    length = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
    status = DeviceIoControl(fileHandle,
                             IOCTL_SCSI_PASS_THROUGH_DIRECT,
                             &sptdwb,  length,
                             &sptdwb,  length,
                             &returned,
                             FALSE);

	if( status==0 ) DisplayError();
	return status;
}


// д����  ------------------------

BOOL ISP_WriteToScsi(HANDLE fileHandle, int cdbLen, void *cdb, int dataLen, BYTE *data)
{
	SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER sptdwb;
	BOOL	status;
	ULONG	length = 0, returned = 0;

    sptdwb.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    sptdwb.sptd.PathId = 0;
    sptdwb.sptd.TargetId = 1;
    sptdwb.sptd.Lun = 0;
    sptdwb.sptd.CdbLength = cdbLen;
    sptdwb.sptd.SenseInfoLength = 26;	        // 24;
    sptdwb.sptd.DataIn = SCSI_IOCTL_DATA_OUT;	// д����
    sptdwb.sptd.DataTransferLength = dataLen;   // sectorSize;
    sptdwb.sptd.TimeOutValue = 10;              // 100
    sptdwb.sptd.DataBuffer = data;
    sptdwb.sptd.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER,ucSenseBuf);
	memcpy(sptdwb.sptd.Cdb, cdb, cdbLen);

    length = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
    status = DeviceIoControl(fileHandle,
                             IOCTL_SCSI_PASS_THROUGH_DIRECT,
                             &sptdwb,  length,
                             &sptdwb,  length,
                             &returned,           
							 FALSE);
	
	if(status==0) DisplayError();
	return status;
}

extern "C" __declspec(dllexport) DWORD USB_WriteToData(HANDLE devHandle, BYTE Command1, BYTE Command2, int wValue, unsigned short bigDataLen, BYTE *bigData)
{
	BOOL ret;

//    if ( JL_Lock = 1 ) return 2;
//    WaitForSingleObject(hMutex, INFINITE);//ȷ�����߳��˳��Ž���main

	memset(cdb,0,16);
//	cdb[0] = ADJUST_CAMERA;  // һ������
    cdb[0] = Command1;       // һ������
	cdb[1] = Command2;       // ��������

	// wValue --> cdb[2][3][4][5]
	set_wValue(wValue);          // С����
	// bigDataLen --> cdb[6][7]
	set_BigDataLen(bigDataLen);  // ���ܵĴ����ݳ���

	ret = ISP_WriteToScsi(devHandle, 16, cdb, bigDataLen, bigData);
//	JL_Lock = 0;
//	ReleaseMutex(hMutex);
	return ret;
}

extern "C" __declspec(dllexport) DWORD USB_WriteTo14Byte(HANDLE devHandle, BYTE Command1, BYTE Command2, unsigned short bigDataLen, BYTE *bigData)
{
	BOOL ret;
	BYTE tmp[512];
//	memcpy(tmp,bigData,bigDataLen);
//    WaitForSingleObject(hMutex, INFINITE);//ȷ�����߳��˳��Ž���main

	memset(cdb,0,16);
    cdb[0] = Command1;       // һ������
	cdb[1] = Command2;       // ��������

    memcpy(&cdb[2], bigData, 6);

	ret = ISP_WriteToScsi(devHandle, 16, cdb, 0, tmp);
//	ReleaseMutex(hMutex);
	return ret;
}


// ������
// bigData �Ƕ�ȡ����ָ�룬������ͷ����ˣ��� camera_param_st �ṹ��֯
// �� UI ��Ҳ�� camera_param_st �ṹ���ͼ���
// camera_param_st  --> BYTE *bigData --> camera_param_st
//    ��֯                  ����              ��ȡ
// ע����뷽ʽ��sizeofĬ����4�ֽڶ����

extern "C" __declspec(dllexport) DWORD USB_ReadFromData(HANDLE devHandle, BYTE Command1, BYTE Command2, int dataLen, BYTE *bigData)
{
	BOOL ret;
	BYTE tmp[512];
    //if ( JL_Lock = 1 ) return 2;
//    WaitForSingleObject(hMutex, INFINITE);//ȷ�����߳��˳��Ž���main
   

	memset(cdb,0,16);
    cdb[0] = Command1;       // һ������ 0x02 -- 0x07
	cdb[1] = Command2;       // ��������

	cdb[3] = bigData[0];     // Sensor ��ȡ�ĵ�ַ
	cdb[4] = bigData[1];     // Sensor ��ȡ�ĵ�ַ 2015.10.14

    if (!devHandle) return 0;
	
	ret = ISP_ReadFromScsi(devHandle, 16, cdb, dataLen, tmp);
	if(ret){
		memcpy(bigData,tmp,dataLen);
	}

    //JL_Lock = 0;
//	ReleaseMutex(hMutex);

	return ret;
}


extern "C" __declspec(dllexport) DWORD USB_CloseHandle(HANDLE handle)
{
//	CloseHandle(hMutex);

    if (!handle)
	   CloseHandle(handle);
	return 0;
}

// �� USB ��ȡ�豸�� ndevName ƥ��
// drv == [ 'd' -- 'z' ] 
// ndevName == DV12 DEVICE V1.00

extern "C" __declspec(dllexport) HANDLE USB_GetDeviceHandle(char drv, char *ndevName)
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    CString csDeviceName, devName;
    CString csPhysical;
    CString str;
    PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;
   
	/*
    hMutex=CreateMutex(NULL,TRUE, "JL_USB"); //TRUE ��ʾ��ǰ�߳�ӵ�иû�����������Ҫ���ͷ� ticketsΪ����������
    if (hMutex)
    {
        if (ERROR_ALREADY_EXISTS==GetLastError())
        {
            cout<<"only one instance can run!"<<endl;
            return 0;
        }
    }
	ReleaseMutex(hMutex);
*/

    devName.Format("%s", ndevName);	
    csDeviceName.Format("\\\\.\\%c:", drv);


    devName.Format("%s", csDeviceName);	
	AfxMessageBox(csDeviceName);  ///--



    handle = CreateFile(csDeviceName,
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		NULL,                /*FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING*/
        NULL);

    if((handle == INVALID_HANDLE_VALUE) || (handle == NULL))
    {
        return INVALID_HANDLE_VALUE;
    }

//USB_DEVICE_DESCRIPTOR

    //PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;
    pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];
    pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;

    if(GetDisksProperty(handle, pDevDesc) == 0)
    {
        if(pDevDesc->BusType == BusTypeUsb)  // �豸�� USB
        {
            CString str;
            char *p = (char *)pDevDesc;
            str = p + pDevDesc->VendorIdOffset;
            str += &p[pDevDesc->VendorIdOffset + 9];

			AfxMessageBox(str);  ///--

/*
	CString sError;
	sError.Format("Ver=%d Size=%d Type=%d Ven=%d  pID=%d", 
		                           pDevDesc->Version, 
		                           pDevDesc->Size,
								   pDevDesc->DeviceType,
                                   pDevDesc->VendorIdOffset,
								   pDevDesc->ProductIdOffset
								   );
	AfxMessageBox(sError);   
*/

            devName.MakeLower();
            devName.Replace(" ", ""); //ȥ���ո�

            str.MakeLower();
            str.Replace(" ", "");     //ȥ���ո�

            if (strcmp(str, devName) == 0)  // USB�豸������豸ƥ��
			{
                delete pDevDesc;
                return handle;              // ������Ч��USB�豸���
			} 
        }
	} 
    CloseHandle(handle);
    delete pDevDesc;
    return NULL;
}

extern "C" __declspec(dllexport) int USB_GetDeviceName(char drv, char *rName) 
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    CString csDeviceName, devName;
    CString csPhysical;
    CString str;
    PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;  
  
 
    csDeviceName.Format("\\\\.\\%c:", drv);  

    handle = CreateFile(csDeviceName,
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		NULL,                /*FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING*/
        NULL);

    if((handle == INVALID_HANDLE_VALUE) || (handle == NULL))
    {
//        AfxMessageBox("���豸ʧ�ܣ�"); 
		return -1;
    }

    pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];
    pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;

    if(GetDisksProperty(handle, pDevDesc) == 0)
    {

        if(pDevDesc->BusType == BusTypeUsb)  // �豸�� USB
        {
			CString str;
            char *p = (char *)pDevDesc;
            str = p + pDevDesc->VendorIdOffset;
            str += &p[pDevDesc->VendorIdOffset + 9];

            devName.MakeLower();
            devName.Replace(" ", ""); //ȥ���ո�

            str.MakeLower();
            str.Replace(" ", "");     //ȥ���ո�
			 
            
			memcpy(rName, str.GetBuffer(0), str.GetLength() + 1); 

			// AfxMessageBox(str);
//			AfxMessageBox(rName);


            CloseHandle(handle);
            delete pDevDesc;

            return 0;
        }
	} 
//	AfxMessageBox("��ȡ�豸��ʧ�ܣ�"); 
    CloseHandle(handle);
    delete pDevDesc;
    return -1;
}


// ------------ ZFY ---------------

// �� USB ��ȡ�豸�� ndevName ƥ��
// drv == [ 'd' -- 'z' ] 
// devName == dv16
// pass = 

HANDLE isDV16_USB(char drv)
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    CString csDeviceName, devName;        
    PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;

	devName = "DV16 DEVICE V1.00";
	//devName = "DV16";

	//for( char drv = 'd'; drv <= 'z'; drv++ ) { 

		csDeviceName.Format("\\\\.\\%c:", drv);

		//AfxMessageBox(csDeviceName);  

		handle = CreateFile(csDeviceName, 
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			NULL,          
			NULL);

        //if(handle == INVALID_HANDLE_VALUE){
        //    continue;
        //}
		if(handle == NULL)   {
			//return INVALID_HANDLE_VALUE;
			return NULL;
		}
		
		pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];
		pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;

		if(GetDisksProperty(handle, pDevDesc) == 0)    {
			if(pDevDesc->BusType == BusTypeUsb) { // �豸�� USB
                CString str;
				char *p = (char *)pDevDesc;
				str = p + pDevDesc->VendorIdOffset;
				str += &p[pDevDesc->VendorIdOffset + 9];

				//AfxMessageBox(str); 

				devName.MakeLower();
				devName.Replace(" ", ""); //ȥ���ո�

				str.MakeLower();
				str.Replace(" ", "");     //ȥ���ո�

				//AfxMessageBox(str + "--" + devName); 
		
				if (strcmp(str, devName) == 0) {  // USB�豸������豸ƥ��
			        delete pDevDesc;					
					return handle;              // ������Ч��USB�豸���
				} 
			}
		} 
	CloseHandle(handle);
    delete pDevDesc;
    return NULL;
	}
char getDv16_MaxPF()  
{  
    TCHAR drv = 0, sdrv;  
    TCHAR cDiskSymbol [] = _T( "C:");  
	CString s, s1, s2, sError;
	s = "";
	sdrv = _T('C');  
	int n, nMax = 0;
	HANDLE h = NULL;
    for(drv =_T('C'); drv <= _T( 'Z'); drv ++)  
    {  
		 //if ( isDV16_USB(drv) == NULL ) continue;
		 h =  isDV16_USB(drv);
		 if(h == NULL ){
			continue;
		 } else {
			CloseHandle(h);
		 }
         cDiskSymbol[0] = drv;  
         TCHAR szBuf [MAX_PATH] = { 0 };  
         QueryDosDevice(cDiskSymbol, szBuf, MAX_PATH);  
		 n = strlen("\\Device\\HarddiskVolume");
		 s1 = szBuf + n;
		 n = atoi(s1);
		 if ( n > nMax ) {
			 nMax = n;
			 sdrv = drv;
		 }
	     sError.Format("cDiskSymbol = %s  szBuf == %s", cDiskSymbol, szBuf);
		 s = s + " --- " + sError;
     }  
     //AfxMessageBox(s);   
	 //s.Format("%d", nMax);
	 //AfxMessageBox(s);   
     return sdrv;  
}  
// �� USB ��ȡ�豸�� ndevName ƥ��
// drv == [ 'd' -- 'z' ] 
// devName == dv16
// pass = 
HANDLE USB_GetZFY_Handle()
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    CString csDeviceName, devName;        
    PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;
	devName = "DV16 DEVICE V1.00";
	char drv = getDv16_MaxPF();
		csDeviceName.Format("\\\\.\\%c:", drv);
		handle = CreateFile(csDeviceName, 
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			NULL,          
			NULL);
		if(handle == NULL)   {
			return NULL;
		}
		pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];
		pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;
		if(GetDisksProperty(handle, pDevDesc) == 0)    {
			if(pDevDesc->BusType == BusTypeUsb) { // �豸�� USB
                CString str;
				char *p = (char *)pDevDesc;
				str = p + pDevDesc->VendorIdOffset;
				str += &p[pDevDesc->VendorIdOffset + 9];
				devName.MakeLower();
				devName.Replace(" ", ""); //ȥ���ո�
				str.MakeLower();
				str.Replace(" ", "");     //ȥ���ո�
				if (strcmp(str, devName) == 0) {  // USB�豸������豸ƥ��
			        delete pDevDesc;					
					return handle;              // ������Ч��USB�豸���
				} 
			}
		} 
	CloseHandle(handle);
    delete pDevDesc;
    return NULL;
}


DWORD zfyDisplayError(int iRet, char *sRet)
{	
	CString sError;
	sError.Format("iRet == %d  sRet == %s", iRet, sRet);
	AfxMessageBox(sError);   
	return 0;
}
//  

// ��ȡ����Ա����
bool GetJL_Password(HANDLE h, char *sPwd, u16 *iRet)
{
	bool ret = false;	


    BYTE tmp[512], getPwd[16];

	memset(cdb,0,16);
    int dataLen = 16;
    cdb[0] = ISD_TOOL_READ;
	cdb[1] = USER_CMD_READ;  
    cdb[5] = GET_JL_PASSWORD;
    cdb[7] = dataLen;

	*iRet = CHECK_PWD_FAILED;
	int r = ISP_ReadFromScsi(h, 16, cdb, dataLen, tmp);
	if ( r ) {
		memcpy(getPwd,tmp,dataLen);	
		if ( strcmp((char*)getPwd, sPwd) == 0 ) {
		   *iRet = CHECK_PWD_SUCCESS;	
		   ret = true;
	}
	}	

	return ret;
}

int get_cdb_return_status(HANDLE h)
{
	BYTE tmp[512];

	memset(cdb,0,16);
    cdb[0] = ISD_TOOL_READ;
	cdb[1] = USER_CMD_READ;  
    cdb[5] = GET_RETURN_STATUS;
	cdb[7] = 4;
	int r = ISP_ReadFromScsi(h, 16, cdb, 4, tmp);
	if( r ) {		
        return *tmp;
	}

	return 0;
	
}
// 1����ʼ���B��
extern "C" _declspec(dllexport) BOOL Init_Device(char *IDCode, u16 *iRet)
{
	bool ret = true;	    

	HANDLE h = USB_GetZFY_Handle();
	if ( h == NULL ) { 
		*iRet = CONNECT_FAILED;		
		return false;
	}

//		CString htmp;
 // 	    htmp.Format("h == %d", h);
//		AfxMessageBox(htmp);

    BYTE tmp[512];

	memset(cdb,0,16);
    cdb[0] = ISD_TOOL_WRITE;
	cdb[1] = USER_CMD_WRITE;  
    cdb[5] = INIT_USB_DEVICE;


	int r = ISP_WriteToScsi(h, 16, cdb, 0, tmp);

	//CString tmp;
	//tmp.Format("%s", IDCode);	
	//tmp.Format("output device code =^= %d ABC JIELI", 99);
	//memcpy(rs, tmp,  tmp.GetLength() + 1);

	//AfxMessageBox(rs);

	if ( r ) {
		*iRet = CONNECT_SUCCESS;
	} else {
		*iRet = CONNECT_FAILED;
		ret = false;
	}

	
	if ( *iRet == CONNECT_SUCCESS ) {
		// ��ȡIDCODE 6λ
		memset(cdb,0,16);
		int dataLen = 6; //6λ��Ч
		cdb[0] = ISD_TOOL_READ;
		cdb[1] = USER_CMD_READ;  
		cdb[5] = GET_ID_CODE;
		cdb[7] = dataLen;

		*iRet = 102;
		r = ISP_ReadFromScsi(h, 16, cdb, dataLen, tmp);
		if( r ) {
			if(!get_cdb_return_status(h)){
				memcpy(IDCode,tmp,dataLen);//		
				*iRet = CONNECT_SUCCESS;		
				//zfyDisplayError(0, IDCode);
			}
		}	
	}
	
	if ( h ) CloseHandle(h);
	return ret;
}


// Command = 0x02

// 2��ݶȡ���������뼰��Ʒ�ͺŴ���
extern "C" _declspec(dllexport) BOOL GetIDCode(char *IDCode, u16 *iRet)
{
	bool ret = true;

	HANDLE h = USB_GetZFY_Handle();
	if ( h == NULL ) { 
		*iRet = CONNECT_FAILED;
		return false;
	}

//	CString htmp;
//	htmp.Format("h == %d", h);
//	AfxMessageBox(htmp);

    BYTE tmp[512];

	memset(cdb,0,16);
	int dataLen = 6;   //6λ��Ч
    cdb[0] = ISD_TOOL_READ;
	cdb[1] = USER_CMD_READ;  
    cdb[5] = GET_ID_CODE;
    cdb[7] = dataLen;

	*iRet = 0;
	int r = ISP_ReadFromScsi(h, 16, cdb, dataLen, tmp);
	if( r ) {
		if(!get_cdb_return_status(h)){
			memcpy(IDCode,tmp,dataLen);		
			*iRet = CONNECT_SUCCESS;		
		}
		//CString stmp;
  	    //stmp.Format("Len == %d %d %d %d %d %s", dataLen, tmp[0], tmp[1], tmp[2], tmp[3], tmp);
	    //AfxMessageBox(stmp);
	}	

	if ( h ) CloseHandle(h);
	return ret;
}


// command = 0x03

// 3����ȡ��¼����Ϣ
extern   "C"  _declspec(dllexport) BOOL GetZFYInfo(ZFY_INFO *info, char *sPwd, u16 *iRet)
{
	bool ret = true;
	
 
	/*
    AfxMessageBox(sPwd);	
	strcpy(info->cSerial, "JL.com");
	strcpy(info->userNo, "81990");
	strcpy(info->userName, "JL.com ZFY");
	strcpy(info->unitName, "JL.com ISP");
	strcpy(info->unitNo, "JL2300");	
    */
    HANDLE h = USB_GetZFY_Handle();
	if ( h == NULL ) { 
		*iRet = CONNECT_FAILED;
		return false;
	}

	if ( !GetJL_Password(h, sPwd, iRet) ) {
		return false;
	}
    BYTE tmp[512];

	memset(cdb,0,16);
	int dataLen = sizeof(ZFY_INFO);
    cdb[0] = ISD_TOOL_READ;
	cdb[1] = USER_CMD_READ;  
    cdb[5] = GET_DEV_INFO;
    cdb[7] = dataLen;
    	
    //AfxMessageBox("IDCode");

	*iRet = 0;
// 	stmp.Format("Len == %d", dataLen);
//	AfxMessageBox(stmp);

	int r = ISP_ReadFromScsi(h, 16, cdb, dataLen, tmp);
	if( r ) {		
		if(!get_cdb_return_status(h)){
			memcpy(info,tmp,dataLen);		
  			//stmp.Format("Len == %d %s", dataLen, tmp);
			*iRet = CONNECT_SUCCESS;
		}
	}	

	if ( h ) CloseHandle(h);

	return ret;
}

// command2 = 0x01

// 4��д���¼����Ϣ
extern   "C"  _declspec(dllexport) BOOL WriteZFYInfo(ZFY_INFO *info, char *sPwd, u16 *iRet)
{
	bool ret = true;

    /*CString stmp;
	stmp.Format("%s \n%s \n%s \n%s \n%s", info->cSerial, info->userNo, info->userName, info->unitNo, info->unitName);
	AfxMessageBox(stmp);*/


    HANDLE h = USB_GetZFY_Handle();
	if ( h == NULL ) { 
		*iRet = CONNECT_FAILED;
		return false;
	}

	if ( !GetJL_Password(h, sPwd, iRet) ) {
		return false;
	}

    BYTE tmp[512]; 

	memset(cdb,0,16);
	int dataLen = sizeof(ZFY_INFO);
    cdb[0] = ISD_TOOL_WRITE;
	cdb[1] = USER_CMD_WRITE;  
    cdb[5] = WRITE_DEV_INFO;
    cdb[7] = dataLen;
    	

//	CString stmp;
// 	stmp.Format("Len == %d", dataLen);
//	AfxMessageBox(stmp);

    memcpy(tmp, info, dataLen);

	*iRet = 0;
	int r = ISP_WriteToScsi(h, 16, cdb, dataLen, tmp);
	if ( r ) {
		if(!get_cdb_return_status(h)){
			*iRet = CONNECT_SUCCESS;
		}
	}

	if ( h ) CloseHandle(h);
	return ret;
}

// 5��ͬ��ִ����¼��ʱ��
extern   "C"  _declspec(dllexport) BOOL SyncDevTime(char *sPwd, u16 *iRet)
{
	bool ret = true;
    struct sys_time{
        u16 year;
        u8 month;
        u8 day;
        u8 hour;
        u8 min;
        u8 sec;
    };

    HANDLE h = USB_GetZFY_Handle();
	if ( h == NULL ) { 
		*iRet = CONNECT_FAILED;
		return false;
	}

	if ( !GetJL_Password(h, sPwd, iRet) ) {
		return false;
	}	

    BYTE tmp[512];

	memset(cdb,0,16);
	int dataLen = sizeof(struct sys_time);
    cdb[0] = ISD_TOOL_WRITE;
	cdb[1] = USER_CMD_WRITE;  
    cdb[5] = SYNC_DEV_TIME;
    cdb[7] = dataLen;
    	

    //��ȡϵͳʱ��

    struct sys_time sys_time;
    time_t rawtime;
    struct tm *t;     
    time(&rawtime);     
    t = localtime (&rawtime);
    sys_time.year = t->tm_year + 1900;
    sys_time.month = t->tm_mon + 1;
    sys_time.day = t->tm_mday;
    sys_time.hour = t->tm_hour;
    sys_time.min = t->tm_min;
    sys_time.sec = t->tm_sec;

    //CString stmp;
   	//stmp.Format("%d-%d-%d %d:%d:%d", sys_time.year,sys_time.month,sys_time.day,sys_time.hour,sys_time.min,sys_time.sec);
	//AfxMessageBox(stmp);

    memcpy(tmp, &sys_time, dataLen);

	*iRet = SET_SYSTEM_FAILED;
	int r = ISP_WriteToScsi(h, 16, cdb, dataLen, tmp);
	if ( r ) {
		if(!get_cdb_return_status(h)){
			*iRet = SET_SYSTEMTIME_SUCCESS;
		}
	}

	if ( h ) CloseHandle(h);

	return ret;
}

// 6������Ϊ�ƶ�����ģʽ
extern   "C"  _declspec(dllexport) BOOL SetMSDC(char *sPwd, u16 *iRet)
{
	bool ret = true;

    HANDLE h = USB_GetZFY_Handle();
	if ( h == NULL ) { 
		*iRet = CONNECT_FAILED;
		return false;
	}

	if ( !GetJL_Password(h, sPwd, iRet) ) {
		return false;
	}

    BYTE tmp[512];

	memset(cdb,0,16);
    cdb[0] = ISD_TOOL_READ;
	cdb[1] = USER_CMD_READ;  
    cdb[5] = GET_SD_STATUS;
    cdb[7] = 4;
	*iRet = 101;
	int r = ISP_ReadFromScsi(h, 16, cdb, 4, tmp);
	if( r ) {		
		//*iRet = CONNECT_SUCCESS;
        //CString stmp;
        //if(*tmp == 0){
        //    stmp.Format("sd card offline\n");
        //    AfxMessageBox(stmp);
        if(*tmp == 0){
			*iRet = MSDC_FAILED;
            return false;
        }
	}	

	memset(cdb,0,16);
    cdb[0] = ISD_TOOL_WRITE;
	cdb[1] = USER_CMD_WRITE;  
    cdb[5] = SET_MSDC;
    	
	*iRet = MSDC_SUCCESS;
	r = ISP_WriteToScsi(h, 16, cdb, 0, tmp);
	if( r ) {	
		
		*iRet = MSDC_SUCCESS;
	}

	if ( h ) CloseHandle(h);

	return ret;
}

// 7����ȡ��ǰ¼��ֱ���
extern   "C"  _declspec(dllexport) BOOL ReadDeviceResolution(int *Width, int *Height, char *sPwd, u16 *iRet)
{
	bool ret = true;

    struct Res_t{
        int width;
        int height;  
    };

    struct Res_t res;
	/*
	int w = 1920;
	int h = 1080;	

	*Width = w;
	*Height = h;

	CString tmp;
	tmp.Format("PWD == %s \nWidth = %d \nHeight = %d", sPwd, *Width, *Height);
	AfxMessageBox(tmp);
	*/

	HANDLE h = USB_GetZFY_Handle();
	if ( h == NULL ) { 
		*iRet = CONNECT_FAILED;
		return false;
	}

	if ( !GetJL_Password(h, sPwd, iRet) ) {
		return false;
	}
    BYTE tmp[512];

	memset(cdb,0,16);
	int dataLen = sizeof(struct Res_t);
    cdb[0] = ISD_TOOL_READ; 
	cdb[1] = USER_CMD_READ;    
	cdb[5] = READ_DEV_RES; 
	cdb[7] = dataLen;
    

	*iRet = 0;
	int r = ISP_ReadFromScsi(h, 16, cdb, dataLen, tmp);
	if( r ) {		
		memcpy(&res,tmp,dataLen);		
		*iRet = CONNECT_SUCCESS;

        *Width = res.width;
        *Height = res.height;
		//CString stmp;
  	    //stmp.Format("width == %d  height == %d \n", *Width,*Height);
	    //AfxMessageBox(stmp);
	}	

	if ( h ) CloseHandle(h);

	return ret;
}

/*
procedure TAV.JL_ReadCammer_YD;
var
  Command2: Byte;
  yd, r: Integer;
begin
  if devHandle = nil then Exit;
  Command2 := 1; // ?����????��?��2a���䨬?
  yd := 1;
  r := USB_ReadFromData(devHandle, JT_R_Cmd1, Command2, SizeOf(Integer), @yd);
  if r = 0 then
  begin
    RSE.Caption := ('?����?��???����2?��y3?�䨪��?');
    TimerMD.Enabled := False;
    Exit;
  end;
  RSMD.Caption := '?�̨���?2?1';
  if yd = 1 then RSMD.Caption := '?�̨���???��';
end;
*/

// 8����ȡ����
extern   "C"  _declspec(dllexport) BOOL ReadDeviceBatteryDumpEnergy(int *Battery, char *sPwd, u16 *iRet)
{
	bool ret = true;

	/*
	int b = 88;	
	*Battery = b;
	CString tmp;
	tmp.Format("PWD == %s \nBattery = %d", sPwd, *Battery);
	AfxMessageBox(tmp);
	*/

	HANDLE h = USB_GetZFY_Handle();
	if ( h == NULL ) { 
		*iRet = CONNECT_FAILED;
		return false;
	}
	
	if ( !GetJL_Password(h, sPwd, iRet) ) {
		return false;
	}
    BYTE tmp[512];

	memset(cdb,0,16);
	int dataLen = 4; // sizeof(int);	
    cdb[0] = ISD_TOOL_READ; 
	cdb[1] = USER_CMD_READ;    
	cdb[5] = READ_BATTERY_VAL; 
	cdb[7] = dataLen;

	*iRet = 0;
	int r = ISP_ReadFromScsi(h, 16, cdb, dataLen, tmp);
	if( r ) {	
		if(!get_cdb_return_status(h)){
			memcpy(Battery,tmp,dataLen);		
			*iRet = CONNECT_SUCCESS;
		}
		// zfyDisplayError(*Battery, "");		
	}	

  
	if ( h ) CloseHandle(h);

	return ret;
}

// 9�����ù���Ա����
extern   "C"  _declspec(dllexport) BOOL SetPassword(char *sPwd, u16 *iRet)
{
    bool ret = true;

    HANDLE h = USB_GetZFY_Handle();
	if ( h == NULL ) { 
		*iRet = CONNECT_FAILED;
		return false;
	}


    BYTE tmp[512];

	memset(cdb,0,16);
    int dataLen = 16;
    cdb[0] = ISD_TOOL_WRITE;
	cdb[1] = USER_CMD_WRITE;  
    cdb[5] = SET_PASSWORD;
    cdb[7] = dataLen;
    	
    memcpy(tmp,sPwd,dataLen);

	*iRet = 0;
	int r = ISP_WriteToScsi(h, 16, cdb, dataLen, tmp);
	if ( r ) {	
		*iRet = CONNECT_SUCCESS;		
	}

	if ( h ) CloseHandle(h);

	return ret;
}

// 10�������豸ʶ����
extern   "C"  _declspec(dllexport) BOOL SetIDCode(char *IDCode, u16 *iRet)
{
	bool ret = true;

    HANDLE h = USB_GetZFY_Handle();
	if ( h == NULL ) { 
		*iRet = CONNECT_FAILED;
		return false;
	}

    BYTE tmp[512];

	memset(cdb,0,16);
    int dataLen = 32;
    cdb[0] = ISD_TOOL_WRITE;
	cdb[1] = USER_CMD_WRITE;  
    cdb[5] = SET_ID_CODE;
    cdb[7] = dataLen;
    	
    memcpy(tmp,IDCode,dataLen);

	*iRet = 0;
	int r = ISP_WriteToScsi(h, 16, cdb, dataLen, tmp);
	if ( r ) {
		*iRet = CONNECT_SUCCESS;		
	}

	if ( h ) CloseHandle(h);

	return ret;
}


