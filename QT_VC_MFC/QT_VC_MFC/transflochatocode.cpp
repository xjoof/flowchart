#include "transflochatocode.h"

FlochaCodeTransfer::FlochaCodeTransfer(QObject *parent)
    : QObject(parent)
{
    objectAssem = NULL;
    psudoCode.clear();
    actualCode.clear();
    
    treeNodeIdx = 1;//0 is root tree node

    codeType = CodeType_PDL;


}

FlochaCodeTransfer::~FlochaCodeTransfer()
{

}


void FlochaCodeTransfer::startTransToCode(FlowchartsAssemble *assem, QStringList &code)
{

    if(assem == NULL)
    {
        return;
    }
    
    //QTextStream out(&file);

    objectAssem = assem;

    //flocahAttriMap.clear();
    //flochaLabelMap.clear();
    labelNum = 0;
    floAttrIdx = 0;

    initTreeNodeStruct();
    buildAttrArray_prevList(objectAssem);
    

    for(int i = 0; i < floAttrIdx; ++i)
    {
        if(floAttrArray[i].flocha == objectAssem->getStartFlocha()->getNextPtr())
        {
            rootFloAttr = &floAttrArray[i];
            break;
        }
    }

    rootFloAttr->flocha = objectAssem->getStartFlocha()->getNextPtr();
    rootFloAttr->idxInNode = 0;
    rootFloAttr->treeNode = treeRoot;
    rootFloAttr->treeNode->startFloAttr = rootFloAttr;
    rootFloAttr->treeNode->flochaNum = 0;


    buildDataForTrans(rootFloAttr);
    
    //parseFlochaAtrri(objectAssem);

#ifdef _DEBUG
    printParseAttriResult();
#endif


    psudoCode.clear();
    actualCode.clear();

    switch(codeType)
    {
    case CodeType_C:
        return;//no solution now
        break;
    case CodeType_CPP:
        return;//no solution now
        break;
    case CodeType_JAVA:
        return;//no solution now
        break;
    case CodeType_PDL:

        
        transToPdlCode(treeRoot);


        break;

    default:

        transToPsudoCode(objectAssem);

        transToActualCode();

        break;
    }



    for(int i = 0; i < psudoCode.size(); ++i)
    {
         code << psudoCode.at(i);
    }

    /*temporary
    for(int i = 0; i < actualCode.size(); ++i)
    {
         code << actualCode.at(i);
    }
    */
}


void FlochaCodeTransfer::transToRtxRunStruct(FlowchartsAssemble *assem)
{

}

void FlochaCodeTransfer::setCodeType(int type)
{
    codeType = type;
}


void FlochaCodeTransfer::setCustomCodeStyle(CodeRuleStruct rule)
{

}

void FlochaCodeTransfer::buildAttrArray_prevList(FlowchartsAssemble* assem)
{
    for(auto it = assem->getFlochaList().begin(); it != assem->getFlochaList().end(); ++it)
    {
        //if((*it)->getflochaType() == flochaTypeTemplate)
        //{
        //    continue;
        //}
 

        if((*it)->getflochaType() == flochaTypeAssem)
        {
            FlowchartsAssemble *subassem = dynamic_cast<FlowchartsAssemble*>(*it);
            if(subassem)
            {
                FlochaAssemAttr attr;
                subassem->getAssemAttr(&attr);

                if(attr.isFunction)
                {
                    addToPreMap(*it, (*it)->getNextPtr(), false);
                }
                else
                {
                    //addToPreMap(*it, subassem->getStartFlocha()->getNextPtr(), false);
                    //addToPreMap(subassem->getEndFlocha(), subassem->getNextPtr(), false);
                    addToPreMap(*it, (*it)->getNextPtr(), false);

                    buildAttrArray_prevList(subassem);
                }
            }

        }
        else
        {
            FlowchartsTemplate *nextptr = (*it)->getNextPtr();
            FlowchartsTemplate *elseptr = (*it)->getElsePtr();

            addToPreMap(*it, nextptr, false);
            addToPreMap(*it, elseptr, true);
        }

    }
}



void FlochaCodeTransfer::initTreeNodeStruct()
{
    int idx = 0;

    memset(treeNodeArray, 0, sizeof(treeNodeArray));

    treeRoot = treeNodeArray;

    /*
    for(idx = 0; idx < 1024; ++idx)
    {
        if((idx * 2 + 2) < BINTREENODEARRAYNUM)
        {
            treeNodeArray[idx].rightNodeP = &treeNodeArray[idx * 2 + 1];
            treeNodeArray[idx].rightNodeP->parent = &treeNodeArray[idx];

            treeNodeArray[idx].leftNodeP = &treeNodeArray[idx * 2 + 2];
            treeNodeArray[idx].leftNodeP->parent = &treeNodeArray[idx];
        }
        else
        {
            break;
        }
    }
    */


    for(int i = 0; i < FLOATTRARRAYNUM; ++i)
    {

        floAttrArray[i].elseAttr = nullptr;
        floAttrArray[i].flocha = nullptr;
        floAttrArray[i].idxInNode = 0;
        floAttrArray[i].label.clear();
        floAttrArray[i].needLabel = false;
        floAttrArray[i].nextAttr = nullptr;
        floAttrArray[i].preList.clear();
        floAttrArray[i].treeNode = nullptr;
        floAttrArray[i].subAttr = nullptr;
        floAttrArray[i].twoPreAreSibling = false;

    }



}



