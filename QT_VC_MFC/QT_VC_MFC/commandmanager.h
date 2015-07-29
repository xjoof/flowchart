#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H
#include <QObject>
#include <QMessageBox>

#include <QTableWidget>
#include <QInputDialog>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHeaderView>
//#include <QEvent>
#include <QKeyEvent>

#include "MpiControl.h"
#ifdef _DEBUG
#include <QDebug>
#endif

#define TABLECHANGE_EDITPCVARNAME 0
#define TABLECHANGE_ADDNEWPCVAR 1


#define ARRAYDIMENSION 3


typedef struct
{
    unsigned int dimension;
    unsigned int totalSize;
    unsigned int dimensionSize[3];

}ArrayDimensInfo;//also use in not array var, but the dimension = 0; totalSize = 0;


class PCVariablePool
{
public:
    PCVariablePool(PCVariablePool* parent = nullptr);
    ~PCVariablePool();

    int getVar(const QString &varname, double *value);
    int setVar(const QString &varname, const double &value);

    int setArrayValue(const QString &arrayname, unsigned int idx, const double &value);
    int getArrayValue(const QString &arrayname, unsigned int idx, double *value);

    int isArrayExist(const QString &arrayname, ArrayDimensInfo *arrayinfo);
    //	int setRecentUseVar(const QString &varname);
    //	int eraseRecentUseVar(const QString &varname);

    int insertVarMap(const QString &varname, const double &value);
    int eraseVarMap(const QString &varname);

    int insertArrayMap(const QString &varname,  const ArrayDimensInfo &arrayinfo);
    int eraseArrayMap(const QString &varname);

protected:

    //std::map<QString, double*> recentUseGloble;
    std::map<QString, double> allVarMap;

    std::map<QString, std::pair<ArrayDimensInfo, double*> > arrayMap;

    //double 

    PCVariablePool *globlePool;

    CRITICAL_SECTION getVarCritiSect; 

};


//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------


class PCVarDimensEdit : public QWidget
{
    Q_OBJECT
public:

    PCVarDimensEdit(QWidget *parent, ArrayDimensInfo initinfo);
    ~PCVarDimensEdit();

    ArrayDimensInfo getDimensInfo();

signals:

    void dimensEditFinish();

private:
    void dimemsSpinChange(int dimens);
    void dimemsSizeSpinChange(int dimens);

    void judgeEditFinish();

    QSpinBox *ui_spin_dimens;

    QSpinBox *ui_spin_dimensSize[ARRAYDIMENSION];

    QHBoxLayout *mainLayout;

    ArrayDimensInfo dimensInfo;
    ArrayDimensInfo dimensInfo_backup;
};





//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------



class PCVarEditWidget : public QWidget
{
    Q_OBJECT
public:
    PCVarEditWidget(QWidget *parent);
    ~PCVarEditWidget();

    void setTextNeedTrans();

    void addVarToTable(unsigned int indx, QString &varname, const ArrayDimensInfo &info);//for load xml to PCVarEditWidget

signals:

    void changeVarSignal(unsigned int *indx, QString &varname, ArrayDimensInfo &info);//*indx = -1 for new local var
    void removeVarSignal(unsigned int indx);

private:

    bool eventFilter(QObject* watched, QEvent* event);
    void keyPressEvent(QKeyEvent * event);


    void dealCellDoubleClick(int row, int column);

    void addNewRow();
    void removeRow(int row);

    void varContextChange(int row, int column);
    void varDimensEditFinish();

    std::map<unsigned int, QString> indexVarNameMap;

    QTableWidget *varTableWidget;


    int editOrAddVar;//use TABLECHANGE_EDITPCVARNAME and TABLECHANGE_ADDNEWPCVAR


};







//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------


class CommandManager : public QObject
{
public:
    CommandManager(CommandManager *parent);
    CommandManager(PCVariablePool *varPool, DriveCtrlBase *driveCtrl);
    ~CommandManager(void);

    int getVar(const QString &varname, double *data);
    int setVar(const QString &varname, const double &data);

    int getVarOfArray(unsigned int arrayid, unsigned int idx, double *value);
    int setVarOfArray(unsigned int arrayid, unsigned int idx, const double &value);

    int getArrayIdx_Info(const QString &arrayname, unsigned int *idx, ArrayDimensInfo *info);
    void delIDFromMapForRun(const QString &arrayname);
    //int registArrayToGenID(const QString &arrayname);

    int connectDrive();

    int runHiwinPDLFunc(const QString &varname);


    int getInsideVar(const QString &varname, double *data);
    int getOutsideVar(const QString &varname, const double &data);


    int setInsideVar(const QString &varname, double *data);
    int setOutsideVar(const QString &varname, const double &data);


    int editVarMap(unsigned int *indx, const QString &varname, const ArrayDimensInfo &info);
    int eraseVarMap(unsigned int indx);


    //	PCVariablePool *getVarPool();
    //	DriveCtrlBase *getDriveCtrl();

private:

    std::map<unsigned int, QString> indexVarNameMap;


    std::map<unsigned int, std::pair<QString, ArrayDimensInfo> > arrayIDMap_forRun;
    unsigned int arrayID_forRun;


    unsigned int localVariableIndex;

    CommandManager *parentCmdManager;
    PCVariablePool *varPool;
    DriveCtrlBase *driveCtrl;

};

#endif