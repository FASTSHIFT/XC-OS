#include "SettingsPrivate.h"
#include "Basic/FileGroup.h"

static void main_event_cb(lv_obj_t * btn, lv_event_t e);
//static void submenu_event_cb(lv_obj_t * btn, lv_event_t e);


/*Declare items*/
static lv_settings_item_t root_item = {.name = "Settings", .value = ""};

#define ITEM_END_DEF {.type = LV_SETTINGS_TYPE_INV}
#define MENU_ITEM_DEF(item_name,item_explain) \
    {.type = LV_SETTINGS_TYPE_LIST_BTN,\
     .name=#item_name,\
     .value=item_explain,\
     .user_data.ptr = (void*)Settings##item_name##_EventHandler}

static lv_settings_item_t main_menu_items[] =
{
    MENU_ITEM_DEF(Display, "Display and theme"),
    MENU_ITEM_DEF(Sound,   "Sound and vibration"),
    MENU_ITEM_DEF(Battery, "Battery info and power control"),
    MENU_ITEM_DEF(Radio,   "Radio config and detection"),
    MENU_ITEM_DEF(About,   "System info"),
    ITEM_END_DEF
};
    
//static lv_settings_item_t main_menu_items[] =
//{
//       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Button",          .value="Test button"},
//       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Switch",          .value="Test switch"},
//       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Drop down list",  .value="Test drop down list"},
//       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Number set",      .value="Test number set"},
//       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Slider",          .value="Test slider"},
//       {.type = LV_SETTINGS_TYPE_INV},     /*Mark the last item*/
//};

void Creat_SettingsMain(lv_obj_t * parent)
{
    /*Create the settings menu with a root item*/
    lv_settings_create(parent);

    /*Create a new page in the menu*/
    lv_settings_open_page(&root_item, main_event_cb);
}

static void main_event_cb(lv_obj_t * obj, lv_event_t event)
{
    /*Get the caller item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    /*Add the items*/
    if(event == LV_EVENT_REFRESH)
    {
        for(int i = 0; main_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++)
        {
            lv_settings_add(&main_menu_items[i]);
        }
    }
    /* Open submenus.
     * The submenus will be very simple so use a common event callback for them.
     * `act_item` is the clicked list button in the main menu.
     * It's name will be the title of the new page*/
    else if(event == LV_EVENT_CLICKED)
    {
        lv_event_cb_t event_cb_func = (lv_event_cb_t)(act_item->user_data.ptr);
        if(event_cb_func)
        {
            lv_settings_open_page(act_item, event_cb_func);
        }
    }
}

//static void submenu_event_cb(lv_obj_t * btn, lv_event_t e)
//{
//    (void)btn;  /*Unused*/

//    /*Get the caller item*/
//    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

//    /*Add items to the submenus*/
//    if(e == LV_EVENT_REFRESH)
//    {
//        if(strcmp("Button", act_item->name) == 0)
//        {
//            static lv_settings_item_t item =
//            {
//                .type = LV_SETTINGS_TYPE_BTN,
//                .name = "System test",
//                .value = "Start"
//            };
//            lv_settings_add(&item);
//        }
//        else if(strcmp("Slider", act_item->name) == 0)
//        {

//            static char value[16] = {"100 V"};
//            static lv_settings_item_t item =
//            {
//                .type = LV_SETTINGS_TYPE_SLIDER,
//                .name = "Voltage",
//                .value = value,
//                .state = 0
//            };

//            lv_settings_add(&item);
//        }
//        else if(strcmp("Switch", act_item->name) == 0)
//        {

//            static lv_settings_item_t item =
//            {
//                .type = LV_SETTINGS_TYPE_SW,
//                .name = "Valve",
//                .value = "Closed",
//                .state = 0
//            };

//            lv_settings_add(&item);
//        }
//        else if(strcmp("Number set", act_item->name) == 0)
//        {
//            static char value[16] = {"12"};
//            static lv_settings_item_t item =
//            {
//                .type = LV_SETTINGS_TYPE_NUMSET,
//                .name = "Age",
//                .value = value,
//                .state = 12
//            };

//            lv_settings_add(&item);
//        }
//        else if(strcmp("Drop down list", act_item->name) == 0)
//        {
//            static lv_settings_item_t item =
//            {
//                .type = LV_SETTINGS_TYPE_DDLIST,
//                .name = "Size",
//                .value = "S\nM\nL\nXL",
//                .state = 0
//            };

//            lv_settings_add(&item);
//        }
//    }
//    /*Handle the items' events*/
//    else if(e == LV_EVENT_VALUE_CHANGED)
//    {
//        if(strcmp("Voltage", act_item->name) == 0)
//        {
//            sprintf((char*)act_item->value, "%d V", act_item->state);
//            lv_settings_refr(act_item);
//        }
//        else if(strcmp("Age", act_item->name) == 0)
//        {
//            if(act_item->state < 6) act_item->state = 6;
//            if(act_item->state > 99) act_item->state = 99;

//            sprintf((char*)act_item->value, "%d", act_item->state);
//            lv_settings_refr(act_item);
//        }
//        else if(strcmp("Valve", act_item->name) == 0)
//        {
//            if(act_item->state) act_item->value = "Open";
//            else act_item->value = "Close";

//            lv_settings_refr(act_item);
//        }
//        else if(strcmp("Size", act_item->name) == 0)
//        {
//            Serial.printf("Size: %d\n", act_item->state);
//        }
//    }
//    else if(e == LV_EVENT_CLICKED)
//    {
//        if(strcmp("System test", act_item->name) == 0)
//        {
//            if(strcmp(act_item->value, "Start") == 0)
//            {
//                act_item->value = "Stop";
//            }
//            else
//            {
//                act_item->value = "Start";
//            }
//            lv_settings_refr(act_item);
//        }
//    }
//}
