#include "flowchartmainwin.h"


FlowchartMainWin::FlowchartMainWin(QWidget *parent, CommandManager *parentcmd, QString &filename) 
    : QMainWindow(parent)
{

    flochaMainWinState.all = 0;
    currFileName = QString();
    xmlFlochaTransfer = nullptr;
    mainFlowchartWidget = nullptr;
    preFlowchartWidget = nullptr;

    pcVarEditWidget = nullptr;
    pcVarEditDock = nullptr;
    runDebug = false;
    flochaPtr = nullptr;

    displayUnitsPlane = nullptr;
    displayUnitsDock = nullptr;

    itemBoxWidget = nullptr;
    itemBoxDock = nullptr;


    codeTransWidget = nullptr;
    codeTransDock = nullptr;

    varMonitorControl = nullptr;
    varMonitorThread = nullptr;

#ifdef _DEBUG
    if(!parentcmd)
        QMessageBox::warning(this, "error", "parent cmd is null");
#endif

    parentCmd = parentcmd;
    commandManager = nullptr;

    createAction();
    createMenu();
    createToolBar();
    updActionEn_Disable();


    resize(800,600);
    move(500,100);
    


    mainTabWidget = new QTabWidget(this);
    mainTabWidget->setTabsClosable(true);


    if(!filename.isEmpty())
    {
        currFileName = filename;
        flochaMainWinState.bits.loadDefaultFile = 1;
        action_openFile_triggered();

    }
    //	flochaPtr = new FlowchartsAssemble(this);

    //	mainFlowchartWidget = new FlowchartWidget(this,flochaPtr);

    //	mainTabWidget->addTab(mainFlowchartWidget,"main");

    setCentralWidget(mainTabWidget);
    connect(mainTabWidget, &QTabWidget::currentChanged, this, &FlowchartMainWin::linkCurrTabWidget);
    connect(mainTabWidget, &QTabWidget::tabCloseRequested, this, &FlowchartMainWin::closeSubAssemTab);




    connect(act_startAutoRun,&QAction::triggered,this,&FlowchartMainWin::action_autoRun_triggered);
    connect(act_startDebugRun,&QAction::triggered,this,&FlowchartMainWin::action_debugRun_triggered);
    connect(act_pauseRun,&QAction::triggered,this,&FlowchartMainWin::action_pauseRun_triggered);
    connect(act_stopRun,&QAction::triggered,this,&FlowchartMainWin::action_stopRun_triggered);

#ifdef _DEBUG


    //	DisplayIconList *testiconlist = new DisplayIconList(his);
    //	DisplayUnitSetting setting;
    //	setting.all = 0;
    //	std::map<double, QString> textmap;
    //	textmap.clear();
    //	textmap.insert(std::make_pair(1,QString("test1")));
    //	textmap.insert(std::make_pair(2,QString("test2")));
    //	textmap.insert(std::make_pair(3,QString("test3")));
    //	textmap.insert(std::make_pair(4,QString("test4")));
    //	std::map<double, int> iconmap;
    //	iconmap.clear();
    //	VarDisplayUnit *testDisplayUnit = new VarDisplayUnit(this, testiconlist, setting ,textmap, iconmap);
    //	testDisplayUnit->show();
    //	testDisplayUnit->move(200,300);
#endif


    setTextNeedTrans();
}


FlowchartMainWin::~FlowchartMainWin(void)
{

#ifdef _DEBUG
    qDebug() << "start ~FlowchartMainWin()";
#endif
    action_stopRun_triggered();

    delete addNewFlochaIf;
    delete addNewFlochaSet;
    delete addNewFlochaRun;
    delete addNewFlochaSleep;
    delete addNewFlochaAssem;




    delete copyFlocha;
    delete pasteFlocha;
    delete removeFlocha;

    delete ui_act_assemFlocha;



    delete act_startAutoRun;
    delete act_startDebugRun;
    delete act_runNextStep;
    delete act_setBreakPoint;
    delete act_resetBreakPoint;
    delete act_pauseRun;
    delete act_stopRun;


    delete createNewFile;
    delete loadXmlToFlocha;
    delete saveFlochaToXml;
    delete ui_act_saveAsFile;


    delete ui_act_varDisplay;
    delete ui_act_localVar;
    delete ui_act_itemBox;
    delete ui_act_timeMonitor;
    delete ui_act_transToCode;

    delete fileMenu;
    delete editMenu;
    delete debugMenu;
    delete runMenu;
    //delete toolBar;
    if(itemBoxWidget)
        delete itemBoxWidget;

    if(pcVarEditWidget)
        delete pcVarEditWidget;

    if(displayUnitsPlane)
        delete displayUnitsPlane;


    if(varMonitorThread)
    {
        varMonitorControl->stopRoutineThread();

        varMonitorThread->quit();
        varMonitorThread->wait();
        delete varMonitorThread;
    }




    if(varMonitorControl)
        delete varMonitorControl;

    if(flochaPtr)
        delete flochaPtr;
    if(xmlFlochaTransfer)
        delete xmlFlochaTransfer;
    if(commandManager)
        delete commandManager;

#ifdef _DEBUG
    qDebug() << "finish ~FlowchartMainWin()";
#endif

}

