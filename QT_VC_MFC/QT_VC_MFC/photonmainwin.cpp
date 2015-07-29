#include "photonmainwin.h"
#include "globalobjmanager.h"

extern GlobalObjManager *globalObjManager;


UIChnageTypeClass::UIChnageTypeClass()
    :target(NULL),
    parent(NULL),
    iconFileName(QString("")),
    xmlFileName(QString("")),//no change
    menuMoveType(0),//no change
    inToolBar(0),//no change
    toolbarMoveType(0)//no change
{
}


UIChnageTypeClass::~UIChnageTypeClass()
{
}



//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------


//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------

PhotonXmlManager::PhotonXmlManager(PhotonMainWin *partner)
    :photonPartner(partner)	
{
#ifdef _DEBUG
    if(!partner)
        QMessageBox::warning(NULL, QString("error"), QString("photon partner is null"));
#endif
    globalObjManager = new GlobalObjManager;


}

PhotonXmlManager::~PhotonXmlManager()
{
    actNodeMap.clear();
    manuNodeMap.clear();

    delete globalObjManager;
    globalObjManager = nullptr;
}

int PhotonXmlManager::loadXmlToCreateUI(QIODevice *device)
{
    photonDomDoc.clear();

    //textAndCommentItem.clear();
    QString errorStr;
    int errorLine;
    int errorColumn;


    if (!photonDomDoc.setContent(device, true, &errorStr, &errorLine, &errorColumn)) 
    {
        QMessageBox::information(NULL, tr("DOM Bookmarks"),
            tr("Parse error at line %1, column %2:\n%3")
            .arg(errorLine)
            .arg(errorColumn)
            .arg(errorStr));
        return -1;
    }


    QDomElement root = photonDomDoc.documentElement();
    if(root.tagName() != PHOTONXMLTAG_ROOTNAME)
    {
        QMessageBox::warning(NULL, tr("Format not correct"), tr("Root name not correct"));
        return -1;
    }



    QDomElement lv_one = root.firstChildElement(PHOTONXMLTAG_MENUBAR);
    if(lv_one.isNull())
    {
        QMessageBox::warning(NULL, tr("Format not correct"), tr("No menuBar tag"));
        return -1;
    }

    //------------------------create menu and action-------------------
    QDomElement lv_two = lv_one.firstChildElement(PHOTONXMLTAG_MENU);
    createMenu(lv_two);
    //------------------------create menu and action-------------------


    lv_one = root.firstChildElement(PHOTONXMLTAG_TOOLBAR);
    if(!lv_one.isNull())
    {
        lv_two = lv_one.firstChildElement(PHOTONXMLTAG_ACTION);
        createToolBar(lv_two);
    }

    lv_one = root.firstChildElement(PHOTONXMLTAG_ICONPATHLIST);
    if(!lv_one.isNull())
    {
        lv_two = lv_one.firstChildElement(PHOTONXMLTAG_ICONPATH);
        createIconList(lv_two);
    }

    loadItemBoxItem();

    return 0;
}


void PhotonXmlManager::closeXmlFile()
{
}


void PhotonXmlManager::newXmlFile()
{
    photonDomDoc.clear();


    QDomElement root = photonDomDoc.createElement(PHOTONXMLTAG_ROOTNAME);
    photonDomDoc.appendChild(root);

    QDomElement menubar = photonDomDoc.createElement(PHOTONXMLTAG_MENUBAR);
    root.appendChild(menubar);

    QDomElement toolbar = photonDomDoc.createElement(PHOTONXMLTAG_TOOLBAR);
    root.appendChild(toolbar);
}


bool PhotonXmlManager::saveXmlFile(QIODevice *device)
{
    const int IndentSize = 4;

    QTextStream out(device);
    photonDomDoc.save(out, IndentSize);

    return true;
}


void PhotonXmlManager::updNodeAfterUIChanged(UIChnageTypeClass chnagedata)
{


}


