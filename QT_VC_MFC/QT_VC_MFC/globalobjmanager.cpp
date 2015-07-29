#include "globalobjmanager.h"


GlobalObjManager::GlobalObjManager()
{
    iconList = new DisplayIconList();
    varMonitorManager = new VarMonitorManager;

    itemFlochaItem = new XmlToFlochaItem(this);
    //varMonitorThread = new QThread();

    //varMonitorManager->moveToThread(varMonitorThread);
    //connect(varMonitorThread, &QThread::started, varMonitorManager, &VarMonitorManager::startVarMonitorThread);
    //varMonitorThread->start();

}

GlobalObjManager::~GlobalObjManager()
{
#ifdef _DEBUG
    qDebug() << "start ~GlobalObjManager()";
#endif
    delete varMonitorManager;
    varMonitorManager = nullptr;
    delete iconList;
    iconList = nullptr;
#ifdef _DEBUG
    qDebug() << "finish ~GlobalObjManager()";
#endif
}


const DisplayIconList* const GlobalObjManager::getIconList() const
{
    return iconList;
}

VarMonitorManager *GlobalObjManager::getVarMonitorManager()
{
    return varMonitorManager;
}

void GlobalObjManager::getItemBoxItem(std::list<FlowchartsAssemble*> &itemlist)
{
    itemFlochaItem->getFlochaItem(itemlist);
}

GlobalObjManager *globalObjManager = 0;



