#include "Basic/FileGroup.h"
#include "DisplayPrivate.h"
#include "Communication/ComPrivate.h"
#include "rng.h"

#define NRF_CHANNELS 126
/*smooths signal strength with numerical range 0 - 0x7FFF*/
static uint16_t SignalStrength[NRF_CHANNELS];
static uint8_t NRF_BKUP_EN_AA = 0;
static uint8_t NRF_BKUP_RF_SETUP = 0;
static uint8_t NRF_BKUP_RF_CH = 0;
static uint8_t NRF_BKUP_CONFIG = 0;
//static bool HoldGraph = false;
static float RecvSensitivity = 1.0f;
static bool IsStartScan = false;

static void NRF_Prepare(bool setup)
{
    if(setup)
    {
//        HoldGraph = false;
        nrf.SetRF_Enable(false);

        /*backup nrf reg*/
        NRF_BKUP_EN_AA = nrf.SPI_Read(nrf.EN_AA);
        NRF_BKUP_RF_SETUP = nrf.SPI_Read(nrf.RF_SETUP);
        NRF_BKUP_RF_CH = nrf.SPI_Read(nrf.RF_CH);
        NRF_BKUP_CONFIG = nrf.SPI_Read(nrf.CONFIG);

        /*write nrf reg*/
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.EN_AA, 0x00); // switch off Shockburst mode
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.RF_SETUP, 0x0F);// write default value to setup register

        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.STATUS, 0x70);        // reset the flag(s)
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.CONFIG, 0x0F);        // switch to RX mode
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.STATUS, nrf.RX_DR | nrf.TX_DS | nrf.MAX_RT);    //reset the flag(s)
        nrf.RX_Mode();

        delayMicroseconds(130);
        nrf.SetRF_Enable(true);
    }
    else
    {
        nrf.SetRF_Enable(false);
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.EN_AA, NRF_BKUP_EN_AA);
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.RF_SETUP, NRF_BKUP_RF_SETUP);
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.RF_CH, NRF_BKUP_RF_CH);
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.CONFIG, NRF_BKUP_CONFIG);
        nrf.SetRF_Enable(true);
    }
}

/*控件*/
static lv_obj_t * appWindow;
static lv_obj_t * chartSpectrum;
static lv_chart_series_t * serSpectrum;

static void ChartSpectrum_AnimEvent(lv_anim_t * a)
{
    IsStartScan = true;
}

static void Creat_ChartSpectrum()
{
    chartSpectrum = lv_chart_create(appWindow, NULL);
    lv_obj_set_size(chartSpectrum, APP_WIN_WIDTH - 60, 120);
    lv_obj_align(chartSpectrum, NULL, LV_ALIGN_IN_TOP_MID, 0, 30);
    lv_obj_set_opa_scale_enable(chartSpectrum, true);
    lv_obj_set_opa_scale(chartSpectrum, LV_OPA_80);
    lv_chart_set_type(chartSpectrum, LV_CHART_TYPE_COLUMN);
    lv_chart_set_series_opa(chartSpectrum, LV_OPA_COVER);
    lv_chart_set_series_width(chartSpectrum, 10);
    lv_chart_set_series_darking(chartSpectrum, LV_OPA_50);
    lv_chart_set_update_mode(chartSpectrum, LV_CHART_UPDATE_MODE_CIRCULAR);

    lv_chart_set_range(chartSpectrum, 0, 50);
    lv_chart_set_point_count(chartSpectrum, NRF_CHANNELS);
    lv_chart_set_div_line_count(chartSpectrum, 2, 4);
    lv_chart_set_margin(chartSpectrum, 100);

    //lv_chart_set_y_tick_length(chartSpectrum, LV_CHART_TICK_LENGTH_AUTO, LV_CHART_TICK_LENGTH_AUTO);
    //lv_chart_set_y_tick_texts(chartSpectrum, "S\n" "3\n" "2\n" "1\n" "0", 2, LV_CHART_AXIS_SKIP_LAST_TICK);
    lv_chart_set_x_tick_length(chartSpectrum, LV_CHART_TICK_LENGTH_AUTO, LV_CHART_TICK_LENGTH_AUTO);
    lv_chart_set_x_tick_texts(chartSpectrum, "2.4\n" "2.425\n" "2.45\n" "2.475\n" "2.5\n" "[GHz]", 2, LV_CHART_AXIS_DRAW_LAST_TICK);

    serSpectrum = lv_chart_add_series(chartSpectrum, lv_style_plain_color.body.main_color);

    static lv_anim_t a;
    lv_obj_add_anim(
        chartSpectrum, &a,
        (lv_anim_exec_xcb_t)lv_obj_set_height,
        lv_obj_get_height(chartSpectrum) / 10, lv_obj_get_height(chartSpectrum),
        200,
        ChartSpectrum_AnimEvent
    );
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    NRF_Prepare(true);
    //lv_obj_set_color(appWindow, LV_COLOR_BLACK);
    /*将此页面移到前台*/
    lv_obj_move_foreground(appWindow);
    Creat_ChartSpectrum();
}

static void Loop()
{
    if(!IsStartScan)
        return;

    randomSeed(RNG_GetRandNum());
    for (int MHz = 0; MHz < NRF_CHANNELS; MHz++ )   // tune to frequency (2400 + MHz) so this loop covers 2.400 - 2.525 GHz (maximum range module can handle) when channels is set to 128.
    {
        nrf.SetFreqency(MHz);
        nrf.SetRF_Enable(true); // start receiving
        delayMicroseconds(random(130, 230)); // allow receiver time to tune and start receiving 130 uS seems to be the minimum time.  Random additional delay helps prevent strobing effects with frequency-hopping transmitters.
        nrf.SetRF_Enable(false); // stop receiving - one bit is now set if received power was > -64 dBm at that instant
        if (nrf.SPI_Read(nrf.CD))   // signal detected so increase signalStrength unless already maxed out
        {
            SignalStrength[MHz] += (0x7FFF - SignalStrength[MHz]) >> 5; // increase rapidly when previous value was low, with increase reducing exponentially as value approaches maximum
        }
        else     // no signal detected so reduce signalStrength unless already at minimum
        {
            SignalStrength[MHz] -= SignalStrength[MHz] >> 5; // decrease rapidly when previous value was high, with decrease reducing exponentially as value approaches zero
        }

        float strength = ((SignalStrength[MHz] + 0x0040) >> 7) * RecvSensitivity;
        serSpectrum->points[MHz] = strength;
    }
    lv_chart_refresh(chartSpectrum);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    lv_obj_clean(appWindow);
    NRF_Prepare(false);
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
        if(btn == btnBack)
        {
            page.PagePop();
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_RadioCfg(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    lv_style_t * style = (lv_style_t *)lv_cont_get_style(appWindow, LV_CONT_STYLE_MAIN);
    *style = lv_style_pretty;
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
