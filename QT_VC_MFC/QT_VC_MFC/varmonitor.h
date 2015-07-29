#ifndef VARMONITOR_H
#define VARMONITOR_H

#include <process.h>
#include <cmath>


#include <QThread>
#include <QDesktopWidget>
#include <QApplication>
#include <QWidget>
#include <QColorDialog >
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QAction>
#include <QMenu>
#include <QToolButton>
#include <QDoubleSpinBox>
#include <QToolBar>
#include <QPixmap>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPainter>


#ifdef _DEBUG
#include <QMessageBox>
#include <QDebug>
#endif

#include "commandmanager.h"

#define	PHOTONICONPATH QString("./icons/photon/")
#define MONITORICONSIZE 24
#define MONITORCOLORICONSIZE 12


#define MAXTIMELINEPLOTNUM 8


#define MAXTIMELINEDATANUM 4000000
#define MAXTIMELINEDATARATE 15000

#define TOPBOTTONSPACEPIX 10
#define TEXTSHIFTPIXEL 5

#define MONITORLABELWIDTH 40
#define DEFAULTTOTALDATATIME 10.0 //in mili second

enum 
{
    DisplayMode_normal,
    DisplayMode_paper,

    ScaleMode_autoScale,
    ScaleMode_fixToCurr,
    ScaleMode_expandOnly,

    ActivateFlag_runing,
    ActivateFlag_pause,
    ActivateFlag_stop,
};


class TimeLinePlotUnit : public QWidget
{
    Q_OBJECT

public:
    TimeLinePlotUnit(QWidget *parent);
    ~TimeLinePlotUnit();

    void appendData(const double &data);

    void startTimeLinePlot();
    void stopTimeLinePlot();
    void pauseTimeLinePlot();

    void changeTotalTime(unsigned int msec);
    void setScaleMode(int mode);
    void setDisplayMode(int mode);
    void setColor(const QColor &color);


private:
    void paintEvent(QPaintEvent *event);

    void dealUpdateTimer();


    unsigned int dataPeriod;//unit is ms
    //unsigned int dataFrequency;//unit is Hz
    unsigned int totalDataTime;//unit is ms
    unsigned int dataNum;

    double tickCountFrequency;

    double *dataVector;
    double *data_start_p;
    double *data_end_p;

    double dataMax;//calculate in TimeLinePlotUnit::appendData()
    double dataMin;//calculate in TimeLinePlotUnit::appendData()

    double dataMaxBuff;//only use in TimeLinePlotUnit::appendData
    double dataMinBuff;//only use in TimeLinePlotUnit::appendData

    double dataAxisMax;//calculate in TimeLinePlotUnit::paintEvent();
    double dataAxisMin;//calculate in TimeLinePlotUnit::paintEvent();

    double axisStep;//use when axisScaleMode == ScaleMode_fixToCurr

    //LARGE_INTEGER tickFrequency;
    LARGE_INTEGER tickCountStart;
    LARGE_INTEGER tickCountLast;
    LARGE_INTEGER tickCountTmp;

    QTimer *timerToUpdatePlot;

    QColor curveColor;
    QColor archiveColor;
    QColor gridLineColor;

    int labelWidth;
    int labelOffset;

    //---------------

    int unitActivateFlag;

    int axisScaleMode;
    bool autoScaleFlag;//even if axisScaleMode == ScaleMode_fixToCurr, also need to autoScale first time

    int displayMode;


    //---------------
};



//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------


typedef struct
{
    int successGetVar;//0-fail 1-success
    TimeLinePlotUnit *plotUnit;
    QString varName;
    QPushButton *curveColorBtn;
    //	QColor curveColor;
    QLineEdit *varNameEdit;
    QLabel *varValuelabel;
    QGridLayout *layout;
    std::vector<QObject*> otherObjects;
    double value;


}TimeLineUnitStruct;


typedef struct
{
    QString varName;
    QColor curveColor;

}TimeLineUnitSetting;

class VarMonitorTimeLine : public QWidget
{
    Q_OBJECT

public:
    VarMonitorTimeLine(QWidget *parent, CommandManager *cmd);
    ~VarMonitorTimeLine();

    bool canUpdate();

    void updateMonitor();

    void setCommandManager(CommandManager *cmd);

private:

    void paintEvent(QPaintEvent *event);

    void loadPlotUnitSetting();

    void adjustPlotUnitNum();

    void varNameEditChange();
    void varNameEditFinish();

    void curveColorStartEdit();
    void setBtnColorIcon(QPushButton *btn, QColor color);

    void updValueLabel();

    std::vector<TimeLineUnitStruct> plotUnitStructList;



    void act_switchMoitorOnOff();
    void act_switchDisplayMode();
    void act_switchScaleMode(QAction *act);

    void act_changeChannelNum(QAction *act);

    void act_changeDataTime(double second);
    void act_dataTimeStartEdit();
    void act_dataTimeChanged();

    //-----------ui----------------

    QAction *ui_act_monitorOnOff;
    QAction *ui_act_paperMode;

    QMenu *ui_menu_scaleMode;
    QToolButton *ui_toolBtn_scaleMode;
    QAction *ui_act_autoScale;
    QAction *ui_act_fixCurrScale;
    QAction *ui_act_expandOnlyScale;

    QMenu *ui_menu_channelNum;
    QActionGroup *ui_actGru_channelNum;
    QToolButton *ui_toolBtn_channelNum;
    QAction *ui_act_channelNum[MAXTIMELINEPLOTNUM];


    QAction *ui_act_editDataTime;
    QDoubleSpinBox *ui_spin_dataTime;


    QToolBar *toolBar;




    //-----------ui----------------

    TimeLineUnitSetting plotUnitSetting[MAXTIMELINEPLOTNUM];

    int currUnitNum;
    int targetUnitNum;

    int axisScaleMode;

    //unsigned int totalDataTime;//unit is ms

    QTimer *updLabelTimer;


    bool canUpdateFlag;
    bool varNameEditing;


    QGridLayout *mainLayout;
    QVBoxLayout *plotUnitLayout;


    CommandManager *commandManager;


};




//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------



class VarMonitorControl : public QObject
{
public:
    VarMonitorControl(CommandManager *cmd);
    ~VarMonitorControl();


    void startRoutineThread();
    void stopRoutineThread();

    void showMonitor();

    void setCommandManager(CommandManager *cmd);

private:

    bool threadRunFlag;

    VarMonitorTimeLine *varMonitorTimeLine;
    //QThread varMonitorThread;

};


#endif // VARMONITOR_H