void PhotonXmlManager::removeNode(QWidget *target)
{
}


void PhotonXmlManager::loadItemBoxItem()
{
    if(!globalObjManager) return;

    XmlToFlochaItem *itembox = globalObjManager->itemFlochaItem;

    if(!itembox) return;

    QDomElement root = photonDomDoc.documentElement();

    QDomElement itemassem = root.firstChildElement(PHOTONXMLTAG_ITEMBOX);
    QDomElement tempnode;


    if(!itemassem.isNull())
    {
        itemassem = itemassem.firstChildElement(XmlNodeTypeAssem);
    }


    while(!itemassem.isNull())
    {
        if(itemassem.tagName() == XmlNodeTypeAssem)
        {
            tempnode = itemassem;
            itembox->addFlochaItem(tempnode);
        }
        itemassem = itemassem.nextSiblingElement();
        //QDomNode testnode = itemassem.nextSibling();
        //if(!testnode.isNull())
        //{
        //    itemassem = testnode.toElement();;
        //}

    }

}


void PhotonXmlManager::createMenu(QDomElement menu_ele)
{

    while(!menu_ele.isNull())
    {

        if(menu_ele.tagName() != PHOTONXMLTAG_MENU)
        {
            menu_ele = menu_ele.nextSiblingElement();
            continue;
        }
        QString menuname = menu_ele.firstChildElement(PHOTONXMLTAG_TITLE).text();
        if(menuname.isEmpty())
        {
            menuname = tr("No name");
        }
        QMenu *menu = new QMenu(menuname,photonPartner);
        photonPartner->addUserDefineMenu(menu);


        manuNodeMap.insert(std::make_pair(menu,menu_ele));
        //------------------------create action-------------------

        QDomElement act_ele = menu_ele.firstChildElement(PHOTONXMLTAG_ACTION);
        createAction(menu,act_ele);

        //------------------------create action-------------------

        menu_ele = menu_ele.nextSiblingElement();
    }
}


void PhotonXmlManager::createAction(QMenu *menu, QDomElement act_ele)
{
    while(!act_ele.isNull())
    {
        if(act_ele.tagName() != PHOTONXMLTAG_ACTION)
        {
            act_ele = act_ele.nextSiblingElement();
            continue;
        }
        QString actname = act_ele.firstChildElement(PHOTONXMLTAG_TITLE).text();
        QString xmlpath = act_ele.firstChildElement(PHOTONXMLTAG_XMLPATH).text();
        QString iconpath = act_ele.firstChildElement(PHOTONXMLTAG_ICONPATH).text();
        QString prompt = act_ele.firstChildElement(PHOTONXMLTAG_PROMPT).text();

        if(actname.isEmpty())
        {
            QMessageBox::warning(NULL, tr("Xml file error"), tr("Ation name can not be null"));
            actname = tr("no name");
        }
        QAction *action = new QAction(actname,photonPartner);
        QIcon icon;

        if(!iconpath.isEmpty())
        {
            QFile file(iconpath);
            if(file.exists())
                icon.addFile(iconpath);
        }
        if(!icon.isNull())
            action->setIcon(icon);

        if(!prompt.isEmpty())
            action->setToolTip(prompt);

        photonPartner->addUserDefineAction(menu ,action, xmlpath);

        actNodeMap.insert(std::make_pair(action,act_ele));

        act_ele = act_ele.nextSiblingElement();
    }
}


void PhotonXmlManager::createToolBar(QDomElement act_ele)
{
    while(!act_ele.isNull())
    {
        if(act_ele.tagName() == PHOTONXMLTAG_ACTION)
        {
            for(auto it = actNodeMap.begin(); it!= actNodeMap.end(); ++it)
            {
                if(it->second == act_ele)
                {
                    photonPartner->addUserDefineToolAct(it->first);
                    break;
                }
            }
        }
        act_ele = act_ele.nextSiblingElement();
    }
}