void FlochaCodeTransfer::buildDataForTrans(FlochaAttriForTrans *attr)
{
    FlochaAttriForTrans *floattr = attr;
    //rootFloAttr->flocha = objectAssem->getStartFlocha()->getNextPtr();
    //floattr->idxInNode = 0;
    //floattr->treeNode = treeRoot;
    //floattr->treeNode->startFloAttr = rootFloAttr;
    //floattr->treeNode->flochaNum = 0;
    
    //FlowchartsTemplate *flocha = assem->getStartFlocha()->getNextPtr();

    

    std::vector<FlochaAttriForTrans*> forkstartqueue;

    std::vector<FlochaAttriForTrans*> floendqueue;

    forkstartqueue.clear();
    floendqueue.clear();


    bool needpopque = false;


    while(floattr)
    {

        switch (floattr->flocha->getflochaType())
        {
        case flochaTypeIf:
            {

                if(!floattr->nextAttr || !floattr->elseAttr)
                {
                    QMessageBox::warning(nullptr, tr("Translate error"), tr("Maybe some flocha not link complete"));
                    return;
                }



                if(!floattr->treeNode->rightNodeP)
                {
                    floattr->treeNode->rightNodeP = &treeNodeArray[treeNodeIdx];
                    ++treeNodeIdx;
                    floattr->treeNode->rightNodeP->parent = floattr->treeNode;
                }
                if(!floattr->treeNode->leftNodeP)
                {
                    floattr->treeNode->leftNodeP = &treeNodeArray[treeNodeIdx];
                    ++treeNodeIdx;
                    floattr->treeNode->leftNodeP->parent = floattr->treeNode;
                }
                


                if(!floattr->elseAttr->treeNode)
                {
                    floattr->elseAttr->treeNode =  floattr->treeNode->leftNodeP;
                    floattr->elseAttr->treeNode->startFloAttr = floattr->elseAttr;
                    floattr->elseAttr->treeNode->flochaNum = 1;
                    floattr->elseAttr->idxInNode = 0;
                    forkstartqueue.push_back(floattr->elseAttr);
                }
                else
                {
                    if(floattr->elseAttr->needLabel == false)
                    {
                        floattr->elseAttr->label = PDLLABELPREFIX + QString("%1").arg(labelNum);
                        ++labelNum;
                        floattr->elseAttr->needLabel = true;
                    }
                }


                if(!floattr->nextAttr->treeNode)
                {
                    floattr->nextAttr->treeNode =  floattr->treeNode->rightNodeP;
                    floattr->nextAttr->treeNode->startFloAttr = floattr->nextAttr;
                    floattr->nextAttr->treeNode->flochaNum = 1;
                    floattr->nextAttr->idxInNode = 0;
                    forkstartqueue.push_back(floattr->nextAttr);
                }
                else
                {
                    if(floattr->nextAttr->needLabel == false)
                    {
                        floattr->nextAttr->label = PDLLABELPREFIX + QString("%1").arg(labelNum);
                        ++labelNum;
                        floattr->nextAttr->needLabel = true;
                    }
                }

                

                floattr->treeNode->endFloAttr = floattr;
                //floattr->idxInNode = floattr->treeNode->flochaNum;

                if(floattr->treeNode->startFloAttr != floattr)
                {
                    floattr->idxInNode = floattr->treeNode->flochaNum;
                    ++floattr->treeNode->flochaNum;
                }

                needpopque = true;

            }
            break;

        case flochaTypeAssem:
            {
                FlowchartsAssemble *floassem = dynamic_cast<FlowchartsAssemble*>(floattr->flocha);
                FlochaAttriForTrans *subfloattr = nullptr;


                for(int i = 0; i < floAttrIdx; ++i)
                {
                    if(floAttrArray[i].flocha == floassem->getStartFlocha()->getNextPtr())
                    {
                        subfloattr = &floAttrArray[i];
                        break;
                    }
                }

                subfloattr->idxInNode = 0;
                subfloattr->treeNode = &treeNodeArray[treeNodeIdx];
                ++treeNodeIdx;
                subfloattr->treeNode->startFloAttr = subfloattr;
                subfloattr->treeNode->flochaNum = 0;
                floattr->subAttr = subfloattr;
                //FlowchartsTemplate *flocha = assem->getStartFlocha()->getNextPtr();

                buildDataForTrans(subfloattr);
            }
        case flochaTypeSet:

        case flochaTypeRun:

        case flochaTypeSleep:
            {
                if(!floattr->nextAttr)
                {
                    QMessageBox::warning(nullptr, tr("Translate error"), tr("Maybe some flocha not link complete"));
                    return;
                }



                if(!floattr->nextAttr->treeNode)
                {
                    floattr->nextAttr->treeNode =  floattr->treeNode;
                    
                    floattr->nextAttr->idxInNode = floattr->treeNode->flochaNum;
                    ++floattr->treeNode->flochaNum;

                }
                else
                {
                    needpopque = true;

                    floattr->treeNode->endFloAttr = floattr;

                    int preisparent = 0;
                    for(auto it_pre = floattr->nextAttr->preList.begin(); it_pre != floattr->nextAttr->preList.end(); ++it_pre)
                    {
                        //if()

                    }
                    if(preisparent >= 2 && floattr->nextAttr->preList.size() == 2)
                    {
                        floattr->nextAttr->twoPreAreSibling = true;
                        //floattr->nextAttr->needLabel = false;
                        //floattr->nextAttr->label = "";
                    }
                    else if(floattr->nextAttr->needLabel == false)
                    {
                        floattr->nextAttr->label = PDLLABELPREFIX + QString("%1").arg(labelNum);
                        ++labelNum;
                        floattr->nextAttr->needLabel = true;
                    }

                }

            }
            break;
        default:
            /*
            if(!floattr->nextAttr->treeNode)
            {
                floattr->nextAttr->treeNode =  floattr->treeNode;

                floattr->nextAttr->idxInNode = floattr->treeNode->flochaNum;
                ++floattr->treeNode->flochaNum;
            }
            else
            {
                needpopque = true;

                floattr->treeNode->endFloAttr = floattr;

                int preisparent = 0;
                for(auto it_pre = floattr->nextAttr->preList.begin(); it_pre != floattr->nextAttr->preList.end(); ++it_pre)
                {
                    //if()

                }
                if(preisparent >= 2 && floattr->nextAttr->preList.size() == 2)
                {
                    floattr->nextAttr->twoPreAreSibling = true;
                    //floattr->nextAttr->needLabel = false;
                    //floattr->nextAttr->label = "";
                }
                else if(floattr->nextAttr->needLabel == false)
                {
                    floattr->nextAttr->label = PDLLABELPREFIX + QString("%1").arg(labelNum);
                    ++labelNum;
                    floattr->nextAttr->needLabel = true;
                }

            }
            

            if(floendqueue.size() > 0)
            {
                if(floendqueue.back() != floattr)
                {
                    floendqueue.push_back(floattr);
                }
            }
            else
            {
                floendqueue.push_back(floattr);
            }
            */


            if(forkstartqueue.size() > 0)
            {
                needpopque = true;
            }
            else
            {
                /*
                if(!floattr->nextAttr->treeNode)
                {
                floattr->nextAttr->treeNode =  floattr->treeNode;

                floattr->nextAttr->idxInNode = floattr->treeNode->flochaNum;
                ++floattr->treeNode->flochaNum;
                }
                else
                {
                QMessageBox::warning(nullptr, "error", "maybe a bug");
                }
                */
                return;
            }
            break;
        }

        if(needpopque)
        {
            do
            {
                if(forkstartqueue.size() == 0)
                {
                    return;
                    break;
                }

                floattr = forkstartqueue.back();
                forkstartqueue.pop_back();


                /*
                if(floattr->treeNode != 0) continue;
                */

            }while(!floattr);
            needpopque = false;
        }
        else
        {
            floattr = floattr->nextAttr;
        }      
    }
}



