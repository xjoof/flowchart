#ifndef GLOBALOBJMANAGER_H
#define GLOBALOBJMANAGER_H

//#include <QThread>
//#include <QTimer>

#include "vardisplaymanage.h"
#include "xmlflochatransfer.h"

class XmlToFlochaItem;

class GlobalObjManager : public QObject
{
    Q_OBJECT
        friend class PhotonXmlManager;
public:

    const DisplayIconList* const getIconList() const;

    VarMonitorManager *getVarMonitorManager();

    void getItemBoxItem(std::list<FlowchartsAssemble*> &itemlist);

    //QThread *varMonitorThread;
private:
    GlobalObjManager();
    ~GlobalObjManager();
    DisplayIconList *iconList;
    VarMonitorManager *varMonitorManager;

    XmlToFlochaItem *itemFlochaItem;

};



#endif // GLOBALOBJMANAGER_H
