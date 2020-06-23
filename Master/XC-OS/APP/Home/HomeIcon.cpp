#include "HomePrivate.h"

/*图片文件*/
LV_IMG_DECLARE(ImgSettings);
LV_IMG_DECLARE(ImgCode);
LV_IMG_DECLARE(ImgGame);
LV_IMG_DECLARE(ImgSubApps);
LV_IMG_DECLARE(ImgFiles);
LV_IMG_DECLARE(ImgEditor);
LV_IMG_DECLARE(ImgVideo);
LV_IMG_DECLARE(ImgMusic);
LV_IMG_DECLARE(ImgTerminal);

/*APP组*/
static APP_TypeDef APP_Grp[] =
{
    {&ImgSettings, "Settings", TYPE_PageJump, PAGE_Settings},
    {&ImgCode,     "IDE",      TYPE_PageJump, PAGE_LuaScript},
    {&ImgGame,     "Game",     TYPE_PageJump, PAGE_GameSelect},
    {&ImgSubApps,  "APPs",     TYPE_PageJump, PAGE_LuaAppSel},
    {&ImgFiles,    "Files",    TYPE_PageJump, PAGE_FileExplorer},
    {&ImgVideo,    "Video",    TYPE_PageJump, PAGE_BvPlayer},
    {&ImgEditor,   "Editor",   TYPE_PageJump, PAGE_TextEditor},
    {&ImgMusic,    "Music",    TYPE_PageJump, PAGE_WavPlayer},
    {&ImgTerminal, "Shell",    TYPE_PageJump, PAGE_Shell},
};

static int AppImg_GetIndex(lv_obj_t * obj)
{
    /*扫描APP组*/
    for(int i = 0; i < __Sizeof(APP_Grp); i++)
    {
        if(obj == APP_Grp[i].imgbtn)
        {
            return i;
        }
    }
    return -1;
}

/**
  * @brief  APP图片事件
  * @param  obj:对象
  * @param  event:事件类型
  * @retval None
  */
static void AppEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    int index = AppImg_GetIndex(obj);
    if(index < 0)
        return;
    
    /*单击事件*/
    if(event == LV_EVENT_CLICKED)
    {
        if(APP_Grp[index].type == TYPE_PageJump)
        {
            /*页面跳转*/
            page.PagePush(APP_Grp[index].param);
            /*播放跳转动画*/
            Home_AppAnim(
                false,
                lv_obj_get_x_center(APP_Grp[index].cont),
                lv_obj_get_y_center(APP_Grp[index].cont),
                AnimCloseTime_Default
            );
        }
    }
}

/**
  * @brief  生成APP图标
  * @param  app:APP引用
  * @param  parent:放置APP图标的父对象
  * @param  index:索引0~9
  * @retval None
  */
void Home_CreatAppIcon(APP_TypeDef &app, lv_obj_t * parent, lv_event_cb_t imgbtn_event_handler)
{
    /*创建APP图标容器*/
    app.cont = lv_cont_create(parent, NULL);
    /*样式设置为透明*/
    lv_cont_set_style(app.cont, LV_CONT_STYLE_MAIN, &lv_style_transp);

    /*创建图标按钮控件*/
    app.imgbtn = lv_imgbtn_create(app.cont, NULL);
    lv_imgbtn_set_src(app.imgbtn, LV_BTN_STATE_REL, app.img_dsc);
    lv_imgbtn_set_src(app.imgbtn, LV_BTN_STATE_PR, app.img_dsc);
    lv_imgbtn_set_toggle(app.imgbtn, false);
    /*图片被按下样式*/
    static lv_style_t style_pr = *lv_imgbtn_get_style(app.imgbtn, LV_IMGBTN_STYLE_PR);
    style_pr.image.color = LV_COLOR_GRAY;
    style_pr.image.intense = LV_OPA_70;
    lv_imgbtn_set_style(app.imgbtn, LV_IMGBTN_STYLE_PR, &style_pr);
    /*关联事件*/
    lv_obj_set_event_cb(app.imgbtn, imgbtn_event_handler);

    /*在图标下创建标签*/
    app.label = lv_label_create(app.cont, NULL);
    lv_label_set_text(app.label, app.lable_text);
    /*居中对齐*/
    lv_obj_align(app.label, app.imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    /*容器大小自适应*/
    lv_cont_set_fit(app.cont, LV_FIT_TIGHT);
    lv_cont_set_layout(app.cont, LV_LAYOUT_COL_M);
}

void Home_AppInit(lv_obj_t * parent)
{
    lv_obj_t * cont = lv_cont_create(parent, NULL);
    lv_obj_set_size(cont, lv_obj_get_width(parent) - 10, lv_obj_get_height(parent) - 10);
    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_cont_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_set_click(cont, false);
    
    static lv_style_t style = lv_style_transp;
    style.body.padding.top = 15;
    style.body.padding.left = 10;
    style.body.padding.right = 10;
    style.body.padding.inner = 20;
    lv_cont_set_style(cont, LV_CONT_STYLE_MAIN, &style);
    
    /*在主Tab创建APP组*/
    __LoopExecute(Home_CreatAppIcon(APP_Grp[i], cont, AppEvent_Handler), __Sizeof(APP_Grp));
}
