#ifndef FLOWCHARTWIDGET_H
#define FLOWCHARTWIDGET_H

#include <QWidget>
#include <QDebug>
#include <iostream>
#include <map>
#include <list>
#include <QPushButton>
#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QMouseEvent>
#include <QApplication>
#include <QToolBar>
#include <QInputDialog>
#include <QLineEdit>
#include <QVector>
#include <QMessageBox>

#include "flowchartsitems.h"
#include "itemboxwidget.h"


enum
{
    editItemMode,
    autoRunMode,
    stepRunMode
};

enum
{
    linkTypeNormal,
    linkTypeIfTrue,
    linkTypeElse,
};



enum
{
    flochaChangeAdd,
    flochaChangeDelete,
    flochaChangeDelete_unlink,
    flochaChangeData,
    pushToUndoStack,
    pushToRedoStack,
};




struct buttonLinkAttri
{
    QPushButton* btnPtr;
    QPushButton* btnPtrIftrue;
    QPushButton* btnPtrElse;
};




class FlowchartWidget : public QWidget
{
    Q_OBJECT

public:
    FlowchartWidget(QWidget *parent,FlowchartsAssemble *parent_flocha);
    ~FlowchartWidget();

    //	void autoRun();

    void setFlochaCmdManager(CommandManager *cmd);



    const FlowchartsAssemble* getMainFlochaAssem();


    int constructFromAssem(FlowchartsAssemble* fromAssem);
    int constructFromAssemLoop(FlowchartsAssemble* parent);


    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent (QKeyEvent * event);


    void addItemIf();
    void addItemSet();
    void addItemRun();
    void addItemSleep();
    void addItemAssem();



    //-----------------------------redo undo---------------------------------------

    //maybe can combine to one func
    void undoFlochaChange();
    void redoFlochaChange();
    //maybe can combine to one func

    //-----------------------------redo undo---------------------------------------


    //-----------------------------run---------------------------------------
    void startAutoRun();
    void startDebugRun();//or continue debug run
    void runNextStep();
    void setBreakPoint();
    void resetBreakPoint();
    void pauseRun();
    void stopRun();

    void setSuspendBtn(FlowchartsTemplate *flocha);
    //-----------------------------run---------------------------------------

    void copySelectFlochas();
    void pasteCopyFlochas();
    void removeSelectFlochas();

    void assembleSelectFlochas();

    void addSelectAssemToItemBox();


signals:
    void flochaIsChanged(FlowchartsTemplate* flocha);//include add new flocha
    void flochaIsRemoved(FlowchartsTemplate* flocha);
    void saveFlocha();
    void subFlochaAssemOpenSignal(FlowchartsAssemble* subflocha);
    void subFlochaAssemDeleteSignal(FlowchartsAssemble* subflocha);

    void hitBreakPointSignal(FlowchartsTemplate *flocha);
    void runFinishedSignal(int result);

private:

    void paintEvent(QPaintEvent *event);
    void drawLinkLine(QPainter *painter,QRect rect1,QRect rect2,int type = linkTypeNormal);
    void drawSelectedBtn(QPainter *painter);//and paint break btn and suspend btn



