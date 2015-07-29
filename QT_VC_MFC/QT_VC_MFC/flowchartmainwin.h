#ifndef FLOWCHARTMAINWIN_H
#define FLOWCHARTMAINWIN_H


#include <QtWidgets/QMainWindow>

#include <QString>

#include <QAction>
#include <QPushButton>
#include <QMenu>
#include <QToolBar>
#include <QMenuBar>
#include <QTabWidget>
#include <QMessageBox>
#include <QDockWidget>

#include <QApplication>

#include <process.h>
#include <QFileDialog>

#ifdef _DEBUG
#include <qDebug>
#endif


#include "qmainwindow.h"
#include "flowchartwidget.h"
#include "commandmanager.h"
#include "xmlflochatransfer.h"
#include "vardisplaymanage.h"
#include "itemboxwidget.h"
#include "varmonitor.h"
#include "transflochatocode.h"

class XmlFlochaTransfer;

struct flochaMainWinSBits
{
    unsigned int fileChanged : 1;
    unsigned int loadDefaultFile : 1;
    unsigned int fileOpened : 1;
    unsigned int fileNotSave : 1;
    unsigned int canRedo : 1;
    unsigned int canUndo : 1;
    unsigned int runDebug : 1;
    unsigned int AutoRun : 1;
    unsigned int pauseRun : 1;


};

union FlochaMainWinState
{
    unsigned int all;
    flochaMainWinSBits bits;

};







class FlowchartMainWin : public QMainWindow
{
    Q_OBJECT
public:


    FlowchartMainWin(QWidget *parent, CommandManager *parentcmd, QString &filename = QString());
    ~FlowchartMainWin(void);

    void setTextNeedTrans();

protected:
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent (QKeyEvent * event);

    void closeEvent(QCloseEvent * event);


signals:

    void closeSignal();

private:
    //function
    void createAction();
    void createMenu();
    void createToolBar();
    void linkCurrTabWidget();

    void updActionEn_Disable();

    void openSubFlochaAssem(FlowchartsAssemble *subflocha);
    void closeSubFlochaAssem(FlowchartsAssemble *subflocha);
    void closeSubAssemTab(int indx);

    void dealHitBreakPoint(FlowchartsTemplate *flocha);
    void dealRunFinished(int result);


    void action_autoRun_triggered();
    void action_debugRun_triggered();
    void action_pauseRun_triggered();
    void action_stopRun_triggered();


    void action_openFile_triggered();
    void action_saveFile_triggered();
    void action_saveFileAs_triggered();

    void action_newFile_triggered();

    //void open_flochaWidget();
    void close_flochaWidget();//can call in top of action_openFile_triggered() or action_newFile_triggered()

    void act_varDisplay_triggered();
    void act_localVar_triggered();
    void act_itemBox_triggered();
    void act_timeMonitor_triggered();


    void act_codeTrans_triggered();

    //function





    //ui member

    //--------------------------action edit-------------------------
    QAction *addNewFlochaIf;
    QAction *addNewFlochaSet;
    QAction *addNewFlochaRun;
    QAction *addNewFlochaSleep;
    QAction *addNewFlochaAssem;


    QAction *undoFlochaChange;
    QAction *redoFlochaChange;


    QAction *copyFlocha;
    QAction *pasteFlocha;
    QAction *removeFlocha;

    QAction *ui_act_assemFlocha;

    //--------------------------action edit-------------------------





    //--------------------------excute-------------------------------
    QAction *act_startAutoRun;
    QAction *act_startDebugRun;
    QAction *act_runNextStep;
    QAction *act_setBreakPoint;
    QAction *act_resetBreakPoint;
    QAction *act_pauseRun;
    QAction *act_stopRun;
    //---------------------------excute-----------------------------





    //--------------------------action file-------------------------
    QAction *createNewFile;
    QAction *loadXmlToFlocha;
    QAction *saveFlochaToXml;
    QAction *ui_act_saveAsFile;
    //--------------------------action file-------------------------


    //--------------------------action view-------------------------
    QAction *ui_act_varDisplay;
    QAction *ui_act_localVar;
    QAction *ui_act_itemBox;
    QAction *ui_act_timeMonitor;
    QAction *ui_act_transToCode;
    //--------------------------action view-------------------------

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *runMenu;
    QMenu *viewMenu;
    QMenu *debugMenu;

    QToolBar *toolBar;


    QTabWidget *mainTabWidget;

    //ui member



    //data member
    FlowchartsAssemble *flochaPtr;
    XmlFlochaTransfer *xmlFlochaTransfer;
    FlowchartWidget *mainFlowchartWidget;
    FlowchartWidget *preFlowchartWidget;
    CommandManager *commandManager;
    CommandManager *parentCmd;


    QString currFileName;


    //data member




    //-----------------display Units Plane-------
    DisplayUnitsPlane *displayUnitsPlane;
    QDockWidget *displayUnitsDock;
    //-----------------display Units Plane-------

    //------------------------pcVarEditWidget---------------
    PCVarEditWidget *pcVarEditWidget;
    QDockWidget *pcVarEditDock;
    //------------------------pcVarEditWidget---------------

    //---------------------item box--------------------
    ItemBoxWidget *itemBoxWidget;
    QDockWidget *itemBoxDock;
    //---------------------item box--------------------


    //----------------------code translator------------
    FlochaToCodeWidget *codeTransWidget;
    QDockWidget *codeTransDock;
    //----------------------code translator------------


    VarMonitorControl *varMonitorControl;
    QThread *varMonitorThread;
    //QDockWidget


    //flags
    int fileState; //0-     1
    bool loadArFile;

    FlochaMainWinState flochaMainWinState;


    bool runDebug;


    //flags
};





#endif //FLOWCHARTMAINWIN_H