void PhotonXmlManager::createIconList(QDomElement icon_ele)
{
    int index = 0;
    while(!icon_ele.isNull())
    {
        if(icon_ele.tagName() == PHOTONXMLTAG_ICONPATH)
        {
            globalObjManager->iconList->insertIcon(icon_ele.text());
        }
        icon_ele = icon_ele.nextSiblingElement();
    }

}

void PhotonXmlManager::setEleNodeText(QDomElement &elenode, const QString &text)
{
    if(elenode.firstChild().isNull() || !elenode.firstChild().isText())
    {
        QDomText textnode = photonDomDoc.createTextNode(text);
        elenode.insertBefore(textnode,elenode.firstChild());
    }
    else
    {
        elenode.firstChild().setNodeValue(text);
    }
}

//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------

PhotonMainWin::PhotonMainWin(QWidget *parent, int mode)
    : QMainWindow(parent),
    engineerMode(mode),
    settingFileState(FileSaved)
{
    ui_toolBar_default = NULL;
    ui_toolBar_engineer = NULL;
    ui_toolBar_user = NULL;

    settingManager = NULL;

    commuSetupUI = nullptr;


    driveControl = new SomeDriveCtrl;
    globleVarPool = new PCVariablePool;
    commandManager = new CommandManager(globleVarPool, driveControl);


    commandManager->connectDrive();



    createDefaultUI();

    if(engineerMode)
    {
        createEngineerUI();
        this->menuBar()->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this->menuBar(), &QMenuBar::customContextMenuRequested,
            this, &PhotonMainWin::ShowContextMenu);
    }

    createUserDefineUI();

#ifdef _DEBUG
    action_openFloMainWin();
#endif




    setTextNeedTrans();

    resize(800,600);
}

PhotonMainWin::~PhotonMainWin()
{

    if(settingManager)//cannot put this at botton of ~PhotonMainWin(). Why???
        delete settingManager;

#ifdef _DEBUG
    qDebug() << "finish ~settingManager";
#endif


    for(auto it = floMainWinList.begin(); it != floMainWinList.end(); ++it)
    {
        delete *it;
    }

    floMainWinList.clear();


#ifdef _DEBUG
    qDebug() << "finish ~floMainWinList";
#endif


    if(commuSetupUI)
        delete commuSetupUI;
    if(commandManager != nullptr)
        delete commandManager;

#ifdef _DEBUG
    qDebug() << "finish ~commandManager";
#endif


    if(driveControl != nullptr)
        delete driveControl;

#ifdef _DEBUG
    qDebug() << "finish ~driveControl";
#endif


    if(globleVarPool != nullptr)
        delete globleVarPool;

    commandManager = nullptr;
    driveControl = nullptr;
    globleVarPool = nullptr;

#ifdef _DEBUG
    qDebug() << "finish ~PhotonMainWin";
#endif

}


void PhotonMainWin::addUserDefineMenu(QMenu *menu)
{
    for(auto it = ui_menuList.begin(); it != ui_menuList.end(); ++it)
    {
        if(*it == menu)
            return;
    }

    if(engineerMode)
    {
        menu->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(menu, &QMenu::customContextMenuRequested,
            this, &PhotonMainWin::ShowContextMenu);
    }

    this->menuBar()->insertMenu(ui_act_addMenu,menu);
    ui_menuList.push_back(menu);

}

void PhotonMainWin::addUserDefineAction(QMenu *menu, QAction *action, QString xmlpath)
{
    for(auto it = ui_actionXmlList.begin(); it != ui_actionXmlList.end(); ++it)
    {
        if(it->first == action)
            return;
    }


    menu->addAction(action);
    ui_actionXmlList.insert(std::make_pair(action,xmlpath));

    //maybe should create flocha main win here
}

