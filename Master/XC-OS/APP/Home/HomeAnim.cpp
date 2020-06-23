#include "HomePrivate.h"

/*容器，用于页面跳转动画*/
static lv_obj_t * contAppSw;

/*动画组*/
enum{A_X, A_Y, A_W, A_H, A_OPA, A_MAX};
static lv_anim_t anim_grp[A_MAX];

static void AnimEnd_EventHandler(lv_anim_t * a)
{
//    lv_obj_del_safe(&contAppSw);
    lv_obj_set_hidden(contAppSw, true);
}

static void AnimInit(uint16_t time)
{
    /*允许调整透明度*/
    lv_obj_set_opa_scale_enable(contAppSw, true),
    
    /*对象*/
    anim_grp[0].var = contAppSw;
    /*路径*/
    anim_grp[0].path_cb  = lv_anim_path_ease_in_out;
    /*时间*/
    anim_grp[0].time = time;
    /*拷贝相同参数*/
    anim_grp[1] = anim_grp[0];
    anim_grp[2] = anim_grp[0];
    anim_grp[3] = anim_grp[0];
    anim_grp[4] = anim_grp[0];
    
    /*动画回调设定*/
    anim_grp[A_X].exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_x;
    anim_grp[A_Y].exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
    anim_grp[A_W].exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_width;
    anim_grp[A_H].exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_height;
    anim_grp[A_OPA].exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_opa_scale;
    
    anim_grp[A_X].ready_cb = AnimEnd_EventHandler;
}

/**
  * @brief  APP切换动画控制
  * @param  close: 是否为关闭动画
  * @param  x:起始坐标X(开启动画有效)
  * @param  y:起始坐标Y(开启动画有效)
  * @param  time:动画时间(ms)
  * @retval None
  */
void Home_AppAnim(bool close , lv_coord_t x , lv_coord_t y ,uint16_t time)
{
    /*创建动画容器*/
    if(contAppSw == NULL)
    {
        contAppSw = lv_cont_create(Home_GetAppWindow(), NULL);
    }
    
    /*动画组初始化，只执行一次*/
    __ExecuteOnce(AnimInit(time));

    /*是否为关闭动画*/
    if(!close)
    {
        /*起点终点设定*/
        anim_grp[A_OPA].start = LV_OPA_TRANSP;
        anim_grp[A_OPA].end = LV_OPA_COVER;
        
        anim_grp[A_W].start = anim_grp[A_H].start = 0;
        anim_grp[A_W].end = APP_WIN_WIDTH;
        anim_grp[A_H].end = APP_WIN_HEIGHT;
        
        anim_grp[A_X].start = x;
        anim_grp[A_Y].start = y;
        
        anim_grp[A_X].end = 0;
        anim_grp[A_Y].end = 0;
    }
    else
    {
        /*反转起点终点设定，倒放*/
#define ANIM_SWAP(anim) {int temp;temp=anim.start,anim.start=anim.end,anim.end=temp;}
        __LoopExecute(ANIM_SWAP(anim_grp[i]), __Sizeof(anim_grp));
    }
    
    lv_obj_set_top(contAppSw, true);
    lv_obj_set_hidden(contAppSw, false);
    
    /*应用动画组*/
    __LoopExecute(lv_anim_create(&(anim_grp[i])), __Sizeof(anim_grp));
}
