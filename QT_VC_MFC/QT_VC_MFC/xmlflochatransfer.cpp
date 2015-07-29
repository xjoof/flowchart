#include "xmlflochatransfer.h"
#include "globalobjmanager.h"

extern GlobalObjManager *globalObjManager;



XmlFloTransBase::XmlFloTransBase(FlowchartMainWin *parent, CommandManager *cmd, DisplayUnitsPlane *unitplane)
    : QObject(parent)
    ,commandManager(cmd)//,
    //flochaMainWin(parent)
{
    buildedFlocha = nullptr;
    if(commandManager == nullptr)
        QMessageBox::warning(nullptr, tr("create XmlFlochaTransfer error"), tr("commandManager is null"));

    //VariableIndex = 0;
}


XmlFloTransBase::XmlFloTransBase()
{

}


XmlFloTransBase::~XmlFloTransBase()
{

}


int XmlFloTransBase::updNodeAfterFloChange(FlowchartsTemplate *flocha)
{
    auto it = flochaNodeHash.find(flocha);

    if(it != flochaNodeHash.end())
    {
        QDomElement tmpnode;


        setEleNodeText(it.value().firstChildElement(XmlEleTagName),QString("%1").arg(flocha->idName));
        if(flocha->getNextPtr() != nullptr)
            setEleNodeText(it.value().firstChildElement(XmlEleTagNext),QString("%1").arg(flocha->getNextPtr()->idName));
        setEleNodeText(it.value().firstChildElement(XmlEleTagPos),QString("%1,%2").arg(flocha->itemPos[0]).arg(flocha->itemPos[1]));

        //std::string test = tmpnode.text().toStdString();


        FlowchartsIf *flochaif = dynamic_cast<FlowchartsIf *>(flocha);
        FlowchartsSetVar* flochaset = dynamic_cast<FlowchartsSetVar*>(flocha);
        FlowchartsSleep *flochasleep = dynamic_cast<FlowchartsSleep*>(flocha);
        FlowchartsRunPDL *flocharun = dynamic_cast<FlowchartsRunPDL*>(flocha);
        FlowchartsAssemble *flochaassem = dynamic_cast<FlowchartsAssemble*>(flocha);

        switch(it.key()->getflochaType())
        {
        case flochaTypeTemplate:

            break;
        case flochaTypeIf:


            if(flochaif->getElsePtr() != nullptr)
                setEleNodeText(it.value().firstChildElement(XmlEleTagElse),QString("%1").arg(flochaif->getElsePtr()->idName));
            setEleNodeText(it.value().firstChildElement(XmlEleTagStamen1),QString("%1").arg(flochaif->getStatement1()));
            setEleNodeText(it.value().firstChildElement(XmlEleTagStamen2),QString("%1").arg(flochaif->getStatement2()));
            setEleNodeText(it.value().firstChildElement(XmlEleTagOperator),QString("%1").arg(flochaif->getOperator()));

            break;
        case flochaTypeSet:

            setEleNodeText(it.value().firstChildElement(XmlEleTagVarName),QString("%1").arg(flochaset->getVarName()));
            setEleNodeText(it.value().firstChildElement(XmlEleTagStamen),QString("%1").arg(flochaset->getStatement()));

            break;
        case flochaTypeRun:


            setEleNodeText(it.value().firstChildElement(XmlEleTagPDLName),QString("%1").arg(flocharun->getPDLName()));

            break;
        case flochaTypeSleep:

            setEleNodeText(it.value().firstChildElement(XmlEleTagSleep),QString("%1").arg(flochasleep->getSleepTime()));
            break;
        case flochaTypeAssem:

            FlochaAssemAttr attr;
            flochaassem->getAssemAttr(&attr);
            setEleNodeText(it.value().firstChildElement(XmlEleTagLoopRunTimes),QString("%1").arg(attr.runLoopTimes));
            setEleNodeText(it.value().firstChildElement(XmlEleTagSleepPerLoop),QString("%1").arg(attr.sleepPerLoop));
            setEleNodeText(it.value().firstChildElement(XmlEleTagLoopRunCntVar),QString("%1").arg(attr.loopRunCntVar));


            break;

        }

    }
    else
    {
        QDomElement tmpnode;
        QString idname = QString("%1").arg(flocha->idName);

        if(idname == QString(XmlEleTagValueStart))
        {
            int test = 0;
            if(flocha->getNextPtr() != nullptr)
                setEleNodeText(flochaNodeHash.find(flocha->getParentPtr()).value().firstChildElement(XmlEleTagStart),QString("%1").arg(flocha->getNextPtr()->idName));
            tmpnode = flochaNodeHash.find(flocha->getParentPtr()).value().firstChildElement(XmlEleTagStartPos);
        }
        else if(idname == QString(XmlEleTagValueEnd))
        {
            tmpnode = flochaNodeHash.find(flocha->getParentPtr()).value().firstChildElement(XmlEleTagEndPos);
        }

        if(!tmpnode.isNull())
            setEleNodeText(tmpnode,QString("%1,%2").arg(flocha->itemPos[0]).arg(flocha->itemPos[1]));





        if((idname != QString(XmlEleTagValueStart)) && (idname != QString(XmlEleTagValueEnd)))
        {
            QDomNode parentsub;
            QDomNode newnode;


            parentsub = flochaNodeHash.find(flocha->getParentPtr()).value().firstChildElement(XmlEleTagSub);

            newnode = createNodeData(parentsub, flocha->getflochaType());

            flochaNodeHash.insert(flocha,newnode);

            updNodeAfterFloChange(flocha);

        }

    }


    if(flocha->getflochaType() == flochaTypeAssem)
    {
        FlowchartsAssemble *flochaassem = dynamic_cast<FlowchartsAssemble*>(flocha);

        for(auto it = flochaassem->getFlochaList().begin(); it != flochaassem->getFlochaList().end(); ++it)
        {
            updNodeAfterFloChange(*it);
        }
    }



    return 0;
}



void XmlFloTransBase::removeNode(FlowchartsTemplate *flocha)
{
    auto it = flochaNodeHash.find(flocha);
    QDomNode node = it.value();
    node.parentNode().removeChild(node);

    flochaNodeHash.erase(it);
}

/*
int XmlFloTransBase::saveFlochaToXml(QIODevice *device)
{

    //updNodeForSave();//temp solution for array var
    //transDisplayUnitToNode();

    const int IndentSize = 4;

    QTextStream out(device);
    domDocument.save(out, IndentSize);

    return 0;
}
*/