void PhotonMainWin::addUserDefineToolAct(QAction *action)
{
    for(auto it = ui_toolbarActList.begin(); it != ui_toolbarActList.end(); ++it)
    {
        if(*it == action)
            return;
    }

    ui_toolBar_user->addAction(action);
}

bool PhotonMainWin::eventFilter(QObject* watched, QEvent* event)
{

    if(event->type()==QEvent::MouseButtonPress) {
        if (dynamic_cast<QMouseEvent*>(event)->buttons() & Qt::RightButton)
        {
            QMenu *target = dynamic_cast<QMenu*>(watched);
            if(target)
            {
                int test = 0;
                qDebug() << target->title();
            }
        }
    }


    return QWidget::eventFilter(watched, event);

}


void PhotonMainWin::closeEvent(QCloseEvent * event)
{
    for(auto it = floMainWinList.begin(); it != floMainWinList.end(); ++it)
    {
        delete (*it);
    }
    floMainWinList.clear();
}



void PhotonMainWin::getDefaultSetting()
{
    if(engineerMode)
        settingPath = QFileDialog::getOpenFileName(this, tr("Open setting file"), QDir::currentPath(),
        tr("stxml files (*.stxml *.xml)"));
    else
    {
        settingPath = PHOTONXMLDEFAULTPATH;//QDir::currentPath() + QString("/photon_setting.stxml");
    }
}


void PhotonMainWin::setTextNeedTrans()
{
    ui_act_commuSetup->setText(tr("Communication setup"));

    ui_act_configCenter->setText(tr("Configeration Center"));
    
    ui_act_tradChines->setText(tr("Traditional Chinese"));

    ui_act_English->setText(tr("English"));

    ui_act_Japanese->setText(tr("Japanese"));

    ui_menu_default->setTitle(tr("Basic"));

    ui_menu_Language->setTitle(tr("Language"));





    if(engineerMode)
    {
        ui_act_addMenu->setText(tr("New menu"));

        ui_act_newSetting->setText(tr("New user setting"));
        ui_act_loadSetting->setText(tr("Load user setting"));
        ui_act_saveSetting->setText(tr("Save user setting"));
        ui_act_openFlochaWin->setText(tr("Open flocha editor"));

        ui_act_rename->setText(tr("Rename"));
        ui_act_remove->setText(tr("Remove"));
        ui_act_movePrevious->setText(tr("Move pre"));
        ui_act_moveNext->setText(tr("Move next"));

        ui_act_editXmlPath->setText(tr("Edit xml path"));
        ui_act_showInToolbar->setText(tr("Show in toolbar"));

        ui_menu_engineer->setTitle(tr("Engineer tool"));
    }



    this->setWindowTitle(tr("Photon ver 1.0"));
}


void PhotonMainWin::createDefaultUI()
{

    ui_act_commuSetup = new QAction(this);
    ui_act_commuSetup->setIcon(QIcon(PHOTONICONPATH + "iconCommuSetup.png"));
    connect(ui_act_commuSetup, &QAction::triggered, this, &PhotonMainWin::act_openCommuSetup);


    ui_act_configCenter = new QAction(this);
    ui_act_configCenter->setIcon(QIcon(PHOTONICONPATH + "iconConfig.png"));
    connect(ui_act_configCenter, &QAction::triggered, this, &PhotonMainWin::act_openConfigCenter);


    ui_act_tradChines = new QAction(this);
    connect(ui_act_tradChines, &QAction::triggered, this, &PhotonMainWin::act_changeLanguage);


    ui_act_English = new QAction(this);
    connect(ui_act_English, &QAction::triggered, this, &PhotonMainWin::act_changeLanguage);


    ui_act_Japanese = new QAction(this);
    connect(ui_act_Japanese, &QAction::triggered, this, &PhotonMainWin::act_changeLanguage);




    ui_menu_default = new QMenu(this);
    

    ui_menu_default->addAction(ui_act_commuSetup);
    ui_menu_default->addAction(ui_act_configCenter);


    ui_menu_Language = new QMenu(this);
    ui_menu_Language->addAction(ui_act_tradChines);
    ui_menu_Language->addAction(ui_act_English);
    ui_menu_Language->addAction(ui_act_Japanese);

    this->menuBar()->addMenu(ui_menu_default);
    this->menuBar()->addMenu(ui_menu_Language);


    if(ui_toolBar_default == NULL)
        ui_toolBar_default = new QToolBar(this);

    ui_toolBar_default->addAction(ui_act_commuSetup);
    ui_toolBar_default->addAction(ui_act_configCenter);

    addToolBar(ui_toolBar_default);

}



