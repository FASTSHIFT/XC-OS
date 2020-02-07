#include "Basic/FileGroup.h"
#include "DisplayPrivate.h"
#include "APP/TextEditor/TextEditor_Private.h"
#include "LuaInterface/LuaScript.h"
#include "Module/Module.h"
#include "Basic/TasksManage.h"
#include "GUI/Page/APP_Type.h"

static lv_obj_t * appWindow;

#define FILE_NAME_LEN_MAX LV_FS_MAX_FN_LENGTH

/*�ļ���·��*/
static SdFile root;

/*�ؼ���*/
static lv_obj_t * tabviewFm;
static lv_obj_t * tabDrive;
static lv_obj_t * tabFileList;
static lv_obj_t * listFiles;

/**
  * @brief  ����Tabview
  * @param  *tabview:ָ��ÿؼ���ָ��
  * @retval None
  */
static void Creat_Tabview(lv_obj_t** tabview)
{
    *tabview = lv_tabview_create(appWindow, NULL);
    lv_obj_set_size(*tabview, APP_WIN_WIDTH, APP_WIN_HEIGHT);
    lv_obj_align(*tabview, appWindow, LV_ALIGN_IN_TOP_MID, 0, 0);
    tabFileList = lv_tabview_add_tab(*tabview, LV_SYMBOL_LIST);
    tabDrive = lv_tabview_add_tab(*tabview, LV_SYMBOL_DRIVE);
}

/**
  * @brief  ����Tab
  * @param  *tab:ָ����������Ϣtab��ָ��
  * @retval None
  */
