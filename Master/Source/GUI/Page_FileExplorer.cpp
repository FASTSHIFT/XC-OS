#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "FileManage.h"
#include "LuaScript.h"
#include "Module.h"

#define FILENAME_LEN_MAX 64
#define FILE_CNT_MAX     64

/*文件根路径*/
static SdFile root;

/*控件们*/
static lv_obj_t * tabviewFm;
static lv_obj_t * tabDrive;
static lv_obj_t * tabFileList;
static lv_obj_t * listFiles;

/**
  * @brief  创建Tabview
  * @param  *tabview:指向该控件的指针
  * @retval None
  */
static void Creat_Tabview(lv_obj_t** tabview)
{
    *tabview = lv_tabview_create(appWindow, NULL);
    lv_obj_set_size(*tabview, APP_WIN_WIDTH, APP_WIN_HEIGHT);
    lv_obj_align(*tabview, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    tabDrive = lv_tabview_add_tab(*tabview, LV_SYMBOL_DRIVE);
    tabFileList = lv_tabview_add_tab(*tabview, LV_SYMBOL_LIST);
}

/**
  * @brief  创建Tab
  * @param  *tab:指向驱动器信息tab的指针
  * @retval None
  */
static void Creat_TabDrive(lv_obj_t * tab)
{
    /*单位转换*/
#define CONV_MB(size) (size*0.000512f)
#define CONV_GB(size) (CONV_MB(size)/1024.0f)
    /*获取储存器大小*/
    uint32_t cardSize = SD.card()->cardSize();
    /*获取储存器剩余空间的大小*/
    uint32_t volFree = SD.vol()->freeClusterCount() * SD.vol()->blocksPerCluster();

    /*总空间*/
    float totalSize = CONV_GB(cardSize);
    /*未使用空间*/
    float freeSize = CONV_GB(volFree);
    /*已使用空间*/
    float usageSize = totalSize - freeSize;

    /*LineMeter显示已使用空间*/
    lv_obj_t * lmeter = lv_lmeter_create(tab, NULL);
    lv_obj_t * lmeterLabel = lv_label_create(lmeter, NULL);
    lv_obj_set_size(lmeter, 150, 150);
    lv_obj_align(lmeter, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_lmeter_set_range(lmeter, 0, 100);
    lv_lmeter_set_scale(lmeter, 240, 31);

    /*已使用空间百分比*/
    int16_t dispUseage = usageSize / totalSize * 100.0f;
    lv_lmeter_set_value(lmeter, dispUseage);
    lv_label_set_text_format(lmeterLabel, "%d%%", dispUseage);

    /*标签样式为大字体*/
    static lv_style_t labelStyle = *lv_obj_get_style(lmeterLabel);
    labelStyle.text.font = &lv_font_roboto_28;
    lv_label_set_style(lmeterLabel, LV_LABEL_STYLE_MAIN, &labelStyle);

    /*标签居中对齐*/
    lv_obj_align(lmeterLabel, lmeter, LV_ALIGN_CENTER, 0, 0);

    /*SD Size*/
    lv_obj_t * labelSD = lv_label_create(lmeter, NULL);
    lv_label_set_text_format(labelSD, "%0.2fGB / %0.2fGB", usageSize, totalSize);
    lv_obj_align(labelSD, lmeter, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
}

/*错误对话框mbox控件*/
static lv_obj_t * mboxError;
/**
  * @brief  mbox控件事件
  * @param  *obj:对象地址
  * @param  event:事件类型
  * @retval None
  */
static void mbox_event_handler(lv_obj_t * obj, lv_event_t event)
{
    /*单击事件*/
    if(event == LV_EVENT_CLICKED)
    {
        /*关闭对话框*/
        lv_mbox_start_auto_close(obj, 20);
    }
    /*取消事件*/
    if(event == LV_EVENT_CANCEL)
    {
        /*关闭对话框*/
        lv_mbox_start_auto_close(obj, 20);
    }
    /*删除事件*/
    if(event == LV_EVENT_DELETE)
    {
        /*删除控件地址*/
        mboxError = NULL;
    }
}

/**
  * @brief  抛出对话框
  * @param  *text:对话框文本
  * @retval true对话框抛出成功，false对话框正在占用
  */
static bool MboxThorw(const char *text)
{
    /*对话框是否占用*/
    if(mboxError)
        return false;
    
    /*对话框激活*/
    MessageBox_Activate(
        true,
        tabFileList,
        &mboxError,
        200, 200,
        text,
        NULL,
        mbox_event_handler
    );
    
    return true;
}

/**
  * @brief  打开文本文件
  * @param  *filename:文件路径
  * @retval None
  */
static void OpenTextFile(const char * filename)
{
    /*判断文本编辑器是否被锁定*/
    if(TextEditorGetLocked())
    {
        MboxThorw("text editor is running");
        return;
    }
    
    /*文本文件对象*/
    SdFile file;
    /*打开文件*/
    if(file.open(filename, O_RDWR))
    {
        /*文件长度是否大于缓冲区长度*/
        if(file.available() < TextBuf_GetSize())
        {
            /*清缓冲区*/
            TextBuf_Clear();
            /*载入缓冲区*/
            file.read(TextBuf_GetBuff(), TextBuf_GetSize());
            
            /*Lua解释器指向文本缓冲区*/
            LuaCodeSet(TextBuf_GetBuff());
            //page.PagePush(PAGE_LuaScript);
            /*文本编辑器指向文本缓冲区*/
            TextEditorSet(TextBuf_GetBuff(), file);
            /*切换文本编辑器页面*/
            page.PagePush(PAGE_TextEditor);
        }
        else
        {
            /*文件过大*/
            char str[50];
            sprintf(
                str, 
                "file size too large!\n(%0.2fKB > buffer size(%0.2fKB))", 
                (float)file.available() / 1024.0f, TextBuf_GetSize() / 1024.0f
            );
            MboxThorw(str);
        }
        file.close();
    }
    else
    {
        MboxThorw("file open faild");
    }
}

/************************File Analyzer*************************/
/*文件类型枚举*/
typedef enum{
    FT_UNKONWN,
    FT_IMG,
    FT_VIDEO,
    FT_AUDIO,
    FT_TEXT,
    FT_DIR
}File_Type;

/*文件结构体*/
typedef struct{
    File_Type type;
    const char* sym;
    String name;
}FileInfo_TypeDef;
FileInfo_TypeDef FileInfo_Grp[FILE_CNT_MAX];//一个目录最多50个文件

/*文件图标*/
typedef struct{
    String extName;
    const char* sym;
    File_Type type;
}FileExtSym_TypeDef;
/*扩展名图标映射表*/
FileExtSym_TypeDef ExtSym_Grp[] = {
    {"",     LV_SYMBOL_FILE,   FT_UNKONWN},
    {".bv",  LV_SYMBOL_VIDEO,  FT_VIDEO},
    {".wav", LV_SYMBOL_AUDIO,  FT_AUDIO},
    {".lua", LV_SYMBOL_EDIT,   FT_TEXT},
    {".txt", LV_SYMBOL_EDIT,   FT_TEXT},
    {".html",LV_SYMBOL_EDIT,   FT_TEXT},
    {".log", LV_SYMBOL_EDIT,   FT_TEXT},
    {".xtrc",LV_SYMBOL_EDIT,   FT_TEXT},
    {".xlrc",LV_SYMBOL_EDIT,   FT_TEXT},
    {".png", LV_SYMBOL_IMAGE,  FT_IMG},
    {".jpg", LV_SYMBOL_IMAGE,  FT_IMG},
    {".gif", LV_SYMBOL_IMAGE,  FT_IMG},
};

/*切换工作路径*/
static void PathChange(lv_obj_t * tab, lv_obj_t** list, const char *newPath);
/*当前工作路径*/
String NowFilePath = "/";
/**
  * @brief  打开文件夹
  * @param  enter:是否进入，切换到上一层文件夹
  * @param  *folder:子文件夹名称
  * @retval None
  */
static void FolderAccess(bool enter = false, String *folder = NULL)
{
    /*打开文件夹*/
    if(enter)
    {
        NowFilePath = NowFilePath + "/" + *folder;
    }
    /*退出文件夹*/
    else
    {
        /*计算当前工作路径最后一个字符开始到'/'号的字符个数*/
        int index = NowFilePath.lastIndexOf('/');
        if(index > 0)
        {
            /*将当前文件夹名从当前工作路径去除*/
            NowFilePath = NowFilePath.substring(0, index);
        }
    }
    /*切换新的工作路径*/
    PathChange(tabFileList, &listFiles, NowFilePath.c_str());
}

/**
  * @brief  获取文件信息索引
  * @param  *filename:文件名
  * @retval 索引号
  */
static int FileInfoGetIndex(const char *filename)
{
    String Name = String(filename);
    /*全部切换为小写*/
    Name.toLowerCase();
    
    /*查表*/
    for(uint8_t i = 1; i < __Sizeof(ExtSym_Grp); i++)
    {
        /*找到对应扩展名*/
        if(Name.endsWith(ExtSym_Grp[i].extName))
        {
            /*返回索引号*/
            return i;
        }
    }
    /*未知文件索引*/
    return 0;
}

/**
  * @brief  获取下一个文件信息
  * @param  &file:引用文件对象
  * @param  index:索引号
  * @retval true获取成功，false index越界
  */
static bool FileInfoLoadNext(SdFile &file, int index)
{
    /*越界判断*/
    if(index >= __Sizeof(FileInfo_Grp))
        return false;
    
    /*文件名获取*/
    char fileName[FILENAME_LEN_MAX];
    file.getName(fileName, sizeof(fileName));
    FileInfo_Grp[index].name = String(fileName);
    
    /*是否为文件夹*/
    if(file.isDir())
    {
        FileInfo_Grp[index].type = FT_DIR;
        FileInfo_Grp[index].sym = LV_SYMBOL_DIRECTORY;
    }
    else
    {
        /*查询文件类型*/
        int i = FileInfoGetIndex(fileName);
        FileInfo_Grp[index].type = ExtSym_Grp[i].type;
        FileInfo_Grp[index].sym = ExtSym_Grp[i].sym;
    }
    
    return true;
}

/**
  * @brief  文件被选中事件
  * @param  *obj:对象地址
  * @param  event:事件类型
  * @retval None
  */
static void FileEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    /*点击事件*/
    if(event == LV_EVENT_CLICKED)
    {
        /*如果对话框开启，先关闭对话框*/
        if(mboxError)
        {
            lv_event_send(mboxError, LV_EVENT_CANCEL, 0);
            return;
        }
        
        /*获取被点击的文件索引*/
        int index = lv_list_get_btn_index(listFiles, obj);
        
        /*将当前工作路径后边加上文件名*/
        String filePath =  NowFilePath + "/" + FileInfo_Grp[index].name;
        
        /*如果为纯文本类型文件*/
        if(FileInfo_Grp[index].type == FT_TEXT)
        {
            /*打开文本文件*/
            OpenTextFile(filePath.c_str());
        }
        /*如果为音频类型文件*/
        else if(FileInfo_Grp[index].type == FT_AUDIO)
        {
            extern void Wav_SetFilePath(String path);
            Wav_SetFilePath(filePath);
            /*跳转至音频播放器页面*/
            page.PagePush(PAGE_WavPlayer);
        }
        else if(FileInfo_Grp[index].type == FT_VIDEO)
        {
            extern void Bv_SetFilePath(String path);
            Bv_SetFilePath(filePath);
            page.PagePush(PAGE_BvPlayer);
        }
        /*是否为文件夹*/
        else if(FileInfo_Grp[index].type == FT_DIR)
        {
            /*进入文件夹*/
            FolderAccess(true, &FileInfo_Grp[index].name);
        }
    }
}

/**
  * @brief  创建文件列表
  * @param  *tab: tab对象
  * @param  **list:list控件二级地址
  * @param  *path:当前文件路径
  * @retval None
  */
static void Creat_TabFileList(lv_obj_t * tab, lv_obj_t** list, const char *path)
{
    /*路径是否合法*/
    if (!root.open(path))
    {
        return;
    }
    
    /*Create a list*/
    *list = lv_list_create(tab, NULL);
    lv_obj_set_size(*list, lv_obj_get_width_fit(tab) - 10, lv_obj_get_height_fit(tab) - 10);
    lv_obj_align(*list, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_auto_realign(*list, true);
    lv_list_set_edge_flash(*list, true);
    
    /*Loading files*/
    SdFile file;
    int index = 0;
    /*遍历目录所有项*/
    while (file.openNext(&root, O_RDONLY))
    {
        /*文件是否隐藏*/
        if (!file.isHidden())
        {
            /*加载下一个文件信息*/
            if(!FileInfoLoadNext(file, index))
            {
                return;
            }

            /*添加list新的按钮*/
            lv_obj_t * list_btn = lv_list_add_btn(
                *list, 
                FileInfo_Grp[index].sym, 
                FileInfo_Grp[index].name.c_str()
            );
            index++;
            
            /*添加按钮墨水效果*/
            lv_btn_set_ink_in_time(list_btn, 200);
            lv_btn_set_ink_out_time(list_btn, 200);
            lv_obj_set_event_cb(list_btn, FileEvent_Handler);
        }
        file.close();
    }
}

/**
  * @brief  改变当前工作路径
  * @param  *tab: tab对象
  * @param  **list:list控件二级地址
  * @param  *newPath:新的工作路径
  * @retval 无
  */
static void PathChange(lv_obj_t * tab, lv_obj_t** list, const char *newPath)
{
    memset(FileInfo_Grp, 0, sizeof(FileInfo_Grp));
    lv_obj_del_safe(&listFiles);
    root.close();
    Creat_TabFileList(tab, &(*list), newPath);
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    __ExecuteOnce(Creat_Tabview(&tabviewFm));
    
    lv_obj_set_hidden(tabviewFm, false);
    PathChange(tabFileList, &listFiles, NowFilePath.c_str()); 
    
//    __ExecuteOnce((
//        Preloader_Activate(true, tabDrive),
//        Creat_TabDrive(tabDrive),
//        Preloader_Activate(false, NULL)
//    ));
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    //lv_obj_del_safe(&tabviewFm);  
    lv_obj_del_safe(&listFiles);
    lv_obj_set_hidden(tabviewFm, true);
}

/**
  * @brief  页面事件
  * @param  event:事件编号
  * @param  param:事件参数
  * @retval 无
  */
static void Event(int event, void* param)
{
    lv_obj_t * btn = (lv_obj_t*)param;
    if(event == LV_EVENT_CLICKED)
    {
        /*返回按键*/
        if(btn == btnBack)
        {
            /*如果对话框开启，先关闭对话框*/
            if(mboxError)
            {
                lv_event_send(mboxError, LV_EVENT_CANCEL, 0);
                return ;
            }
            
            /*如果当前为根目录*/
            if(NowFilePath == "/")
            {
                /*退出该页面*/
                page.PagePop();
            }

            /*文件根路径开启*/
            if(root.isOpen())
            {
                /*退出当前文件夹*/
                FolderAccess();
            }
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_FileExplorer(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