void FlowchartMainWin::setTextNeedTrans()
{


    addNewFlochaIf->setText(tr("Add If"));
    addNewFlochaSet->setText(tr("Add Set"));
    addNewFlochaRun->setText(tr("Add Run"));
    addNewFlochaSleep->setText(tr("Add Sleep"));
    addNewFlochaAssem->setText(tr("Add Assem"));

    undoFlochaChange->setText(tr("Undo"));
    redoFlochaChange->setText(tr("Redo"));
    copyFlocha->setText(tr("Copy"));
    pasteFlocha->setText(tr("Paste"));
    removeFlocha->setText(tr("Delete"));
    ui_act_assemFlocha->setText(tr("Assemble select flocha"));

    act_startAutoRun->setText(tr("Auto run"));
    act_startDebugRun->setText(tr("Debug run"));
    act_runNextStep->setText(tr("Run next"));
    act_setBreakPoint->setText(tr("Set break Point"));
    act_resetBreakPoint->setText(tr("Reset break Point"));
    act_pauseRun->setText(tr("Pause"));
    act_stopRun->setText(tr("Stop"));


    createNewFile->setText(tr("New file"));
    loadXmlToFlocha->setText(tr("Open"));
    saveFlochaToXml->setText(tr("Save"));
    ui_act_saveAsFile->setText(tr("Save"));
    
    ui_act_varDisplay->setText(tr("Var display plane"));
    ui_act_localVar->setText(tr("Local var pool"));
    ui_act_itemBox->setText(tr("Item Box"));
    ui_act_timeMonitor->setText(tr("Var monitor"));
    ui_act_transToCode->setText(tr("Code translater"));



    fileMenu->setTitle(tr("file"));
    editMenu->setTitle(tr("edit"));
    runMenu->setTitle(tr("run tool"));
    viewMenu->setTitle(tr("View"));
    debugMenu->setTitle(tr("debug"));

    if(pcVarEditDock)
        pcVarEditDock->setWindowTitle(tr("PC var edit"));

    if(displayUnitsDock)
        displayUnitsDock->setWindowTitle(tr("Show var"));

    if(pcVarEditDock)
        pcVarEditDock->setWindowTitle(tr("PC var edit"));

    if(itemBoxDock)
        itemBoxDock->setWindowTitle(tr("Item box"));


    if(codeTransDock)
        codeTransDock->setWindowTitle(tr("Code translator"));



    //if(varMonitorControl)
    //    varMonitorControl->setTextNeedTrans();

    if(displayUnitsPlane)
        displayUnitsPlane->setTextNeedTrans();

    if(pcVarEditWidget)
        pcVarEditWidget->setTextNeedTrans();

    if(itemBoxWidget)
        itemBoxWidget->setTextNeedTrans();

    if(codeTransWidget)
        codeTransWidget->setTextNeedTrans();

    setWindowTitle(tr("Flow chart main window"));

}


void FlowchartMainWin::keyPressEvent(QKeyEvent * event)
{
    FlowchartWidget *tmpflowid = dynamic_cast<FlowchartWidget*>(mainTabWidget->currentWidget());
    if(tmpflowid)
        tmpflowid->keyPressEvent(event);
}


void FlowchartMainWin::keyReleaseEvent (QKeyEvent * event)
{
    FlowchartWidget *tmpflowid = dynamic_cast<FlowchartWidget*>(mainTabWidget->currentWidget());
    if(tmpflowid)
        tmpflowid->keyReleaseEvent(event);
}


void FlowchartMainWin::closeEvent(QCloseEvent * event)
{
    if(flochaMainWinState.bits.fileOpened)//temp solve the problem
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Tips"), tr("Save file before close?"),
            QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes);

        if(reply == QMessageBox::Yes)
            action_saveFile_triggered();
        else if(reply == QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }
    }


    action_stopRun_triggered();//temp solve the problem

    emit closeSignal();
}