    //----------------------drag and drop----------------------
    bool eventFilter(QObject* watched, QEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    //----------------------drag and drop----------------------




    int linkButtonFromAssem();
    void deleteAll();

    //------------------------------useful function--------------------------
    QPushButton *findButtonByFlocha(FlowchartsTemplate* flocha); 

    void setBtnTextByflocha(QPushButton *btn,FlowchartsTemplate* flocha);
    //std::list<QPushButton*> findBtnLinkToBtn(QPushButton *btn);
    void unlinkBtn(QPushButton *btn);


    //not use yet, collect all widget change after flocha changed, for redo undo, maybe can use in linkButtonFromAssem
    void updWidgetAfterFloChange(std::list<FlowchartsTemplate *> &flochalist, int type);
    //not use yet, collect all widget change after flocha changed, for redo undo, maybe can use in linkButtonFromAssem

    QPushButton* createBtnForFlocha(FlowchartsTemplate *flocha);//not use yet, can use in add flocha, copy paste,redo undo
    void drawBtn(QPushButton *btn);

    void setNewflochaIdName(FlowchartsTemplate *newflocha);

    void linkTwoBtn_flocha(QPushButton *src, QPushButton *dst);

    std::list<QPushButton*> findBtnListInRect(QRect rect);

    //------------------------------useful function--------------------------






    //-----------------------------redo undo---------------------------------------
    //FlowchartsTemplate* cloneFlocha(FlowchartsTemplate *flocha);//clone this widget main assem's sub

    //if flodst is null, clone flodata. if flodst is not null, paste flodata to flodst
    FlowchartsTemplate* paste_copyFlochaData(FlowchartsTemplate *flodst,FlowchartsTemplate *flodata, FlowchartsAssemble* dstparent, FlowchartsAssemble* srcparent, bool copylink = true, bool copyfullassem = false);
    //if flodst is null, clone flodata. if flodst is not null, paste flodata to flodst



    //notice: if change type is delete, not every flochas in list are delete, some are just data changed
    void pushFlochaDataToReUndo(std::list<FlowchartsTemplate*> flochalist, int unredo, int type);
    //notice: if change type is delete, not every flochas in list are delete, some are just data changed


    void deleteRedoStack();//call in pushToStack() if pushToUndoWhenRedo == true
    bool pushToUndoWhenRedo;//set true in top of redo(). set false in botton of redo()


    //maybe can combine to one func
    //void undoFlochaChange();
    //void redoFlochaChange();
    //maybe can combine to one func

    std::vector<std::pair<std::list<FlowchartsTemplate*>, int> > floActStackForUndo;
    std::vector<std::pair<std::list<FlowchartsTemplate*>, int> > floActStackForRedo;


    std::map<FlowchartsTemplate*,std::vector<FlowchartsTemplate*> > undoFlochaData;
    std::map<FlowchartsTemplate*,std::vector<FlowchartsTemplate*> > redoFlochaData;

    //-----------------------------redo undo---------------------------------------

    //-----------------------------flocha pop up menu-------------------------------

    QAction *ui_act_editAssemSetting;
    QAction *ui_act_openAssemInTab;
    QAction *ui_act_putAssemToItemBox;
    QAction *ui_act_dispelAssem;


    FlowchartsTemplate *flochaNeedPopMenu;
    void flochaPopupMenu(const QPoint &point);

    void act_editAssemSetting();
    void act_openAssemInTab();
    void act_putAssemToItemBox();
    void act_dispelAssem();


    //-----------------------------flocha pop up menu-------------------------------


    QPushButton *men_preButton;
    QPoint mousePos;
    bool btnLinkElse;
    bool btnDragging;
    bool selectAreaDrag;

    QPushButton *startBtn;
    QPushButton *endBtn;






    //-----------------------------------------------container---------------------------------
    std::map<QPushButton*, FlowchartsTemplate*> button_flocha_map;//include start & end button
    std::map<QPushButton*, buttonLinkAttri> buttonLinkMap;//include start & end button

    //maybe no need
    std::list<QPushButton*> buttonList;//not include start & end button//2015-04-01 include start & end button??

    std::list<QPushButton*> selectedBtnList;
    std::list<QPushButton*> breakBtnList;
    QPushButton* SuspendBtn;

    //-----------------------copy and paste-------------------------------------
    std::list<FlowchartsTemplate*> copyflochaList;
    int copyFlag;

    //FlowchartsAssemble* copyFlochaAssem(FlowchartsAssemble *src);
    std::list<FlowchartsTemplate*> fullCopyFlochaList(const std::list<FlowchartsTemplate*> &copyflochaList, FlowchartsAssemble*dstparent, FlowchartsAssemble*srcparent);

    //-----------------------copy and paste-------------------------------------
    //-----------------------------------------------container---------------------------------




    //std::map<buttonLinkAttri, buttonLinkAttri> buttonLinkMap;
    //std::map<QPushButton*, QPushButton*> buttonLinkMap;
    FlowchartsAssemble *flochaAssem;
    //	FlowchartsAssemble *flochaAssem_parent;




    int tmpCnt;
    QPoint mouseStartPos;
    QPoint posInButton;
    QPoint tempPos;
    QPoint scrollOffsetPos;
    QPoint posOfPushButton;
    CommandManager *commandManager;

    bool pressedCtrlKey;



    int operationMode;










#ifdef _DEBUG
    void printFlochaDebugMsg(FlowchartsTemplate *flocha);


#endif




};

#endif // FLOWCHARTWIDGET_H
