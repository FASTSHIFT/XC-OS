#include "PageManager.h"

#define IS_PAGE(page)   (page<MaxPage)

/**
  * @brief  ��ʼ��ҳ�������
  * @param  pageMax: ҳ���������
  * @param  eventMax: �¼��������
  * @retval ��
  */
PageManager::PageManager(uint8_t pageMax, uint8_t pageStackSize)
{
    MaxPage = pageMax;
    NewPage = 0;
    OldPage = 0;
    IsPageBusy = false;

    /* �����ڴ棬����б� */
    PageList = new PageList_TypeDef[MaxPage];
    for(uint8_t page = 0; page < MaxPage; page++)
    {
        PageClear(page);
    }
    
    /*ҳ��ջ����*/
    PageStackSize = pageStackSize;
    PageStack = new uint8_t[PageStackSize];
    PageStackClear();
}

/**
  * @brief  ���һ��ҳ��
  * @param  pageID: ҳ����
  * @retval true:�ɹ� false:ʧ��
  */
bool PageManager::PageClear(uint8_t pageID)
{
    if(!IS_PAGE(pageID))
        return false;

    PageList[pageID].SetupCallback = 0;
    PageList[pageID].LoopCallback = 0;
    PageList[pageID].ExitCallback = 0;
    PageList[pageID].EventCallback = 0;

    return true;
}

/**
  * @brief  ע��һ������ҳ�棬����һ����ʼ��������ѭ���������˳��������¼�����
  * @param  pageID: ҳ����
  * @param  setupCallback: ��ʼ�������ص�
  * @param  loopCallback: ѭ�������ص�
  * @param  exitCallback: �˳������ص�
  * @param  eventCallback: �¼������ص�
  * @retval true:�ɹ� false:ʧ��
  */
bool PageManager::PageRegister(uint8_t pageID,
                                     CallbackFunction_t setupCallback,
                                     CallbackFunction_t loopCallback,
                                     CallbackFunction_t exitCallback,
                                     EventFunction_t eventCallback)
{
    if(!IS_PAGE(pageID))
        return false;

    PageList[pageID].SetupCallback = setupCallback;
    PageList[pageID].LoopCallback = loopCallback;
    PageList[pageID].ExitCallback = exitCallback;
    PageList[pageID].EventCallback = eventCallback;
    return true;
}

/**
  * @brief  ҳ���¼�����
  * @param  eventID: �¼����
  * @param  param: �¼�����
  * @retval ��
  */
void PageManager::PageEventTransmit(int event, void* param)
{
    /*���¼����ݵ���ǰҳ��*/
    if(PageList[NowPage].EventCallback)
        PageList[NowPage].EventCallback(event, param);
}

/**
  * @brief  �л���ָ��ҳ��
  * @param  pageID: ҳ����
  * @retval ��
  */
void PageManager::PageChangeTo(uint8_t pageID)
{
    if(!IS_PAGE(pageID))
        return;
    
    /*���ҳ���Ƿ�æµ*/
    if(!IsPageBusy)
    {
        /*��ҳ��ID*/
        NextPage = NewPage = pageID;

        /*���Ϊæµ״̬*/
        IsPageBusy = true;
    }
}

/**
  * @brief  ҳ��ѹջ����ת����ҳ��
  * @param  pageID: ҳ����
  * @retval true:�ɹ� false:ʧ��
  */
bool PageManager::PagePush(uint8_t pageID)
{
    if(!IS_PAGE(pageID))
        return false;
    
    /*���ҳ���Ƿ�æµ*/
    if(IsPageBusy)
       return false; 
    
    /*��ֹջ���*/
    if(PageStackTop >= PageStackSize - 1)
        return false;
    
    /*��ֹ�ظ�ҳ��ѹջ*/
    if(pageID == PageStack[PageStackTop])
        return false;

    /*ջ��ָ������*/
    PageStackTop++;
    
    /*ҳ��ѹջ*/
    PageStack[PageStackTop] = pageID;
    
    /*ҳ����ת*/
    PageChangeTo(PageStack[PageStackTop]);
    
    return true;
}

/**
  * @brief  ҳ�浯ջ����ת����һ��ҳ��
  * @param  ��
  * @retval true:�ɹ� false:ʧ��
  */
bool PageManager::PagePop()
{
    /*���ҳ���Ƿ�æµ*/
    if(IsPageBusy)
       return false; 
    
    /*��ֹջ���*/
    if(PageStackTop == 0)
        return false;
    
    /*��յ�ǰҳ��*/
    PageStack[PageStackTop] = 0;
    
    /*ջ��ָ������*/
    PageStackTop--;
    
    /*ҳ�浯ջ����ת*/
    PageChangeTo(PageStack[PageStackTop]);
    
    return true;
}

/**
  * @brief  ���ҳ��ջ
  * @param  ��
  * @retval ��
  */
void PageManager::PageStackClear()
{
    /*���ҳ���Ƿ�æµ*/
    if(IsPageBusy)
       return; 
    
    for(uint8_t i = 0; i < PageStackSize; i++)
    {
        PageStack[i] = 0;
    }
    PageStackTop = 0;
}

/**
  * @brief  ҳ�������״̬��
  * @param  ��
  * @retval ��
  */
void PageManager::Running()
{
    /*ҳ���л��¼�*/
    if(NewPage != OldPage)
    {
        /*���Ϊæµ״̬*/
        IsPageBusy = true;

        /*ִ�о�ҳ����˳�����*/
        if(PageList[OldPage].ExitCallback && IS_PAGE(OldPage))
            PageList[OldPage].ExitCallback();
        
        /*��Ǿ�ҳ��*/
        LastPage = OldPage;
        
        /*�����ҳ��Ϊ��ǰҳ��*/
        NowPage = NewPage;

        /*ִ����ҳ���ʼ������*/
        if(PageList[NewPage].SetupCallback && IS_PAGE(NewPage))
            PageList[NewPage].SetupCallback();

        /*��ҳ���ʼ����ɣ����Ϊ��ҳ��*/
        OldPage = NewPage;
    }
    else
    {
        /*ҳ�治æµ*/
        IsPageBusy = false;
        
        /*ҳ��ѭ���¼�*/
        if(PageList[NowPage].LoopCallback && IS_PAGE(NowPage))
            PageList[NowPage].LoopCallback();
    }
}
