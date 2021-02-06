#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <locale.h>
#include <string>
#include <fstream>


std::string date = "";

void setDate()
{
    time_t t = time(NULL);
    struct tm tM;
    errno_t err_t = localtime_s(&tM, &t);
    int year = tM.tm_year + 1900;
    int mon = tM.tm_mon + 1;
    int day = tM.tm_mday;
    int hour = tM.tm_hour;
    int min = tM.tm_min;
    std::string yearStr = std::to_string(year);
    std::string monStr = std::to_string(mon);
    if (monStr.length() <= 1) { monStr = "0" + monStr; }
    std::string dayStr = std::to_string(day);
    if (dayStr.length() <= 1) { dayStr = "0" + dayStr; }
    std::string hourStr = std::to_string(hour);
    if (hourStr.length() <= 1) { hourStr = "0" + hourStr; }
    std::string minStr = std::to_string(min);
    if (minStr.length() <= 1) { minStr = "0" + minStr; }

    date = yearStr + "-" + monStr + "-" + dayStr + "T" + hourStr
        + ":" + minStr + "+09:00";
}

void writeLog(std::string str)
{
    std::ofstream outputfile;
    std::string path_str = "";
    path_str += "C:/Users/tobita/OneDrive/デスクトップ/dir_change_log.txt";

    outputfile.open(path_str, std::ios::app);
    outputfile << str << std::endl;
    
    outputfile.close();
}

int main()
{
    // UAC機能を回避する方法を入れないとダメな場合が多い
    const wchar_t* dirName = L"C:\\Windows\\System32";
    //const wchar_t* dirName = L"C:\\Users\\tobita\\test";

    HANDLE hDir = CreateFileW(
        dirName,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) 
    {
        std::cout << "[!!!] failed create handle." << std::endl;
    }
    else 
    {
        std::cout << "[*] success create handle." << std::endl;
    }

    while (true) 
    {
        wchar_t lpBuf[1024] = { L'\0' };
        DWORD byteReturned;
        DWORD notifyFilter;
        bool bRet;

        notifyFilter = 
            FILE_NOTIFY_CHANGE_FILE_NAME |  //ファイル名変更
            FILE_NOTIFY_CHANGE_DIR_NAME |  //ディレクトリ名変更
            FILE_NOTIFY_CHANGE_ATTRIBUTES |  //属性変更
            FILE_NOTIFY_CHANGE_LAST_WRITE |  //最終書き込み日時変更
            FILE_NOTIFY_CHANGE_LAST_ACCESS |  //最終アクセス日時変更
            FILE_NOTIFY_CHANGE_CREATION;       //作成日時変更

        bRet = ReadDirectoryChangesW(
            hDir,  //ディレクトリのハンドル
            lpBuf, //FILE_NOTIFY_INFORMATION構造体へのポインタ
            sizeof(lpBuf) / sizeof(lpBuf[0]), //lpBufferのサイズ
            TRUE, //サブディレクトリを監視するためのフラグ
            notifyFilter, //監視に使うフィルタ条件
            &byteReturned, //返されたバイト数
            NULL, //重複I/O操作に必要な構造体へのポインタ
            NULL);//完了ルーチンへのポインタ

        if (!bRet) 
        {
            std::cout << "[!!!] failed ReadDirectoryChangesW." << std::endl;
            break;
        }

        int i = 0;
        while (true)
        {
            FILE_NOTIFY_INFORMATION* lpInfo = (FILE_NOTIFY_INFORMATION*)&lpBuf[i];

            std::wstring ws(lpInfo->FileName);
            std::string fileName(ws.begin(), ws.end());

            std::string actionType = "";
            switch (lpInfo->Action)
            {
            case FILE_ACTION_ADDED:
                actionType = "FILE_ACTION_ADDED";
                break;
            case FILE_ACTION_REMOVED:
                actionType = "FILE_ACTION_REMOVED";
                break;
            case FILE_ACTION_MODIFIED:
                actionType = "FILE_ACTION_MODIFIED";
                break;
            case FILE_ACTION_RENAMED_OLD_NAME:
                actionType = "FILE_ACTION_RENAMED_OLD_NAME";
                break;
            case FILE_ACTION_RENAMED_NEW_NAME:
                actionType = "FILE_ACTION_RENAMED_NEW_NAME";
                break;
            default:
                break;
            }
            setDate();
            std::string out = date +" "+ fileName + ": " + actionType;
            writeLog(out);
            std::cout << out << std::endl;

            if(lpInfo->NextEntryOffset == 0)
            {
                break;
            }
            i += lpInfo->NextEntryOffset / 2;
        }

    }

    CloseHandle(hDir);
    return 0;
}