void FlowchartMainWin::createAction()
{

    addNewFlochaIf = new QAction(this);
    addNewFlochaIf->setIcon(QIcon(PHOTONICONPATH + "iconAddIf.png"));
    addNewFlochaIf->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));

    addNewFlochaSet = new QAction(this);
    addNewFlochaSet->setIcon(QIcon(PHOTONICONPATH + "iconAddSet.png"));
    addNewFlochaSet->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));

    addNewFlochaRun = new QAction(this);
    addNewFlochaRun->setIcon(QIcon(PHOTONICONPATH + "iconAddRun.png"));
    addNewFlochaRun->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));

    addNewFlochaSleep = new QAction(this);
    addNewFlochaSleep->setIcon(QIcon(PHOTONICONPATH + "iconAddSleep.png"));
    addNewFlochaSleep->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_4));

    addNewFlochaAssem = new QAction(this);
    addNewFlochaAssem->setIcon(QIcon(PHOTONICONPATH + "iconAddAssem.png"));
    addNewFlochaAssem->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_5));





    undoFlochaChange = new QAction(this);
    undoFlochaChange->setIcon(QIcon(PHOTONICONPATH + "iconUndo.png"));
    undoFlochaChange->setShortcut(QKeySequence::Undo);

    redoFlochaChange = new QAction(this);
    redoFlochaChange->setIcon(QIcon(PHOTONICONPATH + "iconRedo.png"));
    redoFlochaChange->setShortcut(QKeySequence::Redo);



    copyFlocha = new QAction(this);
    copyFlocha->setIcon(QIcon(PHOTONICONPATH + "iconCopy.png"));
    copyFlocha->setShortcut(QKeySequence::Copy);

    pasteFlocha = new QAction(this);
    pasteFlocha->setIcon(QIcon(PHOTONICONPATH + "iconPaste.png"));
    pasteFlocha->setShortcut(QKeySequence::Paste);

    removeFlocha = new QAction(this);
    removeFlocha->setIcon(QIcon(PHOTONICONPATH + "iconDelete.png"));
    removeFlocha->setShortcut(QKeySequence::Delete);

    ui_act_assemFlocha = new QAction(this);
    ui_act_assemFlocha->setIcon(QIcon(PHOTONICONPATH + "iconAssembleSelect.png"));
    ui_act_assemFlocha->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));


    act_startAutoRun = new QAction(this);
    act_startAutoRun->setIcon(QIcon(PHOTONICONPATH + "iconStart.png"));
    act_startAutoRun->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));

    act_startDebugRun = new QAction(this);
    act_startDebugRun->setIcon(QIcon(PHOTONICONPATH + "iconStartDebug.png"));
    act_startDebugRun->setShortcut(QKeySequence(Qt::Key_F5));

    act_runNextStep = new QAction(this);
    act_runNextStep->setIcon(QIcon(PHOTONICONPATH + "iconNext.png"));
    act_runNextStep->setShortcut(QKeySequence(Qt::Key_F10));



    act_setBreakPoint = new QAction(this);
    act_setBreakPoint->setIcon(QIcon(PHOTONICONPATH + "iconSetBreak.png"));
    act_setBreakPoint->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));

    act_resetBreakPoint = new QAction(this);
    act_resetBreakPoint->setIcon(QIcon(PHOTONICONPATH + "iconResetBreak.png"));

    act_pauseRun = new QAction(this);
    act_pauseRun->setIcon(QIcon(PHOTONICONPATH + "iconPause.png"));



    act_stopRun = new QAction(this);
    act_stopRun->setIcon(QIcon(PHOTONICONPATH + "iconStop.png"));
    act_stopRun->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F5));



    createNewFile = new QAction(this);
    createNewFile->setIcon(QIcon(PHOTONICONPATH + "iconNewFile.png"));
    connect(createNewFile, &QAction::triggered, this, &FlowchartMainWin::action_newFile_triggered);


    loadXmlToFlocha = new QAction(this);
    loadXmlToFlocha->setIcon(QIcon(PHOTONICONPATH + "iconOpen.png"));
    loadXmlToFlocha->setShortcut(QKeySequence::Open);
    connect(loadXmlToFlocha, &QAction::triggered, this, &FlowchartMainWin::action_openFile_triggered);


    saveFlochaToXml = new QAction(this);
    saveFlochaToXml->setIcon(QIcon(PHOTONICONPATH + "iconSave.png"));
    saveFlochaToXml->setShortcut(QKeySequence::Save);
    connect(saveFlochaToXml, &QAction::triggered, this, &FlowchartMainWin::action_saveFile_triggered);


    ui_act_saveAsFile = new QAction(this);
    ui_act_saveAsFile->setIcon(QIcon(PHOTONICONPATH + "iconSaveAs.png"));
    ui_act_saveAsFile->setShortcut(QKeySequence::SaveAs);
    connect(ui_act_saveAsFile, &QAction::triggered, this, &FlowchartMainWin::action_saveFileAs_triggered);



    ui_act_varDisplay = new QAction(this);
    connect(ui_act_varDisplay, &QAction::triggered, this, &FlowchartMainWin::act_varDisplay_triggered);

    ui_act_localVar = new QAction(this);
    connect(ui_act_localVar, &QAction::triggered, this, &FlowchartMainWin::act_localVar_triggered);

    ui_act_itemBox = new QAction(this);
    connect(ui_act_itemBox, &QAction::triggered, this, &FlowchartMainWin::act_itemBox_triggered);

    ui_act_timeMonitor = new QAction(this);
    connect(ui_act_timeMonitor, &QAction::triggered, this, &FlowchartMainWin::act_timeMonitor_triggered);

    ui_act_transToCode = new QAction(this);
    connect(ui_act_transToCode, &QAction::triggered, this, &FlowchartMainWin::act_codeTrans_triggered);
}



void FlowchartMainWin::createMenu()
{


    fileMenu = new QMenu(this);
    editMenu = new QMenu(this);
    runMenu = new QMenu(this);
    viewMenu = new QMenu(this);
    debugMenu = new QMenu(this);



    fileMenu->addAction(createNewFile);
    fileMenu->addAction(loadXmlToFlocha);
    fileMenu->addAction(saveFlochaToXml);
    fileMenu->addAction(ui_act_saveAsFile);




    editMenu->addAction(undoFlochaChange);
    editMenu->addAction(redoFlochaChange);

    editMenu->addSeparator();

    editMenu->addAction(addNewFlochaIf);
    editMenu->addAction(addNewFlochaSet);
    editMenu->addAction(addNewFlochaRun);
    editMenu->addAction(addNewFlochaSleep);
    editMenu->addAction(addNewFlochaAssem);



    editMenu->addAction(copyFlocha);
    editMenu->addAction(pasteFlocha);
    editMenu->addAction(removeFlocha);
    editMenu->addAction(ui_act_assemFlocha);

    runMenu->addAction(act_startAutoRun);
    runMenu->addAction(act_startDebugRun);
    runMenu->addAction(act_runNextStep);
    runMenu->addAction(act_setBreakPoint);
    runMenu->addAction(act_resetBreakPoint);
    runMenu->addAction(act_pauseRun);
    runMenu->addAction(act_stopRun);

    viewMenu->addAction(ui_act_varDisplay);
    viewMenu->addAction(ui_act_localVar);
    viewMenu->addAction(ui_act_itemBox);
    viewMenu->addAction(ui_act_timeMonitor);
    viewMenu->addAction(ui_act_transToCode);

    this->menuBar()->addMenu(fileMenu);
    this->menuBar()->addMenu(editMenu);
    this->menuBar()->addMenu(runMenu);
    this->menuBar()->addMenu(viewMenu);
    this->menuBar()->addMenu(debugMenu);


}