int XmlFloTransBase::buildFlochaLoop(QDomNode &node, FlowchartsAssemble *parent)
{
    while(!node.isNull()) {

#ifdef _DEBUG

        QDomNode test = node.firstChildElement(XmlEleTagName);
        qDebug() << node.nodeName() << node.firstChildElement(XmlEleTagName).text();

#endif

        FlowchartsTemplate *flocha = creatFlocha(node,parent);
        //		if(commandManager != nullptr)
        //			flocha->setMpiController(commandManager);

        //nodeItem->setFlags(nodeItem->flags() | Qt::ItemIsEditable); //為什麼加了這一行，drop的node的nodeValue會被清除
        if(node.nodeName() == XmlNodeTypeAssem) {

            QDomNode Child = node.firstChildElement(XmlEleTagSub).firstChild();
            FlowchartsAssemble *assem = dynamic_cast<FlowchartsAssemble *>(flocha);
            if(assem != 0)
                buildFlochaLoop(Child,assem);
        }
        node = node.nextSibling();


        if(/*(node.nodeName() != XmlNodeTypeAssem) && */(parent == nullptr))
            break;

    }


    return 0;
}


FlowchartsTemplate *XmlFloTransBase::creatFlocha(QDomNode &node, FlowchartsAssemble *parent)
{

#ifdef _DEBUG
    qDebug() << "start creatFlocha";
#endif




    FlowchartsTemplate *flocha;

    if((parent==nullptr) && (node.nodeName() != XmlNodeTypeAssem))
    {
        QMessageBox::warning(NULL,tr("error"),tr("parent==nullptr & nodeName != XmlNodeTypeAssem"));
        return nullptr;
    }

    if(node.nodeName() == XmlNodeTypeSet)
    {
        FlowchartsSetVar *tmpflo = new FlowchartsSetVar(parent,commandManager);
        QString tmpstr = node.firstChildElement(XmlEleTagVarName).text();
        tmpflo->setVarName(tmpstr);
        tmpstr = node.firstChildElement(XmlEleTagStamen).text();
        tmpflo->setStatement(tmpstr);
        flocha = dynamic_cast<FlowchartsTemplate*>(tmpflo);
    }
    else if(node.nodeName() == XmlNodeTypeIf)
    {
        FlowchartsIf *tmpflo = new FlowchartsIf(parent,commandManager);

        QString tmpstr;
        QDomElement tmpnode = node.firstChildElement(XmlEleTagStamen1);
        if(tmpnode.isNull())
            return nullptr;

        tmpstr = tmpnode.text();
        tmpflo->setStatement1(tmpstr);


        tmpnode = node.firstChildElement(XmlEleTagStamen2);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();
        tmpflo->setStatement2(tmpstr);


        tmpnode = node.firstChildElement(XmlEleTagOperator);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();
        tmpflo->setOperator(tmpstr.toInt());


        flocha = dynamic_cast<FlowchartsTemplate*>(tmpflo);
    }
    else if(node.nodeName() == XmlNodeTypeRun)
    {
        FlowchartsRunPDL *tmpflo = new FlowchartsRunPDL(parent, commandManager);
        QString tmpstr;
        QDomElement tmpnode = node.firstChildElement(XmlEleTagPDLName);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();
        tmpflo->setPDLName(tmpstr);
        flocha = dynamic_cast<FlowchartsTemplate*>(tmpflo);
    }
    else if(node.nodeName() == XmlNodeTypeSleep)
    {
        FlowchartsSleep *tmpflo = new FlowchartsSleep(parent);
        QString tmpstr;
        QDomElement tmpnode = node.firstChildElement(XmlEleTagSleep);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();

        tmpflo->setSleepTime(tmpstr);
        flocha = dynamic_cast<FlowchartsTemplate*>(tmpflo);
    }
    else if(node.nodeName() == XmlNodeTypeAssem)
    {
        FlowchartsAssemble *tmpflo;
        FlochaAssemAttr attr;
        QString tmpstr;
        QDomElement tmpnode;
        if(parent == nullptr)
        {
            buildedFlocha = new FlowchartsAssemble(flochaParent, commandManager);
            tmpflo = buildedFlocha;
        }
        else
        {
            tmpflo = new FlowchartsAssemble(parent, commandManager);
        }
        tmpnode = node.firstChildElement(XmlEleTagStartPos);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();

        QStringList poslist = tmpstr.split(",");
        if(poslist.size() > 1)
        {
            tmpflo->getStartFlocha()->itemPos[0] = poslist.at(0).toInt();
            tmpflo->getStartFlocha()->itemPos[1] = poslist.at(1).toInt();
        }


        tmpnode = node.firstChildElement(XmlEleTagEndPos);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();
        poslist = tmpstr.split(",");
        if(poslist.size() > 1)
        {
            tmpflo->getEndFlocha()->itemPos[0] = poslist.at(0).toInt();
            tmpflo->getEndFlocha()->itemPos[1] = poslist.at(1).toInt();
        }

        tmpflo->getAssemAttr(&attr);


        tmpnode = node.firstChildElement(XmlEleTagLoopRunTimes);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();
        attr.runLoopTimes = tmpstr.toUInt();
        //tmpflo->setLoopRunTimes(tmpstr.toUInt());



        tmpnode = node.firstChildElement(XmlEleTagSleepPerLoop);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();
        attr.sleepPerLoop = tmpstr.toUInt();
        //tmpflo->setSleepTimePerLoop(tmpstr.toUInt());



        tmpnode = node.firstChildElement(XmlEleTagLoopRunCntVar);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();
        attr.loopRunCntVar = tmpstr;
        //tmpflo->setLoopRunCntStr(tmpstr);


        tmpflo->setAssemAttr(attr);
        

        flocha = dynamic_cast<FlowchartsTemplate*>(tmpflo);

    }



    if(parent != nullptr)
        parent->addTemplate(flocha);
    QString idname = node.firstChildElement(XmlEleTagName).text();
    memset(flocha->idName,0, sizeof(flocha->idName));
    memcpy(flocha->idName,idname.toStdString().c_str(), sizeof(flocha->idName));

    QString posstr = node.firstChildElement(XmlEleTagPos).text();
    QStringList poslist = posstr.split(",");
    if(poslist.size() > 1)
    {
        flocha->itemPos[0] = poslist.at(0).toInt();
        flocha->itemPos[1] = poslist.at(1).toInt();
    }
    //flocha->idName = node.firstChildElement(XmlEleTagName).nodeValue().toStdString().data();


    flochaNodeHash.insert(flocha, node);
    //	nodeFlochaHash.insert(node, flocha);

    return flocha;
}



