#ifndef FLOWCHARTSITEMS_H
#define FLOWCHARTSITEMS_H

#include <windows.h>
#include <process.h>
#include <tchar.h>

#include <QObject>
#include <QDebug>
#include <iostream>
#include <map>
#include <QString>
#include <QChar>
#include <QTimer>

#include "flowchartsTemplate.h"


//#define startFlochaIdname "virtualstartflocha"
//#define endFlochaIdname "virtualendflocha"

#define FLOTHREADMAXNUM 3

struct FloRunFlagBits
{
    //	BYTE running :1;
    BYTE stepRun :1;
    BYTE debug :1;
};


union FloRunFlagStuct
{
    BYTE all;
    FloRunFlagBits bits;
};

class FloRunHandleClass
{
    friend FlowchartsAssemble;
    friend unsigned int WINAPI AssemFlochaRunThread(void *);
public:
    FloRunHandleClass();
    ~FloRunHandleClass();
    HANDLE threadHandle;
    HANDLE eventHandle;
    FlowchartsAssemble *caller;
private:
    bool isRunning;


};


enum flochaError
{
    flochaTypeNotFind,

};



#define UNLINKALL 0
#define UNLINKPREV 1
#define UNLINKNEXT 2
#define UNLINKELSE 3


typedef struct 
{
    int fromElseLink;//1->yes 0->no -1->no prev
    FlowchartsTemplate *p_floChaT;
    FlowchartsTemplate *p_prev;
    FlowchartsTemplate *p_next;
    FlowchartsTemplate *p_else;
}FlowchartsStruct;






class FlowchartsSleep : public FlowchartsTemplate
{
    Q_OBJECT

public:

    FlowchartsSleep(QObject *parent);
    ~FlowchartsSleep();

    int getflochaType();
    FlowchartsTemplate * startTemplate();
    void setSleepTime(const QString &statement);

    QString getSleepTime();

protected:


private:
    QTimer timer;
    int sleepTime;
};


//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------




class FlowchartsIf : public FlowchartsTemplate
{
    Q_OBJECT

public:

    FlowchartsIf(QObject *parent,CommandManager *cmd = NULL);// cmd = NULL is for copy. otherwise must not null
    ~FlowchartsIf();

    int getflochaType();
    FlowchartsTemplate * startTemplate();
    void setStatement1(const QString &statement);
    void setStatement2(const QString &statement);
    void setOperator(int op);

    QString getStatement1();
    QString getStatement2();
    int getOperator();
protected:

    bool parseStatement();

private:
    StatementRunTool *statementTool1;
    StatementRunTool *statementTool2;

    QString ifStatement1;
    QString ifStatement2;
    int operatorType;
};

//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------



class FlowchartsSetVar : public FlowchartsTemplate
{
    Q_OBJECT

public:


    FlowchartsSetVar(QObject *parent,CommandManager *cmd = NULL); // cmd = NULL is for copy. otherwise must not null
    ~FlowchartsSetVar();

    int getflochaType();
    FlowchartsTemplate * startTemplate();
    void setVarName(const QString &name);
    void setStatement(const QString &statement);
    QString getStatement();
    QString getVarName();

protected:

    double parseStatement();

private:

    StatementRunTool *statementTool;

    StatementRunTool *statementTool_idx[ARRAYDIMENSION];


    //int varDimension;//0- non array variable

    QString valueStatement;

    QString idxStatement[ARRAYDIMENSION];
    ArrayDimensInfo arrayVarInfo;

    char varName[80];
    unsigned int varID;//only use when var is array

};


//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------


class FlowchartsRunPDL : public FlowchartsTemplate
{
    Q_OBJECT

public:

    FlowchartsRunPDL(QObject *parent, CommandManager *cmd);// = NULL);// cmd = NULL is for copy. otherwise must not null);
    ~FlowchartsRunPDL();

    int getflochaType();
    FlowchartsTemplate * startTemplate();
    void setPDLName(const QString &name);
    QString getPDLName();

protected:

private:
    char pdlName[80];
};



//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------

typedef struct
{
    bool isFunction;
    unsigned int runLoopTimes;
    unsigned int sleepPerLoop;
    QString loopRunCntVar;


}FlochaAssemAttr;



class FlowchartsAssemble : public FlowchartsTemplate
{
    Q_OBJECT

        friend class FlowchartsTemplate;
public:


    FlowchartsAssemble(QObject *parent, CommandManager *cmd);
    ~FlowchartsAssemble();

    int getflochaType();

    void startRunAssem();
    FlowchartsTemplate *startTemplate();

    FlowchartsTemplate *getStartFlocha();
    FlowchartsTemplate *getEndFlocha();

    //int buildFlochaAssem(std::vector<FlowchartsStruct> FlowchartsData);
    //std::vector<FlowchartsStruct> outputFlochaAssem();

    int linkTwoTemplate(FlowchartsTemplate *flowChaT, FlowchartsTemplate *next, bool linkElse = false);
    int unlinkTemplate(FlowchartsTemplate *flowChaT, int type = UNLINKALL);
    int addTemplate(FlowchartsTemplate *flowChaT);
    int removeTemplate(FlowchartsTemplate *flowChaT);//notice: remove is not delete, because of redo undo
    const std::list<FlowchartsTemplate *> &getFlochaList();

    void setNextPtr(FlowchartsTemplate *nextptr);

    //void setLoopRunTimes(unsigned int looptimes);
    //void setSleepTimePerLoop(unsigned int sleepperloop);
    //void setLoopRunCntStr(const QString &cntstr);

    //unsigned int getLoopRunTimes();
    //unsigned int getSleepTimePerLoop();
    //QString getLoopRunCntStr();

    void setAssemAttr(const FlochaAssemAttr &attr);
    void getAssemAttr(FlochaAssemAttr *attr);


    const FloRunFlagStuct &getRunFlag();
    void setRunFlag(const FloRunFlagStuct &flag);

    void setContinueRunEvent();

    int getThreadRunNum();
    //const HANDLE getContinueEvent();
    bool whetherRunning();
    void stopRunning();

    void hitBreakPoint(FlowchartsTemplate *flocha);
    void runAssemFinshed(int result);
signals:
    void hitBreakPointSignal(FlowchartsTemplate *flocha);
    void runFinishedSignal(int result);

protected:

    virtual void additionFuncCallInChild();//for loop run assem, will call in startflo and endflo
    //double parseStatement();

private:
    //	std::map<FlowchartsTemplate *,FlowchartsStruct> template_Struct_Map;

    std::list<FlowchartsTemplate *> childFlochaList;//include start end end flocha

    //	QString valueStatement;
    //    char varName[80];
    FlochaAssemAttr assemAttr;

    unsigned int runLoopCnt;
    unsigned int loopRunCntVarIndx;

    FlowchartsTemplate *startflocha;
    FlowchartsTemplate *endflocha;

    FloRunFlagStuct runFlag;
    int runThreadNum;

};




//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------
//--------------------------------


unsigned int WINAPI AssemFlochaRunThread(void *OwnerObject);


#endif // FLOWCHARTSITEMS_H