void FlowchartMainWin::createToolBar()
{
    toolBar = new QToolBar(this);


    toolBar->addAction(createNewFile);
    toolBar->addAction(loadXmlToFlocha);
    toolBar->addAction(saveFlochaToXml);
    toolBar->addAction(ui_act_saveAsFile);

    toolBar->addSeparator();

    toolBar->addAction(undoFlochaChange);
    toolBar->addAction(redoFlochaChange);

    toolBar->addSeparator();

    toolBar->addAction(addNewFlochaIf);
    toolBar->addAction(addNewFlochaSet);
    toolBar->addAction(addNewFlochaRun);
    toolBar->addAction(addNewFlochaSleep);
    toolBar->addAction(addNewFlochaAssem);


    toolBar->addAction(copyFlocha);
    toolBar->addAction(pasteFlocha);
    toolBar->addAction(removeFlocha);
    toolBar->addAction(ui_act_assemFlocha);

    toolBar->addSeparator();

    toolBar->addAction(act_startAutoRun);
    toolBar->addAction(act_startDebugRun);
    toolBar->addAction(act_runNextStep);
    toolBar->addAction(act_setBreakPoint);
    toolBar->addAction(act_resetBreakPoint);
    toolBar->addAction(act_pauseRun);
    toolBar->addAction(act_stopRun);


    addToolBar(toolBar);
}



void FlowchartMainWin::linkCurrTabWidget()
{
    //	if(runDebug)
    //		return;


    FlowchartWidget *currflochaW = dynamic_cast<FlowchartWidget *>(mainTabWidget->currentWidget());
    if(currflochaW == nullptr)
    {
        QMessageBox::warning(this, tr("error"),	tr("tabwidget curr widget is not flowchawidget"));
        return;
    }

    if(preFlowchartWidget)
    {

        if(!runDebug)
        {

            disconnect(addNewFlochaIf, &QAction::triggered, preFlowchartWidget, &FlowchartWidget::addItemIf);
            disconnect(addNewFlochaSet, &QAction::triggered, preFlowchartWidget, &FlowchartWidget::addItemSet);
            disconnect(addNewFlochaRun, &QAction::triggered, preFlowchartWidget, &FlowchartWidget::addItemRun);
            disconnect(addNewFlochaSleep, &QAction::triggered, preFlowchartWidget, &FlowchartWidget::addItemSleep);
            disconnect(addNewFlochaAssem,&QAction::triggered,preFlowchartWidget,&FlowchartWidget::addItemAssem);

            disconnect(undoFlochaChange,&QAction::triggered,preFlowchartWidget,&FlowchartWidget::undoFlochaChange);
            disconnect(redoFlochaChange,&QAction::triggered,preFlowchartWidget,&FlowchartWidget::redoFlochaChange);


            disconnect(copyFlocha,&QAction::triggered,preFlowchartWidget,&FlowchartWidget::copySelectFlochas);
            disconnect(pasteFlocha,&QAction::triggered,preFlowchartWidget,&FlowchartWidget::pasteCopyFlochas);
            disconnect(removeFlocha,&QAction::triggered,preFlowchartWidget,&FlowchartWidget::removeSelectFlochas);
            disconnect(ui_act_assemFlocha,&QAction::triggered,preFlowchartWidget,&FlowchartWidget::assembleSelectFlochas);



            disconnect(act_setBreakPoint,&QAction::triggered,preFlowchartWidget,&FlowchartWidget::setBreakPoint);
            disconnect(act_resetBreakPoint,&QAction::triggered,preFlowchartWidget,&FlowchartWidget::resetBreakPoint);




            disconnect(preFlowchartWidget,&FlowchartWidget::subFlochaAssemOpenSignal, this, &FlowchartMainWin::openSubFlochaAssem);
            disconnect(preFlowchartWidget,&FlowchartWidget::subFlochaAssemDeleteSignal, this, &FlowchartMainWin::closeSubFlochaAssem);
        }
        else
        {

            disconnect(act_setBreakPoint,&QAction::triggered,preFlowchartWidget,&FlowchartWidget::setBreakPoint);
            disconnect(act_resetBreakPoint,&QAction::triggered,preFlowchartWidget,&FlowchartWidget::setBreakPoint);
        }


    }


    if(!runDebug)
    {

        connect(addNewFlochaIf, &QAction::triggered, currflochaW, &FlowchartWidget::addItemIf);
        connect(addNewFlochaSet, &QAction::triggered, currflochaW, &FlowchartWidget::addItemSet);
        connect(addNewFlochaRun, &QAction::triggered, currflochaW, &FlowchartWidget::addItemRun);
        connect(addNewFlochaSleep, &QAction::triggered, currflochaW, &FlowchartWidget::addItemSleep);
        connect(addNewFlochaAssem, &QAction::triggered, currflochaW, &FlowchartWidget::addItemAssem);

        connect(undoFlochaChange, &QAction::triggered, currflochaW, &FlowchartWidget::undoFlochaChange);
        connect(redoFlochaChange, &QAction::triggered, currflochaW, &FlowchartWidget::redoFlochaChange);


        connect(copyFlocha,&QAction::triggered,currflochaW,&FlowchartWidget::copySelectFlochas);
        connect(pasteFlocha,&QAction::triggered,currflochaW,&FlowchartWidget::pasteCopyFlochas);
        connect(removeFlocha,&QAction::triggered,currflochaW,&FlowchartWidget::removeSelectFlochas);
        connect(ui_act_assemFlocha,&QAction::triggered,currflochaW,&FlowchartWidget::assembleSelectFlochas);


        connect(act_setBreakPoint,&QAction::triggered,currflochaW,&FlowchartWidget::setBreakPoint);
        connect(act_resetBreakPoint,&QAction::triggered,currflochaW,&FlowchartWidget::resetBreakPoint);





        connect(currflochaW,&FlowchartWidget::subFlochaAssemOpenSignal, this, &FlowchartMainWin::openSubFlochaAssem);
        
        //here has a bug
        connect(preFlowchartWidget,&FlowchartWidget::subFlochaAssemDeleteSignal, this, &FlowchartMainWin::closeSubFlochaAssem);
    }
    else
    {

        connect(act_setBreakPoint,&QAction::triggered,currflochaW,&FlowchartWidget::setBreakPoint);
        connect(act_resetBreakPoint,&QAction::triggered,currflochaW,&FlowchartWidget::resetBreakPoint);


    }

    preFlowchartWidget = currflochaW;

}