int XmlFloTransBase::linkFlocha()
{

    for(auto it = flochaNodeHash.begin(); it != flochaNodeHash.end(); ++it)
    {
        if(it.value().nodeName() == XmlNodeTypeAssem)
        {
            QString startstr = it.value().firstChildElement(XmlEleTagStart).text();
            QDomNode startdom = findSiblingName(it.value().firstChildElement(XmlEleTagSub).firstChild(),startstr);
            if(!startdom.isNull())
            {
                FlowchartsTemplate *startflo = flochaNodeHash.key(startdom);
                FlowchartsAssemble *parentflo = dynamic_cast<FlowchartsAssemble*>(flochaNodeHash.key(startdom.parentNode().parentNode()));


                if(parentflo != nullptr)
                    parentflo->linkTwoTemplate(parentflo->getStartFlocha(),startflo);
                //parentflo->getStartFlocha()->p_next = startflo;
                else
                    QMessageBox::warning(NULL,tr("error"),tr("parentflo == nullptr"));
                //parentflo
            }
        }

        if(it.value().nodeName() == XmlNodeTypeIf)
        {
            QString elsestr = it.value().firstChildElement(XmlEleTagElse).text();
            QDomNode elsedom = findSiblingName(it.value(),elsestr);
            if(!elsedom.isNull())
            {
                FlowchartsTemplate *elseflo = flochaNodeHash.key(elsedom);
                FlowchartsAssemble *parentflo = dynamic_cast<FlowchartsAssemble*>(flochaNodeHash.key(elsedom.parentNode().parentNode()));



                if(parentflo != nullptr)
                    parentflo->linkTwoTemplate(it.key(),elseflo,true);
                else
                    QMessageBox::warning(NULL,tr("error"),tr("parentflo = nullptr"));

            }
            else if(elsestr == QString(XmlEleTagValueEnd))
            {	
                if(it.key()->getParentPtr() != nullptr)
                    it.key()->getParentPtr()->linkTwoTemplate(it.key(),it.key()->getParentPtr()->getEndFlocha(),true);
            }
        }

        QString nextstr = it.value().firstChildElement(XmlEleTagNext).text();

        //judge every time. maybe not good
        if(nextstr == QString(XmlEleTagValueEnd))
        {	
            if(it.key()->getParentPtr() != nullptr)
                it.key()->getParentPtr()->linkTwoTemplate(it.key(),it.key()->getParentPtr()->getEndFlocha());
        }
        else
        {

            QDomNode nextdom = findSiblingName(it.value(),nextstr);

            if(!nextdom.isNull())
            {
                FlowchartsTemplate *nextflo = flochaNodeHash.key(nextdom);
                FlowchartsAssemble *parentflo = dynamic_cast<FlowchartsAssemble*>(flochaNodeHash.key(nextdom.parentNode().parentNode()));


                if(parentflo != nullptr)
                    parentflo->linkTwoTemplate(it.key(),nextflo);
                else
                    QMessageBox::warning(NULL,tr("error"),tr("parentflo = nullptr"));

            }
        }
    }


    return 0;
}



QDomNode XmlFloTransBase::findSiblingName(QDomNode &node,QString &name)
{
    if(name.isEmpty())
        return QDomNode();

    QDomNode parent = node.parentNode();
    QDomNode itnode = parent.firstChild();

    while(!itnode.isNull())
    {
        if(itnode.firstChildElement(XmlEleTagName).text() == name)
            return itnode;
        itnode = itnode.nextSibling();
    }

    return itnode;
}




QDomNode XmlFloTransBase::createNodeData(QDomNode &parentSub,int type)
{
    QDomNode newnode;
    QString tagname;
    std::list<QString> taglist;
    taglist.clear();


    taglist.push_back(XmlEleTagName);
    taglist.push_back(XmlEleTagNext);
    taglist.push_back(XmlEleTagPos);


    switch(type)
    {

    case flochaTypeTemplate:

        break;
    case flochaTypeIf:
        tagname = XmlNodeTypeIf;
        taglist.push_back(XmlEleTagElse);
        taglist.push_back(XmlEleTagOperator);
        taglist.push_back(XmlEleTagStamen1);
        taglist.push_back(XmlEleTagStamen2);
        break;
    case flochaTypeSet:
        tagname = XmlNodeTypeSet;

        taglist.push_back(XmlEleTagVarName);
        taglist.push_back(XmlEleTagStamen);


        break;
    case flochaTypeRun:
        tagname = XmlNodeTypeRun;

        taglist.push_back(XmlEleTagPDLName);

        break;
    case flochaTypeSleep:
        tagname = XmlNodeTypeSleep;

        taglist.push_back(XmlEleTagSleep);


        break;
    case flochaTypeAssem:
        tagname = XmlNodeTypeAssem;

        taglist.push_back(XmlEleTagStart);
        taglist.push_back(XmlEleTagSub);
        taglist.push_back(XmlEleTagStartPos);
        taglist.push_back(XmlEleTagEndPos);

        taglist.push_back(XmlEleTagLoopRunTimes);
        taglist.push_back(XmlEleTagSleepPerLoop);
        taglist.push_back(XmlEleTagLoopRunCntVar);

        break;
    }

    newnode = domDocument.createElement(tagname);
    parentSub.appendChild(newnode);

    for(auto it = taglist.begin(); it != taglist.end(); ++it)
    {
        newnode.appendChild(domDocument.createElement(*it));
    }

    return newnode;
}



void XmlFloTransBase::setEleNodeText(QDomElement &elenode, const QString &text)
{
    if(elenode.firstChild().isNull() || !elenode.firstChild().isText())
    {
        QDomText textnode = domDocument.createTextNode(text);
        elenode.insertBefore(textnode,elenode.firstChild());
    }
    else
    {
        elenode.firstChild().setNodeValue(text);
    }


}





////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////





XmlFlochaTransfer::XmlFlochaTransfer(FlowchartMainWin *parent, CommandManager *cmd, DisplayUnitsPlane *unitplane)
    //: XmlFloTransBase(parent),
    : displayUnitsPlane(unitplane),
    flochaMainWin(parent)
{
    commandManager = cmd;

    buildedFlocha = nullptr;
    if(commandManager == nullptr)
        QMessageBox::warning(nullptr, tr("create XmlFlochaTransfer error"), tr("commandManager is null"));

    //VariableIndex = 0;
}