void FlochaCodeTransfer::addToPreMap(FlowchartsTemplate *pre, FlowchartsTemplate *flocha, bool iselse)
{
    if(flocha != NULL)
    {
        bool foundpre = false;
        bool foundflo = false;
        int preidx = 0;
        int floidx = 0;
        for(preidx = 0; preidx < floAttrIdx; ++preidx)
        {
            if(floAttrArray[preidx].flocha == pre)
            {
                foundpre = true;
                break;
            }
        }

        for(floidx = 0; floidx < floAttrIdx; ++floidx)
        {
            if(floAttrArray[floidx].flocha == flocha)
            {
                foundflo = true;
                break;
            }
        }

        if(!foundpre)
        {
            preidx = floAttrIdx;
            ++floAttrIdx;

            floAttrArray[preidx].flocha = pre;
        }

        if(!foundflo)
        {
            floidx = floAttrIdx;
            ++floAttrIdx;

            floAttrArray[floidx].flocha = flocha;
        }

        floAttrArray[floidx].preList.push_back(&floAttrArray[preidx]);


        if(iselse)
        {
            floAttrArray[preidx].elseAttr = &floAttrArray[floidx];
        }
        else
        {
            floAttrArray[preidx].nextAttr = &floAttrArray[floidx];
        }
    }
}





