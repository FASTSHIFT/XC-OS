#include "WavPlayer_Private.h"
#include "Basic/TasksManage.h"

static String musicListPath;

static lv_obj_t * appWindow;
static lv_obj_t * listMusic;
static lv_obj_t * btnFirst;
static lv_obj_t * btnEnd;
static lv_obj_t * btnNow;

lv_obj_t * MusicList_GetList()
{
    return listMusic;
}

bool MusicList_GetActivate()
{
    return lv_obj_get_hidden(listMusic);
}

void MusicList_SetActivate(bool act)
{
    if(act)lv_obj_move_foreground(listMusic);
    lv_obj_set_hidden(listMusic, act);
}

String MusicList_GetNextMusic(bool next)
{
    btnNow = next ? lv_list_get_next_btn(listMusic, btnNow) : lv_list_get_prev_btn(listMusic, btnNow);
    if(btnNow == NULL)
    {
        btnNow = next ? btnFirst : btnEnd;
    }
    return MusicList_GetNowMusic();
}

String MusicList_GetNowMusic()
{
    return musicListPath + "/" + lv_list_get_btn_text(btnNow);
}

void MusicList_PlayNext(bool next)
{
    WavPlayer_SetEnable(false);
    vTaskDelay(20);
    
    String path = MusicList_GetNextMusic(next);
    if(WavPlayer_LoadFile(path))
    {
        WavPlayer_SetEnable(true);
    }
}

static void MusicList_EventHandler(lv_obj_t * obj, lv_event_t event)
{
    
}

static void Creat_MusicList(const char* path)
{
    /*Create a list*/
    listMusic = lv_list_create(appWindow, NULL);
    lv_obj_set_size(listMusic, APP_WIN_WIDTH / 2, APP_WIN_HEIGHT / 2);
    lv_obj_set_hidden(listMusic, true);
    lv_obj_align(listMusic, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_opa_scale_enable(listMusic, true);
    lv_obj_set_opa_scale(listMusic, LV_OPA_80);
//    lv_obj_set_auto_realign(listMusic, true);
//    lv_list_set_edge_flash(listMusic, true);
    
    SdFile root;
    /*路径是否合法*/
    if (!root.open(path))
        return;
    
    /*Loading files*/
    SdFile file;
    char fileName[LV_FS_MAX_FN_LENGTH];
    bool first = false;
    /*遍历目录所有项*/
    while (file.openNext(&root, O_RDONLY))
    {
        /*文件是否隐藏*/
        if (!file.isHidden())
        {
            file.getName(fileName,sizeof(fileName));
            
            String wavName = String(fileName);
            wavName.toLowerCase();
            if(wavName.endsWith(".wav"))
            {
                /*添加list新的按钮*/
                lv_obj_t * list_btn = lv_list_add_btn(
                    listMusic,
                    LV_SYMBOL_AUDIO,
                    fileName
                );
                
                if(!first)
                {
                    btnFirst = list_btn;
                    first = true;
                }
                
                btnNow = btnEnd = list_btn;
                
//                /*添加按钮墨水效果*/
//                lv_btn_set_ink_in_time(list_btn, 200);
//                lv_btn_set_ink_out_time(list_btn, 200);
                lv_obj_set_event_cb(list_btn, MusicList_EventHandler);
            }
        }
        file.close();
    }
    root.close();
}

void MusicList_Init(const char* path)
{
    musicListPath = String(path);
    appWindow = AppWindow_GetCont(PAGE_WavPlayer);
    Creat_MusicList(path);
}


