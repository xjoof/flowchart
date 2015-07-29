#include "flowchartsitems.h"
#include "xmlflochatransfer.h"


FloRunHandleClass::FloRunHandleClass()
{
    threadHandle = NULL;
    eventHandle = CreateEvent(NULL,false,false,NULL);
    isRunning = false;
    caller = NULL;
}

FloRunHandleClass::~FloRunHandleClass()
{
    isRunning = false;
    if(threadHandle)
    {
        WaitForSingleObject( threadHandle, INFINITE );
        CloseHandle(threadHandle);
    }
    if(eventHandle)
        CloseHandle(eventHandle);
}


FloRunHandleClass floRunHandle[FLOTHREADMAXNUM];




FlowchartsSleep::FlowchartsSleep(QObject *parent)
    : FlowchartsTemplate(parent)
{
    sleepTime = 0;
    timer.setSingleShot(true);
    //connect(&timer,&QTimer::timeout,this,&FlowchartsTemplate::EndSignal);
}

FlowchartsSleep::~FlowchartsSleep()
{

}

int FlowchartsSleep::getflochaType()
{
    return flochaTypeSleep;
}


FlowchartsTemplate * FlowchartsSleep::startTemplate()
{
    //	timer.start(sleepTime);
    Sleep(sleepTime);
    //	emit EndSignal();
    return p_next;
}


void FlowchartsSleep::setSleepTime(const QString &statement)
{
    sleepTime = statement.toInt();
    if(sleepTime < 0)
        sleepTime = 0;
}



QString FlowchartsSleep::getSleepTime()
{
    return QString("%1").arg(sleepTime);
}







//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------


FlowchartsIf::FlowchartsIf(QObject *parent,CommandManager *cmd)
    : FlowchartsTemplate(parent)
{
    if(cmd)
    {
        statementTool1 = new StatementRunTool(this,cmd);
        statementTool2 = new StatementRunTool(this,cmd);
    }
    else
    {
        statementTool1 = nullptr;
        statementTool2 = nullptr;
    }
    operatorType = 1;//equal to
}

FlowchartsIf::~FlowchartsIf()
{
    if(statementTool1)
        delete statementTool1;
    if(statementTool2)
        delete statementTool2;
}

int FlowchartsIf::getflochaType()
{
    return flochaTypeIf;
}


FlowchartsTemplate * FlowchartsIf::startTemplate()
{
    // cmd = NULL is for copy.
    if((!statementTool1) || (!statementTool2))
        return nullptr;


    double var1;
    double var2;
    statementTool1->calculateStatement(&var1);
    statementTool2->calculateStatement(&var2);

    //	qDebug() << "var1" << var1 << "var2" << var2;


    if (IfOperatorClass::CompareTwoArg(&var1, &var2, operatorType))
    {

        //		qDebug() << "if true";
        return p_next;
    }
    else
    {
        //		qDebug() << "if false";
        return p_else;
    }

}


void FlowchartsIf::setStatement1(const QString &statement)
{
    ifStatement1 = statement;

    // cmd = NULL is for copy.
    if(statementTool1)
        statementTool1->parseStatement(statement);
}

void FlowchartsIf::setStatement2(const QString &statement)
{

    ifStatement2 = statement;
    // cmd = NULL is for copy.
    if(statementTool2)
        statementTool2->parseStatement(statement);

}
void FlowchartsIf::setOperator(int op)
{
    operatorType = op;
}



QString FlowchartsIf::getStatement1()
{
    return ifStatement1;
}

QString FlowchartsIf::getStatement2()
{
    return ifStatement2;
}

int FlowchartsIf::getOperator()
{
    return operatorType;
}


bool FlowchartsIf::parseStatement()
{
    bool rtn = false;





    return rtn;
}




//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------

FlowchartsSetVar::FlowchartsSetVar(QObject *parent,CommandManager *cmd)
    : FlowchartsTemplate(parent)
{
    memset(varName,0,sizeof(varName));
    memset(&arrayVarInfo,0,sizeof(arrayVarInfo));
    commandManager = cmd;
    if(cmd)
    {
        statementTool = new StatementRunTool(this,cmd);
        for(int i = 0; i < ARRAYDIMENSION; ++i)
        {
            statementTool_idx[i] = new StatementRunTool(this,cmd);
        }
    }
    else
    {
        statementTool = nullptr;
        for(int i = 0; i < ARRAYDIMENSION; ++i)
        {
            statementTool_idx[i] = nullptr;
        }
    }
}

