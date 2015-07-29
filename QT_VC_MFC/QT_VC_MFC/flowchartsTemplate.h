#ifndef FLOWCHARTSABSTRACT_H
#define FLOWCHARTSABSTRACT_H

#include <QObject>
#include <QString>
#include <QDeBug>
#include <iostream>
#include <string>
#include <map>
#include "commandmanager.h"


class FlowchartsAssemble;

#define PARSESTACKNUM 50
#define PARSEOPERATORNUM 5
#define RUNTOOLSTACKMAXSIZE 20

enum flochaType
{
    flochaTypeTemplate,
    flochaTypeIf,
    flochaTypeSet,
    flochaTypeRun,
    flochaTypeSleep,
    flochaTypeAssem
};


typedef struct
{
    unsigned int arrayID;
    unsigned int currDimension;//index start from 0
    ArrayDimensInfo dimensionInfo;
}ArrayInfoForParse;


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------



class IfOperatorClass
{
public:
    IfOperatorClass();
    ~IfOperatorClass();


    static std::vector<std::pair<int, QString > > getOperNumStrList();
    static std::vector<std::pair<int, QString > > getCompNumTypeList();
    static bool CompareTwoArg(double *arg1, double *arg2, int opnum);
    static std::pair<QString, QString > getOperStr(int opnum);//first is operator, second is variable type

private:
    enum compOpBit
    {
        greaterThan = 0x01,
        lessThan = 0x01 << 1,
        equalTo = 0x01 << 2,
        greater_equal = 0x01 << 3,
        less_equal = 0x01 << 4,
        notEqualTo = 0x01 << 5,
        comparInt = 0x01 << 6,
        comparDouble = 0x01 << 7
    };

    //	static std::vector<std::pair<int, QString > > OperNumStrList;
    //	static std::vector<std::pair<int, QString > > CompNumTypeList;

    static int init;


};



//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------






typedef void (*funcPtrArg2)(double*,double*);

struct FuncNamePtrStruct
{
    char funcName[20];
    funcPtrArg2 funPtr;
    int priority;
    int argNum;
};

typedef struct _VarPtrLinkList_ VarPtrLinkList;

struct _VarPtrLinkList_
{
    double *dataPtr;
    VarPtrLinkList *nextPtr;
};

typedef struct
{
    char varName[20];
    VarPtrLinkList *dataListStartPtr;
}VarNamePtrStruct ;



typedef struct
{
    funcPtrArg2 funcPtr;
    int argStartIdx;
    int argNum;
}FuncPtr_ArgIdx ;

enum
{
    stringStartReset,
    stringStartNum,
    stringStartLetter
};


class StatementRunTool : public QObject
{
public:
    StatementRunTool(QObject *parent, CommandManager *cmd);
    ~StatementRunTool();

    static int parseVarName(QString &varname, ArrayInfoForParse *arrayinfo, QString *idstatement, StatementRunTool* idxruntool[], CommandManager *cmd);

    int parseStatement(QString statement);
    int calculateStatement(double *out);

private:

    VarNamePtrStruct varDataStruct[RUNTOOLSTACKMAXSIZE];//high speed process
    FuncPtr_ArgIdx funcPtrArgIdx[RUNTOOLSTACKMAXSIZE];//high speed process
    double varStack[RUNTOOLSTACKMAXSIZE];//high speed process
    //	double *varPtrStack[RUNTOOLSTACKMAXSIZE];
    double varStack_back_up[RUNTOOLSTACKMAXSIZE];
    //	double *varPtrStack_backup[RUNTOOLSTACKMAXSIZE];
    VarPtrLinkList varPtrLinkList[RUNTOOLSTACKMAXSIZE];//high speed process


    std::vector<std::string> usedArrayName;//not necessary high speed process
    std::vector<std::string> usedArrayName_backup;

    CommandManager *commandManager;

};






class StatementParseTool : public QObject
{
public:
    StatementParseTool(QObject *parent);
    ~StatementParseTool();

    void infixToPostfix(QString infix,std::vector<std::string> &posfix);
    void calculatePostfix(const std::vector<std::string> &posfix, int out);
    void calculatePostfix(const std::vector<std::string> &posfix, double out);
private:

    void push(char c);
    char pop();
    bool IsEmpty();
    char top_data();
    int priority(char c);



    char dataStack[50];
    int topIndex;

    char operatorList[PARSEOPERATORNUM];
    char op_priority[PARSEOPERATORNUM];

};


//
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------






class FlowchartsTemplate : public QObject
{
    Q_OBJECT

public:
    FlowchartsTemplate(QObject *parent);
    ~FlowchartsTemplate();

    char idName[20];
    int itemPos[2];


    //virtual void startTemplate();
    virtual FlowchartsTemplate *startTemplate();
    virtual int getflochaType();

    //	virtual void startTemplate(int varIn) = 0;
    //	virtual void startTemplate(double varIn) = 0;


    void getPreparationState();

    //	void setMpiController(CommandManager *cmd);




    const bool getbreakPointFlag();
    void setbreakPointFlag(const bool &flag);

    FlowchartsTemplate *getNextPtr();
    FlowchartsTemplate *getElsePtr();
    FlowchartsAssemble *getParentPtr();

    virtual void setNextPtr(FlowchartsTemplate *nextptr);//because flocha assem alse need to set endflocha's next ptr
    void setElsePtr(FlowchartsTemplate *elseptr);
    void setParentPtr(FlowchartsAssemble *parentptr);

signals:
    //	void EndSignal();
    //	void EndSignal_else();
    void errorSignal(int error);

protected:

    int PreparationState;

    FlowchartsAssemble *p_parent;
    FlowchartsTemplate *p_next;
    FlowchartsTemplate *p_else;

    virtual void additionFuncCallInChild();//will only implement in assemble;
    //	unsigned int runLoopTimes;
    //	unsigned int runLoopCnt;
    //	unsigned int sleepPerLoop;



    CommandManager *commandManager;

private:
    bool breakPointOn;

};






#endif // FLOWCHARTSABSTRACT_H




