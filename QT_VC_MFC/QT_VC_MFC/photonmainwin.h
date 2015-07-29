#ifndef PHOTONMAINWIN_H
#define PHOTONMAINWIN_H

#include <QMainWindow>
#include <qDebug>

#include <QObject>
#include <QDomDocument>
#include <QString>

#include <QList>

#include <QAction>
#include <QPushButton>
#include <QMenu>
#include <QToolBar>
#include <QMenuBar>
#include <QTabWidget>
#include <QMessageBox>
#include <QTranslator>

#include <QApplication>


#include "flowchartmainwin.h"
#include "commandmanager.h"
#include "MpiControl.h"
#include "commusetupui.h"


#define PHOTONXMLDEFAULTPATH QDir::currentPath() + QString("/defaultSetting.stxml")

#define PHOTONXMLTAG_ROOTNAME QString("PhotonMainWin")
#define PHOTONXMLTAG_MENUBAR QString("MenuBar")
#define PHOTONXMLTAG_MENU QString("Menu")
#define PHOTONXMLTAG_ACTION QString("Action")
#define PHOTONXMLTAG_TOOLBAR QString("ToolBar")
#define PHOTONXMLTAG_TITLE QString("Title")
#define PHOTONXMLTAG_XMLPATH QString("XmlPath")
#define PHOTONXMLTAG_ICONPATH QString("IconPath")
#define PHOTONXMLTAG_PROMPT QString("Prompt")


#define PHOTONXMLTAG_ICONPATHLIST QString("IconPathList")
#define PHOTONXMLTAG_ICONPATH QString("IconPath")


#define PHOTONXMLTAG_ITEMBOX QString("ItemBox")

//#define PHOTONXMLEMPTYFORMAT 


class PhotonMainWin;

struct UIChnageTypeClass
{
public:
    UIChnageTypeClass();
    ~UIChnageTypeClass();
    QWidget *target;
    QWidget *parent;
    QString xmlFileName;
    QString iconFileName;
    int menuMoveType;
    int inToolBar;
    int toolbarMoveType;
};



//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------


class PhotonXmlManager : public QObject
{

public:
    PhotonXmlManager(PhotonMainWin *partner);
    ~PhotonXmlManager();


    int loadXmlToCreateUI(QIODevice *device);

    void closeXmlFile();
    void newXmlFile();
    bool saveXmlFile(QIODevice *device);

    void updNodeAfterUIChanged(UIChnageTypeClass chnagedata);
    void removeNode(QWidget *target);

private:

    void loadItemBoxItem();

    void createMenu(QDomElement menu_ele);
    void createAction(QMenu *menu, QDomElement act_ele);
    void createToolBar(QDomElement act_ele);

    void createIconList(QDomElement icon_ele);


    void setEleNodeText(QDomElement &elenode, const QString &text);



    PhotonMainWin *photonPartner;



    QDomDocument photonDomDoc;

    std::map<QAction*,QDomElement> actNodeMap;
    std::map<QMenu*,QDomElement> manuNodeMap;




};




//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------



class PhotonMainWin : public QMainWindow
{
    Q_OBJECT

public:
    PhotonMainWin(QWidget *parent = nullptr, int mode = 0);
    ~PhotonMainWin();


    //-----------------------engineer func. for xml manager use----------------------
    void addUserDefineMenu(QMenu *menu);
    void addUserDefineAction(QMenu *menu, QAction *action, QString xmlpath);
    void addUserDefineToolAct(QAction *action);
    //-----------------------engineer func. for xml manager use----------------------


private:
    //-------------------------------------------------
    bool eventFilter(QObject* watched, QEvent* event);
    //-------------------------------------------------

    void closeEvent(QCloseEvent * event);

    void getDefaultSetting();

    void setTextNeedTrans();


    void createDefaultUI();
    void createEngineerUI();
    void createUserDefineUI();

    //-----------------------engineer func----------------------

    void renameItem();
    void removeItem();

    void addMenu();
    void addAction();
    void addToToolbar();


    void moveItemToPrevious();
    void moveItemToNext();

    void newUserSetting();
    void loadUserSetting();
    bool saveUserSetting();
    bool saveAsUserSetting();



    void ShowContextMenu(const QPoint&);

    void action_openFloMainWin();
    void closeFloMainWinSlot();

    //-----------------------engineer func----------------------



    //-----------------------basic func----------------------
    void act_openCommuSetup();
    void act_openConfigCenter();

    void act_changeLanguage();

    //-----------------------basic func----------------------




    //------------------data container--------------------------
    std::list<FlowchartMainWin*> floMainWinList;

    //------------------data container--------------------------


    //------------------ui--------------------------

    //	QAction *ui_act_openFlochaWin;





    //-----------------------default ui----------------------
    QAction *ui_act_commuSetup;
    QAction *ui_act_configCenter;


    QAction *ui_act_tradChines;
    QAction *ui_act_English;
    QAction *ui_act_Japanese;

    QMenu *ui_menu_default;
    QMenu *ui_menu_Language;


    QToolBar *ui_toolBar_default;
    //-----------------------default ui----------------------


    //-----------------------engineer ui----------------------

    QAction *ui_act_newSetting;
    QAction *ui_act_loadSetting;
    QAction *ui_act_saveSetting;



    QAction *ui_act_openFlochaWin;


    QAction *ui_act_addMenu;
    QAction *ui_act_addAction;


    //------------------------Context Menu-----------------
    QAction *ui_act_rename;
    QAction *ui_act_remove;
    QAction *ui_act_movePrevious;
    QAction *ui_act_moveNext;

    QAction *ui_act_editXmlPath;
    QAction *ui_act_showInToolbar;
    //------------------------Context Menu-----------------


    QMenu *ui_menu_engineer;

    QToolBar *ui_toolBar_engineer;
    //-----------------------engineer ui----------------------





    //------------------user define ui--------------------------
    std::list<QMenu*> ui_menuList;//not include default and engineer menu
    std::map<QAction*,QString> ui_actionXmlList;

    std::list<QAction*> ui_toolbarActList;

    QToolBar *ui_toolBar_user;
    //------------------user define ui--------------------------


    //------------------ui--------------------------



    //--------------------------------------------
    PhotonXmlManager *settingManager;

    DriveCtrlBase *driveControl;
    PCVariablePool *globleVarPool;
    CommandManager *commandManager;


    CommuSetupUI *commuSetupUI;

    int engineerMode;

    QString settingPath;


    QTranslator translator;

    //--------------------only one can be non null at one time------------
    QAction *m_actToOperat;
    QMenu *m_manuToOperat;
    //--------------------only one can be non null at one time------------


    enum{
        FileNotSave,
        FileSaved,
    };
    int settingFileState;


    //--------------------------------------------
};

#endif // PHOTONMAINWIN_H