FlowchartsSetVar::~FlowchartsSetVar()
{
    delete statementTool;
    for(int i = 0; i < ARRAYDIMENSION; ++i)
    {
        delete statementTool_idx[i];
    }
}

int FlowchartsSetVar::getflochaType()
{
    return flochaTypeSet;
}


FlowchartsTemplate * FlowchartsSetVar::startTemplate()
{

    double value;
    double idxdouble;
    unsigned int idxint = 0;
    int result;
    int vardim = arrayVarInfo.dimension;
    // cmd = NULL is for copy.
    if((!statementTool) || (!commandManager))
        return nullptr;


    


    result = statementTool->calculateStatement(&value);

    if(result == -1)
        return nullptr;
    if(varName[0] == 0)
        return nullptr;

    for(int i = 0; i < vardim; ++i)
    {
        result = statementTool_idx[i]->calculateStatement(&idxdouble);
        if(result != -1)
        {
            int size = 1;
            for(int j = i; j < vardim - 1; ++j)
            {
                size *= arrayVarInfo.dimensionSize[j+1];
            }
            idxint += (unsigned int)idxdouble * size;
        }
    }


#ifdef _DEBUG	
    //	qDebug() << "name" << varName << "value" << value;
#endif


    if(commandManager != nullptr)
    {
        if(vardim)
        {
            commandManager->setVarOfArray(varID, idxint, value);
        }
        else
        {
            commandManager->setVar(varName, value);
        }
    }


    return p_next;
}

void FlowchartsSetVar::setVarName(const QString &name)
{
    QString nametmp = name;
    ArrayInfoForParse arrayinfo;
    int result = StatementRunTool::parseVarName(nametmp, &arrayinfo, idxStatement, statementTool_idx, commandManager);

    if(result)
    {
        nametmp = name;
    }

    memset(varName,0,sizeof(varName));
    for(int i = 0; i<nametmp.size(); ++i)
        varName[i] = nametmp.at(i).toLatin1();
}

void FlowchartsSetVar::setStatement(const QString &statement)
{
    valueStatement = statement;

    // cmd = NULL is for copy.
    if(statementTool)
        int result = statementTool->parseStatement(statement);

}

QString FlowchartsSetVar::getStatement()
{
    return valueStatement;
}

QString FlowchartsSetVar::getVarName()
{
    QString var(varName);
    for(int i = 0; i < arrayVarInfo.dimension; ++i)
    {
        var += "[" + idxStatement[i] + "]";
    }

    return var;
}
//--------------------------------
//--------------------------------
//--------------------------------
FlowchartsRunPDL::FlowchartsRunPDL(QObject *parent, CommandManager *cmd)
    : FlowchartsTemplate(parent)
{
    memset(pdlName,0,sizeof(pdlName));
    commandManager = cmd;
    qDebug() << sizeof(pdlName);
}

FlowchartsRunPDL::~FlowchartsRunPDL()
{

}

int FlowchartsRunPDL::getflochaType()
{
    return flochaTypeRun;
}


FlowchartsTemplate * FlowchartsRunPDL::startTemplate()
{
    //	qDebug() << "run pdl";

    char err[80];

    if(pdlName[0] != 0)
        if(commandManager != nullptr)
            commandManager->runHiwinPDLFunc(pdlName);

    return p_next;
}

void FlowchartsRunPDL::setPDLName(const QString &name)
{
    memset(pdlName,0,sizeof(pdlName));
    for(int i = 0; i<name.size(); ++i)
        pdlName[i] = name.at(i).toLatin1();
}


QString FlowchartsRunPDL::getPDLName()
{
    QString pdl(pdlName);
    return pdl;
}