void FlowchartMainWin::updActionEn_Disable()
{

    if(flochaMainWinState.bits.fileOpened)
    {
        if(flochaMainWinState.bits.AutoRun)
        {
            act_startAutoRun->setEnabled(false);
            act_startDebugRun->setEnabled(false);
            act_pauseRun->setEnabled(false);
            act_stopRun->setEnabled(true);
        }
        else if(flochaMainWinState.bits.runDebug)
        {
            if(flochaMainWinState.bits.pauseRun)
            {
                act_startDebugRun->setEnabled(true);
                act_pauseRun->setEnabled(false);
                act_runNextStep->setEnabled(true);
            }
            else
            {
                act_startAutoRun->setEnabled(false);
                act_startDebugRun->setEnabled(false);
                act_pauseRun->setEnabled(true);
                act_stopRun->setEnabled(true);
                act_runNextStep->setEnabled(false);
            }
        }
        else if(!flochaMainWinState.bits.AutoRun && !flochaMainWinState.bits.runDebug)
        {
            act_startAutoRun->setEnabled(true);
            act_startDebugRun->setEnabled(true);
            act_pauseRun->setEnabled(false);
            act_stopRun->setEnabled(false);
            act_runNextStep->setEnabled(false);
        }

        act_setBreakPoint->setEnabled(true);
        act_resetBreakPoint->setEnabled(true);


        if(flochaMainWinState.bits.AutoRun || flochaMainWinState.bits.runDebug)
        {
            createNewFile->setEnabled(false);
            loadXmlToFlocha->setEnabled(false);

            addNewFlochaIf->setEnabled(false);
            addNewFlochaSet->setEnabled(false);
            addNewFlochaRun->setEnabled(false);
            addNewFlochaSleep->setEnabled(false);
            addNewFlochaAssem->setEnabled(false);

            undoFlochaChange->setEnabled(false);
            redoFlochaChange->setEnabled(false);

            copyFlocha->setEnabled(false);
            pasteFlocha->setEnabled(false);
            removeFlocha->setEnabled(false);
            ui_act_assemFlocha->setEnabled(false);
        }
        else
        {
            createNewFile->setEnabled(true);
            loadXmlToFlocha->setEnabled(true);

            addNewFlochaIf->setEnabled(true);
            addNewFlochaSet->setEnabled(true);
            addNewFlochaRun->setEnabled(true);
            addNewFlochaSleep->setEnabled(true);
            addNewFlochaAssem->setEnabled(true);

            undoFlochaChange->setEnabled(true);
            redoFlochaChange->setEnabled(true);

            copyFlocha->setEnabled(true);
            pasteFlocha->setEnabled(true);
            removeFlocha->setEnabled(true);
            ui_act_assemFlocha->setEnabled(true);
        }


        if(flochaMainWinState.bits.fileNotSave)
        {
            saveFlochaToXml->setEnabled(true);
        }
        else
        {
            saveFlochaToXml->setEnabled(true);//temp solution
        }

        ui_act_saveAsFile->setEnabled(true);


    }
    else
    {

        act_startAutoRun->setEnabled(false);
        act_startDebugRun->setEnabled(false);
        act_pauseRun->setEnabled(false);
        act_stopRun->setEnabled(false);
        act_runNextStep->setEnabled(false);
        act_setBreakPoint->setEnabled(false);
        act_resetBreakPoint->setEnabled(false);


        addNewFlochaIf->setEnabled(false);
        addNewFlochaSet->setEnabled(false);
        addNewFlochaRun->setEnabled(false);
        addNewFlochaSleep->setEnabled(false);
        addNewFlochaAssem->setEnabled(false);

        undoFlochaChange->setEnabled(false);
        redoFlochaChange->setEnabled(false);

        copyFlocha->setEnabled(false);
        pasteFlocha->setEnabled(false);
        removeFlocha->setEnabled(false);
        ui_act_assemFlocha->setEnabled(false);

        saveFlochaToXml->setEnabled(false);
        ui_act_saveAsFile->setEnabled(false);

    }

}

void FlowchartMainWin::openSubFlochaAssem(FlowchartsAssemble *subflocha)
{
    //	int test = mainTabWidget->currentIndex();

    for(int indx = 0; indx < mainTabWidget->count(); ++indx)
    {
        FlowchartWidget *tmpwid = dynamic_cast<FlowchartWidget*>(mainTabWidget->widget(indx));
        const FlowchartsAssemble *tmpflo = tmpwid->getMainFlochaAssem();
#ifdef _DEBUG
        if(tmpflo == nullptr)
            QMessageBox::warning(this,tr("error"),tr("tabwidget is not flochaassem"));
#endif

        if(subflocha == tmpflo)
        {
            mainTabWidget->setCurrentIndex(indx);
            return;
        }
    }


    FlowchartWidget *subwidget = new FlowchartWidget(this,subflocha);
    subwidget->setFlochaCmdManager(commandManager);
    connect(subwidget, &FlowchartWidget::flochaIsChanged, xmlFlochaTransfer, &XmlFlochaTransfer::updNodeAfterFloChange);
    connect(subwidget, &FlowchartWidget::flochaIsRemoved, xmlFlochaTransfer, &XmlFlochaTransfer::removeNode);

    //for the perpose that remove the mark of interupt when finish run. maybe better way is emit a signal when finish run
    connect(act_runNextStep,&QAction::triggered,subwidget,&FlowchartWidget::runNextStep);

    mainTabWidget->addTab(subwidget,QString("%1").arg(subflocha->idName));

    mainTabWidget->setCurrentWidget(subwidget);


}