XmlFlochaTransfer::~XmlFlochaTransfer()
{

}



FlowchartsAssemble * XmlFlochaTransfer::createNewFlocha(QObject *parent)
{
    //QByteArray byteArray;
    //	QFile empyfile("./tempfiletttt.xml");
    //	empyfile.open(QFile::ReadWrite|QFile::Text);
    //	const int IndentSize = 4;
    QDomDocument empydomdoc(XmlRootNodeName);
    QDomElement root = empydomdoc.createElement(XmlRootNodeName);
    empydomdoc.appendChild(root);
    //    QTextStream out(&empyfile);

    QDomElement newelement;


    //newelement = empydomdoc.createElement(XmlNodeTypeAssem);
    //root.appendChild(newelement);

    //QDomNode assemnode = 
    createNodeData(root,flochaTypeAssem);


    newelement = empydomdoc.createElement(XmlNodeTypeVarPool);
    root.appendChild(newelement);

    newelement = empydomdoc.createElement(XmlNodeTypeDisplayUnits);
    root.appendChild(newelement);


    //    empydomdoc.save(out, IndentSize);
    //qDebug() << byteArray.toStdString().c_str();
    //	QString test = out.readAll();
    //	qDebug() << test;
    //qDebug() << empydomdoc.toString();
    QByteArray byteArray = empydomdoc.toByteArray();
    QBuffer empyfile(&byteArray);
    return loadXmlToFlocha(&empyfile, parent);
}