//--------------------------------
//--------------------------------
//--------------------------------
FlowchartsAssemble::FlowchartsAssemble(QObject *parent, CommandManager *cmd)
    : FlowchartsTemplate(parent)
{
    childFlochaList.clear();
    //	runFlag.all = 0;
    runThreadNum = -1;
    startflocha = new FlowchartsTemplate(this);
    endflocha = new FlowchartsTemplate(this);
    char start[] = XmlEleTagValueStart;
    char end[] = XmlEleTagValueEnd;
    memcpy(startflocha->idName,start,sizeof(start));
    memcpy(endflocha->idName,end,sizeof(start));

    
    assemAttr.runLoopTimes = 0;
    assemAttr.sleepPerLoop = 0;
    assemAttr.isFunction = false;

    commandManager = cmd;

    startflocha->itemPos[0] = 200;
    startflocha->itemPos[1] = 50;

    endflocha->itemPos[0] = 200;
    endflocha->itemPos[1] = 300;



    addTemplate(startflocha);
    addTemplate(endflocha);

    //connect(endflocha,&FlowchartsTemplate::EndSignal,this,&FlowchartsTemplate::EndSignal);



}

FlowchartsAssemble::~FlowchartsAssemble()
{
#ifdef _DEBUG
    qDebug() << "start ~FlowchartsAssemble()";
#endif

    //	delete startflocha;
    //	startflocha = nullptr;
    //	delete endflocha;
    //	endflocha = nullptr;

    for(auto it = childFlochaList.begin(); it != childFlochaList.end(); ++it)
    {
        delete *it;
    }
    childFlochaList.clear();



#ifdef _DEBUG
    qDebug() << "finish ~FlowchartsAssemble()";
#endif
}


int FlowchartsAssemble::getflochaType()
{
    return flochaTypeAssem;
}


void FlowchartsAssemble::startRunAssem()
{
    //	startflocha->startTemplate();
    bool runsecess = false;
    unsigned int pThreadId=0;

    for(int i = 0; i < FLOTHREADMAXNUM; ++i)
    {
        if(!floRunHandle[i].isRunning)
        {

            CloseHandle(floRunHandle[i].threadHandle);

            //0:=Running  ;CREATE_SUSPENDED:= Suspend
            floRunHandle[i].threadHandle = (HANDLE)_beginthreadex(NULL,0,  (unsigned (_stdcall *)(void *))AssemFlochaRunThread, (LPVOID)this,CREATE_SUSPENDED,&pThreadId); 

            if(floRunHandle[i].threadHandle)
            {
                //	runFlag.bits.running = 1;
                floRunHandle[i].isRunning = true;
                ResetEvent(floRunHandle[i].eventHandle);
                floRunHandle[i].caller = this;
                runThreadNum = i;
                runsecess = true;

                ResumeThread(floRunHandle[i].threadHandle);
                break;
            }



        }
    }

    return;
}




FlowchartsTemplate * FlowchartsAssemble::startTemplate()
{
    if(assemAttr.runLoopTimes)
    {
        endflocha->setNextPtr(startflocha);
        runLoopCnt = 0;
        loopRunCntVarIndx = 0;
        ArrayDimensInfo info;
        memset(&info, 0, sizeof(ArrayDimensInfo));
        if(commandManager && (assemAttr.loopRunCntVar.size() != 0))
            commandManager->editVarMap(&loopRunCntVarIndx, assemAttr.loopRunCntVar, info);
    }

    return startflocha;
}



FlowchartsTemplate *FlowchartsAssemble::getStartFlocha()
{
    return startflocha;
}

FlowchartsTemplate *FlowchartsAssemble::getEndFlocha()
{
    return endflocha;
}

/*
int FlowchartsAssemble::buildFlochaAssem(std::vector<FlowchartsStruct> FlowchartsData)
{
    int rtn = 0;

    return rtn;
}


std::vector<FlowchartsStruct> FlowchartsAssemble::outputFlochaAssem()
{
    std::vector<FlowchartsStruct> rtn;
    return rtn;
}
*/


int FlowchartsAssemble::linkTwoTemplate(FlowchartsTemplate *flowChaT, FlowchartsTemplate *next, bool linkElse)
{

    int rtn = 0;
    auto it = std::find(childFlochaList.begin(), childFlochaList.end(), flowChaT);


    if(!linkElse)
    {
        if((*it)->getNextPtr() != 0)
            unlinkTemplate(flowChaT,UNLINKNEXT);

        (*it)->setNextPtr(next);
        //		template_Struct_Map.find(next)->second.p_prev = flowChaT;
        //connect(flowChaT,&FlowchartsTemplate::EndSignal,next,&FlowchartsTemplate::startTemplate);
    }
    else
    {
        if((*it)->getflochaType() != flochaTypeIf)
            rtn = -1;
        else
        {
            if((*it)->getElsePtr() != 0)
                unlinkTemplate(flowChaT,UNLINKELSE);

            (*it)->setElsePtr(next);
            //			template_Struct_Map.find(next)->second.p_prev = flowChaT;
            //connect(flowChaT,&FlowchartsTemplate::EndSignal_else,next,&FlowchartsTemplate::startTemplate);
        }

    }
    return rtn;
}