/*
void FlochaCodeTransfer::parseFlochaAtrri(FlowchartsAssemble* assem)
{
    FlowchartsTemplate *flocha = assem->getStartFlocha()->getNextPtr();

    int nodenum = 1;

    std::queue<FlowchartsTemplate*> forkstartqueue;

    bool needpopque = false;


    while(flocha)
    {

        switch (flocha->getflochaType())
        {
        case flochaTypeIf:
            {
                auto it_attr = flocahAttriMap.find(flocha);
                if(it_attr != flocahAttriMap.end())
                {
                    if(it_attr->second.nodeNum == 0)
                    {
                        it_attr->second.nodeNum = nodenum;
                        //forkstartqueue.push(it_if->first);
                        
                        forkstartqueue.push(flocha->getNextPtr());
                        forkstartqueue.push(flocha->getElsePtr());
                    }
                    else
                    {
                        int preisparent = 0;
                        for(auto it_pre = it_attr->second.preList.begin(); it_pre != it_attr->second.preList.end(); ++it_pre)
                        {
                            auto it_pre_attr = flocahAttriMap.find(*it_pre);
                            if(it_pre_attr != flocahAttriMap.end())
                            {
                                if(it_pre_attr->second.nodeNum * 2 == it_attr->second.nodeNum)
                                {
                                    ++preisparent;
                                }
                                else if(it_pre_attr->second.nodeNum * 2 + 1 == it_attr->second.nodeNum)
                                {
                                    ++preisparent;
                                }
                                else if(it_pre_attr->second.nodeNum == (it_attr->second.nodeNum >> 1) * 2)
                                {
                                    ++preisparent;
                                }
                                else if(it_pre_attr->second.nodeNum == (it_attr->second.nodeNum >> 1) * 2 + 1)
                                {
                                    ++preisparent;
                                }
                            }

                        }
                        if(preisparent == 2 && it_attr->second.preList.size() == 2)
                        {
                            it_attr->second.twoPreAreSibling = true;
                            it_attr->second.needLabel = false;
                            it_attr->second.label = "";
                        }
                        else if(it_attr->second.needLabel == false)
                        {
                            it_attr->second.label = PDLLABELPREFIX + QString("%1").arg(labelNum);
                            ++labelNum;
                            it_attr->second.needLabel = true;
                        }
                    }
                    
                }
                needpopque = true;
            }
            break;

        case flochaTypeAssem:
            {
                FlowchartsAssemble *floassem = dynamic_cast<FlowchartsAssemble*>(flocha);
                parseFlochaAtrri(floassem);
            }
        case flochaTypeSet:

        case flochaTypeRun:

        case flochaTypeSleep:
            {
                auto it_attr = flocahAttriMap.find(flocha);
                if(it_attr != flocahAttriMap.end())
                {
                    if(it_attr->second.nodeNum == 0)
                    {
                        it_attr->second.nodeNum = nodenum;
                    }
                    else// if(forkstartqueue.size() > 0)
                    {
                        needpopque = true;


                        int preisparent = 0;
                        for(auto it_pre = it_attr->second.preList.begin(); it_pre != it_attr->second.preList.end(); ++it_pre)
                        {
                            auto it_pre_attr = flocahAttriMap.find(*it_pre);
                            if(it_pre_attr != flocahAttriMap.end())
                            {
                                if(it_pre_attr->second.nodeNum * 2 == it_attr->second.nodeNum)
                                {
                                    ++preisparent;
                                }
                                else if(it_pre_attr->second.nodeNum * 2 + 1 == it_attr->second.nodeNum)
                                {
                                    ++preisparent;
                                }
                                else if(it_pre_attr->second.nodeNum == (it_attr->second.nodeNum >> 1) * 2)
                                {
                                    ++preisparent;
                                }
                                else if(it_pre_attr->second.nodeNum == (it_attr->second.nodeNum >> 1) * 2 + 1)
                                {
                                    ++preisparent;
                                }
                            }

                        }
                        if(preisparent == 2 && it_attr->second.preList.size() == 2)
                        {
                            it_attr->second.twoPreAreSibling = true;
                            it_attr->second.needLabel = false;
                            it_attr->second.label = "";
                        }
                        else if(it_attr->second.needLabel == false)
                        {
                            it_attr->second.label = PDLLABELPREFIX + QString("%1").arg(labelNum);
                            ++labelNum;
                            it_attr->second.needLabel = true;
                        }
                    }
                    //else
                    //{
                    //    return;
                    //}
                }

            }
            break;
        default:
            
            if(forkstartqueue.size() > 0)
            {
                needpopque = true;
            }
            else
            {
                return;
            }
            break;
        }

        if(needpopque)
        {
            do
            {
                if(forkstartqueue.size() == 0)
                {
                    return;
                    break;
                }

                flocha = forkstartqueue.front();
                forkstartqueue.pop();
                ++nodenum;
                auto it = flocahAttriMap.find(flocha);
                if(it != flocahAttriMap.end())
                {
                    if(it->second.nodeNum != 0)
                    {
                        continue;
                    }
                }
            
            }while(!flocha);
            needpopque = false;
        }
        else
        {
            flocha = flocha->getNextPtr();
        }      
    }
}
*/

#ifdef _DEBUG
void FlochaCodeTransfer::printParseAttriResult()
{

    for(int i = 0; i < floAttrIdx; ++i)
    {

        qDebug() << "idname" << floAttrArray[i].flocha->idName << "label" << floAttrArray[i].label 
            << "need" << floAttrArray[i].needLabel
            << "twoPreAreSibling" << floAttrArray[i].twoPreAreSibling << "pre size" << floAttrArray[i].preList.size();

    }




}
#endif


