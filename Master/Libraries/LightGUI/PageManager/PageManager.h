
/**********      ����״̬�����첽ҳ�������      **********/
/**********        Designed By _VIFEXTech        **********/
/**
  * @Finished: 2018.10.6  - v1.0 ӵ�м򵥵�ҳ���л������¼����ݵ���
  * @Upgrade:  2019.4.1   - v1.1 ����ע�ͣ���д�¼����ݵ��ȣ������ظ�����
  * @Upgrade:  2019.10.5  - v1.2 ʹ��ջ�ṹ����ҳ��Ƕ�ף�Ĭ��ջ���10��(�ɵ�)
  * @Upgrade:  2019.12.4  - v1.3 �޸���Setup��Exit���������ҳ���л���BUG���������
  */

#ifndef __PAGEMANAGER_H
#define __PAGEMANAGER_H

#include "stdint.h"

class PageManager {
    typedef void(*CallbackFunction_t)(void);
    typedef void(*EventFunction_t)(int,void*);
    typedef struct {
        CallbackFunction_t SetupCallback;
        CallbackFunction_t LoopCallback;
        CallbackFunction_t ExitCallback;
        EventFunction_t EventCallback;
    } PageList_TypeDef;
public:
    PageManager(uint8_t pageMax, uint8_t pageStackSize = 10);
    ~PageManager();
    uint8_t NowPage, LastPage, NextPage;
    uint8_t NewPage, OldPage;

    bool PageRegister(
        uint8_t pageID,
        CallbackFunction_t setupCallback,
        CallbackFunction_t loopCallback,
        CallbackFunction_t exitCallback,
        EventFunction_t eventCallback
    );
    bool PageClear(uint8_t pageID);
    bool PagePush(uint8_t pageID);
    bool PagePop();
    void PageChangeTo(uint8_t pageID);
    void PageEventTransmit(int event, void* param);
    void PageStackClear();
    void Running();
private:
    PageList_TypeDef* PageList;
    uint8_t *PageStack;
    uint8_t PageStackSize;
    uint8_t PageStackTop;
    uint8_t MaxPage;
    bool IsPageBusy;
};

#endif
