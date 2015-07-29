#include "MpiControl.h"


//------------------------------------------------
//------------------------------------------------
//------------------------------------------------
DriveCtrlBase::DriveCtrlBase()
{


}

DriveCtrlBase::~DriveCtrlBase()
{


}


//------------------------------------------------
//------------------------------------------------
//------------------------------------------------

SomeDriveCtrl::SomeDriveCtrl(void)
{
    typeList.push_back(QString("type1"));
    typeList.push_back(QString("type2"));
    typeList.push_back(QString("type3"));
    typeList.push_back(QString("type4"));
    typeList.push_back(QString("type5"));
    typeList.push_back(QString("type6"));



    pCom = NULL;
    portN = 1;
    baudrateN = 8;
    connectState = 0;

    typeN = 0;
    slaveN = 0;

}


SomeDriveCtrl::~SomeDriveCtrl(void)
{
    typeList.clear();
    if(pCom)
        delete pCom;
    pCom = NULL;
}



int SomeDriveCtrl::connectDrive()
{
    disconnectDrive();

    char dcechar[80];

    memset(dcechar, 0, sizeof(dcechar));
    memcpy(dcechar, typeList.at(typeN).toStdString().c_str(), typeList.at(typeN).size());

    pCom = new SomeDrive;

    if(portN > 0)
    {
        
        //connectState = pCom->setComPar(portN,baudrateN=8,1,0,0,0,0,0,100,200,6);
    }

    return connectState;
}

int SomeDriveCtrl::disconnectDrive()
{
    int rtn = -1;
    if(pCom)
    {
        SomeDrive *tmp = pCom;
        pCom = NULL;
        //maybe here should wait getvar/setvar finish
        delete pCom;
    }


    return rtn;
}


int SomeDriveCtrl::getVar(const QString &varname, double *value)
{
    int rtn = -1;
    int state;
    char namechar[20] = "";
    if(!pCom)
        return rtn;
    memcpy(namechar, varname.toStdString().c_str(), varname.size());
//    rtn = pCom->GetVarN(namechar, value, slaveN);

    return rtn;
}


std::vector<QString> SomeDriveCtrl::getDriveTypeList()
{
    return typeList;
}

int SomeDriveCtrl::changeDriveType(int type)
{
    int rtn = -1;
    typeN = type;
    rtn = 0;

    return rtn;
}


int SomeDriveCtrl::changePortNum(int port)
{
    int rtn = -1;
    portN = port;
    return rtn;
}


int SomeDriveCtrl::setVar(const QString &varname, const double &value)
{
    int rtn = -1;
    int state = int(value);
    char namechar[20] = "";

    memcpy(namechar, varname.toStdString().c_str(), varname.size());
    //rtn = pCom->SetVarN(namechar, value, slaveN);

    return rtn;
}


int SomeDriveCtrl::getVarOrArrayInfo(const QString &arrayname, int *size, int *type)
{
    int rtn = -1;
    char namechar[20] = "";
    if(!pCom)
    {
        return -1;
    }
    memcpy(namechar, arrayname.toStdString().c_str(), arrayname.size());
    //rtn = pCom->GetVarAddrType(namechar, slaveN, size, type);
    return rtn;
}


int SomeDriveCtrl::runFunction(const QString &funname)
{
    int rtn = -1;
    char namechar[20] = "";
    memcpy(namechar, funname.toStdString().c_str(), funname.size());
    //rtn = pCom->RunFunc(namechar, errStr, slaveN);
    return rtn;
}


int SomeDriveCtrl::waitDriveMsg( int *pcode, char *pmsg, int *pcolor, int *pparam, int *pnumovrflw, int timeout)
{
    int rtn = -1;
    //rtn = pCom->waitOnMsgP( pcode, pmsg, pcolor, pparam, pnumovrflw, timeout, pCom);
    return rtn;

}