FlowchartsAssemble * XmlFlochaTransfer::loadXmlToFlocha(QIODevice *device, QObject *parent)
{

    domDocument.clear();

    //textAndCommentItem.clear();
    QString errorStr;
    int errorLine;
    int errorColumn;


    if (!domDocument.setContent(device, true, &errorStr, &errorLine, &errorColumn)) 
    {

        QMessageBox::information(nullptr, tr("DOM Bookmarks"),
            tr("Parse error at line %1, column %2:\n%3")
            .arg(errorLine)
            .arg(errorColumn)
            .arg(errorStr));

        return nullptr;
    }

    QDomElement root = domDocument.documentElement();

    //    timeMeasure.restart();

    //    curLevel = -1;
    flochaParent = parent;


    varPoolParentNode = root.firstChildElement(XmlNodeTypeVarPool);
    buildLocalVarPool(varPoolParentNode);//should call before buildFlochaLoop() because array var


    buildFlochaLoop(root.firstChildElement(XmlNodeTypeAssem));//.firstChildElement(XmlNodeTypeAssem));
    linkFlocha();

    

    //	auto testlist = buildedFlocha->getFlochaList();
    //	for(auto it = buildedFlocha->getFlochaList().begin(); it != buildedFlocha->getFlochaList().end(); ++it)
    //	{
    //		qDebug() << (*it)->idName ;
    //	}
    if(displayUnitsPlane)
    {
        unitsPlaneParentNode = root.firstChildElement(XmlNodeTypeDisplayUnits);
        buildDisplayUnitsPlane(unitsPlaneParentNode);
    }


    return buildedFlocha;
}
/*
int XmlFlochaTransfer::buildFlochaLoop(QDomNode &node, FlowchartsAssemble *parent)
{
    while(!node.isNull()) {

#ifdef _DEBUG

        QDomNode test = node.firstChildElement(XmlEleTagName);
        qDebug() << node.nodeName() << node.firstChildElement(XmlEleTagName).text();

#endif

        FlowchartsTemplate *flocha = creatFlocha(node,parent);
        //		if(commandManager != nullptr)
        //			flocha->setMpiController(commandManager);

        //nodeItem->setFlags(nodeItem->flags() | Qt::ItemIsEditable); //為什麼加了這一行，drop的node的nodeValue會被清除
        if(node.nodeName() == XmlNodeTypeAssem) {

            QDomNode Child = node.firstChildElement(XmlEleTagSub).firstChild();
            FlowchartsAssemble *assem = dynamic_cast<FlowchartsAssemble *>(flocha);
            if(assem != 0)
                buildFlochaLoop(Child,assem);
        }
        node = node.nextSibling();


        if((node.nodeName() != XmlNodeTypeAssem) && (parent == nullptr))
            break;

    }


    return 0;
}


FlowchartsTemplate *XmlFlochaTransfer::creatFlocha(QDomNode &node, FlowchartsAssemble *parent)
{

#ifdef _DEBUG
    qDebug() << "start creatFlocha";
#endif




    FlowchartsTemplate *flocha;

    if((parent==nullptr) && (node.nodeName() != XmlNodeTypeAssem))
    {
        QMessageBox::warning(NULL,tr("error"),tr("parent==nullptr & nodeName != XmlNodeTypeAssem"));
        return nullptr;
    }

    if(node.nodeName() == XmlNodeTypeSet)
    {
        FlowchartsSetVar *tmpflo = new FlowchartsSetVar(parent,commandManager);
        QString tmpstr = node.firstChildElement(XmlEleTagVarName).text();
        tmpflo->setVarName(tmpstr);
        tmpstr = node.firstChildElement(XmlEleTagStamen).text();
        tmpflo->setStatement(tmpstr);
        flocha = dynamic_cast<FlowchartsTemplate*>(tmpflo);
    }
    else if(node.nodeName() == XmlNodeTypeIf)
    {
        FlowchartsIf *tmpflo = new FlowchartsIf(parent,commandManager);

        QString tmpstr;
        QDomElement tmpnode = node.firstChildElement(XmlEleTagStamen1);
        if(tmpnode.isNull())
            return nullptr;

        tmpstr = tmpnode.text();
        tmpflo->setStatement1(tmpstr);


        tmpnode = node.firstChildElement(XmlEleTagStamen2);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();
        tmpflo->setStatement2(tmpstr);


        tmpnode = node.firstChildElement(XmlEleTagOperator);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();
        tmpflo->setOperator(tmpstr.toInt());


        flocha = dynamic_cast<FlowchartsTemplate*>(tmpflo);
    }
    else if(node.nodeName() == XmlNodeTypeRun)
    {
        FlowchartsRunPDL *tmpflo = new FlowchartsRunPDL(parent, commandManager);
        QString tmpstr;
        QDomElement tmpnode = node.firstChildElement(XmlEleTagPDLName);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();
        tmpflo->setPDLName(tmpstr);
        flocha = dynamic_cast<FlowchartsTemplate*>(tmpflo);
    }
    else if(node.nodeName() == XmlNodeTypeSleep)
    {
        FlowchartsSleep *tmpflo = new FlowchartsSleep(parent);
        QString tmpstr;
        QDomElement tmpnode = node.firstChildElement(XmlEleTagSleep);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();

        tmpflo->setSleepTime(tmpstr);
        flocha = dynamic_cast<FlowchartsTemplate*>(tmpflo);
    }
    else if(node.nodeName() == XmlNodeTypeAssem)
    {
        FlowchartsAssemble *tmpflo;
        FlochaAssemAttr attr;
        QString tmpstr;
        QDomElement tmpnode;
        if(parent == nullptr)
        {
            buildedFlocha = new FlowchartsAssemble(flochaParent, commandManager);
            tmpflo = buildedFlocha;
        }
        else
        {
            tmpflo = new FlowchartsAssemble(parent, commandManager);
        }
        tmpnode = node.firstChildElement(XmlEleTagStartPos);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();

        QStringList poslist = tmpstr.split(",");
        if(poslist.size() > 1)
        {
            tmpflo->getStartFlocha()->itemPos[0] = poslist.at(0).toInt();
            tmpflo->getStartFlocha()->itemPos[1] = poslist.at(1).toInt();
        }


        tmpnode = node.firstChildElement(XmlEleTagEndPos);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();
        poslist = tmpstr.split(",");
        if(poslist.size() > 1)
        {
            tmpflo->getEndFlocha()->itemPos[0] = poslist.at(0).toInt();
            tmpflo->getEndFlocha()->itemPos[1] = poslist.at(1).toInt();
        }

        tmpflo->getAssemAttr(&attr);


        tmpnode = node.firstChildElement(XmlEleTagLoopRunTimes);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();
        attr.runLoopTimes = tmpstr.toUInt();
        //tmpflo->setLoopRunTimes(tmpstr.toUInt());



        tmpnode = node.firstChildElement(XmlEleTagSleepPerLoop);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();
        attr.sleepPerLoop = tmpstr.toUInt();
        //tmpflo->setSleepTimePerLoop(tmpstr.toUInt());



        tmpnode = node.firstChildElement(XmlEleTagLoopRunCntVar);
        if(tmpnode.isNull())
            return nullptr;
        tmpstr = tmpnode.text();
        attr.loopRunCntVar = tmpstr;
        //tmpflo->setLoopRunCntStr(tmpstr);


        tmpflo->setAssemAttr(attr);
        

        flocha = dynamic_cast<FlowchartsTemplate*>(tmpflo);

    }



    if(parent != nullptr)
        parent->addTemplate(flocha);
    QString idname = node.firstChildElement(XmlEleTagName).text();
    memset(flocha->idName,0, sizeof(flocha->idName));
    memcpy(flocha->idName,idname.toStdString().c_str(), sizeof(flocha->idName));

    QString posstr = node.firstChildElement(XmlEleTagPos).text();
    QStringList poslist = posstr.split(",");
    if(poslist.size() > 1)
    {
        flocha->itemPos[0] = poslist.at(0).toInt();
        flocha->itemPos[1] = poslist.at(1).toInt();
    }
    //flocha->idName = node.firstChildElement(XmlEleTagName).nodeValue().toStdString().data();


    flochaNodeHash.insert(flocha, node);
    //	nodeFlochaHash.insert(node, flocha);

    return flocha;
}



int XmlFlochaTransfer::linkFlocha()
{

    for(auto it = flochaNodeHash.begin(); it != flochaNodeHash.end(); ++it)
    {
        if(it.value().nodeName() == XmlNodeTypeAssem)
        {
            QString startstr = it.value().firstChildElement(XmlEleTagStart).text();
            QDomNode startdom = findSiblingName(it.value().firstChildElement(XmlEleTagSub).firstChild(),startstr);
            if(!startdom.isNull())
            {
                FlowchartsTemplate *startflo = flochaNodeHash.key(startdom);
                FlowchartsAssemble *parentflo = dynamic_cast<FlowchartsAssemble*>(flochaNodeHash.key(startdom.parentNode().parentNode()));


                if(parentflo != nullptr)
                    parentflo->linkTwoTemplate(parentflo->getStartFlocha(),startflo);
                //parentflo->getStartFlocha()->p_next = startflo;
                else
                    QMessageBox::warning(NULL,tr("error"),tr("parentflo == nullptr"));
                //parentflo
            }
        }

        if(it.value().nodeName() == XmlNodeTypeIf)
        {
            QString elsestr = it.value().firstChildElement(XmlEleTagElse).text();
            QDomNode elsedom = findSiblingName(it.value(),elsestr);
            if(!elsedom.isNull())
            {
                FlowchartsTemplate *elseflo = flochaNodeHash.key(elsedom);
                FlowchartsAssemble *parentflo = dynamic_cast<FlowchartsAssemble*>(flochaNodeHash.key(elsedom.parentNode().parentNode()));



                if(parentflo != nullptr)
                    parentflo->linkTwoTemplate(it.key(),elseflo,true);
                else
                    QMessageBox::warning(NULL,tr("error"),tr("parentflo = nullptr"));

            }
            else if(elsestr == QString(XmlEleTagValueEnd))
            {	
                if(it.key()->getParentPtr() != nullptr)
                    it.key()->getParentPtr()->linkTwoTemplate(it.key(),it.key()->getParentPtr()->getEndFlocha(),true);
            }
        }

        QString nextstr = it.value().firstChildElement(XmlEleTagNext).text();

        //judge every time. maybe not good
        if(nextstr == QString(XmlEleTagValueEnd))
        {	
            if(it.key()->getParentPtr() != nullptr)
                it.key()->getParentPtr()->linkTwoTemplate(it.key(),it.key()->getParentPtr()->getEndFlocha());
        }
        else
        {

            QDomNode nextdom = findSiblingName(it.value(),nextstr);

            if(!nextdom.isNull())
            {
                FlowchartsTemplate *nextflo = flochaNodeHash.key(nextdom);
                FlowchartsAssemble *parentflo = dynamic_cast<FlowchartsAssemble*>(flochaNodeHash.key(nextdom.parentNode().parentNode()));


                if(parentflo != nullptr)
                    parentflo->linkTwoTemplate(it.key(),nextflo);
                else
                    QMessageBox::warning(NULL,tr("error"),tr("parentflo = nullptr"));

            }
        }
    }


    return 0;
}



QDomNode XmlFlochaTransfer::findSiblingName(QDomNode &node,QString &name)
{
    if(name.isEmpty())
        return QDomNode();

    QDomNode parent = node.parentNode();
    QDomNode itnode = parent.firstChild();

    while(!itnode.isNull())
    {
        if(itnode.firstChildElement(XmlEleTagName).text() == name)
            return itnode;
        itnode = itnode.nextSibling();
    }

    return itnode;
}



QDomNode XmlFlochaTransfer::createNodeData(QDomNode &parentSub,int type)
{
    QDomNode newnode;
    QString tagname;
    std::list<QString> taglist;
    taglist.clear();


    taglist.push_back(XmlEleTagName);
    taglist.push_back(XmlEleTagNext);
    taglist.push_back(XmlEleTagPos);


    switch(type)
    {

    case flochaTypeTemplate:

        break;
    case flochaTypeIf:
        tagname = XmlNodeTypeIf;
        taglist.push_back(XmlEleTagElse);
        taglist.push_back(XmlEleTagOperator);
        taglist.push_back(XmlEleTagStamen1);
        taglist.push_back(XmlEleTagStamen2);
        break;
    case flochaTypeSet:
        tagname = XmlNodeTypeSet;

        taglist.push_back(XmlEleTagVarName);
        taglist.push_back(XmlEleTagStamen);


        break;
    case flochaTypeRun:
        tagname = XmlNodeTypeRun;

        taglist.push_back(XmlEleTagPDLName);

        break;
    case flochaTypeSleep:
        tagname = XmlNodeTypeSleep;

        taglist.push_back(XmlEleTagSleep);


        break;
    case flochaTypeAssem:
        tagname = XmlNodeTypeAssem;

        taglist.push_back(XmlEleTagStart);
        taglist.push_back(XmlEleTagSub);
        taglist.push_back(XmlEleTagStartPos);
        taglist.push_back(XmlEleTagEndPos);

        taglist.push_back(XmlEleTagLoopRunTimes);
        taglist.push_back(XmlEleTagSleepPerLoop);
        taglist.push_back(XmlEleTagLoopRunCntVar);

        break;
    }

    newnode = domDocument.createElement(tagname);
    parentSub.appendChild(newnode);

    for(auto it = taglist.begin(); it != taglist.end(); ++it)
    {
        newnode.appendChild(domDocument.createElement(*it));
    }

    return newnode;
}



std::list<QDomNode> XmlFlochaTransfer::createListNode(std::list<QString> &taglist)
{
    std::list<QDomNode> nodelist;

    for(auto it = taglist.begin(); it != taglist.end(); ++it)
    {
        nodelist.push_back(domDocument.createElement(*it));
    }
    return nodelist;
}

void XmlFlochaTransfer::setEleNodeText(QDomElement &elenode, const QString &text)
{
    if(elenode.firstChild().isNull() || !elenode.firstChild().isText())
    {
        QDomText textnode = domDocument.createTextNode(text);
        elenode.insertBefore(textnode,elenode.firstChild());
    }
    else
    {
        elenode.firstChild().setNodeValue(text);
    }


}
*/

