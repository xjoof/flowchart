#ifndef VARDISPLAYMANAGE_H
#define VARDISPLAYMANAGE_H

#include <process.h>

#include <QTimer>

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QPixmap>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPainter>


#ifdef _DEBUG
#include <QMessageBox>
#endif

#include "commandmanager.h"


#define UNITDISPLAY_DIRECT 0
#define UNITDISPLAY_TEXTMAP 1
#define UNITDISPLAY_ICONMAP 2

//#define qApp (static_cast<QApplication *>(QCoreApplication::instance()))


struct DisplayUnitSettingBits
{
    int editMode : 1;
    int displayType : 3;
    int disable : 1;
    int isReadOnly : 1;

};



union DisplayUnitSetting
{
    int all;
    DisplayUnitSettingBits bits;
};


class DisplayIconList
{
public:
    DisplayIconList();
    ~DisplayIconList();

    const QPixmap * const getIcon(int num) const;
    const int iconCount() const;

    void insertIcon(const QString &path);

private:
    std::map<int, QPixmap*> iconMap;
    int totalIconNum;
};


//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------


class VarDisplayUnit : public QWidget
{
    Q_OBJECT

public:
    VarDisplayUnit(QWidget *parent, CommandManager *cmd, QString varname, DisplayUnitSetting setting, 
        const std::map<double, QString> &textmap,
        const std::map<double, int> &iconmap);
    ~VarDisplayUnit();

    void setTextNeedTrans();

    void updateVar();

    void switchMode(const int&mode);

    std::map<double, QString> getValueTextMap() const;
    std::map<double, int> getValueIconIdxMap() const;
    QString getVarName() const;
    DisplayUnitSetting getUnitSetting() const;

signals:
    void varValueChangeSignal(double value);
    void unitEnableSignal(bool enable);


private:
    //	QTimer *updateVarTimer;
    //	void updateVar_shadow();
    //	double varValueToUpd;
    void paintEvent(QPaintEvent *paintevent);

    bool eventFilter(QObject* watched, QEvent* evt);

    void keyPressEvent(QKeyEvent *keyevt);
    void keyReleaseEvent(QKeyEvent *keyevt);

    void editingVarValue();
    void editVarValueFinish();
    void varValueSelectChange();

    void editVarNameStart();
    void editVarNameFinish();


    void startEditContents();


    void updEditState();
    void updEditStateDelay();
    QTimer *updEditDelaytimer;
    QWidget *updEditCaller;

    void editFinishOK();
    void editFinishCancel();
    void deleteEditArea();


    void rebuildUnitDisplay();





    //--------------------------display ui--------------------------
    QLabel *ui_label_varName;
    QLineEdit *ui_edit_varValue;
    QComboBox *ui_combo_textMap;
    QLabel *ui_label_icon;

    //--------------------------display ui--------------------------



    //--------------------------edit ui--------------------------
    QLineEdit *ui_edit_varName;
    QComboBox *ui_combo_enable;
    QComboBox *ui_combo_type;
    QComboBox *ui_combo_readWrite;


    QPushButton *ui_button_edit;
    QPushButton *ui_button_ok;
    QPushButton *ui_button_cancel;

    std::list<QLabel*> labelList;

    std::map<QLineEdit*, QLineEdit*> edit_edit_map;
    std::map<QLineEdit*, QComboBox*> edit_combo_map;

    QLineEdit *ui_edit_newKey;
    QLineEdit *ui_edit_newText;
    QComboBox *ui_combo_forNew;
    //--------------------------edit ui--------------------------


    DisplayUnitSetting unitSetting;


    int unitID;//important

    double tempValue;
    QString varName;
    std::map<double, QString> valueTextMap;
    std::map<double, int> valueIconIdxMap;

    bool needBackup;
    std::map<double, QString> valueTextMap_backup;
    std::map<double, int> valueIconIdxMap_backup;

    bool varValueEditing;
    bool varValueCanUpd;

    bool varNameInvalid;

    QVBoxLayout *mainLayout;
    QHBoxLayout *displayLayout;
    QGridLayout *editAreaLayout;


    CommandManager *commandManager;


    bool ctrlKeyPressed;

};


//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

class DisplayUnitsPlane : public QWidget
{
    Q_OBJECT

public:
    DisplayUnitsPlane(QWidget* parent, CommandManager *cmd);
    ~DisplayUnitsPlane();

    void addDisplayUnit(VarDisplayUnit *unit);

    void setTextNeedTrans();

    std::list<VarDisplayUnit*> getDisplayUnitList() const;//use in void XmlFlochaTransfer::transDisplayUnitToNode()

private:

#ifdef _DEBUG
    void paintEvent(QPaintEvent *paintevent);
#endif

    void btn_addNewUnit();
    void btn_removeUnit();
    void btn_removeAllUnit();

    QPushButton *ui_button_newUnit;
    QPushButton *ui_button_removeUnit;

    CommandManager *commandManager;

    std::list<VarDisplayUnit*> displayUnitList;
    VarDisplayUnit* selectedUnit;
    QVBoxLayout *mainLayout;
    QVBoxLayout *unitLayout;
    QGridLayout *spaceLayout;
    int unitCount;
};





//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------


class VarMonitorManager : public QObject
{
public:
    VarMonitorManager();
    ~VarMonitorManager();

    void addHighPriorityUnit(VarDisplayUnit *unit);
    void addLowPriorityUnit(VarDisplayUnit *unit);

    void removeDisplayUnit(VarDisplayUnit *unit);

    int startVarMonitorThread();
    void stopVarMonitorThread();

    void setUpdateLowPriorty(bool yesno);

private:

    QTimer *routineTimer;
    void VarMonitorRoutine();

    std::list<VarDisplayUnit*> highPriorityUnitList;
    std::list<VarDisplayUnit*> lowPriorityUnitList;

    std::list<VarDisplayUnit*> highPriorityList_buff;
    std::list<VarDisplayUnit*> lowPriorityList_buff;


    std::list<VarDisplayUnit*>::iterator it_high;// = this->highPriorityUnitList.begin();
    std::list<VarDisplayUnit*>::iterator it_low;// = this->lowPriorityUnitList.begin();

    //	HANDLE threadHandle;
    HANDLE timerEndEvent;
    //	HANDLE lowListCopyEvent;

    bool highListChanged;
    bool lowListChanged;
    bool updateLowPriorty;
    bool timerRunFlag;

};



/*
class VarMonitorManager : public QObject
{
friend unsigned int WINAPI VarMonitorThread(void *ownerobject);
public:
VarMonitorManager();
~VarMonitorManager();

void addHighPriorityUnit(VarDisplayUnit *unit);
void addLowPriorityUnit(VarDisplayUnit *unit);

void startVarMonitorThread();
void stopVarMonitorThread();

void setUpdateLowPriorty(bool yesno);

private:

std::list<VarDisplayUnit*> highPriorityUnitList;
std::list<VarDisplayUnit*> lowPriorityUnitList;

std::list<VarDisplayUnit*> highPriorityList_buff;
std::list<VarDisplayUnit*> lowPriorityList_buff;

HANDLE threadHandle;
HANDLE highListCopyEvent;
HANDLE lowListCopyEvent;

bool highListChanged;
bool lowListChanged;
bool updateLowPriorty;
bool threadRunFlag;

};

//---------------------------------
//---------------------------------
//---------------------------------
//---------------------------------
//---------------------------------
//---------------------------------
unsigned int WINAPI VarMonitorThread(void *ownerobject);
*/










#endif // VARDISPLAYMANAGE_H