void FlochaCodeTransfer::transToPdlCode(BinTreeNodeForTrans *node)
{
    BinTreeNodeForTrans *treenode = node;//is attrfrom's treenode, only can use in if(nextofend)
    FlochaAttriForTrans *floattr = node->startFloAttr;
    FlochaAttriForTrans *attrfrom;//is floattr's parent
    QString statement;

    std::vector<FlochaAttriForTrans*> notfinfork;

    bool inend = false;
    bool nextofend = false;//for attrfrom==if, another means is start of new node

    int noendif = 0;

    while(floattr)
    {
        if(nextofend)
        {
            nextofend = false;
            if(attrfrom->flocha->getflochaType() == flochaTypeIf)
            {
                if(attrfrom->nextAttr == floattr)
                {
                    if(treenode->rightNodeP != floattr->treeNode)
                    {
                        if(floattr->needLabel)
                        {

                            statement = QString("goto %1; \n").arg(floattr->label);
                            psudoCode.push_back(statement);
                            qDebug() << statement;

                            statement = QString("else do \n");
                            psudoCode.push_back(statement);
                            qDebug() << statement;


                            if(treenode != attrfrom->treeNode)
                            {
                                treenode = attrfrom->treeNode;
                            }
                            floattr = attrfrom->elseAttr;
                            nextofend = true;
                            continue;

                        }
                        else
                        {
                            QMessageBox::warning(nullptr, "debug", "maybe a bug");
                        }
                    }
                    else if(treenode->rightNodeP->startFloAttr)
                    {

                        //treenode = treenode->rightNodeP;
                        notfinfork.push_back(attrfrom);
                        floattr = treenode->rightNodeP->startFloAttr;
                        
                    }
                    else
                    {
                        QMessageBox::warning(nullptr, "debug", "maybe a bug");
                    }
                }
                else
                {
                    if(treenode->leftNodeP != floattr->treeNode)
                    {
                        if(floattr->needLabel)
                        {

                            statement = QString("goto %1; \n").arg(floattr->label);
                            psudoCode.push_back(statement);
                            qDebug() << statement;

                            --noendif;
                            statement = QString("end; \n");
                            psudoCode.push_back(statement);
                            qDebug() << statement;


                            if(notfinfork.size() > 0)
                            {
                                while(noendif > notfinfork.size())
                                {
                                    --noendif;
                                    statement = QString("end; \n");
                                    psudoCode.push_back(statement);
                                    qDebug() << statement;
                                }
                                attrfrom = notfinfork.back();
                                notfinfork.pop_back();
                                statement = QString("else do; \n");
                                psudoCode.push_back(statement);
                                qDebug() << statement;
                            }
                            else
                            {
                                break;
                            }
                            floattr = attrfrom->elseAttr;
                            treenode = attrfrom->treeNode;
                            nextofend =true;
                            continue;
                        }
                        else
                        {
                            QMessageBox::warning(nullptr, "error", "should need label");
                        }


                    }
                    else if(treenode->leftNodeP->startFloAttr)
                    {
                        //treenode = treenode->leftNodeP;
                        floattr = treenode->leftNodeP->startFloAttr;
                    }
                    else
                    {
                        QMessageBox::warning(nullptr, "debug", "maybe a bug");
                    }
                }

            }
            else
            {
                if(floattr->twoPreAreSibling)
                {
                    if(floattr->treeNode == floattr->treeNode->parent->rightNodeP)
                    {
                        statement = QString("else do; \n");
                        psudoCode.push_back(statement);
                        qDebug() << statement;

                        if(notfinfork.size() > 0)
                        {
                            attrfrom = notfinfork.back();
                            notfinfork.pop_back();
                        }
                        else
                        {
                            break;
                        }
                        floattr = attrfrom->elseAttr;
                        treenode = attrfrom->treeNode;
                        nextofend =true;
                        continue;

                    }
                    else
                    {
                        --noendif;
                        statement = QString("end; \n");
                        psudoCode.push_back(statement);
                        qDebug() << statement;

                    }

                }
                else
                {


                    statement = QString("goto %1; \n").arg(floattr->label);
                    psudoCode.push_back(statement);
                    qDebug() << statement;


                    //if(attrfrom->treeNode == attrfrom->treeNode->parent->rightNodeP)
                    //{
                    //    statement = QString("else do; \n");
                    //    psudoCode.push_back(statement);
                    //    qDebug() << statement;
                    //}
                    //else
                    if((attrfrom->treeNode == attrfrom->treeNode->parent->leftNodeP))
                    {
                        while(noendif > notfinfork.size())
                        {
                            --noendif;
                            statement = QString("end; \n");
                            psudoCode.push_back(statement);
                            qDebug() << statement;
                        }
                    }

                    if(notfinfork.size() > 0)
                    {
                        
                        attrfrom = notfinfork.back();
                        notfinfork.pop_back();
                        statement = QString("else do; \n");
                        psudoCode.push_back(statement);
                        qDebug() << statement;
                    }
                    else
                    {
                        break;
                    }
                    floattr = attrfrom->elseAttr;
                    treenode = attrfrom->treeNode;
                    nextofend =true;
                    continue;

                }


            }


        }

        if(floattr->treeNode != floattr->treeNode->parent->rightNodeP && floattr->treeNode != floattr->treeNode->parent->leftNodeP)
        {

        }


        if(floattr->needLabel)
        {
            statement = QString("%1:\n").arg(floattr->label);
            psudoCode.push_back(statement);
            qDebug() << statement;
        }

        switch (floattr->flocha->getflochaType())
        {
        case flochaTypeIf:
            {
                FlowchartsIf *floif = dynamic_cast<FlowchartsIf*>(floattr->flocha);
                QString operstr = IfOperatorClass::getOperStr(floif->getOperator()).first;
                statement = QString("if(%1 %2 %3) do //%4 \n").arg(floif->getStatement1()).arg(operstr).arg(floif->getStatement2()).arg(floattr->flocha->idName);
                ++noendif;
                psudoCode.push_back(statement);
                qDebug() << statement;
            }
            break;
        case flochaTypeSet:
            {
                FlowchartsSetVar *floset = dynamic_cast<FlowchartsSetVar*>(floattr->flocha);

                statement = QString("%1 = %2;//%3 \n").arg(floset->getVarName()).arg(floset->getStatement()).arg(floattr->flocha->idName);
                psudoCode.push_back(statement);
                qDebug() << statement;


            }
            break;
        case flochaTypeRun:
            {
                FlowchartsRunPDL *florun = dynamic_cast<FlowchartsRunPDL*>(floattr->flocha);

                statement = QString("%1;//%2 \n").arg(florun->getPDLName()).arg(floattr->flocha->idName);
                psudoCode.push_back(statement);
                qDebug() << statement;


            }
            break;
        case flochaTypeSleep:
            {
                FlowchartsSleep *flosleep = dynamic_cast<FlowchartsSleep*>(floattr->flocha);

                statement = QString("sleep %1;//%2 \n").arg(flosleep->getSleepTime()).arg(floattr->flocha->idName);
                psudoCode.push_back(statement);
                qDebug() << statement;

            }
            break;
        case flochaTypeAssem:
            {
                FlowchartsAssemble *floassem = dynamic_cast<FlowchartsAssemble*>(floattr->flocha);

                //QMessageBox::warning(nullptr, "error", "flocha is assem transToPdlCode()");
                transToPdlCode(floattr->subAttr->treeNode);


            }
            break;
        default:
            {
                if(notfinfork.size() == 0)
                {
                    while(noendif > 0)
                    {
                        --noendif;
                        statement = QString("end; \n");
                        psudoCode.push_back(statement);
                        qDebug() << statement;
                    }
                    return;
                }
                else
                {

                    if(floattr->treeNode->parent->rightNodeP == floattr->treeNode)
                    {
                        statement = QString("else do; \n");
                        psudoCode.push_back(statement);
                        qDebug() << statement;
                    }
                    else if(floattr->treeNode->parent->leftNodeP == floattr->treeNode)
                    {
                        --noendif;
                        statement = QString("end; \n");
                        psudoCode.push_back(statement);
                        qDebug() << statement;
                    }
                    else
                    {
                        QMessageBox::warning(nullptr, "debug", "maybe a bug");
                    }

                    attrfrom = notfinfork.back();
                    notfinfork.pop_back();
                    

                    floattr = attrfrom->elseAttr;
                    treenode = attrfrom->treeNode;
                    nextofend =true;
                    continue;
                }

            }

            break;
        }

        if(floattr->treeNode->endFloAttr == floattr)
        {
            nextofend = true;
        }
        
        
        attrfrom = floattr;
        treenode = attrfrom->treeNode;
        floattr = floattr->nextAttr;
        
        //if(floattr->flocha->getflochaType() == flochaTypeTemplate)
        //{
        //    nextofend = true;
        //}

    }
    while(noendif > 0)
    {
        --noendif;
        statement = QString("end; \n");
        psudoCode.push_back(statement);
        qDebug() << statement;
    }

}
/*
void FlochaCodeTransfer::transToPdlCode(FlowchartsAssemble* assem)
{
    FlowchartsTemplate *flocha = assem->getStartFlocha()->getNextPtr();

    std::vector<FlowchartsTemplate*> notfinfork;
    
    bool gobackfork = false;
    //std::vector<FlowchartsTemplate*> convergemap;
    bool needend = false;//only use in endflocha

    bool inelse = false;

    std::list<FlowchartsTemplate*> travedlist;

    unsigned int refnodeidx = 1;
    //std::map<FlowchartsTemplate*, QString> flolabelmap;//FlowchartsTemplate in map, implied that it has been traversal at least once
    //flolabelmap.clear();

    while(flocha)
    {
        QString statement;
        QString tempstr;

        if(notfinfork.size() != 0 && gobackfork)
        {
            if(flocha == notfinfork.back())
            {
                auto it_attr = flocahAttriMap.find(flocha);
                if(it_attr != flocahAttriMap.end())
                {
                    refnodeidx = it_attr->second.nodeNum * 2 + 1;
                }

                inelse = true;
                flocha = notfinfork.back()->getElsePtr();
                notfinfork.pop_back();
                gobackfork = false;
                statement = QString("else do \n");
                psudoCode.push_back(statement);
#ifdef _DEBUG
        qDebug() << statement;
#endif
            }
        }




        auto it_attr = flocahAttriMap.find(flocha);

        if(it_attr != flocahAttriMap.end())
        {
            int preisparent = 0;
            int parenthastrav = 0;

            if(it_attr->second.preList.size() > 1)
            {
                //////////////////////////////find the flocha's previous is been traversal, I have another idea to do this: use list of fork-head

                for(auto it_pre = it_attr->second.preList.begin(); it_pre != it_attr->second.preList.end(); ++it_pre)
                {
                    bool isparent = false;

                    auto it_attr2 = flocahAttriMap.find(*it_pre);
                    if(it_attr2 != flocahAttriMap.end())
                    {
                        if((it_attr2->second.nodeNum * 2) == refnodeidx)
                        {
                            ++preisparent;
                            isparent = true;
                        }
                        else if((it_attr2->second.nodeNum * 2 + 1) == refnodeidx)
                        {
                            ++preisparent;
                            isparent = true;
                        }

                        if(isparent)
                        {
                            for(auto it_trav = travedlist.begin(); it_trav != travedlist.end(); ++it_trav)
                            {
                                if((*it_pre) == (*it_trav))
                                {
                                    ++parenthastrav;
                                    break;
                                }
                            }
                            
                        }
                    }
                }
                //////////////////////////////find the flocha's previous is been traversal





                //auto it_label = flolabelmap.find(flocha);
                if((it_attr->second.nodeNum != refnodeidx))
                {
                    if(preisparent == 2 && (parenthastrav == 2))
                    {
                        needend = false;
                        inelse = false;
                        statement = QString("end; \n");
                        psudoCode.push_back(statement);

                        #ifdef _DEBUG
        qDebug() << statement;
#endif
                    }
                    else
                    {
                        statement = QString("goto ") + it_attr->second.label;
                        psudoCode.push_back(statement + "\n");


                        #ifdef _DEBUG
        qDebug() << statement;
#endif



                        if(notfinfork.size() != 0)
                        {
                            gobackfork = true;
                            flocha = notfinfork.back();

                            if(inelse)
                            {
                                statement = QString("end; \n");
                                psudoCode.push_back(statement);
                                inelse = false;

                                #ifdef _DEBUG
        qDebug() << statement;
#endif

                            }
                            continue;
                        }
                        else
                        {
                            statement = QString("end; \n");
                            psudoCode.push_back(statement);


                            #ifdef _DEBUG
        qDebug() << statement;
#endif

                            //QMessageBox::warning(NULL, "waring", "debuging qqq188");
                            return;
                        }

                    }


                }


                if(it_attr->second.needLabel)
                {
                    psudoCode.push_back(it_attr->second.label + ":");
                    #ifdef _DEBUG
        qDebug() << it_attr->second.label + ":";
#endif
                }

            }


            psudoCode.push_back(QString("//Node number:%1").arg(it_attr->second.nodeNum));
            #ifdef _DEBUG
        qDebug() << QString("//Node number:%1").arg(it_attr->second.nodeNum);
#endif

        }
#ifdef _DEBUG
        else
        {
            QMessageBox::warning(NULL, "error","flocahAttriMap not found flocha");
        }
#endif





        switch (flocha->getflochaType())
        {
        case flochaTypeIf:
            {
            FlowchartsIf *floif = dynamic_cast<FlowchartsIf*>(flocha);
            QString operstr = IfOperatorClass::getOperStr(floif->getOperator()).first;
            statement = QString("if(%1 %2 %3) do //%4 \n").arg(floif->getStatement1()).arg(operstr).arg(floif->getStatement2()).arg(flocha->idName);
            psudoCode.push_back(statement);

            auto it_attr = flocahAttriMap.find(flocha);
            if(it_attr != flocahAttriMap.end())
            {
                refnodeidx = it_attr->second.nodeNum * 2;
            }
            inelse = false;
            notfinfork.push_back(flocha);
            needend = true;
            
            }
            break;
        case flochaTypeSet:
            {
            FlowchartsSetVar *floset = dynamic_cast<FlowchartsSetVar*>(flocha);

            statement = QString("%1 = %2;//%3 \n").arg(floset->getVarName()).arg(floset->getStatement()).arg(flocha->idName);
            psudoCode.push_back(statement);


            }
            break;
        case flochaTypeRun:
            {
            FlowchartsRunPDL *florun = dynamic_cast<FlowchartsRunPDL*>(flocha);

            statement = QString("%1;//%2 \n").arg(florun->getPDLName()).arg(flocha->idName);
            psudoCode.push_back(statement);


            }
            break;
        case flochaTypeSleep:
            {
            FlowchartsSleep *flosleep = dynamic_cast<FlowchartsSleep*>(flocha);

            statement = QString("sleep %1;//%2 \n").arg(flosleep->getSleepTime()).arg(flocha->idName);
            psudoCode.push_back(statement);

            }
            break;
        case flochaTypeAssem:
            {
            FlowchartsAssemble *floassem = dynamic_cast<FlowchartsAssemble*>(flocha);
            transToPdlCode(floassem);


            }
            break;
        default:
            
            if(notfinfork.size() != 0)
            {
                needend = true;
                gobackfork = true;
                flocha = notfinfork.back();
                continue;
            }
            else
            {
                if(needend)
                {
                    statement = QString("end; \n");
                    psudoCode.push_back(statement);
                }
                return;
            }
            
            break;
        }
#ifdef _DEBUG
        qDebug() << statement;
#endif

        travedlist.push_back(flocha);
        flocha = flocha->getNextPtr();
    }

}
*/