int XmlFlochaTransfer::buildLocalVarPool(QDomNode &node)
{
    int rtn = 0;
    QDomNode childnode = node.firstChild();
    unsigned int indx = 0;

    while(!childnode.isNull())
    {
        indx = 0;
        QStringList strlist = childnode.toElement().text().split(",");
        
        ArrayDimensInfo info;
        
        memset(&info, 0, sizeof(ArrayDimensInfo));
        if(strlist.size() == 4)
        {
            info.dimension = strlist.at(0).toInt();
            for(int i = 0; i < ARRAYDIMENSION; ++i)
            {
                info.dimensionSize[i] = strlist.at(i+1).toInt();
            }
        }

        commandManager->editVarMap(&indx, childnode.nodeName(), info);

        indexVarNameNodeMap.insert(std::make_pair(indx,std::make_pair(childnode.nodeName(),childnode)));

        childnode = childnode.nextSibling();
        //++VariableIndex;
    }



    return rtn;
}



int XmlFlochaTransfer::updDomChangedVar(unsigned int *indx, QString &varname, const ArrayDimensInfo &info)
{
    int rtn = -1;
    auto it = indexVarNameNodeMap.find(*indx);


    if(it != indexVarNameNodeMap.end())
    {
        it->second.first = varname;
        it->second.second.toElement().setTagName(varname);
        setEleNodeText(it->second.second.toElement(), QString("%1,%2,%3,%4").arg(info.dimension)
                       .arg(info.dimensionSize[0]).arg(info.dimensionSize[1]).arg(info.dimensionSize[2]));
        commandManager->editVarMap(indx, varname, info);
    }
    else
    {
        QDomElement newnode;
        newnode = domDocument.createElement(varname);
        varPoolParentNode.appendChild(newnode);
        setEleNodeText(newnode.toElement(), QString("%1,%2,%3,%4").arg(info.dimension)
                       .arg(info.dimensionSize[0]).arg(info.dimensionSize[1]).arg(info.dimensionSize[2]));

        commandManager->editVarMap(indx, varname, info);
        indexVarNameNodeMap.insert(std::make_pair(*indx,std::make_pair(varname,newnode)));
        //*indx = VariableIndex;
        //++VariableIndex;

    }


    return rtn;
}


int XmlFlochaTransfer::updDomRemovedVar(unsigned int indx)
{
    int rtn = 0;
    commandManager->eraseVarMap(indx);
    indexVarNameNodeMap.erase(indx);

    return rtn;
}

void XmlFlochaTransfer::updNodeForSave()
{
    for(auto it = indexVarNameNodeMap.begin(); it != indexVarNameNodeMap.end(); ++it)
    {
        double value;

        commandManager->getVar(it->second.first, &value);

        setEleNodeText(it->second.second.toElement(),QString("%1").arg(value));
    }

}

