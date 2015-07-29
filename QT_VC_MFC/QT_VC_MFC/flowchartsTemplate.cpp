#include "flowchartsTemplate.h"
#include "flowchartsitems.h"





static CommandManager *cmdForRunTool = NULL;


void operator_add(double *a,double *b)
{
    *b = *a + *b;
}

void operator_minus(double *a,double *b)
{
    *b = *a - *b;
}

void operator_multi(double *a,double *b)
{
    *b = *a * *b;
}

void operator_divide(double *a,double *b)
{
    *b = *a / *b;
}

void operator_mod(double *a,double *b)
{
    int num = (int)*a;
    int div = (int)*b;
    *b = num % div;
}


void func_doNothing(double *nouse,double *b)
{
}


void func_sin(double *nouse,double *b)
{
    *b = sin(*b);
}

void func_cos(double *nouse,double *b)
{
    *b = cos(*b);
}


void special_getArray(double *idx,double *id_value)
{
    if(!cmdForRunTool)
    {
        *id_value = 0;
        return;
    }
    cmdForRunTool->getVarOfArray((unsigned int)*id_value, (unsigned int)*idx, id_value);
}


FuncNamePtrStruct funcNamePtrStruct[20] = 
{

    {"doNothing", func_doNothing,0,1},
    {"+",operator_add,1,2},
    {"-",operator_minus,1,2},
    {"*",operator_multi,2,2},
    {"/",operator_divide,2,2},
    {"%",operator_mod,2,2},
    {"cos",func_cos,3,1},
    {"sin",func_sin,3,1},
    {"special_GetArray",special_getArray,3,2}
};


int priority(std::string str)
{

    int i;

    for( i=0; i < 20; i++)
        if(str.compare(funcNamePtrStruct[i].funcName) == 0)
            return funcNamePtrStruct[i].priority;
    return -1;
}
FuncNamePtrStruct findFuncAttri(std::string str)
{
    for(int i = 0; i < sizeof(funcNamePtrStruct)/sizeof(FuncNamePtrStruct); ++i)
    {
        if(str.compare(funcNamePtrStruct[i].funcName) == 0)
            return funcNamePtrStruct[i];
    }

    //need to modify
    return funcNamePtrStruct[0];
}




//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

int IfOperatorClass::init = 0;

IfOperatorClass::IfOperatorClass() 
{
}

IfOperatorClass::~IfOperatorClass() 
{
}

std::vector<std::pair<int, QString > > IfOperatorClass::getOperNumStrList()
{
    std::vector<std::pair<int, QString > > rtnvect;
    rtnvect.clear();
    //	if(IfOperatorClass::init & 0x01)
    //	{
    rtnvect.push_back(std::make_pair(greaterThan, QString(">")));
    rtnvect.push_back(std::make_pair(lessThan, QString("<")));
    rtnvect.push_back(std::make_pair(equalTo, QString("=")));
    rtnvect.push_back(std::make_pair(greater_equal, QString(">=")));
    rtnvect.push_back(std::make_pair(less_equal, QString("<=")));
    rtnvect.push_back(std::make_pair(notEqualTo, QString("!=")));


    //		IfOperatorClass::OperNumStrList.clear();
    //		IfOperatorClass::OperNumStrList = rtnvect;
    //		IfOperatorClass::init |= 0x01;
    //	}
    //	rtnvect = IfOperatorClass::OperNumStrList;

    return rtnvect;
}

std::vector<std::pair<int, QString > > IfOperatorClass::getCompNumTypeList()
{
    std::vector<std::pair<int, QString > > rtnvect;
    rtnvect.clear();

    //	if(IfOperatorClass::init & 0x02)
    //	{

    rtnvect.push_back(std::make_pair(comparInt, QString("(int)")));
    rtnvect.push_back(std::make_pair(comparDouble, QString("(double)")));

    //		IfOperatorClass::CompNumTypeList.clear();
    //		IfOperatorClass::CompNumTypeList = rtnvect;
    //		IfOperatorClass::init |= 0x02;
    //	}
    //	rtnvect = IfOperatorClass::CompNumTypeList;

    return rtnvect;
}


