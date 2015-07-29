#pragma once

#include <windows.h>

#include <iostream>
#include <vector>

#include <QString>


///////////////////////////api provide by vender...///////////////////////
class SomeDrive
{
public:
    SomeDrive(){};
    ~SomeDrive(){};
    //int setComPar(int port, int baudrate, int mode, int trid, int rcid, int cancanbaudrate, int msgStand, int canpipelevel, int timeout, int locktime, int iternum);
};
///////////////////////////api provide by vender...///////////////////////



class DriveCtrlBase
{
public:
    DriveCtrlBase();
    virtual ~DriveCtrlBase() = 0;

    virtual int connectDrive() = 0;
    virtual int disconnectDrive() = 0;

    virtual int setVar(const QString &varname, const double &value) = 0;
    virtual int getVar(const QString &varname, double *value) = 0;

    virtual int getVarOrArrayInfo(const QString &varname, int *size, int *type) = 0;

    virtual int runFunction(const QString &funname) = 0;

    virtual int changeDriveType(int type) = 0;
    virtual int changePortNum(int port) = 0;

    virtual std::vector<QString> getDriveTypeList() = 0;

};



class SomeDriveCtrl : public DriveCtrlBase
{
public:
    SomeDriveCtrl(void);
    ~SomeDriveCtrl(void);

    int connectDrive();

    int disconnectDrive();

    //	int getVarOrState(char *varName,double *pdata);
    //	int getVarOrState(char *varName,int *pdata);

    //	int setVarOrState(char *varName,double data);
    //	int setVarOrState(char *varName,int mode);


    int setVar(const QString &varname, const double &value);
    int getVar(const QString &varname, double *value);

    int getVarOrArrayInfo(const QString &varname, int *size, int *type);


    int runFunction(const QString &funname);

    std::vector<QString> getDriveTypeList();

    int changeDriveType(int type);
    int changePortNum(int port);




    int waitDriveMsg( int *pcode, char *pmsg=NULL, int *pcolor=NULL, int *pparam=NULL, int *pnumovrflw=NULL, int timeout=0xfffffffff);



private:

    SomeDrive *pCom;
    int portN;
    int baudrateN;
    int connectState;
    int typeN;
    char errStr[80];

    std::vector<QString> typeList;


    int slaveN;
};