void FlochaCodeTransfer::transToPsudoCode(FlowchartsAssemble* assem)
{







}

void FlochaCodeTransfer::transToActualCode()
{



}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

CodeToHiliHtml::CodeToHiliHtml(QObject *parent)
    : QObject(parent)
{
    keywordColor = "<font color=\"#0000ff\">";
    typeClaimColor = "<font color=\"#000099\">";
    commentColor = "<font color=\"#00ff00\">";
    digitColor = "<font color=\"#ff00ff\">";
    stringColor = "<font color=\"#0099ff\">";
    fontEnd = "</font>";


    keywordList.push_back("if");
    keywordList.push_back("for");
    keywordList.push_back("goto");
    keywordList.push_back("while");
    keywordList.push_back("else");
    keywordList.push_back("end");
    keywordList.push_back("do");
    keywordList.push_back("sin");
    keywordList.push_back("cos");



    specialCharMap.insert(std::make_pair("<", "&lt;"));
    specialCharMap.insert(std::make_pair(">", "&gt;"));
    specialCharMap.insert(std::make_pair("-", "&ndash;"));
    specialCharMap.insert(std::make_pair("¡P", "&middot;"));
    

}
CodeToHiliHtml::~CodeToHiliHtml()
{

}

void CodeToHiliHtml::startCodeToHiliHtml(const QStringList &src, QStringList &dest)
{
    int wordstart = -1;
    int wordend = -1;

    int indentnum = 0;
    bool addtionindent = false;

    dest.clear();

    for(int i = 0; i < src.size(); ++i)
    {
        QString linestr = src.at(i);
        int indx = -1;

        for(auto it = specialCharMap.begin(); it != specialCharMap.end(); ++it)
        {
            linestr.replace(it->first,it->second);
        }



        addStringHtmlColor(linestr);

        addtionindent = true;
        for(auto it = keywordList.begin(); it != keywordList.end(); ++it)
        {
            int result = replaceString(linestr, (*it), keywordColor +  (*it) + fontEnd);

            if((*it) == "if")
            {
                if(result == 0 || result == 1)
                {
                    ++indentnum;
                    addtionindent = false;
                }
            }
            else if((*it) == "else")
            {
                if(result == 0 || result == 1)
                {
                    addtionindent = false;
                }
            }
            else if((*it) == "end")
            {
                if(result == 0 || result == 1)
                {
                    --indentnum;
                    addtionindent = true;
                }
            }
            else
            {
                
            }

        }

        for(auto it = typeClaimList.begin(); it != typeClaimList.end(); ++it)
        {
            replaceString(linestr, (*it), typeClaimColor +  (*it) + fontEnd);
        }


        if(addtionindent)
        {
            for(int i = 0; i < indentnum; ++i)
            {
                linestr.insert(0,"........");
            }
        }
        else
        {
            for(int i = 0; i < indentnum - 1; ++i)
            {
                linestr.insert(0,"........");
            }
        }

        linestr.insert(linestr.size(),"<br><br>");


        dest.push_back(linestr);
    }
}