bool IfOperatorClass::CompareTwoArg(double *arg1, double *arg2, int opNum)
{
    bool rtn = false;

    if(opNum & comparInt)
    {
        *arg1 = (int)(*arg1);
        *arg2 = (int)(*arg2);
    }

    switch(opNum & 0x1f)
    {
    case greaterThan:
        if((*arg1) > (*arg2))
            rtn = true;
        break;
    case lessThan:
        if((*arg1) < (*arg2))
            rtn = true;
        break;
    case equalTo:
        if((*arg1) == (*arg2))
            rtn = true;
        break;
    case greater_equal:
        if((*arg1) >= (*arg2))
            rtn = true;
        break;
    case less_equal:
        if((*arg1) <= (*arg2))
            rtn = true;
        break;
    case notEqualTo:
        if((*arg1) != (*arg2))
            rtn = true;
        break;
    }
    return rtn;
}


std::pair<QString, QString > IfOperatorClass::getOperStr(int opnum)
{
    QString opstr;
    QString typestr;
    std::vector<std::pair<int, QString > > list = IfOperatorClass::getOperNumStrList();
    for(auto it = list.begin(); it != list.end(); ++it)
    {
        if(it->first & opnum)
            opstr = it->second;
    }

    list = IfOperatorClass::getCompNumTypeList();
    for(auto it = list.begin(); it != list.end(); ++it)
    {
        if(it->first & opnum)
            typestr = it->second;
    }
    return std::make_pair(opstr, typestr);
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------






StatementRunTool::StatementRunTool(QObject *parent,CommandManager *cmd)
    : QObject(parent)

{
    this->commandManager = cmd;

    //memset(varPtrStack,0,sizeof(varPtrStack));



#ifdef _DEBUG	

    qDebug() << funcNamePtrStruct[0].funcName;
    qDebug() << funcNamePtrStruct[1].funcName;
    qDebug() << funcNamePtrStruct[2].funcName;
    qDebug() << funcNamePtrStruct[3].funcName;
#endif

}

StatementRunTool::~StatementRunTool()
{


}


int StatementRunTool::parseVarName(QString &varname, ArrayInfoForParse *arrayinfo, QString *idstatement, StatementRunTool* idxruntool[], CommandManager *cmd)
{
    int rtn = 0;
    QString nametmp = varname;
    int dimens = 0;
    bool isarray = false;
    int leftbracket = 0;
    int bracketstack = 0;

    if(cmd)//when redo undo, commandManager is null, just record full var name
    {
        for(int i = 0; i < varname.size(); ++i)
        {
            if(dimens > ARRAYDIMENSION)
            {
                break;
            }
            if(varname.at(i) == "[")
            {
                if((!isarray) && (i > 0))
                {
                    isarray = true;
                    nametmp = varname.mid(0, i);
                    
                    if(cmd->getArrayIdx_Info(nametmp, &(arrayinfo->arrayID), &(arrayinfo->dimensionInfo)) != 0)
                    {
                        rtn = -1;
                    }
                }
                ++bracketstack;
                leftbracket = i;
            }

            if(varname.at(i) == "]")
            {
                --bracketstack;
            }

            if(isarray && (bracketstack == 0))
            {
                idstatement[dimens] = varname.mid(leftbracket + 1, i - leftbracket - 1);
                idxruntool[dimens]->parseStatement(idstatement[dimens]);
                ++dimens;
            }

        }
    }

    if(arrayinfo->dimensionInfo.dimension != dimens)
    {
        rtn = -1;
    }

    varname = nametmp;

    return rtn;
}

int StatementRunTool::parseStatement(QString statement)
{
    int rtn = -1;

    bool inStr = false;

    std::vector<std::string> stack_f_op;
    //	std::vector<std::string> postfix;

    std::string tmpStr;//str contain varname, number, operator
    std::string str_buff;//str contain top of stack_f_op?

    std::vector<std::pair< std::string, ArrayInfoForParse> > array_stack;

    //memset(varPtrStack,0,sizeof(varPtrStack));
    memset(funcPtrArgIdx,0,sizeof(funcPtrArgIdx));
    memset(varStack,0,sizeof(varStack));
    memset(varDataStruct,0,sizeof(varDataStruct));
    memset(varStack_back_up,0,sizeof(varStack_back_up));
    //memset(varPtrStack_backup,0,sizeof(varPtrStack_backup));
    memset(varPtrLinkList,0,sizeof(varPtrLinkList));

    for(auto it = usedArrayName.begin(); it != usedArrayName.end(); ++it)//maybe not necessary
    {
        it->clear();
    }
    usedArrayName.clear();


    array_stack.clear();
    str_buff.clear();
    stack_f_op.clear();
    tmpStr.clear();








    ////first element of array is hold for single arg func
    //int varPtrIdx = 0;
    int linkListIdx = 0;
    int varIdx = 0;
    varStack[varIdx] = 0;
    //varPtrStack[varPtrIdx] = &varStack[varIdx];
    //++varPtrIdx;
    ++varIdx;
    //first element of array is hold for single arg fun



    int varDataIdx = 0;


    int funcPtrIdx = 0;



    int strStart = stringStartReset;



    char x,y;


    for(int i = 0; i < statement.size(); ++i)
    {
#ifdef _DEBUG	
        //		qDebug() << "for(int i = 0; i < statement.size(); ++i)  307";
#endif	
        x=statement.at(i).toLatin1();
        switch(x)
        {
        case '(':
            if(strStart == stringStartLetter)
            {
                stack_f_op.push_back(tmpStr);
                strStart =stringStartReset;
#ifdef _DEBUG	
                //		qDebug() << "strStart == stringStartLetter  315";
#endif		
            }
            tmpStr.clear();
            tmpStr.push_back(x);
            stack_f_op.push_back(tmpStr);
            tmpStr.clear();

            break;
        case ')' : 
            while(stack_f_op.size() != 0)
            {
#ifdef _DEBUG	
                //		qDebug() << "while(stack_f_op.size() != 0)  330";
#endif		

                str_buff=stack_f_op.back();
                stack_f_op.pop_back();
                if(str_buff.compare("(") != 0)
                {
                    funcPtrArgIdx[funcPtrIdx].funcPtr = findFuncAttri(str_buff).funPtr;
                    funcPtrArgIdx[funcPtrIdx].argNum = findFuncAttri(str_buff).argNum;
                    funcPtrArgIdx[funcPtrIdx].argStartIdx = varIdx-1;
                    ++funcPtrIdx;
#ifdef _DEBUG	
                    //		qDebug() << "str_buff.compare(() != 0  333";
#endif					
                }
                else
                    break;
            }
            break;
        case '[':
            if(strStart == stringStartLetter)
            {
                ArrayInfoForParse arrayinfo;
                int result = commandManager->getArrayIdx_Info(QString(tmpStr.c_str()), &(arrayinfo.arrayID), &(arrayinfo.dimensionInfo));
                if(result)
                {
                    return rtn;
                }
                arrayinfo.currDimension = 0;
                array_stack.push_back(std::make_pair(tmpStr, arrayinfo));
                usedArrayName.push_back(tmpStr);
                strStart =stringStartReset;
            }
            tmpStr.clear();
            tmpStr.push_back(x);
            stack_f_op.push_back(tmpStr);
            tmpStr.clear();

            break;
        case ']':
            {







                if(strStart == stringStartLetter)
                {
                    bool existed = false;
                    for(int j = 0;j < RUNTOOLSTACKMAXSIZE; ++j)
                    {
                        if(tmpStr.compare(varDataStruct[j].varName) == 0)
                        {
                            //varPtrStack[varPtrIdx] = varDataStruct[j].dataPtr;
                            varPtrLinkList[linkListIdx].dataPtr = &varStack[varIdx];
                            varPtrLinkList[linkListIdx].nextPtr = varDataStruct[j].dataListStartPtr;
                            varDataStruct[j].dataListStartPtr = &varPtrLinkList[linkListIdx];
                            existed = true;

                        }
                    }
                    if(!existed)
                    {

                        varPtrLinkList[linkListIdx].dataPtr = &varStack[varIdx];
                        for(int j = 0; j < tmpStr.size(); ++j)
                            varDataStruct[varDataIdx].varName[j] = tmpStr.at(j);

                        varDataStruct[varDataIdx].dataListStartPtr = &varPtrLinkList[linkListIdx];
                        ++varDataIdx;


                    }

                    ++varIdx;
                    ++linkListIdx;
                }
                else if(strStart == stringStartNum)
                {
                    varStack[varIdx] = std::stod(tmpStr);
                    //varPtrStack[varPtrIdx] = &varStack[varIdx];

                    //++varPtrIdx;
                    ++varIdx;
                }


                strStart = stringStartReset;








                while(stack_f_op.size() != 0)
                {
                    str_buff=stack_f_op.back();
                    stack_f_op.pop_back();
                    if(str_buff.compare("[") != 0)
                    {
                        funcPtrArgIdx[funcPtrIdx].funcPtr = findFuncAttri(str_buff).funPtr;
                        funcPtrArgIdx[funcPtrIdx].argNum = findFuncAttri(str_buff).argNum;
                        funcPtrArgIdx[funcPtrIdx].argStartIdx = varIdx-1;
                        ++funcPtrIdx;

                    }
                    else
                        break;
                }

                ArrayInfoForParse tmpinfo = array_stack.back().second;
                unsigned int currdim = tmpinfo.currDimension;
                
                
                if(tmpinfo.dimensionInfo.dimension == 1)//simplest case
                {
                    varStack[varIdx] = (double)tmpinfo.arrayID;
                    ++varIdx;

                    funcPtrArgIdx[funcPtrIdx].funcPtr = findFuncAttri(std::string("special_GetArray")).funPtr;
                    funcPtrArgIdx[funcPtrIdx].argNum = findFuncAttri(std::string("special_GetArray")).argNum;
                    funcPtrArgIdx[funcPtrIdx].argStartIdx = varIdx-2;
                    ++funcPtrIdx;
                }
                else if(tmpinfo.dimensionInfo.dimension < 4)
                {
                    int size = 1;
                    for(int j = currdim; j < tmpinfo.dimensionInfo.dimension - 1; ++j)
                    {
                        size *= tmpinfo.dimensionInfo.dimensionSize[j+1];
                    }

                    if(currdim == 0)
                    {
                        varStack[varIdx] = (double)(size);
                        ++varIdx;

                        funcPtrArgIdx[funcPtrIdx].funcPtr = findFuncAttri(std::string("*")).funPtr;
                        funcPtrArgIdx[funcPtrIdx].argNum = findFuncAttri(std::string("*")).argNum;
                        funcPtrArgIdx[funcPtrIdx].argStartIdx = varIdx-2;
                        ++funcPtrIdx;

                    }
                    else if(currdim < tmpinfo.dimensionInfo.dimension-1)
                    {
                        varStack[varIdx] = (double)(size);
                        ++varIdx;

                        funcPtrArgIdx[funcPtrIdx].funcPtr = findFuncAttri(std::string("*")).funPtr;
                        funcPtrArgIdx[funcPtrIdx].argNum = findFuncAttri(std::string("*")).argNum;
                        funcPtrArgIdx[funcPtrIdx].argStartIdx = varIdx-2;
                        ++funcPtrIdx;

                        funcPtrArgIdx[funcPtrIdx].funcPtr = findFuncAttri(std::string("+")).funPtr;
                        funcPtrArgIdx[funcPtrIdx].argNum = findFuncAttri(std::string("+")).argNum;
                        funcPtrArgIdx[funcPtrIdx].argStartIdx = varIdx-2;
                        ++funcPtrIdx;
                    }
                    else if(currdim == tmpinfo.dimensionInfo.dimension-1)
                    {
                        funcPtrArgIdx[funcPtrIdx].funcPtr = findFuncAttri(std::string("+")).funPtr;
                        funcPtrArgIdx[funcPtrIdx].argNum = findFuncAttri(std::string("+")).argNum;
                        funcPtrArgIdx[funcPtrIdx].argStartIdx = varIdx-2;
                        ++funcPtrIdx;

                        varStack[varIdx] = (double)tmpinfo.arrayID;
                        ++varIdx;

                        funcPtrArgIdx[funcPtrIdx].funcPtr = findFuncAttri(std::string("special_GetArray")).funPtr;
                        funcPtrArgIdx[funcPtrIdx].argNum = findFuncAttri(std::string("special_GetArray")).argNum;
                        funcPtrArgIdx[funcPtrIdx].argStartIdx = varIdx-2;
                        ++funcPtrIdx;

                    }
                    else
                    {
                        return rtn;
                    }
                }
                else
                {
                    return rtn;
                }

                ++array_stack.back().second.currDimension;

            }
            break;
        case '+' :
        case '-' :
            {
                if(strStart != stringStartLetter && strStart != stringStartNum)
                {






                    break;
                }


            }


        case '*' :
        case '/' : 
        case '&' :
        case '|' :

#ifdef _DEBUG	
            //		qDebug() << "case | & / * + -  360";
#endif

            if(strStart == stringStartLetter)
            {

#ifdef _DEBUG	
                //		qDebug() << "if(strStart == stringStartLetter)  360";
#endif
                bool existed = false;
                for(int j = 0;j < RUNTOOLSTACKMAXSIZE; ++j)
                {
                    if(tmpStr.compare(varDataStruct[j].varName) == 0)
                    {
                        //varPtrStack[varPtrIdx] = varDataStruct[j].dataPtr;
                        varPtrLinkList[linkListIdx].dataPtr = &varStack[varIdx];
                        varPtrLinkList[linkListIdx].nextPtr = varDataStruct[j].dataListStartPtr;
                        varDataStruct[j].dataListStartPtr = &varPtrLinkList[linkListIdx];
                        existed = true;
#ifdef _DEBUG	

                        //		qDebug() << "varPtrStack[varPtrIdx] = varDataStruct[j].dataPtr;  355";
#endif
                    }
                }
                if(!existed)
                {
                    varPtrLinkList[linkListIdx].dataPtr = &varStack[varIdx];
                    for(int j = 0; j < tmpStr.size(); ++j)
                        varDataStruct[varDataIdx].varName[j] = tmpStr.at(j);

                    varDataStruct[varDataIdx].dataListStartPtr = &varPtrLinkList[linkListIdx];
                    ++varDataIdx;
#ifdef _DEBUG	
                    //		qDebug() << "if(!existed);  382";
#endif

                }

                ++linkListIdx;
                ++varIdx;
                //++varIdx;//why in here, not in if(!existed){} ????????????????
                //++varPtrIdx;
            }
            else if(strStart == stringStartNum)
            {
                double test = std::stod(tmpStr);
                varStack[varIdx] = test;
                //varPtrStack[varPtrIdx] = &varStack[varIdx];
#ifdef _DEBUG	
                //		qDebug() << "strStart == stringStartNum  397";
#endif
                //++varPtrIdx;
                ++varIdx;
            }

            strStart = stringStartReset;


            tmpStr.clear();
            tmpStr.push_back(x);

            if(stack_f_op.size() != 0)
            {
                str_buff=stack_f_op.back();
                int numofpopop = 0;
                while((priority(str_buff) >= priority(tmpStr)) && (stack_f_op.size() != 0))
                {
                    ++numofpopop;//I am not sure, why i add this, but it seems not need anymore?????
#ifdef _DEBUG
                    if(numofpopop > 2)//condition (1 < numofpopop < 2) is OK
                        QMessageBox::warning(nullptr, "debuging", "numofpopop > 2");
                    qDebug() << "numofpopop" << numofpopop;
                    std::cout << str_buff;
#endif


                    funcPtrArgIdx[funcPtrIdx].funcPtr = findFuncAttri(str_buff).funPtr;
                    funcPtrArgIdx[funcPtrIdx].argNum = findFuncAttri(str_buff).argNum;
                    funcPtrArgIdx[funcPtrIdx].argStartIdx = varIdx-1-1;//funcPtrArgIdx[funcPtrIdx].argStartIdx = varIdx-1-numofpopop

                    stack_f_op.pop_back();
                    if(stack_f_op.size() != 0)
                        str_buff=stack_f_op.back();
                    ++funcPtrIdx;
#ifdef _DEBUG	
                    //		qDebug() << "str_buff=stack_f_op.back();397";
#endif
                }

            }
            stack_f_op.push_back(tmpStr);
            tmpStr.clear();


            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if(strStart == stringStartReset)
                strStart = stringStartNum;
            tmpStr.push_back(x);
            break;
        case '.':
            if(tmpStr.size() != 0)
                tmpStr.push_back(x);
            else
                return rtn;

            break;


        case '_':

        default : // x 為運算元
            if(strStart == stringStartNum)
            {
                return rtn;
            }
            if((x > 65) && (x < 122) && (x != 96) && (x != 94) && (x != 92))
            {
                strStart = stringStartLetter;
                tmpStr.push_back(x);

#ifdef _DEBUG	
                //		qDebug() << "strStart = stringStartLetter;  463";
#endif

            }
            //			postfix[++j]=x;
        }
    }
#ifdef _DEBUG	
    //		qDebug() << "end for()  463";
#endif

    while(stack_f_op.size() != 0)
    {
        str_buff=stack_f_op.back();
        stack_f_op.pop_back();
        funcPtrArgIdx[funcPtrIdx].funcPtr = findFuncAttri(str_buff).funPtr;
        funcPtrArgIdx[funcPtrIdx].argNum = findFuncAttri(str_buff).argNum;
        funcPtrArgIdx[funcPtrIdx].argStartIdx = varIdx-1;
        ++funcPtrIdx;
#ifdef _DEBUG	
        //		qDebug() << "++funcPtrIdx;452";
#endif

        //postfix[++j]=pop();
        //postfix[++j]='\0';
    }

#ifdef _DEBUG	
    //		qDebug() << "before if(strStart == stringStartLetter)  495";
#endif

    if(strStart == stringStartLetter)
    {
        bool existed = false;
        for(int j = 0;j < RUNTOOLSTACKMAXSIZE; ++j)
        {
            if(tmpStr.compare(varDataStruct[j].varName) == 0)
            {
                //varPtrStack[varPtrIdx] = varDataStruct[j].dataPtr;
                varPtrLinkList[linkListIdx].dataPtr = &varStack[varIdx];
                varPtrLinkList[linkListIdx].nextPtr = varDataStruct[j].dataListStartPtr;
                varDataStruct[j].dataListStartPtr = &varPtrLinkList[linkListIdx];
                existed = true;
#ifdef _DEBUG	
                //		qDebug() << "existed = true;";
#endif
            }
        }
        if(!existed)
        {

            varPtrLinkList[linkListIdx].dataPtr = &varStack[varIdx];
            for(int j = 0; j < tmpStr.size(); ++j)
                varDataStruct[varDataIdx].varName[j] = tmpStr.at(j);

            varDataStruct[varDataIdx].dataListStartPtr = &varPtrLinkList[linkListIdx];
            ++varDataIdx;


        }

        ++varIdx;
        ++linkListIdx;
    }
    else if(strStart == stringStartNum)
    {
        varStack[varIdx] = std::stod(tmpStr);
        //varPtrStack[varPtrIdx] = &varStack[varIdx];

        //++varPtrIdx;
        ++varIdx;
    }

    tmpStr = "doNothing";
    funcPtrArgIdx[funcPtrIdx].funcPtr = findFuncAttri(tmpStr).funPtr;
    funcPtrArgIdx[funcPtrIdx].argNum = findFuncAttri(tmpStr).argNum;
    funcPtrArgIdx[funcPtrIdx].argStartIdx = varIdx-2;
    //maybe not good
    if(funcPtrArgIdx[funcPtrIdx].argStartIdx < 0)
        funcPtrArgIdx[funcPtrIdx].argStartIdx = 0;
    //maybe not good


    //memcpy(varPtrStack_backup,varPtrStack,sizeof(varPtrStack));
    memcpy(varStack_back_up,varStack,sizeof(varStack));
#ifdef _DEBUG	
    //		qDebug() << "debug";
    //	double debug_out;
    //	calculateStatement(&debug_out);
    //	qDebug() << "debug calculate" << debug_out;
#endif


    for(auto it = usedArrayName_backup.begin(); it != usedArrayName_backup.end(); ++it)
    {
        bool found = false;
        for(auto it_find = usedArrayName.begin(); it_find != usedArrayName.end(); ++it_find)
        {
            if((*it) == (*it_find))
            {
                found = true;
                break;
            }
        }
        if(!found)
        {
            commandManager->delIDFromMapForRun(QString((*it).c_str()));
        }
    }

    usedArrayName_backup = usedArrayName;

    rtn = 1;
    return rtn;
}


int StatementRunTool::calculateStatement(double *out)
{
    int rtn = -1;
    int i = 0;
    int argIdx = 0;
    int sizeofdouble_p = sizeof(double);
    //	int varPtrSize = sizeof(varPtrStack);
#ifdef _DEBUG	
    //	qDebug() << "start calculate";
#endif

    if(commandManager == 0)//here has a bug
    {
        if(varDataStruct[i].dataListStartPtr != 0)
            return rtn;
    }
    else
    {
        double tmpvalue;
        VarPtrLinkList *linklistp = varDataStruct[i].dataListStartPtr;
        while(linklistp != 0)
        {

            commandManager->getVar(varDataStruct[i].varName, &tmpvalue);

            while(linklistp != 0)
            {
                *(linklistp->dataPtr) = tmpvalue;
                linklistp = linklistp->nextPtr;
            }
            ++i;
            linklistp = varDataStruct[i].dataListStartPtr;
        }
    }

    i = 0;

    //here should lock
    cmdForRunTool = commandManager;

    while(funcPtrArgIdx[i].funcPtr != 0)
    {
        argIdx = funcPtrArgIdx[i].argStartIdx;
        funcPtrArgIdx[i].funcPtr(&varStack[argIdx],&varStack[argIdx + 1]);

        if((funcPtrArgIdx[i].argNum ==2) && (argIdx > 0))
            memcpy(&varStack[1],&varStack[0],argIdx * sizeofdouble_p);
        ++i;
    }

    cmdForRunTool = NULL;
    //here should unlock

    *out = varStack[argIdx + 1];


    //	memcpy(varPtrStack,varPtrStack_backup,sizeof(varPtrStack));
    memcpy(varStack,varStack_back_up,sizeof(varStack));
#ifdef _DEBUG	
    //	qDebug() << *out;
#endif
    rtn = 0;
    return rtn;
}
//-----------------------------------
//-----------------------------------
//-----------------------------------
//-----------------------------------
//-----------------------------------
//-----------------------------------


StatementParseTool::StatementParseTool(QObject *parent)
    : QObject(parent)

{
    topIndex = -1;

    char op[PARSEOPERATORNUM] = {'(','+','-','*','/',};
    int prior[PARSEOPERATORNUM] = {0,1,1,2,2};// 與op[OP]對應,用以存放運算子的優先順序

    for(int i = 0; i < PARSEOPERATORNUM; ++i)
    {
        operatorList[i] = op[i];
        op_priority[i] = prior[i];
    }

}
StatementParseTool::~StatementParseTool()
{

}

void StatementParseTool::infixToPostfix(QString infix,std::vector<std::string> &postfix)
{
    int i=0, j=-1;
    char x, y;

    while((x=infix.at(i).toLatin1()) != '\0'){
        switch(x){
        case '(' : push(x);
            break;
        case ')' : 
            while(! IsEmpty() && (x=pop()) != '(')
            {
                std::string str(&x);
                postfix.push_back(str);
            }
            break;
        case '+' :
        case '-' :
        case '*' :
        case '/' : y=top_data();
            while(priority(y) >= priority(x)){
                std::string str;
                //str.append(pop());
                postfix.push_back(str);
                y=top_data();
            }
            push(x);
            break;
        default : // x 為運算元
            postfix[++j]=x;
        }
    }
    while(! IsEmpty())
        postfix[++j]=pop();
    postfix[++j]='\0';


}

void StatementParseTool::calculatePostfix(const std::vector<std::string> &posfix, int out)
{
    /*
    int point=0;
    while(postfix[point]!='\0')
    {
    while(IsDight(postfix[point]))
    push(postfix[point++]);
    int a=pop()-'0', b=pop()-'0',c=0;
    switch(postfix[point])
    {
    case'+':c=b+a;
    break;
    case'-':c=b-a;
    break;
    case'*':c=b*a;
    break;
    case'/':c=b/a;
    break;
    }
    push(c+'0');
    point++;
    }
    out = pop()-'0';
    */
}

void StatementParseTool::calculatePostfix(const std::vector<std::string> &posfix, double out)
{

}

void StatementParseTool::push(char c)
{
    if (topIndex>=PARSESTACKNUM-1){
        printf("Stack full!\n");
        exit(-1);
    }
    dataStack[++topIndex]=c;

}

char StatementParseTool::pop()
{
    if (topIndex==-1){
        printf("Stack empty!\n");
        exit(-1);
    }
    return dataStack[topIndex--];
}



bool StatementParseTool::IsEmpty()
{
    return (topIndex < 0) ? true : false; 
}




char StatementParseTool::top_data()
{
    return dataStack[topIndex]; 
}


int StatementParseTool::priority(char c)
{
    int i;

    for( i=0; i < PARSEOPERATORNUM; i++)
        if(operatorList[i] == c)
            return op_priority[i];
    return -1;
}






//-----------------------------------
//-----------------------------------
//-----------------------------------
//-----------------------------------
//-----------------------------------
//-----------------------------------



FlowchartsTemplate::FlowchartsTemplate(QObject *parent)
    : QObject(parent)
{
    breakPointOn = false;
    commandManager = nullptr;
    if(parent!=nullptr)
        p_parent = dynamic_cast<FlowchartsAssemble *>(parent);
    else
        p_parent = nullptr;


    memset(idName,0,sizeof(idName));
    memset(itemPos,0,sizeof(itemPos));

    p_next = nullptr;
    p_else = nullptr;
}

FlowchartsTemplate::~FlowchartsTemplate()
{

}

FlowchartsTemplate * FlowchartsTemplate::startTemplate()
{
    if(p_parent)
        p_parent->additionFuncCallInChild();
    //if(runLoopTimes)
    //{
    //	Sleep(sleepPerLoop);
    //	if(runLoopCnt < runLoopTimes)
    //	{
    //		++runLoopCnt;
    //		return p_else;
    //	}
    //}

    return p_next;
}


int FlowchartsTemplate::getflochaType()
{
    return flochaTypeTemplate;
}




/*
void FlowchartsTemplate::setMpiController(CommandManager *cmd)
{
commandManager = cmd;
}
*/

const bool FlowchartsTemplate::getbreakPointFlag()
{
    return breakPointOn;
}
void FlowchartsTemplate::setbreakPointFlag(const bool &flag)
{
    breakPointOn = flag;
}

FlowchartsTemplate *FlowchartsTemplate::getNextPtr()
{
    return p_next;
}

FlowchartsTemplate *FlowchartsTemplate::getElsePtr()
{
    return p_else;
}
FlowchartsAssemble *FlowchartsTemplate::getParentPtr()
{
    return p_parent;
}

void FlowchartsTemplate::setNextPtr(FlowchartsTemplate *nextptr)
{
    p_next = nextptr;
}

void FlowchartsTemplate::setElsePtr(FlowchartsTemplate *elseptr)
{
    p_else = elseptr;
}

void FlowchartsTemplate::setParentPtr(FlowchartsAssemble *parentptr)
{
    p_parent = parentptr;
}


void FlowchartsTemplate::additionFuncCallInChild()
{
}