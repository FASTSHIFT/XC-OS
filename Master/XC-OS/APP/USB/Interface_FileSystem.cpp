#include "USB_Private.h"
#include "SdFat.h"
#include "stdio.h"
#include "Basic/TasksManage.h"
#include "XC_COM/XC_COM.h"

#define PATH_START "/"

static SdFile root;

static String NowWorkPath = PATH_START;
static char NowPromptMsg[128] = XC_OS_NAME"@shell:~"PATH_START"$: ";

const char* FS_GetPromptMsg()
{
    return NowPromptMsg;
}

static bool PathCheck(const char* newPath)
{
    bool retval = false;
    if(SD.exists(newPath))
    {
        NowWorkPath = newPath;
        sprintf(NowPromptMsg, XC_OS_NAME"@shell:~%s$: ", NowWorkPath.c_str());
        retval = true;
    }
    return retval;
}

static bool PathAccess(bool enter = false, const char* folder = NULL)
{
    String path = NowWorkPath;
    /*打开文件夹*/
    if(enter)
    {
        path = (folder[0] == '/') ? folder : path + "/" + String(folder);
    }
    /*退出文件夹*/
    else
    {
        /*计算当前工作路径最后一个字符开始到'/'号的字符个数*/
        int index = path.lastIndexOf('/');
        if(index > 0)
        {
            /*将当前文件夹名从当前工作路径去除*/
            path = path.substring(0, index);
        }
    }
    return PathCheck(path.c_str());
}

static int FS_ChangeDirectory(int argc, char** argv)
{
    if(argc != 2)
    {
        shell_print_error(E_SHELL_ERR_ARGCOUNT, "example: cd /Directory");
        return SHELL_RET_FAILURE;
    }

    if(strcmp(argv[1], "..") == 0)
    {
        return PathAccess(false) ? SHELL_RET_SUCCESS : SHELL_RET_FAILURE;
    }
    else if(PathAccess(true, argv[1]))
    {
        return SHELL_RET_SUCCESS;
    }
    else
    {
        shell_print_error(E_SHELL_ERR_VALUE, "error path!");
        return SHELL_RET_FAILURE;
    }
}

static int FS_ListFiles(int argc, char** argv)
{
    if (!root.open(NowWorkPath.c_str()))
    {
        shell_print_error(E_SHELL_ERR_VALUE, "open root failed");
        return SHELL_RET_FAILURE;
    }
    FatFile file;
    int rootFileCount = 0;
    while (file.openNext(&root, O_RDONLY))
    {
        if (!file.isHidden())
        {
            rootFileCount++;
            char name[64];
            file.getName(name, sizeof(name));
            shell_print(name);
             
            shell_print(file.isDir() ? "\tDIR" : "\tFILE");
            
            shell_printf("\t%d Bytes\r\n", file.isDir() ? file.dirSize() : file.fileSize());
        }
        file.close();
    }
    shell_printf("--- Found %d File(s) ---\r\n", rootFileCount);
    root.close();
    return SHELL_RET_SUCCESS;
}

static int FS_DownLoadFile(int argc, char** argv)
{
    if(argc != 2)
    {
        shell_print_error(E_SHELL_ERR_ARGCOUNT, NULL);
        return SHELL_RET_FAILURE;
    }
    
    const char* name = argv[1];
    String path = (name[0] == '/') ? name : NowWorkPath + "/" + String(name);
    if(SD.exists(path.c_str()))
    {
        XC_COM_SendFile(path.c_str());
        return SHELL_RET_SUCCESS;
    }
    else
    {
        shell_print_error(E_SHELL_ERR_VALUE, "file open error!");
        return SHELL_RET_FAILURE;
    }
}

static int FS_UpLoadFile(int argc, char** argv)
{
    String path;
    if(argc == 1)
    {
        path = NowWorkPath + "/";
    }
    else if(argc == 2)
    {
        path = argv[1];
    }
    else
    {
        shell_print_error(E_SHELL_ERR_ARGCOUNT, NULL);
        return SHELL_RET_FAILURE;
    }

    if(SD.exists(path.c_str()))
    {
        XC_COM_RecvFile(path.c_str());
        return SHELL_RET_SUCCESS;
    }
    else
    {
        shell_print_error(E_SHELL_ERR_VALUE, "path error!");
        return SHELL_RET_FAILURE;
    }
}

void ShellReg_FS()
{
    shell_register(FS_ChangeDirectory, "cd");
    shell_register(FS_ListFiles, "ll");
    shell_register(FS_DownLoadFile, "dnld");
    shell_register(FS_UpLoadFile, "upld");
}