std::map<unsigned int, std::pair<QString, ArrayDimensInfo> > XmlFlochaTransfer::getVarPoolToInitTable()
{
    std::map<unsigned int, std::pair<QString, ArrayDimensInfo> > varpooldata;
    varpooldata.clear();

    for(auto it = indexVarNameNodeMap.begin(); it != indexVarNameNodeMap.end(); ++it)
    {
        QStringList strlist = it->second.second.toElement().text().split(",");
        
        ArrayDimensInfo info;
        
        memset(&info, 0, sizeof(ArrayDimensInfo));
        if(strlist.size() == 4)
        {
            info.dimension = strlist.at(0).toInt();
            for(int i = 0; i < ARRAYDIMENSION; ++i)
            {
                info.dimensionSize[i] = strlist.at(i+1).toInt();
            }
        }

        varpooldata.insert(std::make_pair(it->first,std::make_pair(it->second.first, info)));
    }
    return varpooldata;
}
/*
int XmlFlochaTransfer::updNodeAfterFloChange(FlowchartsTemplate *flocha)
{
    auto it = flochaNodeHash.find(flocha);

    if(it != flochaNodeHash.end())
    {
        QDomElement tmpnode;


        setEleNodeText(it.value().firstChildElement(XmlEleTagName),QString("%1").arg(flocha->idName));
        if(flocha->getNextPtr() != nullptr)
            setEleNodeText(it.value().firstChildElement(XmlEleTagNext),QString("%1").arg(flocha->getNextPtr()->idName));
        setEleNodeText(it.value().firstChildElement(XmlEleTagPos),QString("%1,%2").arg(flocha->itemPos[0]).arg(flocha->itemPos[1]));

        //std::string test = tmpnode.text().toStdString();


        FlowchartsIf *flochaif = dynamic_cast<FlowchartsIf *>(flocha);
        FlowchartsSetVar* flochaset = dynamic_cast<FlowchartsSetVar*>(flocha);
        FlowchartsSleep *flochasleep = dynamic_cast<FlowchartsSleep*>(flocha);
        FlowchartsRunPDL *flocharun = dynamic_cast<FlowchartsRunPDL*>(flocha);
        FlowchartsAssemble *flochaassem = dynamic_cast<FlowchartsAssemble*>(flocha);

        switch(it.key()->getflochaType())
        {
        case flochaTypeTemplate:

            break;
        case flochaTypeIf:


            if(flochaif->getElsePtr() != nullptr)
                setEleNodeText(it.value().firstChildElement(XmlEleTagElse),QString("%1").arg(flochaif->getElsePtr()->idName));
            setEleNodeText(it.value().firstChildElement(XmlEleTagStamen1),QString("%1").arg(flochaif->getStatement1()));
            setEleNodeText(it.value().firstChildElement(XmlEleTagStamen2),QString("%1").arg(flochaif->getStatement2()));
            setEleNodeText(it.value().firstChildElement(XmlEleTagOperator),QString("%1").arg(flochaif->getOperator()));

            break;
        case flochaTypeSet:

            setEleNodeText(it.value().firstChildElement(XmlEleTagVarName),QString("%1").arg(flochaset->getVarName()));
            setEleNodeText(it.value().firstChildElement(XmlEleTagStamen),QString("%1").arg(flochaset->getStatement()));

            break;
        case flochaTypeRun:


            setEleNodeText(it.value().firstChildElement(XmlEleTagPDLName),QString("%1").arg(flocharun->getPDLName()));

            break;
        case flochaTypeSleep:

            setEleNodeText(it.value().firstChildElement(XmlEleTagSleep),QString("%1").arg(flochasleep->getSleepTime()));
            break;
        case flochaTypeAssem:

            FlochaAssemAttr attr;
            flochaassem->getAssemAttr(&attr);
            setEleNodeText(it.value().firstChildElement(XmlEleTagLoopRunTimes),QString("%1").arg(attr.runLoopTimes));
            setEleNodeText(it.value().firstChildElement(XmlEleTagSleepPerLoop),QString("%1").arg(attr.sleepPerLoop));
            setEleNodeText(it.value().firstChildElement(XmlEleTagLoopRunCntVar),QString("%1").arg(attr.loopRunCntVar));


            break;

        }

    }
    else
    {
        QDomElement tmpnode;
        QString idname = QString("%1").arg(flocha->idName);

        if(idname == QString(XmlEleTagValueStart))
        {
            int test = 0;
            if(flocha->getNextPtr() != nullptr)
                setEleNodeText(flochaNodeHash.find(flocha->getParentPtr()).value().firstChildElement(XmlEleTagStart),QString("%1").arg(flocha->getNextPtr()->idName));
            tmpnode = flochaNodeHash.find(flocha->getParentPtr()).value().firstChildElement(XmlEleTagStartPos);
        }
        else if(idname == QString(XmlEleTagValueEnd))
        {
            tmpnode = flochaNodeHash.find(flocha->getParentPtr()).value().firstChildElement(XmlEleTagEndPos);
        }

        if(!tmpnode.isNull())
            setEleNodeText(tmpnode,QString("%1,%2").arg(flocha->itemPos[0]).arg(flocha->itemPos[1]));





        if((idname != QString(XmlEleTagValueStart)) && (idname != QString(XmlEleTagValueEnd)))
        {
            QDomNode parentsub;
            QDomNode newnode;


            parentsub = flochaNodeHash.find(flocha->getParentPtr()).value().firstChildElement(XmlEleTagSub);

            newnode = createNodeData(parentsub, flocha->getflochaType());

            flochaNodeHash.insert(flocha,newnode);

            updNodeAfterFloChange(flocha);

        }

    }


    return 0;
}

void XmlFlochaTransfer::removeNode(FlowchartsTemplate *flocha)
{
    auto it = flochaNodeHash.find(flocha);
    QDomNode node = it.value();
    node.parentNode().removeChild(node);

    flochaNodeHash.erase(it);
}
*/

int XmlFlochaTransfer::saveFlochaToXml(QIODevice *device)
{

    //updNodeForSave();//temp solution for array var
    transDisplayUnitToNode();

    const int IndentSize = 4;

    QTextStream out(device);
    domDocument.save(out, IndentSize);


    return 0;
}



