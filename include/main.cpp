#include "graph.h"
#include <windows.h>
#include <conio.h>
#include <process.h>
#include "Psapi.h"
#include <TlHelp32.h>
#include <tchar.h>
#include <assert.h>


typedef long long int64_t;
typedef unsigned long long uint64_t;

static uint64_t file_time_2_utc(const FILETIME* ftime){
    LARGE_INTEGER li;
    assert(ftime);
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

static int get_processor_number(){
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return (int)info.dwNumberOfProcessors;
}

int get_cpu_usage(){
    static int processor_count = -1;
    static int64_t last_time = 0;
    static int64_t last_system_time = 0;

    FILETIME now;
    FILETIME creation_time;
    FILETIME exit_time;
    FILETIME kernel_time;
    FILETIME user_time;
    int64_t system_time;
    int64_t time;
    int64_t system_time_delta;
    int64_t time_delta;

    int cpu = -1;

    if(processor_count == -1){
        processor_count = get_processor_number();
    }

    GetSystemTimeAsFileTime(&now);
    if(!GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time, &kernel_time, &user_time)){
        return -1;
    }
    system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time)) / processor_count;
    time = file_time_2_utc(&now);

    if((last_system_time == 0) || (last_time == 0)){
        last_system_time = system_time;
        last_time = time;
        return -1;
    }
    system_time_delta = system_time - last_system_time;
    time_delta = time - last_time;

    assert(time_delta != 0);
    if(time_delta == 0){
        return -1;
    }
    cpu = (int)((system_time_delta * 100 + time_delta / 2) / time_delta);
    last_system_time = system_time;
    last_time = time;
    return cpu;
}



//#define MY_PROCESS_ERROR(Condition) do{if(!(Condition)) goto Exit0;} while(false)

//#include <QApplication>

//static DWORD g_sdwTickCountOld = 0;
//static LARGE_INTEGER g_slgProcessTimeOld;
//static DWORD g_sdwProcessorCoreNum = 0;
//static HANDLE g_shExitEvent = NULL;

//typedef struct _TARGET_PROCESS{
//    DWORD dwProcessId;
//} _TARGET_PROCESS, *PTARGET_PROCESS;

//HRESULT FindFirstProcessIdByName(const TCHAR* cpszExeFileName, DWORD &dwPID){
//    HRESULT hr = E_FAIL;
//    PROCESSENTRY32 pe = { 0 };
//    HANDLE hSnapshot = NULL;
//    if(NULL == cpszExeFileName){
//        hr = HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS);
//        goto Exit0;
//    }
//    pe.dwSize = sizeof(PROCESSENTRY32);
//    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//    if(FALSE == Process32First(hSnapshot, &pe)){
//        hr = HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES);
//        goto Exit0;
//    }
//    hr = S_FALSE;
//    do{
//        if(_tcsicmp(cpszExeFileName, pe.szExeFile) == 0){
//            dwPID = pe.th32ProcessID;
//            hr = S_OK;
//            break;
//        }
//    }while(Process32Next(hSnapshot, &pe));

//    Exit0:
//        if(hSnapshot){
//            CloseHandle(hSnapshot);
//            hSnapshot = NULL;
//        }
//    return hr;
//}

//int GetProcessCpuPercent(const HANDLE hProcess, const DWORD dwElepsedTime){
//    int nProcCpuPercent = 0;
//    BOOL bRetCode = FALSE;

//    FILETIME CreateTime, ExitTime, KernelTime, UserTime;
//    LARGE_INTEGER lgKernelTime;
//    LARGE_INTEGER lgUserTime;
//    LARGE_INTEGER lgCurTime;

//    bRetCode = GetProcessTimes(hProcess, &CreateTime, &ExitTime, &KernelTime, &UserTime);

//    if(bRetCode){
//        lgKernelTime.HighPart = KernelTime.dwHighDateTime;
//        lgKernelTime.LowPart = KernelTime.dwLowDateTime;

//        lgUserTime.HighPart = UserTime.dwHighDateTime;
//        lgUserTime.LowPart = UserTime.dwLowDateTime;

//        lgCurTime.QuadPart = (lgKernelTime.QuadPart + lgUserTime.QuadPart) / 10000;
//        nProcCpuPercent = (int)((lgCurTime.QuadPart - g_slgProcessTimeOld.QuadPart) * 100 / dwElepsedTime);
//        g_slgProcessTimeOld = lgCurTime;
//        nProcCpuPercent = nProcCpuPercent / g_sdwProcessorCoreNum;
//    }
//    else {nProcCpuPercent = -1;}

//    return nProcCpuPercent;
//}

//unsigned _stdcall WorkerThread(void *pArg){
//    HRESULT hr = E_FAIL;
//    HANDLE hProcess = NULL;
//    DWORD dwProcessId = 0;
//    DWORD dwRetVal = 0;
//    DWORD dwCurrentTickCount = 0;
//    int nProcessCpuPercent = 0;

//    TARGET_PROCESS *pTar
//}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Graph w;
    w.show();
    return a.exec();
}