int FlowchartsAssemble::unlinkTemplate(FlowchartsTemplate *flowChaT, int type)
{
    int rtn = 0;

    auto it = std::find(childFlochaList.begin(), childFlochaList.end(), flowChaT);

    if(type == UNLINKALL || type == UNLINKPREV)
    {
        for(auto it_pre = childFlochaList.begin(); it_pre != childFlochaList.end(); ++it_pre)
        {
            if((*it_pre)->getNextPtr() == flowChaT)
                (*it_pre)->setNextPtr(nullptr);

            if((*it_pre)->getElsePtr() == flowChaT)
                (*it_pre)->setElsePtr(nullptr);
        }

        /*
        auto it_pre = template_Struct_Map.find(it->second.p_prev);
        (*it)->p_prev = 0;
        if(it->second.fromElseLink == 1)
        {
        disconnect(it_pre->first,&FlowchartsTemplate::EndSignal_else,flowChaT,&FlowchartsTemplate::startTemplate);
        it_pre->second.p_else = 0;

        }
        else if(it->second.fromElseLink == 0)
        {
        disconnect(it_pre->first,&FlowchartsTemplate::EndSignal,flowChaT,&FlowchartsTemplate::startTemplate);
        it_pre->second.p_next = 0;
        }
        */
    }


    if(type == UNLINKALL || type == UNLINKNEXT)
    {
        //disconnect(flowChaT,&FlowchartsTemplate::EndSignal,flowChaT->p_next,&FlowchartsTemplate::startTemplate);
        flowChaT->setNextPtr(nullptr);

    }

    if(type == UNLINKALL || type == UNLINKELSE)
    {

        //disconnect(flowChaT,&FlowchartsTemplate::EndSignal_else,flowChaT->p_else,&FlowchartsTemplate::startTemplate);
        flowChaT->setElsePtr(nullptr);
    }

    return rtn;
}


int FlowchartsAssemble::addTemplate(FlowchartsTemplate *flowChaT)
{
    int rtn = 0;

    auto it = std::find(childFlochaList.begin(), childFlochaList.end(), flowChaT);

    if(it == childFlochaList.end())
    {
        childFlochaList.push_back(flowChaT);
        rtn = 1;
    }
    return rtn;
}


int FlowchartsAssemble::removeTemplate(FlowchartsTemplate *flowChaT)
{
    int rtn = 0;
    auto it = std::find(childFlochaList.begin(), childFlochaList.end(), flowChaT);

    if(it != childFlochaList.end())
    {
        unlinkTemplate(flowChaT);
        //		childFlochaList.erase(flowChaT);
        childFlochaList.erase(it);

        //remove not delete
        //		delete flowChaT;
        //		flowChaT = nullptr;

    }

    return rtn;
}



const std::list<FlowchartsTemplate *> & FlowchartsAssemble::getFlochaList()
{
    return childFlochaList;
}

void FlowchartsAssemble::setNextPtr(FlowchartsTemplate *nextptr)
{
    p_next = nextptr;
    endflocha->setNextPtr(nextptr);
}

/*
void FlowchartsAssemble::setLoopRunTimes(unsigned int looptimes)
{
    runLoopTimes = looptimes;
}

void FlowchartsAssemble::setSleepTimePerLoop(unsigned int sleepperloop)
{
    sleepPerLoop = sleepperloop;
}
void FlowchartsAssemble::setLoopRunCntStr(const QString &cntstr)
{
    loopRunCntVar = cntstr;
}

unsigned int FlowchartsAssemble::getLoopRunTimes()
{
    return runLoopTimes;
}

unsigned int FlowchartsAssemble::getSleepTimePerLoop()
{
    return sleepPerLoop;
}
QString FlowchartsAssemble::getLoopRunCntStr()
{
    return loopRunCntVar;
}
*/