void FlowchartMainWin::closeSubFlochaAssem(FlowchartsAssemble *subflocha)
{
    for(int indx = 0; indx < mainTabWidget->count(); ++indx)
    {
        FlowchartWidget *tmpwid = dynamic_cast<FlowchartWidget*>(mainTabWidget->widget(indx));
        const FlowchartsAssemble *tmpflo = tmpwid->getMainFlochaAssem();

        if(subflocha == tmpflo)
        {
            closeSubAssemTab(indx);
            return;
        }
    }

}

void FlowchartMainWin::closeSubAssemTab(int indx)
{
    QWidget *widtoclose = mainTabWidget->widget(indx);

    if(widtoclose == mainFlowchartWidget)
        return;


    mainTabWidget->removeTab(indx);
    delete widtoclose;
}


void FlowchartMainWin::dealHitBreakPoint(FlowchartsTemplate *flocha)
{
    flochaMainWinState.bits.pauseRun = 1;
    updActionEn_Disable();

    FlowchartsAssemble *parent = flocha->getParentPtr();
    openSubFlochaAssem(parent);

    FlowchartWidget *widget = dynamic_cast<FlowchartWidget *>(mainTabWidget->currentWidget());
    widget->setSuspendBtn(flocha);
}


void FlowchartMainWin::dealRunFinished(int result)
{
    for(int i = 0; i < mainTabWidget->count(); ++i)
    {
        FlowchartWidget *flowidget = dynamic_cast<FlowchartWidget*>(mainTabWidget->widget(i));
        if(flowidget)
        {
            try
            {
                flowidget->setSuspendBtn(nullptr);

            }
            catch(...)
            {

            }
        }

    }

    flochaMainWinState.bits.AutoRun = 0;
    flochaMainWinState.bits.runDebug = 0;
    flochaMainWinState.bits.pauseRun = 0;
    updActionEn_Disable();

}

void FlowchartMainWin::action_debugRun_triggered()
{
    if(!mainFlowchartWidget)
        return;

    flochaMainWinState.bits.runDebug = 1;
    flochaMainWinState.bits.pauseRun = 0;
    updActionEn_Disable();

    for(int i = 0; i < mainTabWidget->count(); ++i)
    {
        FlowchartWidget *flowidget = dynamic_cast<FlowchartWidget*>(mainTabWidget->widget(i));
        if(flowidget)
        {
            try
            {
                flowidget->setSuspendBtn(nullptr);
            }
            catch(...)
            {

            }
        }

    }
    mainFlowchartWidget->startDebugRun();
}

void FlowchartMainWin::action_autoRun_triggered()
{
    if(mainFlowchartWidget)
    {
        mainFlowchartWidget->startAutoRun();

        flochaMainWinState.bits.AutoRun = 1;
        updActionEn_Disable();
    }
}

void FlowchartMainWin::action_pauseRun_triggered()
{
    if(mainFlowchartWidget)
    {
        mainFlowchartWidget->pauseRun();

        flochaMainWinState.bits.pauseRun = 1;
        updActionEn_Disable();
    }
}

void FlowchartMainWin::action_stopRun_triggered()
{
    if(mainFlowchartWidget)
    {
        mainFlowchartWidget->stopRun();

        flochaMainWinState.bits.AutoRun = 0;
        flochaMainWinState.bits.runDebug = 0;
        flochaMainWinState.bits.pauseRun = 0;
        updActionEn_Disable();
    }
}

