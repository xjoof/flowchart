#ifndef XMLFLOCHATRANSFER_H
#define XMLFLOCHATRANSFER_H

#include <QObject>
#include <QDomDocument>
#include <QString>

#include <QHash>
#include <QMap>
#include <QMessageBox>
#include <QBuffer>
#include <iostream>
#include <map>


#include "flowchartsitems.h"
#include "vardisplaymanage.h"
#include "flowchartmainwin.h"
#include "globalobjmanager.h"

#define XmlRootNodeName QString("Flowchart")


#define XmlNodeTypeVarPool QString("VarPool")


#define XmlNodeTypeDisplayUnits QString("VarDisplayUnits")
#define XmlNodeTypeUnit QString("Unit")
#define XmlNodeTypeVarName QString("VarName")

#define XmlNodeTypeTextMap QString("TextMap")
#define XmlNodeTypePair QString("Pair")
#define XmlNodeTypeKey QString("Key")
#define XmlNodeTypeText QString("Text")

#define XmlNodeTypeIconMap QString("IconMap")

#define XmlNodeTypeUnitSet QString("UnitSetting")

//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------

#define XmlNodeTypeSet QString("Set")
#define XmlNodeTypeIf QString("If")
#define XmlNodeTypeRun QString("Run")
#define XmlNodeTypeSleep QString("Sleep")
#define XmlNodeTypeAssem QString("Assem")







//general tag
#define XmlEleTagName QString("Name")
#define XmlEleTagNext QString("Next")
#define XmlEleTagPos QString("Pos")

#define XmlEleTagValueEnd "virtualendflocha"
#define XmlEleTagValueStart "virtualstartflocha"
//general tag



//assem tag
#define XmlEleTagStart QString("Start")
#define XmlEleTagSub QString("Sub")

#define XmlEleTagStartPos QString("StartPos")
#define XmlEleTagEndPos QString("EndPos")


#define XmlEleTagLoopRunTimes QString("LoopRunTimes")
#define XmlEleTagSleepPerLoop QString("SleepPerLoop")
#define XmlEleTagLoopRunCntVar QString("LoopRunCntStr")

//assem tag


//if tag
#define XmlEleTagElse QString("Else")
#define XmlEleTagOperator QString("Operator")
#define XmlEleTagStamen1 QString("Statement1")
#define XmlEleTagStamen2 QString("Statement2")
//if tag



//set tag
#define XmlEleTagVarName QString("VarName")
#define XmlEleTagStamen QString("Statement")
//set tag


//run tag
#define XmlEleTagPDLName QString("PDLName")
//run tag

//sleep tag
#define XmlEleTagSleep QString("Sleep")
//sleep tag

class GlobalObjManager;

class FlowchartMainWin;

extern GlobalObjManager *globalObjManager;





class XmlFloTransBase : public QObject
{
    Q_OBJECT

public:
    XmlFloTransBase(FlowchartMainWin *parent, CommandManager *cmd, DisplayUnitsPlane *unitplane = nullptr);
    XmlFloTransBase();
    ~XmlFloTransBase();


    int updNodeAfterFloChange(FlowchartsTemplate *flocha);
    void removeNode(FlowchartsTemplate *flocha);


    //int saveFlochaToXml(QIODevice *device);


protected:

    int buildFlochaLoop(QDomNode &node, FlowchartsAssemble *parent = nullptr);
    FlowchartsTemplate *creatFlocha(QDomNode &node, FlowchartsAssemble *parent = nullptr);
    int linkFlocha();
    QDomNode findSiblingName(QDomNode &node,QString &name);

    QDomNode createNodeData(QDomNode &parentSub,int type);

    //std::list<QDomNode> createListNode(std::list<QString> &taglist);
    void setEleNodeText(QDomElement &elenode, const QString &text);


    QMap<FlowchartsTemplate *, QDomNode> flochaNodeHash;//not include start and end



    QDomDocument domDocument;
    //	QMap<QDomNode, FlowchartsTemplate *> nodeFlochaHash;
    //FlowchartMainWin *flochaMainWin;
    QObject *flochaParent;
    FlowchartsAssemble *buildedFlocha;
    CommandManager *commandManager;

};


////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////



class XmlFlochaTransfer : public XmlFloTransBase
{
    Q_OBJECT

public:
    XmlFlochaTransfer(FlowchartMainWin *parent, CommandManager *cmd, DisplayUnitsPlane *unitplane = nullptr);

    ~XmlFlochaTransfer();


    FlowchartsAssemble * createNewFlocha(QObject *parent);
    FlowchartsAssemble * loadXmlToFlocha(QIODevice *device, QObject *parent);


    //int updNodeAfterFloChange(FlowchartsTemplate *flocha);
    //void removeNode(FlowchartsTemplate *flocha);


    int saveFlochaToXml(QIODevice *device);

    //-----------------------LocalVarPool---------------------------
    QString getVarNameByIdx(unsigned int indx);
    int updDomChangedVar(unsigned int *indx, QString &varname, const ArrayDimensInfo &info);
    int updDomRemovedVar(unsigned int indx);

    std::map<unsigned int, std::pair<QString, ArrayDimensInfo> > getVarPoolToInitTable();
    //-----------------------LocalVarPool---------------------------



private:

    //int buildFlochaLoop(QDomNode &node, FlowchartsAssemble *parent = nullptr);
    //FlowchartsTemplate *creatFlocha(QDomNode &node, FlowchartsAssemble *parent = nullptr);
    //int linkFlocha();
    //QDomNode findSiblingName(QDomNode &node,QString &name);

    //QDomNode createNodeData(QDomNode &parentSub,int type);

    //std::list<QDomNode> createListNode(std::list<QString> &taglist);
    //void setEleNodeText(QDomElement &elenode, const QString &text);




    QMap<FlowchartsTemplate *, QDomNode> flochaNodeHash;//not include start and end



    //-----------------------LocalVarPool---------------------------
    QDomNode varPoolParentNode;
    int buildLocalVarPool(QDomNode &node);


    void updNodeForSave();

    std::map<unsigned int, std::pair<QString,QDomNode> > indexVarNameNodeMap;

    //unsigned int VariableIndex;
    //-----------------------LocalVarPool---------------------------



    //-----------------display Units Plane-------
    DisplayUnitsPlane *displayUnitsPlane;

    void transDisplayUnitToNode();

    QDomNode unitsPlaneParentNode;
    int buildDisplayUnitsPlane(QDomNode &node);


    //-----------------display Units Plane-------



    //QDomDocument domDocument;
    //	QMap<QDomNode, FlowchartsTemplate *> nodeFlochaHash;
    FlowchartMainWin *flochaMainWin;
    //QObject *flochaParent;
    //FlowchartsAssemble *buildedFlocha;
    //CommandManager *commandManager;


};



//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------



class XmlToFlochaItem : public XmlFloTransBase
{
    Q_OBJECT

public:
    XmlToFlochaItem(QObject *parent);
    ~XmlToFlochaItem();

    void addFlochaItem(QDomElement &node);

    void getFlochaItem(std::list<FlowchartsAssemble*> &itemlist);


private:


    std::list<FlowchartsAssemble*> flochaItemList;


};



#endif // XMLFLOCHATRANSFER_H