static void Creat_TabDrive(lv_obj_t * tab)
{
    /*��λת��*/
#define CONV_MB(size) (size*0.000512f)
#define CONV_GB(size) (CONV_MB(size)/1024.0f)
    /*��ȡ��������С*/
    uint32_t cardSize = SD.card()->cardSize();
    /*��ȡ������ʣ��ռ�Ĵ�С*/
    uint32_t volFree = SD.vol()->freeClusterCount() * SD.vol()->blocksPerCluster();

    /*�ܿռ�*/
    float totalSize = CONV_GB(cardSize);
    /*δʹ�ÿռ�*/
    float freeSize = CONV_GB(volFree);
    /*��ʹ�ÿռ�*/
    float usageSize = totalSize - freeSize;

    /*LineMeter��ʾ��ʹ�ÿռ�*/
    lv_obj_t * lmeter = lv_lmeter_create(tab, NULL);
    lv_obj_t * lmeterLabel = lv_label_create(lmeter, NULL);
    lv_obj_set_size(lmeter, 150, 150);
    lv_obj_align(lmeter, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_lmeter_set_range(lmeter, 0, 100);
    lv_lmeter_set_scale(lmeter, 240, 31);

    /*��ʹ�ÿռ�ٷֱ�*/
    int16_t dispUseage = usageSize / totalSize * 100.0f;
    lv_lmeter_set_value(lmeter, dispUseage);
    lv_label_set_text_fmt(lmeterLabel, "%d%%", dispUseage);

    /*��ǩ��ʽΪ������*/
    static lv_style_t labelStyle = *lv_obj_get_style(lmeterLabel);
    labelStyle.text.font = &lv_font_roboto_28;
    lv_label_set_style(lmeterLabel, LV_LABEL_STYLE_MAIN, &labelStyle);

    /*��ǩ���ж���*/
    lv_obj_align(lmeterLabel, lmeter, LV_ALIGN_CENTER, 0, 0);

    /*SD Size*/
    lv_obj_t * labelSD = lv_label_create(lmeter, NULL);
    lv_label_set_text_fmt(labelSD, "%0.2fGB / %0.2fGB", usageSize, totalSize);
    lv_obj_align(labelSD, lmeter, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
}

/*����Ի���mbox�ؼ�*/
static lv_obj_t * mboxError;
/**
  * @brief  mbox�ؼ��¼�
  * @param  *obj:�����ַ
  * @param  event:�¼�����
  * @retval None
  */
static void mbox_event_handler(lv_obj_t * obj, lv_event_t event)
{
    /*�����¼�*/
    if(event == LV_EVENT_CLICKED)
    {
        /*�رնԻ���*/
        lv_mbox_start_auto_close(obj, 20);
    }
    /*ȡ���¼�*/
    if(event == LV_EVENT_CANCEL)
    {
        /*�رնԻ���*/
        lv_mbox_start_auto_close(obj, 20);
    }
    /*ɾ���¼�*/
    if(event == LV_EVENT_DELETE)
    {
        /*ɾ���ؼ���ַ*/
        mboxError = NULL;
    }
}

/**
  * @brief  �׳��Ի���
  * @param  *text:�Ի����ı�
  * @retval true�Ի����׳��ɹ���false�Ի�������ռ��
  */
static bool MboxThorw(const char *text)
{
    /*�Ի����Ƿ�ռ��*/
    if(mboxError)
        return false;
    
    /*�Ի��򼤻�*/
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
  * @brief  ���ı��ļ�
  * @param  filename:�ļ�·��
  * @retval None
  */
static void OpenTextFile(String filename)
{
    /*�ж��ı��༭���Ƿ�����*/
    if(TextEditorGetLocked())
    {
        MboxThorw("text editor is running");
        return;
    }
    
    /*�ı��ļ�����*/
    SdFile file;
    /*���ļ�*/
    if(file.open(filename.c_str(), O_RDWR))
    {
        /*�ļ������Ƿ���ڻ���������*/
        if(file.available() < TextBuf_GetSize())
        {
            /*�建����*/
            TextBuf_Clear();
            /*���뻺����*/
            file.read(TextBuf_GetBuff(), TextBuf_GetSize());
            
            /*Lua������ָ���ı�������*/
            LuaCodeSet(TextBuf_GetBuff());
            //page.PagePush(PAGE_LuaScript);
            /*�ı��༭��ָ���ı�������*/
            TextEditorSet(TextBuf_GetBuff(), file);
            /*�л��ı��༭��ҳ��*/
            page.PagePush(PAGE_TextEditor);
        }
        else
        {
            /*�ļ�����*/
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
/*�ļ�����ö��*/
typedef enum{
    FT_UNKONWN,
    FT_IMG,
    FT_VIDEO,
    FT_AUDIO,
    FT_TEXT,
    FT_DIR,
    FT_MAX
}File_Type;

typedef void(*OpenFileFunc_t)(const char*);
extern void Wav_OpenFile(const char* path);
extern void Bv_OpenFile(const char* path);

/*�ļ����ͱ�*/
typedef struct{
    const char* extName;
    const char* sym;
    File_Type type;
    OpenFileFunc_t openFileFunc;
}FileTypeList_TypeDef;
/*��չ��ͼ��ӳ���*/
static FileTypeList_TypeDef FileTypeList[] = {
    {"",     LV_SYMBOL_FILE,   FT_UNKONWN},
    {".bv",  LV_SYMBOL_VIDEO,  FT_VIDEO, Bv_OpenFile},
    {".wav", LV_SYMBOL_AUDIO,  FT_AUDIO, Wav_OpenFile},
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

/*�л�����·��*/
static void PathChange(lv_obj_t * tab, const char *newPath);
/*��ǰ����·��*/
String NowFilePath = "/";
/**
  * @brief  ���ļ���
  * @param  enter:�Ƿ���룬�л�����һ���ļ���
  * @param  folder:���ļ�������
  * @retval None
  */
static void FolderAccess(bool enter = false, const char* folder = NULL)
{
    /*���ļ���*/
    if(enter)
    {
        NowFilePath = NowFilePath + "/" + String(folder);
    }
    /*�˳��ļ���*/
    else
    {
        /*���㵱ǰ����·�����һ���ַ���ʼ��'/'�ŵ��ַ�����*/
        int index = NowFilePath.lastIndexOf('/');
        if(index > 0)
        {
            /*����ǰ�ļ������ӵ�ǰ����·��ȥ��*/
            NowFilePath = NowFilePath.substring(0, index);
        }
    }
    /*�л��µĹ���·��*/
    PathChange(tabFileList, NowFilePath.c_str());
}

/**
  * @brief  ��ȡ�ļ���Ϣ����
  * @param  *filename:�ļ���
  * @retval ������
  */
static int FileInfoGetIndex(const char *filename)
{
    String Name = String(filename);
    /*ȫ���л�ΪСд*/
    Name.toLowerCase();
    
    /*���*/
    for(uint8_t i = 1; i < __Sizeof(FileTypeList); i++)
    {
        /*�ҵ���Ӧ��չ��*/
        if(Name.endsWith(FileTypeList[i].extName))
        {
            /*����������*/
            return i;
        }
    }
    /*δ֪�ļ�����*/
    return 0;
}

/**
  * @brief  �ļ���ѡ���¼�
  * @param  *obj:�����ַ
  * @param  event:�¼�����
  * @retval None
  */
static void FileEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    /*����¼�*/
    if(event == LV_EVENT_CLICKED)
    {
        /*����Ի��������ȹرնԻ���*/
        if(mboxError)
        {
            lv_event_send(mboxError, LV_EVENT_CANCEL, 0);
            return;
        }
        
        /*��ȡ��������ļ���Ϣ*/
        const char* name = lv_list_get_btn_text(obj);
        const char* sym = (const char*)lv_img_get_src(lv_list_get_btn_img(obj));
        int index = FileInfoGetIndex(name);
        
        /*����ǰ����·����߼����ļ���*/
        String filePath =  NowFilePath + "/" + String(name);
        
        /*�Ƿ�Ϊ�ļ���*/
        if(strcmp(sym, LV_SYMBOL_DIRECTORY) == 0)
        {
            /*�����ļ���*/
            FolderAccess(true, name);
        }
        /*���Ϊ���ı������ļ�*/
        else if(FileTypeList[index].type == FT_TEXT)
        {
            /*���ı��ļ�*/
            OpenTextFile(filePath);
        }
        /*����д򿪷�ʽ*/
        else if(index > 0)
        {
            if(FileTypeList[index].openFileFunc)
                FileTypeList[index].openFileFunc(filePath.c_str());
        }
    }
}

/**
  * @brief  �����ļ��б�
  * @param  *tab: tab����
  * @param  **list:list�ؼ�������ַ
  * @param  *path:��ǰ�ļ�·��
  * @retval None
  */
static void Creat_TabFileList(lv_obj_t * tab, lv_obj_t** list, const char *path)
{
    /*·���Ƿ�Ϸ�*/
    if (!root.open(path))
        return;
    
    /*Create a list*/
    *list = lv_list_create(tab, NULL);
    lv_obj_set_size(*list, lv_obj_get_width_fit(tab) - 10, lv_obj_get_height_fit(tab) - 10);
    lv_obj_align(*list, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_auto_realign(*list, true);
    lv_list_set_edge_flash(*list, true);
    
    /*Loading files*/
    SdFile file;
    char fileName[FILE_NAME_LEN_MAX];
    /*����Ŀ¼������*/
    while (file.openNext(&root, O_RDONLY))
    {
        /*�ļ��Ƿ�����*/
        if (!file.isHidden())
        {
            file.getName(fileName,sizeof(fileName));
            
            int i = FileInfoGetIndex(fileName);
            /*���list�µİ�ť*/
            lv_obj_t * list_btn = lv_list_add_btn(
                *list,
                file.isDir() ? LV_SYMBOL_DIRECTORY : FileTypeList[i].sym,
                fileName
            );
            
            /*��Ӱ�ťīˮЧ��*/
            lv_btn_set_ink_in_time(list_btn, 200);
            lv_btn_set_ink_out_time(list_btn, 200);
            lv_obj_set_event_cb(list_btn, FileEvent_Handler);
        }
        file.close();
    }
}

/**
  * @brief  �ı䵱ǰ����·��
  * @param  *tab: tab����
  * @param  *newPath:�µĹ���·��
  * @retval ��
  */
static void PathChange(lv_obj_t * tab, const char *newPath)
{
    lv_obj_del_safe(&listFiles);
    root.close();
    Creat_TabFileList(tab, &listFiles, newPath);
}

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    /*��ȡ�ļ�ϵͳʹ��Ȩ*/
    xSemaphoreTake(SemHandle_FileSystem, 1000);
    
    lv_obj_move_foreground(appWindow);
    __ExecuteOnce(Creat_Tabview(&tabviewFm));
    
    lv_obj_set_hidden(tabviewFm, false);
    PathChange(tabFileList, NowFilePath.c_str()); 
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
    /*����Ƿ��ڵڶ���tab(ID:1)*/
    if(lv_tabview_get_tab_act(tabviewFm) == 1)
    {
        /*���ش����豸ʹ�����*/
        __ExecuteOnce((
            lv_tabview_set_sliding(tabviewFm, false),
            Preloader_Activate(true, tabDrive),
            Creat_TabDrive(tabDrive),
            Preloader_Activate(false, NULL),
            lv_tabview_set_sliding(tabviewFm, true)
        ));
    }
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    lv_obj_del_safe(&listFiles);
    lv_obj_set_hidden(tabviewFm, true);
    
    /*�黹�ļ�ϵͳʹ��Ȩ*/
    xSemaphoreGive(SemHandle_FileSystem);
}

/**
  * @brief  ҳ���¼�
  * @param  event:�¼����
  * @param  param:�¼�����
  * @retval ��
  */
static void Event(int event, void* param)
{
    lv_obj_t * btn = (lv_obj_t*)param;
    if(event == LV_EVENT_CLICKED)
    {
        /*���ذ���*/
        if(btn == btnBack)
        {
            /*����Ի��������ȹرնԻ���*/
            if(mboxError)
            {
                lv_event_send(mboxError, LV_EVENT_CANCEL, 0);
                return ;
            }
            
            /*�����ǰΪ��Ŀ¼*/
            if(NowFilePath == "/")
            {
                /*�˳���ҳ��*/
                page.PagePop();
            }

            /*�ļ���·������*/
            if(root.isOpen())
            {
                /*�˳���ǰ�ļ���*/
                FolderAccess();
            }
        }
    }
}

/**
  * @brief  ҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_FileExplorer(uint8_t pageID)
{
    appWindow = Page_GetAppWindow(pageID);
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