void FlowchartMainWin::action_openFile_triggered()
{

    if(flochaMainWinState.bits.fileOpened)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Tips"), tr("Save file before open?"),
            QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes);

        if(reply == QMessageBox::Yes)
            action_saveFile_triggered();
        else if(reply == QMessageBox::Cancel)
            return;

    }



    QString xmlfilename;

    if(!currFileName.isEmpty() && (flochaMainWinState.bits.loadDefaultFile == 1))
        xmlfilename = currFileName;
    else
        xmlfilename = QFileDialog::getOpenFileName(this, tr("Open XML File"), QDir::currentPath(),
        tr("XML Files (*.flxml *.xml)"));


    if(xmlfilename.isNull())
    {
        return;
    }
    //QFileInfo fInfo(filename);
    QFile file(xmlfilename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("error"),	tr("Cannot read file %1:\n%2.").arg(xmlfilename).arg(file.errorString()));
        currFileName.clear();
        return;
    }
    else
    {
        close_flochaWidget();

        if(commandManager == nullptr)
            commandManager = new CommandManager(parentCmd);


        if(varMonitorControl)
        {
            varMonitorControl->setCommandManager(commandManager);
            varMonitorThread->start();
        }


        currFileName = xmlfilename;
        flochaMainWinState.bits.loadDefaultFile = 0;

        if(displayUnitsPlane == nullptr)
            displayUnitsPlane = new DisplayUnitsPlane(this, commandManager);

        if(xmlFlochaTransfer == NULL)
            xmlFlochaTransfer = new XmlFlochaTransfer(this, commandManager, displayUnitsPlane);

        flochaPtr = xmlFlochaTransfer->loadXmlToFlocha(&file,this);

        if(!flochaPtr)
        {
            QMessageBox::warning(this, tr("error"),tr("Can not load xml to flocha"));
            close_flochaWidget();
            return;
        }


        if(displayUnitsDock == nullptr)
        {
            displayUnitsDock = new QDockWidget(this);
            setTextNeedTrans();
        }

        displayUnitsDock->setWidget(displayUnitsPlane);
        displayUnitsDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
        this->addDockWidget(Qt::RightDockWidgetArea, displayUnitsDock);



        if(pcVarEditWidget == nullptr)
            pcVarEditWidget = new PCVarEditWidget(this);

        std::map<unsigned int, std::pair<QString, ArrayDimensInfo> > pcvardata;
        pcvardata = xmlFlochaTransfer->getVarPoolToInitTable();
        for(auto it = pcvardata.begin(); it != pcvardata.end(); ++it)
        {
            pcVarEditWidget->addVarToTable(it->first, it->second.first, it->second.second);
        }
        if(pcVarEditDock == nullptr)
        {
            pcVarEditDock = new QDockWidget(this);
            setTextNeedTrans();
        }

        pcVarEditDock->setWidget(pcVarEditWidget);
        pcVarEditDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
        this->addDockWidget(Qt::RightDockWidgetArea, pcVarEditDock);

        connect(pcVarEditWidget, &PCVarEditWidget::changeVarSignal,
            xmlFlochaTransfer, &XmlFlochaTransfer::updDomChangedVar);

        connect(pcVarEditWidget, &PCVarEditWidget::removeVarSignal,
            xmlFlochaTransfer, &XmlFlochaTransfer::updDomRemovedVar);


        connect(mainTabWidget, &QTabWidget::currentChanged, this, &FlowchartMainWin::linkCurrTabWidget);

    }




    if(mainFlowchartWidget == NULL)
    {
        mainFlowchartWidget = new FlowchartWidget(this,flochaPtr);
        if(commandManager != nullptr)
            mainFlowchartWidget->setFlochaCmdManager(commandManager);

        mainTabWidget->addTab(mainFlowchartWidget,tr("main"));


        connect(mainFlowchartWidget,&FlowchartWidget::flochaIsChanged,
            xmlFlochaTransfer,&XmlFlochaTransfer::updNodeAfterFloChange);//why here QObject::connect: invalid null parameter
        connect(mainFlowchartWidget,&FlowchartWidget::flochaIsRemoved,
            xmlFlochaTransfer,&XmlFlochaTransfer::removeNode);



        connect(act_runNextStep,&QAction::triggered,mainFlowchartWidget,&FlowchartWidget::runNextStep);
        connect(mainFlowchartWidget,&FlowchartWidget::hitBreakPointSignal,this,&FlowchartMainWin::dealHitBreakPoint);
        connect(mainFlowchartWidget,&FlowchartWidget::runFinishedSignal,this,&FlowchartMainWin::dealRunFinished);
        //		connect(mainFlowchartWidget,&FlowchartWidget::subFlochaAssemOpenSignal,
        //			this, &FlowchartMainWin::openSubFlochaAssem);
        linkCurrTabWidget();
    }

    flochaMainWinState.bits.fileOpened = 1;
    updActionEn_Disable();




#ifdef _DEBUG
    act_codeTrans_triggered();
#endif

}


void FlowchartMainWin::action_saveFile_triggered()
{
    //if(currFileName.size() == 0)
    //	return;
    if(flochaMainWinState.bits.fileOpened == 0)
        return;

    QFile file(currFileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        xmlFlochaTransfer->saveFlochaToXml(&file);
    }
    else
    {
        action_saveFileAs_triggered();
    }
}



void FlowchartMainWin::action_saveFileAs_triggered()
{
    if(flochaMainWinState.bits.fileOpened == 0)
        return;

    QString xmlfilename;

    //	if(!filename.isEmpty())
    //		xmlfilename = filename;
    //	else
    xmlfilename = QFileDialog::getSaveFileName(this, tr("Open XML File"), QDir::currentPath(),
        tr("XML Files (*.flxml *.xml)"));

    //QFileInfo fInfo(xmlfilename);

    QFile file(xmlfilename);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        xmlFlochaTransfer->saveFlochaToXml(&file);
        currFileName = xmlfilename;
    }
}




void FlowchartMainWin::action_newFile_triggered()
{

    if(flochaMainWinState.bits.fileOpened)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Tips"), tr("Save file before open?"),
            QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes);

        if(reply == QMessageBox::Yes)
            action_saveFile_triggered();
        else if(reply == QMessageBox::Cancel)
            return;

    }
    close_flochaWidget();






    if(commandManager)
        delete commandManager;
    commandManager = new CommandManager(parentCmd);

    if(varMonitorControl)
    {
        varMonitorControl->setCommandManager(commandManager);
        varMonitorThread->start();
    }


    if(displayUnitsPlane)
        delete displayUnitsPlane;
    displayUnitsPlane = new DisplayUnitsPlane(this, commandManager);

    if(xmlFlochaTransfer)
        delete xmlFlochaTransfer;
    xmlFlochaTransfer = new XmlFlochaTransfer(this, commandManager, displayUnitsPlane);

    if(flochaPtr)
        delete flochaPtr;
    flochaPtr = xmlFlochaTransfer->createNewFlocha(this);
    if(displayUnitsDock == nullptr)
    {
        displayUnitsDock = new QDockWidget(this);
        setTextNeedTrans();
    }

    displayUnitsDock->setWidget(displayUnitsPlane);
    displayUnitsDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    this->addDockWidget(Qt::RightDockWidgetArea, displayUnitsDock);


    if(pcVarEditWidget)
        delete pcVarEditWidget;
    pcVarEditWidget = new PCVarEditWidget(this);


    if(pcVarEditDock == nullptr)
    {
        pcVarEditDock = new QDockWidget(this);
        setTextNeedTrans();
    }

    pcVarEditDock->setWidget(pcVarEditWidget);
    pcVarEditDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    this->addDockWidget(Qt::RightDockWidgetArea, pcVarEditDock);

    connect(pcVarEditWidget, &PCVarEditWidget::changeVarSignal,
        xmlFlochaTransfer, &XmlFlochaTransfer::updDomChangedVar);

    connect(pcVarEditWidget, &PCVarEditWidget::removeVarSignal,
        xmlFlochaTransfer, &XmlFlochaTransfer::updDomRemovedVar);




    disconnect(mainTabWidget, &QTabWidget::currentChanged, this, &FlowchartMainWin::linkCurrTabWidget);
    preFlowchartWidget = nullptr;

    if(mainFlowchartWidget)
        delete mainFlowchartWidget;
    mainFlowchartWidget = new FlowchartWidget(this,flochaPtr);
    if(commandManager != nullptr)
        mainFlowchartWidget->setFlochaCmdManager(commandManager);

    connect(mainTabWidget, &QTabWidget::currentChanged, this, &FlowchartMainWin::linkCurrTabWidget);
    mainTabWidget->addTab(mainFlowchartWidget,tr("main"));


    connect(mainFlowchartWidget,&FlowchartWidget::flochaIsChanged,
        xmlFlochaTransfer,&XmlFlochaTransfer::updNodeAfterFloChange);
    connect(mainFlowchartWidget,&FlowchartWidget::flochaIsRemoved,
        xmlFlochaTransfer,&XmlFlochaTransfer::removeNode);



    connect(act_runNextStep,&QAction::triggered,mainFlowchartWidget,&FlowchartWidget::runNextStep);
    connect(mainFlowchartWidget,&FlowchartWidget::hitBreakPointSignal,this,&FlowchartMainWin::dealHitBreakPoint);
    connect(mainFlowchartWidget,&FlowchartWidget::runFinishedSignal,this,&FlowchartMainWin::dealRunFinished);
    linkCurrTabWidget();
    //		connect(mainFlowchartWidget,&FlowchartWidget::subFlochaAssemOpenSignal,
    //			this, &FlowchartMainWin::openSubFlochaAssem);

    flochaMainWinState.bits.fileOpened = 1;
    updActionEn_Disable();
}