void XmlFlochaTransfer::transDisplayUnitToNode()
{
    std::list<VarDisplayUnit*> unitslist;

    if(!displayUnitsPlane)
        return;
    if(unitsPlaneParentNode.isNull())
        return;


    QDomNode removenode = unitsPlaneParentNode.firstChild();


    while(!removenode.isNull())
    {
        QDomNode tmpnode = removenode.nextSibling();
        unitsPlaneParentNode.removeChild(removenode);
        removenode = tmpnode;
    }

    unitslist = displayUnitsPlane->getDisplayUnitList();
    
    for(auto it = unitslist.begin(); it != unitslist.end(); ++it)
    {
        QDomElement unitnode = domDocument.createElement(XmlNodeTypeUnit);
        QDomElement varname = domDocument.createElement(XmlNodeTypeVarName);
        QDomElement textmap = domDocument.createElement(XmlNodeTypeTextMap);
        QDomElement iconmap = domDocument.createElement(XmlNodeTypeIconMap);
        QDomElement setnode = domDocument.createElement(XmlNodeTypeUnitSet);

        QString varnamestr = (*it)->getVarName();
        DisplayUnitSetting unitset = (*it)->getUnitSetting();
        std::map<double, QString> vartextmap = (*it)->getValueTextMap();
        std::map<double, int> variconmap = (*it)->getValueIconIdxMap();

        unitsPlaneParentNode.appendChild(unitnode);
        unitnode.appendChild(varname);
        unitnode.appendChild(textmap);
        unitnode.appendChild(iconmap);
        unitnode.appendChild(setnode);


        setEleNodeText(varname, varnamestr);
        setEleNodeText(setnode, QString("%1").arg(unitset.all));

        
        for(auto it_txt = vartextmap.begin(); it_txt != vartextmap.end(); ++it_txt)
        {
            QDomElement pairnode = domDocument.createElement(XmlNodeTypePair);
            QDomElement keynode = domDocument.createElement(XmlNodeTypeKey);
            QDomElement txtnode = domDocument.createElement(XmlNodeTypeText);

            textmap.appendChild(pairnode);
            pairnode.appendChild(keynode);
            pairnode.appendChild(txtnode);

            setEleNodeText(keynode, QString("%1").arg(it_txt->first));
            setEleNodeText(txtnode, it_txt->second);

        }

        for(auto it_ico = variconmap.begin(); it_ico != variconmap.end(); ++it_ico)
        {
            QDomElement pairnode = domDocument.createElement(XmlNodeTypePair);
            iconmap.appendChild(pairnode);

            setEleNodeText(pairnode, QString("%1,%2").arg(it_ico->first).arg(it_ico->second));
        }
        
    }
    



}


int XmlFlochaTransfer::buildDisplayUnitsPlane(QDomNode &node)
{
    int rtn = 0;
    std::map<double, QString> textmap;
    std::map<double, int> iconmap;
    QString varname;

    QDomElement unitnode = node.firstChildElement(XmlNodeTypeUnit);

    while(!unitnode.isNull())
    {
        varname.clear();

        if(unitnode.nodeName() != XmlNodeTypeUnit)
        {
            unitnode = unitnode.nextSiblingElement();
            continue;
        }

        QDomElement unitchild = unitnode.firstChildElement(XmlNodeTypeVarName);//unitchild will use many times in this function

        if(!unitchild.isNull())
            varname = unitchild.text();

        //------------------------text map---------------------------

        textmap.clear();

        unitchild = unitnode.firstChildElement(XmlNodeTypeTextMap);


        QDomElement pairnode;
        if(!unitchild.isNull())
            pairnode = unitchild.firstChildElement(XmlNodeTypePair);
        else 
        {
            QDomElement textmapnode = domDocument.createElement(XmlNodeTypeTextMap);
            unitnode.appendChild(textmapnode);
        }

        while(!pairnode.isNull())
        {
            if(pairnode.tagName() != XmlNodeTypePair)
            {
                pairnode = pairnode.nextSiblingElement();
                continue;
            }

            QDomElement keynode = pairnode.firstChildElement(XmlNodeTypeKey);
            QDomElement textnode = pairnode.firstChildElement(XmlNodeTypeText);

            if(!keynode.isNull())
            {
                if(!keynode.text().isEmpty())
                {
                    if(textnode.isNull())
                    {
                        textnode = domDocument.createElement(XmlNodeTypeText);
                        pairnode.appendChild(textnode);
                    }
                    textmap.insert(std::make_pair(keynode.text().toDouble(),textnode.text()));
                }
            }
            pairnode = pairnode.nextSiblingElement();
        }
        //------------------------text map---------------------------

        //------------------------icon map---------------------------
        unitchild = unitnode.firstChildElement(XmlNodeTypeIconMap);

        iconmap.clear();

        if(!unitchild.isNull())
            pairnode = unitchild.firstChildElement(XmlNodeTypePair);
        else 
        {
            QDomElement textmapnode = domDocument.createElement(XmlNodeTypeIconMap);
            unitnode.appendChild(textmapnode);
        }

        while(!pairnode.isNull())
        {
            if(pairnode.tagName() != XmlNodeTypePair)
            {
                pairnode = pairnode.nextSiblingElement();
                continue;
            }
            QString pairstr = pairnode.text();
            QStringList iconmaplist = pairstr.split(",");
            if(iconmaplist.size() > 1)
            {
                iconmap.insert(std::make_pair(iconmaplist.at(0).toDouble(),iconmaplist.at(1).toInt()));
            }

            pairnode = pairnode.nextSiblingElement();
        }

        //------------------------icon map---------------------------
        unitchild = unitnode.firstChildElement(XmlNodeTypeUnitSet);
        DisplayUnitSetting setting;
        QString unitset;
        if(!unitchild.isNull())
            unitset = unitchild.text();

        setting.all = 0;
        if(!unitset.isEmpty())
        {
            bool ok;
            int tmpset = unitset.toInt(&ok);
            if(ok)
                setting.all = tmpset;
        }

        VarDisplayUnit *unit = new VarDisplayUnit(displayUnitsPlane, commandManager, varname, setting, textmap, iconmap);
        displayUnitsPlane->addDisplayUnit(unit);

        globalObjManager->getVarMonitorManager()->addLowPriorityUnit(unit);

        unitnode = unitnode.nextSiblingElement();
        //		indexVarNameNodeMap.insert(std::make_pair(VariableIndex,std::make_pair(childnode.nodeName(),childnode)));

        //		commandManager->editVarMap(VariableIndex, childnode.nodeName(),childnode.toElement().text().toDouble());

        //		childnode = childnode.nextSibling();
        //		++VariableIndex;

    }



    return rtn;


}



//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------



XmlToFlochaItem::XmlToFlochaItem(QObject *parent)
{
    flochaItemList.clear();
    flochaParent = parent;
}

XmlToFlochaItem::~XmlToFlochaItem()
{
}


void XmlToFlochaItem::addFlochaItem(QDomElement &node)
{
    buildedFlocha = nullptr;
    flochaNodeHash.clear();

    buildFlochaLoop(node);
    linkFlocha();
    flochaItemList.push_back(buildedFlocha);
    
    
    buildedFlocha = nullptr;
}

void XmlToFlochaItem::getFlochaItem(std::list<FlowchartsAssemble*> &itemlist)
{
    itemlist = flochaItemList;
}