void PhotonMainWin::createEngineerUI()
{
    ui_act_addMenu = new QAction(this);

    ui_act_newSetting = new QAction(this);
    ui_act_loadSetting = new QAction(this);
    ui_act_saveSetting = new QAction(this);
    ui_act_openFlochaWin = new QAction(this);

    connect(ui_act_openFlochaWin, &QAction::triggered, this, &PhotonMainWin::action_openFloMainWin);


    ui_act_rename = new QAction(this);
    ui_act_remove = new QAction(this);
    ui_act_movePrevious = new QAction(this);
    ui_act_moveNext = new QAction(this);

    ui_act_editXmlPath = new QAction(this);
    ui_act_showInToolbar = new QAction(this);






    ui_menu_engineer = new QMenu(this);



    ui_menu_engineer->addAction(ui_act_newSetting);
    ui_menu_engineer->addAction(ui_act_loadSetting);
    ui_menu_engineer->addAction(ui_act_saveSetting);
    ui_menu_engineer->addSeparator();
    ui_menu_engineer->addAction(ui_act_openFlochaWin);


    this->menuBar()->addAction(ui_act_addMenu);
    this->menuBar()->addMenu(ui_menu_engineer);


    if(ui_toolBar_default == NULL)
        ui_toolBar_default = new QToolBar(this);

    ui_toolBar_default->addAction(ui_act_newSetting);
    ui_toolBar_default->addAction(ui_act_loadSetting);
    ui_toolBar_default->addAction(ui_act_saveSetting);
    ui_toolBar_default->addSeparator();
    ui_toolBar_default->addAction(ui_act_openFlochaWin);

    addToolBar(ui_toolBar_default);


}



void PhotonMainWin::createUserDefineUI()
{
    getDefaultSetting();

    QFile file(settingPath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("error"),	tr("Cannot read file %1:\n%2.").arg(settingPath).arg(file.errorString()));

        newUserSetting();
    }
    else
    {
        if(settingManager == NULL)
            settingManager = new PhotonXmlManager(this);

        settingManager->loadXmlToCreateUI(&file);
    }
    file.close();

}

void PhotonMainWin::renameItem()
{

}

void PhotonMainWin::newUserSetting()
{
    if(settingFileState == FileNotSave)
    {
        if(!saveAsUserSetting())
            return;
    }

    if(settingManager == NULL)
        settingManager = new PhotonXmlManager(this);

    settingManager->newXmlFile();

}


void PhotonMainWin::loadUserSetting()
{
    if(settingFileState == FileNotSave)
    {
        if(!saveAsUserSetting())
            return;
    }

    settingPath = QFileDialog::getOpenFileName(this, tr("Open setting file"), QDir::currentPath(),
        tr("stxml files (*.stxml)"));
    QFile file(settingPath);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("error"),	tr("Cannot read file %1:\n%2.").arg(settingPath).arg(file.errorString()));
    }
    else
    {
        if(settingManager == NULL)
            settingManager = new PhotonXmlManager(this);

        settingManager->loadXmlToCreateUI(&file);
    }
}

bool PhotonMainWin::saveUserSetting()
{


    settingFileState = FileSaved;
    return true;
}

