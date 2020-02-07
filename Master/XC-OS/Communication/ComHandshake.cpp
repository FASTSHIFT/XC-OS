#include "Basic/FileGroup.h"
#include "ComPrivate.h"

Protocol_Handshake_t HandshakePack_Master;//主机握手数据包
Protocol_Handshake_t HandshakePack_Slave[SlaveNum_MAX] = {0};//从机握手列表

/**
  * @brief  检查列表内是否有重复ID
  * @param  *hs: 列表指针
  * @param  id: 被检查的ID号
  * @retval true: 有重复ID, false: 无重复ID
  */
static bool CheckNewID(Protocol_Handshake_t *hs, uint8_t id)
{
    for(uint8_t i = 0; i < SlaveNum_MAX; i++)//遍历列表
    {
        //Serial.printf("Check List[%d] ", i);
        if(id == hs[i].ID)//查找是否有相同ID号
        {
            //Serial.printf(" 0x%x is old ID\r\n", id);
            return false;
        }
    }
    //Serial.printf(" 0x%x is new ID\r\n", id);
    return true;
}

/**
  * @brief  生成随机地址和随机跳频表
  * @param  *hs: 列表指针
  * @param  randSeed: 随机种子
  * @retval 无
  */
static void GenerateRandomConfig(Protocol_Handshake_t *hs, uint32_t randSeed)
{
    randomSeed(randSeed);//选择随机数种子
    for(uint8_t i = 0; i < 5; i++)
    {
        hs->Address[i] = random(0, 255);//生成随机地址
    }
    for(uint8_t i = 0; i < sizeof(hs->FerqList); i++)
    {
        hs->FerqList[i] = random(0, 126);//生成随机跳频表
    }
}

/**
  * @brief  NRF通信处理(T = 10ms)
  * @param  无
  * @retval 无
  */
static void NRF_ComProcess_Handler()
{
    nrfTRM.TranRecv(NRF_TxBuff, NRF_RxBuff);
}

/**
  * @brief  与从机通信,交换数据
  * @param  *slave:从机握手包指针
  * @retval 无
  */
static void ComToSlave(Protocol_Handshake_t &slave)
{
#ifndef TIM_Handshake
    __IntervalExecute(NRF_ComProcess_Handler(), 10);
#endif
    *(Protocol_Handshake_t*)&NRF_TxBuff = HandshakePack_Master;
    slave = *(Protocol_Handshake_t*)&NRF_RxBuff;
}

/**
  * @brief  握手通信状态机
  * @param  State: 状态机状态
  * @param  SlaveNum: 从机位于列表内的编号
  * @param  Cmd: 命令
  * @retval 详见ret
  */
uint8_t HandshakeRun(uint8_t State, uint8_t SlaveNum, uint8_t Cmd)
{
    static volatile uint8_t SlaveCnt = 0;//从机数量
    uint8_t ret = false;//函数返回值
    Protocol_Handshake_t Slave_temp;//从机数据包缓存

    switch(State)
    {
    case HandshakeState_Prepare://握手准备状态
        SlaveCnt = 0;//从机数量清零
        memset(&NRF_RxBuff, 0, sizeof(NRF_RxBuff));
        memset(&HandshakePack_Slave, 0, sizeof(HandshakePack_Slave));

        nrf.SetRF_Enable(false);
        nrf.SetAddress(BroadcastAddr);//设置广播地址
        nrf.SetFreqency(BroadcastFreq);//设置广播频段
        nrf.SetSpeed(BroadcastSpeed);//设置广播通信速度
        nrf.SetAutoRetryTimeout(10);
        nrf.SetRF_Enable(true);

        HandshakePack_Master.HeadCode = BroadcastHead_MasterWait;//设置帧头识别码为主机等待连接
        HandshakePack_Master.ID = Master_ID;//设置主机ID
        HandshakePack_Master.EnableFunction.Passback = State_PassBack;
        HandshakePack_Master.EnableFunction.FHSS = State_FHSS;
        strcpy(HandshakePack_Master.Description, Master_Description);//设置主机文字描述
        GenerateRandomConfig(&HandshakePack_Master, micros());//生成随机地址和随机跳频表

#ifdef TIM_Handshake
        Timer_SetInterrupt(TIM_Handshake, 10 * 1000, NRF_ComProcess_Handler); //设置10ms定时中断运行NRF_ComProcess_Handler
        TIM_Cmd(TIM_Handshake, ENABLE);//开启定时器
#endif
        ret = true;//返回 成功
        break;

    case HandshakeState_Search://广播搜索状态
        ComToSlave(Slave_temp);//在广播地址交换数据，将搜到的主机信息存至Slave_temp
        if((Slave_temp.HeadCode == BroadcastHead_SlaveWait)//帧头识别码为从机等待连接
                && (CheckNewID(HandshakePack_Slave, Slave_temp.ID) == true)//是否为新ID
                && (SlaveCnt < SlaveNum_MAX))//未超出从机列表大小
        {
            HandshakePack_Slave[SlaveCnt] = Slave_temp;//保存至从机列表
            SlaveCnt ++;//从机个数++
        }
        ret = SlaveCnt;//返回从机个数
        break;

    case HandshakeState_ReqConnect://申请连接状态
        ComToSlave(Slave_temp);//在广播地址交换数据
        HandshakePack_Master.HeadCode = BroadcastHead_MasterAttach;//设置帧头识别码为主机绑定状态
        HandshakePack_Master.ID = HandshakePack_Slave[SlaveNum].ID;//设置主机ID与从机ID一致
        HandshakePack_Master.Commond = Cmd;//向从机发送命令
        HandshakePack_Master.Speed = NRF_Cfg.Speed;//设置通信速率
        if(Slave_temp.HeadCode == BroadcastHead_SlaveAttach)//如果从机的帧头识别码为从机绑定状态
            ret = Slave_temp.Commond;//返回从机命令
        else
            ret = false;//返回 失败
        break;

    case HandshakeState_Connected://连接之后状态
#ifdef TIM_Handshake
        TIM_Cmd(TIM_Handshake, DISABLE);//关定时中断
#endif
        NRF_Cfg.Address = 5;
        memcpy(
            (NRF_AddressConfig + NRF_Cfg.Address * 5),
            HandshakePack_Master.Address,
            sizeof(HandshakePack_Master.Address)
        );//保存地址信息
        NRF_Cfg.Freq = HandshakePack_Master.FerqList[0];//保存频率
        NRF_Cfg.Speed = HandshakePack_Master.Speed;//保存通信速度
//      EEPROM_Handle(EEPROM_Chs::SaveData);//保存所有数据

        nrf.SetRF_Enable(false);
        nrf.SetAddress(HandshakePack_Master.Address);//应用新地址
        nrf.SetFreqency(HandshakePack_Master.FerqList[0]);//应用新频道
        nrf.SetSpeed(HandshakePack_Master.Speed);//应用新通信速率
        nrf.SetAutoRetryTimeout(10);
            
        nrfFHSS.SetFreqHoppingList(HandshakePack_Master.FerqList, FreqList_Length);
            
        nrf.TX_Mode();
        nrf.SetRF_Enable(true);

        ret = true;//返回 成功
        break;
    }
    return ret;
}