//return -1: no replace  0:repalce start of line  1: repalce start of line and middle of line  2:only replace middle of line
int CodeToHiliHtml::replaceString(QString &srcstr, const QString &fromstr, const QString &tostr)
{
    int rtn = -1;
    int lento = tostr.size();
    int lenfrom = fromstr.size();
    int idx = 0;
    int tmpidx = 0;
    
    while(1)
    {
        if(tmpidx == 0)
        {
            idx = srcstr.indexOf(QRegularExpression("^" + fromstr + "[^\\w]"));
            tmpidx = idx;
            if(idx == 0)
            {
                srcstr.replace(tmpidx, lenfrom, tostr);
                idx = lento + tmpidx;
                rtn = 0;
            }
            idx = 0;
            tmpidx = 1;
            continue;
        }
        else
        {
            idx = srcstr.indexOf(QRegularExpression("[^\\w]" + fromstr + "[^\\w]"), idx);
            tmpidx = idx + 1;
        }

        if(idx != -1)
        {
            srcstr.replace(tmpidx, lenfrom, tostr);
            idx = lento + tmpidx;
            if(rtn == 0)
            {
                rtn = 1;
            }
            else
            {
                rtn = 2;
            }
        }
        else
        {
            break;
        }
    }

    return rtn;
}