bool PhotonMainWin::saveAsUserSetting()
{

    settingFileState = FileSaved;
    return true;
}


void PhotonMainWin::ShowContextMenu(const QPoint& pos)
{
    QMenu menu;
    QAction *action = nullptr;
    QAction *actmenu = dynamic_cast<QAction *>(this->menuBar()->actionAt(pos));

    for(auto it_menu = ui_menuList.begin(); it_menu != ui_menuList.end(); ++it_menu)
    {
        /*
        for(int i = 0; i <500; ++i)
        {
        for(int j = 0; j <500; ++j)
        {
        action = (*it_menu)->actionAt(QPoint(i,j));
        if(action)
        {
        qDebug() << action->text();
        qDebug() << "action pos" << i << j;
        qDebug() << "menu pos" << (*it_menu)->pos();
        break;
        }
        }
        if(action)
        {
        //qDebug() << action->text();
        break;
        }
        }
        */
        action = (*it_menu)->actionAt(pos);
        if(action)
        {
            m_actToOperat = action;
            m_manuToOperat = NULL;

#ifdef _DEBUG
            qDebug() << action->text();
#endif


            menu.addAction(ui_act_rename);
            menu.addAction(ui_act_remove);
            menu.addAction(ui_act_movePrevious);
            menu.addAction(ui_act_moveNext);
            menu.addAction(ui_act_editXmlPath);
            menu.addAction(ui_act_showInToolbar);

            QPoint popPt = (*it_menu)->mapToGlobal(pos);

            menu.exec(popPt);

            break;
        }

        if((*it_menu)->menuAction() == actmenu)
        {

            m_actToOperat = NULL;
            m_manuToOperat = (*it_menu);


#ifdef _DEBUG
            qDebug() << (*it_menu)->title();
#endif


            menu.addAction(ui_act_rename);
            menu.addAction(ui_act_remove);
            menu.addAction(ui_act_movePrevious);
            menu.addAction(ui_act_moveNext);

            QPoint popPt = this->mapToGlobal(pos);

            menu.exec(popPt);

            break;
        }
    }

}

void PhotonMainWin::action_openFloMainWin()
{
    FlowchartMainWin *flomainwin;
    if(commandManager != nullptr)
        flomainwin = new FlowchartMainWin(nullptr, commandManager);

    connect(flomainwin, &FlowchartMainWin::closeSignal, this, &PhotonMainWin::closeFloMainWinSlot);

    flomainwin->show();


    floMainWinList.push_back(flomainwin);
}


void PhotonMainWin::closeFloMainWinSlot()
{
    FlowchartMainWin *toclose = dynamic_cast<FlowchartMainWin*>(QObject::sender());

    for(auto it = floMainWinList.begin(); it != floMainWinList.end(); ++it)
    {
        if(*it == toclose)
        {
            delete (*it);
            floMainWinList.erase(it);
            break;
        }
    }

}

void PhotonMainWin::act_openCommuSetup()
{
    if(commuSetupUI == nullptr)
        commuSetupUI = new CommuSetupUI(nullptr, driveControl);
    commuSetupUI->show();
}


void PhotonMainWin::act_openConfigCenter()
{
    int test = 0;
    test = 100;
}


void PhotonMainWin::act_changeLanguage()
{
    QAction *sender = dynamic_cast<QAction*>(QObject::sender());

    if(sender == ui_act_tradChines)
    {
        translator.load("./photon_zh.qm");
        qApp->installTranslator(&translator);
        
    }
    else if(sender == ui_act_English)
    {
        translator.load("./photon_en.qm");
        qApp->installTranslator(&translator);
    }
    else if(sender == ui_act_Japanese)
    {
        translator.load("./photon_ja.qm");
        qApp->installTranslator(&translator);
    }




    setTextNeedTrans();

    for(auto it = floMainWinList.begin(); it != floMainWinList.end(); ++it)
    {
        (*it)->setTextNeedTrans();
    }
}