//void FlowchartMainWin::open_flochaWidget()
//{
//}

void FlowchartMainWin::close_flochaWidget()
{
    if(varMonitorControl)
    {
        varMonitorControl->stopRoutineThread();
        varMonitorThread->quit();
        varMonitorThread->wait();
    }

    currFileName.clear();

    if(commandManager)
        delete commandManager;
    commandManager = nullptr;


    if(displayUnitsPlane)
        delete displayUnitsPlane;
    displayUnitsPlane = nullptr;




    if(xmlFlochaTransfer)
        delete xmlFlochaTransfer;
    xmlFlochaTransfer = nullptr;


    if(flochaPtr)
        delete flochaPtr;
    flochaPtr = nullptr;


    if(displayUnitsDock)
        delete displayUnitsDock;
    displayUnitsDock = nullptr;



    if(pcVarEditWidget)
        delete pcVarEditWidget;
    pcVarEditWidget = nullptr;


    if(pcVarEditDock)
        delete pcVarEditDock;
    pcVarEditDock = nullptr;

    disconnect(mainTabWidget, &QTabWidget::currentChanged, this, &FlowchartMainWin::linkCurrTabWidget);
    preFlowchartWidget = nullptr;


    while(mainTabWidget->count()){
        QWidget *widtoclose = mainTabWidget->widget(0);
        mainTabWidget->removeTab(0);

        if(widtoclose == mainFlowchartWidget)
        {
            delete mainFlowchartWidget;
            mainFlowchartWidget = nullptr;
        }
        else
        {
            delete widtoclose;
        }
    }

    //if(mainFlowchartWidget)
    //    delete mainFlowchartWidget;
    //mainFlowchartWidget = nullptr;

    flochaMainWinState.bits.fileOpened = 0;
}

void FlowchartMainWin::act_varDisplay_triggered()
{
    if(displayUnitsDock->isHidden())
        displayUnitsDock->setHidden(false);
    else
        displayUnitsDock->setHidden(true);
}


void FlowchartMainWin::act_localVar_triggered()
{
    if(pcVarEditDock->isHidden())
        pcVarEditDock->setHidden(false);
    else
        pcVarEditDock->setHidden(true);

}



void FlowchartMainWin::act_itemBox_triggered()
{
    if(!itemBoxWidget)
        itemBoxWidget = new ItemBoxWidget(this);

    if(!itemBoxDock)
    {
        itemBoxDock = new QDockWidget(this);
        itemBoxDock->setWidget(itemBoxWidget);
        itemBoxDock->setFloating(true);
        setTextNeedTrans();
    }
    if(itemBoxDock->isHidden())
    {
        itemBoxDock->setHidden(false);
    }
    else
        itemBoxDock->setHidden(true);




}


void FlowchartMainWin::act_timeMonitor_triggered()
{
    if(!varMonitorThread)
        varMonitorThread = new QThread();
    if(!varMonitorControl)
    {
        varMonitorControl = new VarMonitorControl(commandManager);
        varMonitorControl->moveToThread(varMonitorThread);

        connect(varMonitorThread, &QThread::started, varMonitorControl, &VarMonitorControl::startRoutineThread);

        varMonitorThread->start();
    }
    varMonitorControl->showMonitor();
}


void FlowchartMainWin::act_codeTrans_triggered()
{
    if(flochaPtr == nullptr)
        return;


    if(!codeTransWidget)
        codeTransWidget = new FlochaToCodeWidget(this);

    if(!codeTransDock)
    {
        codeTransDock = new QDockWidget(this);
        codeTransDock->setWidget(codeTransWidget);
        codeTransDock->setFloating(true);
        setTextNeedTrans();
    }
    if(codeTransDock->isHidden())
    {
        codeTransDock->setHidden(false);
    }
    else
        codeTransDock->setHidden(true);



    codeTransWidget->setFlochaAssem(flochaPtr);

}