void CodeToHiliHtml::addStringHtmlColor(QString &srcstr)
{
    int idx = 0;
    int tmpidx = 0;

    QString digit;
    bool indigit = false;
    bool lastisw = false;
    bool lastisslash = false;
    bool lastisbackslash = false;
    bool lastisdbqout = false;
    //QRegularExpression regex1("^([\\d]+\\.?[\\d]*)[^\\w]");
    //QRegularExpression regex2("[^\\w]([\\d]+\\.?[\\d]*)[^\\w]");
    
    while(idx < srcstr.size())
    {
        char cchar = srcstr.at(idx).toLatin1();

        if(cchar == '/') {
            if(lastisbackslash)
            {
                srcstr.insert(idx - 1, commentColor);
                srcstr.insert(srcstr.size(), fontEnd);
                break;
            }
            lastisbackslash = true;
        }else {
            lastisbackslash = false;
        }

        switch (cchar)
        {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            {
                if(indigit)
                {
                    digit.push_back(cchar);
                }
                else if(!lastisw)
                {
                    tmpidx = idx;
                    indigit = true;
                    digit.clear();
                    digit.push_back(cchar);
                }

            }
            break;
        case '.':
            {
                if(indigit)
                {
                    digit.push_back(cchar);
                }
            }
            break;
        
            break;
        default:
            if((cchar > 65) && (cchar < 122) && (cchar != 96) && (cchar != 94) && (cchar != 92))
            {
                lastisw = true;
            }
            else
            {
                lastisw = false;
            }

            if(indigit)
            {
                int len = idx - tmpidx;
                QString colorhtml = digitColor + digit + fontEnd;

                srcstr.replace(tmpidx, len, colorhtml);

                idx = tmpidx + colorhtml.size();
            }
            indigit = false;

            break;
        }

        idx += 1;
    }
    
    /*
    while(1)
    {
        if(tmpidx == 0)
        {
            idx = srcstr.indexOf(regex1);
            digit = regex1.match(srcstr, idx).captured(1);
            if(idx == 0)
            {
                srcstr.replace(idx, digit.size(), digitColor + digit + fontEnd);
                idx += tmpidx + digit.size();
            }
            tmpidx = 1;
            continue;
        }
        else
        {
            idx = srcstr.indexOf(regex1, idx);
            digit = regex2.match(srcstr, idx).captured(1);
            tmpidx = idx + 1;
        }

        if(idx != -1)
        {
            srcstr.replace(idx, digit.size(), digitColor + digit + fontEnd);
            idx += tmpidx + digit.size();
        }
        else
        {
            break;
        }
    }
    */
}




//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

CodeEditorParser::CodeEditorParser(QWidget *parent)
    : QTextEdit(parent)
{
}

CodeEditorParser::~CodeEditorParser()
{

}

void CodeEditorParser::setCodeThenDraw(const QStringList &code)
{

    QStringList html;
    QTextCursor cursor = this->textCursor();
    
    CodeToHiliHtml tohtml(this);

    tohtml.startCodeToHiliHtml(code, html);

    for(int i = 0; i < html.size(); ++i)
    {
        this->insertHtml(html.at(i));
        cursor.movePosition(QTextCursor::End);
        this->setTextCursor(cursor);
    }


    /*
    QTextCursor cursor = this->textCursor();
    for(int linenum = 0; linenum < code.size(); ++linenum)
    {
        QTextCharFormat format;
        format.setForeground(QBrush(QColor(linenum*10,0,0)));
        append(code.at(linenum));

        cursor.setPosition(0);
        cursor.setPosition(5, QTextCursor::KeepAnchor);
        cursor.mergeBlockCharFormat(format);
        this->setCurrentCharFormat(format);
//        this->setTextColor(QColor(linenum*10,0,0));

        cursor.movePosition(QTextCursor::End);
        this->setTextColor(QColor(0,0,0));
    }
    */


}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


FlochaToCodeWidget::FlochaToCodeWidget(QWidget *parent)
    : QWidget(parent)
{
    objectAssem = NULL;
    createUI();
    codeTranslator = new FlochaCodeTransfer(this);

    setTextNeedTrans();
}

FlochaToCodeWidget::~FlochaToCodeWidget()
{


}

void FlochaToCodeWidget::setFlochaAssem(FlowchartsAssemble *assem)
{
    objectAssem = assem;
}


void FlochaToCodeWidget::setTextNeedTrans()
{
    ui_act_genCode->setText(tr("Start translate"));
}


void FlochaToCodeWidget::createUI()
{

    mainLayout = new QGridLayout(this);

    

    ui_act_genCode = new QAction(this);
    

    connect(ui_act_genCode, &QAction::triggered, this, &FlochaToCodeWidget::startGenCode);

    ui_toolBar = new QToolBar(this);

    ui_toolBar->addAction(ui_act_genCode);
    mainLayout->addWidget(ui_toolBar, 0, 0);


    ui_textEdit_Code = new CodeEditorParser(this);

    mainLayout->addWidget(ui_textEdit_Code, 1, 0);



    this->setLayout(mainLayout);
}


void FlochaToCodeWidget::startGenCode()
{
    QStringList code;

    if(!objectAssem)
    {
        return;
    }
    ui_textEdit_Code->clear();

    codeTranslator->startTransToCode(objectAssem, code);


    ui_textEdit_Code->setCodeThenDraw(code);


}