void FlowchartsAssemble::setAssemAttr(const FlochaAssemAttr &attr)
{
    assemAttr = attr;
}

void FlowchartsAssemble::getAssemAttr(FlochaAssemAttr *attr)
{
    *attr = assemAttr;
}


const FloRunFlagStuct &FlowchartsAssemble::getRunFlag()
{
    return runFlag;
}
void FlowchartsAssemble::setRunFlag(const FloRunFlagStuct &flag)
{
    runFlag = flag;
}

void FlowchartsAssemble::setContinueRunEvent()
{
    if(runThreadNum == -1)
        return;
    if(!floRunHandle[runThreadNum].isRunning)
        return;
    SetEvent(floRunHandle[runThreadNum].eventHandle);
}


int FlowchartsAssemble::getThreadRunNum()
{
    return runThreadNum;
}

bool FlowchartsAssemble::whetherRunning()
{
    bool rtn = true;
    if(runThreadNum == -1)
        rtn = false;
    else 
        rtn = floRunHandle[runThreadNum].isRunning;

    return rtn;
}

void FlowchartsAssemble::stopRunning()
{
    if(runThreadNum == -1)
        return;
    else if(!floRunHandle[runThreadNum].isRunning)
        return;


    floRunHandle[runThreadNum].isRunning = 0;
    SetEvent(floRunHandle[runThreadNum].eventHandle);
}

void FlowchartsAssemble::hitBreakPoint(FlowchartsTemplate *flocha)
{
    emit hitBreakPointSignal(flocha);
}

void FlowchartsAssemble::runAssemFinshed(int result)
{
    emit runFinishedSignal(result);
}

void FlowchartsAssemble::additionFuncCallInChild()
{
    if(!assemAttr.runLoopTimes)
        return;

    ++runLoopCnt;
    if(runLoopCnt & 0x01)//startflo no need
        return;

    if(commandManager && (assemAttr.loopRunCntVar.size() != 0))
        commandManager->setVar(assemAttr.loopRunCntVar, runLoopCnt >> 1);


    if(assemAttr.sleepPerLoop)
        Sleep(assemAttr.sleepPerLoop);


    if(runLoopCnt >= assemAttr.runLoopTimes * 2)
    {
        endflocha->setNextPtr(p_next);
        if(commandManager)// && (loopRunCntVar.size() != 0)) need not to check here
            commandManager->eraseVarMap(loopRunCntVarIndx);
    }

}

//const HANDLE FlowchartsAssemble::getContinueEvent()
//{
//	return continueEvent;
//}

//--------------------------------
//--------------------------------
//--------------------------------




unsigned int WINAPI AssemFlochaRunThread(void *floassem)
{
    FlowchartsAssemble *targetassem = (FlowchartsAssemble *)floassem;
    FlowchartsTemplate *floprerun = nullptr;
    FlowchartsTemplate *flotorun = targetassem->startTemplate();
    bool isdebug = targetassem->getRunFlag().bits.debug;
    int threadid = targetassem->getThreadRunNum();
    int rtn = 0;

    if(threadid == -1)
    {
        rtn = -1;
        return rtn;
    }

    while(floRunHandle[threadid].isRunning)// && targetassem->getRunFlag().bits.running)
    {
        if(!flotorun)
        {
            rtn = -1;
            break;
        }
        floprerun = flotorun;

        if(isdebug)
        {
            if(targetassem->getRunFlag().bits.stepRun)
            {
                targetassem->hitBreakPoint(flotorun);
                WaitForSingleObject(floRunHandle[threadid].eventHandle,INFINITE);
            }
            else if(flotorun->getbreakPointFlag())
            {
                targetassem->hitBreakPoint(flotorun);
                WaitForSingleObject(floRunHandle[threadid].eventHandle,INFINITE);
            }
        }

        //if(flotorun->getflochaType() == flochaTypeAssem)
        //	flotorun = dynamic_cast<FlowchartsAssemble*>(flotorun)->getStartFlocha();
        //else
        flotorun = flotorun->startTemplate();

    }
    if(floprerun == targetassem->getEndFlocha())
        targetassem->runAssemFinshed(0);
    else
        targetassem->runAssemFinshed(1);

    floRunHandle[threadid].isRunning = false;

    return rtn;

}