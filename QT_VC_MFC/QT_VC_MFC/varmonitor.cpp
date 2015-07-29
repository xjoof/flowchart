#include "varmonitor.h"

#include "vardisplaymanage.h"
#include "globalobjmanager.h"
extern GlobalObjManager *globalObjManager;



TimeLinePlotUnit::TimeLinePlotUnit(QWidget *parent)
    :QWidget(parent)
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    tickCountFrequency = (double)MAXTIMELINEDATARATE / (double)frequency.QuadPart;


    dataAxisMin = -100;
    dataAxisMax = 100;

    unitActivateFlag = ActivateFlag_stop;
    //dataFrequency = 20;
    totalDataTime = 10000;

    timerToUpdatePlot = nullptr;
    dataVector = nullptr;

    displayMode = DisplayMode_normal;
    axisScaleMode = ScaleMode_autoScale;
    autoScaleFlag = true;

    curveColor = QColor(80, 80, 255, 255);
    archiveColor = QColor(0, 0, 255, 150);
    gridLineColor = QColor(50, 50, 255, 120);

    labelOffset = 0;
    labelWidth = MONITORLABELWIDTH;
    //	this->setFrameShape(QFrame::Box);
    //	this->setFrameShadow(QFrame::Sunken);
    //	this->setLineWidth(3);
    //	this->setMidLineWidth(0);
}


TimeLinePlotUnit::~TimeLinePlotUnit(void)
{




}


void TimeLinePlotUnit::appendData(const double &data)
{
    if(unitActivateFlag != ActivateFlag_runing)
        return;

    if(!dataVector)
        return;

    if(!tickCountLast.QuadPart)
    {
        QueryPerformanceCounter(&tickCountLast);
        dataVector[0] = data;
        dataMax = data + 1;
        dataMin = data - 1;
        dataMaxBuff = dataMax;
        dataMinBuff = dataMin;
    }

    QueryPerformanceCounter(&tickCountTmp);

    bool cyclicagain = false; 

    int stepcnt = (int)(tickCountFrequency * (double)(tickCountTmp.QuadPart - tickCountLast.QuadPart));
    int cnt = 0;
    double lastdata = *data_end_p;

    if(stepcnt <= 0)
        return;//still not very accuracy
    //stepcnt = 1;

    double datastep = (data - lastdata) / stepcnt;

#ifdef _DEBUG
    //if(MAXTIMELINEDATARATE % dataFrequency)
    //	QMessageBox::warning(this, "error", "MAXTIMELINEDATARATE % dataFrequency is not 0");
#endif


    for(cnt = 0; cnt < stepcnt - 1; ++cnt)
    {
        if(data_end_p == &dataVector[dataNum - 1])
        {
            data_end_p = dataVector;
            cyclicagain = true;
        }
        else
            ++data_end_p;
        lastdata += datastep;
        *data_end_p = lastdata;
    }
    if(data_end_p == &dataVector[dataNum - 1])
    {
        data_end_p = dataVector;
        cyclicagain = true;
    }
    else
    {
        ++data_end_p;
    }
    *data_end_p = data;


    if(cyclicagain)
    {
        dataMax = dataMaxBuff;
        dataMin = dataMinBuff;

        dataMaxBuff = data + 1;
        dataMinBuff = data - 1;

    }
    else
    {
        if(data > dataMaxBuff)
            dataMaxBuff = data;
        if(data < dataMinBuff)
            dataMinBuff = data;

        if(data > dataMax)
            dataMax = data;
        if(data < dataMin)
            dataMin = data;
    }

    tickCountLast = tickCountTmp;
}


void TimeLinePlotUnit::startTimeLinePlot()
{

    if(unitActivateFlag == ActivateFlag_pause)
    {
        stopTimeLinePlot();
    }

    if(dataVector)
        return;


    if(!timerToUpdatePlot)
        timerToUpdatePlot = new QTimer(this);
    connect(timerToUpdatePlot, &QTimer::timeout, this, &TimeLinePlotUnit::dealUpdateTimer);

    dataNum = (MAXTIMELINEDATARATE / 1000) * totalDataTime;

    if(dataNum > MAXTIMELINEDATANUM)
        dataNum = MAXTIMELINEDATANUM;

    dataVector = new double[dataNum];


    if(!dataVector)
    {
        QMessageBox::warning(this, tr("error"), tr("can not allocate"));
        return;
    }

    memset(dataVector, 0, dataNum * sizeof(double));

    tickCountLast.QuadPart = 0;

    data_start_p = &dataVector[0];
    data_end_p = &dataVector[0];


    timerToUpdatePlot->start(50);
    unitActivateFlag = ActivateFlag_runing;
}


void TimeLinePlotUnit::stopTimeLinePlot()
{
    if(unitActivateFlag == ActivateFlag_stop)
        return;

    unitActivateFlag = ActivateFlag_stop;

    if(!dataVector)
        return;

    timerToUpdatePlot->stop();
    disconnect(timerToUpdatePlot, &QTimer::timeout, this, &TimeLinePlotUnit::dealUpdateTimer);



    delete[] dataVector;
    dataVector = nullptr;




}

void TimeLinePlotUnit::pauseTimeLinePlot()
{
    unitActivateFlag = ActivateFlag_pause;
}

void TimeLinePlotUnit::changeTotalTime(unsigned int msec)
{
    int tmpflag = unitActivateFlag;
    if(tmpflag == ActivateFlag_runing)//fix bug: change var monitor time will start monitor
        stopTimeLinePlot();

    totalDataTime = msec;

    if(tmpflag == ActivateFlag_runing)
        startTimeLinePlot();
}



void TimeLinePlotUnit::setScaleMode(int mode)
{
    axisScaleMode = mode;
    autoScaleFlag = true;
}

void TimeLinePlotUnit::setDisplayMode(int mode)
{
    displayMode = mode;
}


void TimeLinePlotUnit::setColor(const QColor &color)
{
    curveColor = color;
    archiveColor = QColor(color.red(), color.green(), color.blue(), 150);
    gridLineColor = QColor(color.red(), color.green(), color.blue(), 120);
}


void TimeLinePlotUnit::paintEvent(QPaintEvent *event)
{
    //QFrame::paintEvent(event);


    QPainter painter(this);

    QPoint point;
    QPoint lastpoint;

    int labelwidth = labelWidth;
    int width = this->width();
    int height = this->height() - TOPBOTTONSPACEPIX * 2;
    if((height <= 10) || (width < 10))//fix bug: var monitor too small will cause infinity loop
        return;
    double dataidxstep;
    double dataidxoffset = 0;

    double axisstep;

    if(unitActivateFlag == ActivateFlag_stop)
        return;

    if(!dataVector)
        return;

    dataidxstep = ((double)totalDataTime * (double)MAXTIMELINEDATARATE * 0.001) / (double)(width - labelwidth);


    if(autoScaleFlag)
    {
        double tmpvar;

        double tmpdataaxismax = dataMax;
        double tmpdataaxismin = dataMin;

        if(axisScaleMode == ScaleMode_fixToCurr)
        {
            autoScaleFlag = false;
        }
        else if(axisScaleMode == ScaleMode_expandOnly)
        {
            if(dataAxisMax > dataMax)
                tmpdataaxismax = dataAxisMax; 
            if(dataMin < dataAxisMin)
                tmpdataaxismin = dataMin;
        }

        dataAxisMax = tmpdataaxismax;
        dataAxisMin = tmpdataaxismin;

        if((tmpdataaxismax > 0) && (tmpdataaxismin < 0))
        {
            axisstep = abs(tmpdataaxismin);
            tmpvar = (tmpdataaxismax > axisstep) ? tmpdataaxismax : axisstep;

            axisstep = tmpvar * 0.2;

            for(int i = 1; i < 1000000; i *= 10)
            {
                if(axisstep < (double)i)
                {
                    double tmp = i * 0.1;//may be can simplification
                    axisstep /= tmp;
                    axisstep = ceil(axisstep);
                    axisstep *= tmp;
                    break;
                }

            }
            //-----------------axisstep calculate OK----------------
            dataAxisMax = axisstep;
            dataAxisMin = axisstep * (-1);

            while(dataAxisMax < tmpdataaxismax)
            {
                dataAxisMax += axisstep;
            }

            while(dataAxisMin > tmpdataaxismin)
            {
                dataAxisMin -= axisstep;
            }
        }
        else
        {


            tmpvar = abs(tmpdataaxismax - tmpdataaxismin);

            axisstep = tmpvar * 0.1;

            for(int i = 1; i < 1000000; i *= 10)
            {
                if(axisstep < (double)i)
                {
                    double tmp = i * 0.1;
                    axisstep /= tmp;
                    axisstep = floor(axisstep);
                    axisstep *= tmp;
                    break;
                }

            }
            //-----------------axisstep calculate OK----------------

            //if(dataMin > 0)
            dataAxisMin = floor(tmpdataaxismin / axisstep) * axisstep;

            dataAxisMax = dataAxisMin;

            while(dataAxisMax < tmpdataaxismax)
            {
                dataAxisMax += axisstep;
            }


        }



        axisStep = axisstep;
    }
    else
    {
        axisstep = axisStep;
    }



    double axisrange = (dataAxisMax - dataAxisMin) / (double)height;

    painter.save();
    painter.setPen(QPen(Qt::black,1));



    //---------------draw axis-------------------


    painter.drawLine(labelwidth, TOPBOTTONSPACEPIX, labelwidth, height + TOPBOTTONSPACEPIX);


    if((dataAxisMax > 0) && (dataAxisMin < 0))
    {
        double tmpvar;
        double tmpaxisstep;
        double labelstep;
        double label;
        double label0 = dataAxisMax / axisrange + TOPBOTTONSPACEPIX;
        painter.setPen(QPen(gridLineColor, 1));
        painter.drawLine(labelwidth, label0, width, label0);
        painter.setPen(QPen(curveColor, 1));
        painter.drawText(labelOffset, label0 + TEXTSHIFTPIXEL, QString("0"));


        if(height > 160)
        {
            tmpaxisstep = axisstep;
        }
        else if(height > 60)
        {
            tmpaxisstep = axisstep * 2.5;
        }
        else
        {
            tmpaxisstep = axisstep * 5;
        }

        labelstep = tmpaxisstep / axisrange;
        label = label0 - labelstep;
        tmpvar = tmpaxisstep;
        do
        {
            painter.setPen(QPen(gridLineColor, 1));
            painter.drawLine(labelwidth, label, width, label);
            painter.setPen(QPen(curveColor, 1));
            painter.drawText(labelOffset, label + TEXTSHIFTPIXEL, QString("%1").arg(tmpvar));

            tmpvar += tmpaxisstep;
            label -= labelstep;

        }while(label > 5);



        label = label0 + labelstep;
        tmpvar = tmpaxisstep * (-1);
        do
        {
            painter.setPen(QPen(gridLineColor, 1));
            painter.drawLine(labelwidth, label, width, label);
            painter.setPen(QPen(curveColor, 1));
            painter.drawText(labelOffset, label+TEXTSHIFTPIXEL, QString("%1").arg(tmpvar));

            tmpvar -= tmpaxisstep;
            label += labelstep;

        }while(label < height + 15);

    }
    else
    {

        double tmpvar;
        double tmpaxisstep;
        double labelstep;
        double label = height + TOPBOTTONSPACEPIX;

        if(height > 160)
        {
            tmpaxisstep = axisstep;
        }
        else if(height > 60)
        {
            tmpaxisstep = axisstep * 2.5;
        }
        else
        {
            tmpaxisstep = axisstep * 5;
        }

        labelstep = tmpaxisstep / axisrange;
        tmpvar = dataAxisMin;

        do
        {
            painter.setPen(QPen(gridLineColor, 1));
            painter.drawLine(labelwidth, label, width, label);
            painter.setPen(QPen(curveColor, 1));
            painter.drawText(labelOffset, label + TEXTSHIFTPIXEL, QString("%1").arg(tmpvar));

            tmpvar += tmpaxisstep;
            label -= labelstep;

        }while(label > 5);


    }

    //---------------draw axis-------------------


    if(displayMode == DisplayMode_normal)
    {
        painter.setPen(QPen(curveColor, 1));
        for(int i = labelwidth; i < width; ++i)
        {
            double *tmpdatap = data_start_p + (unsigned int)dataidxoffset;


            if((unsigned int)tmpdatap > (unsigned int)&dataVector[dataNum-1])
                break;


            int heightoffset = (*tmpdatap - dataAxisMin) / axisrange;


            point.rx() = i;
            point.ry() = height - heightoffset + TOPBOTTONSPACEPIX;


            // modify behavior: var monitor, no line link between active curve and archive curve
            if(((unsigned int)tmpdatap > (unsigned int)data_end_p) 
                && ((unsigned int)tmpdatap < (unsigned int)data_end_p + (unsigned int)dataidxstep*10))//why test result is 10?? 
            {
                painter.setPen(QPen(archiveColor, 1));
                lastpoint = point;
            }

            if(i == labelwidth)
                lastpoint = point;
            painter.drawLine(lastpoint,point);

            lastpoint = point;
            dataidxoffset += dataidxstep;
        }
    }	
    else if(displayMode == DisplayMode_paper)
    {
        painter.setPen(QPen(curveColor, 1));
        for(int i = width; i > labelwidth; --i)
        {
            double *tmpdatap = data_end_p - (unsigned int)dataidxoffset;

            //if((unsigned int)data_start_p < (int)dataidxoffset)

            if((unsigned int)tmpdatap < (unsigned int)data_start_p)
                tmpdatap = (double*)((unsigned int)&dataVector[dataNum - 1] - ((unsigned int)data_start_p - (unsigned int)tmpdatap));

            int heightoffset = (*tmpdatap - dataAxisMin) / axisrange;


            point.rx() = i;
            point.ry() = height - heightoffset + TOPBOTTONSPACEPIX;

            if(i == width)
                lastpoint = point;
            painter.drawLine(lastpoint,point);

            lastpoint = point;
            dataidxoffset += dataidxstep;

        }
    }

    painter.restore();


}

void TimeLinePlotUnit::dealUpdateTimer()
{
    update();
}

//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------

VarMonitorTimeLine::VarMonitorTimeLine(QWidget *parent, CommandManager *cmd)
    : QWidget(parent),commandManager(cmd)
{
    currUnitNum = 0;
    targetUnitNum = 2;


    canUpdateFlag = false;
    varNameEditing = false;

    updLabelTimer = new QTimer(this);

    mainLayout = new QGridLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    this->setLayout(mainLayout);

    plotUnitLayout = new QVBoxLayout(this);
    mainLayout->addLayout(plotUnitLayout, 0, 0);
    plotUnitLayout->setMargin(0);
    plotUnitLayout->setSpacing(0);



    //-------------------create toolbar-----------------

    ui_act_monitorOnOff = new QAction(this);
    ui_act_monitorOnOff->setText(tr("Switch on/off"));
    ui_act_monitorOnOff->setIcon(QIcon(PHOTONICONPATH + "iconOnOff.png"));
    ui_act_monitorOnOff->setCheckable(true);

    ui_act_paperMode = new QAction(this);
    ui_act_paperMode->setText(tr("Display mode"));
    ui_act_paperMode->setIcon(QIcon(PHOTONICONPATH + "iconPaperMode.png"));
    ui_act_paperMode->setCheckable(true);

    ui_act_autoScale = new QAction(this);
    ui_act_autoScale->setIcon(QIcon(PHOTONICONPATH + "iconAutoScale.png"));
    ui_act_autoScale->setCheckable(false);
    ui_act_autoScale->setText(tr("Auto scale"));

    ui_act_fixCurrScale = new QAction(this);
    ui_act_fixCurrScale->setIcon(QIcon(PHOTONICONPATH + "iconFixScale.png"));
    ui_act_fixCurrScale->setCheckable(false);
    ui_act_fixCurrScale->setText(tr("Fix scale to current data"));

    ui_act_expandOnlyScale = new QAction(this);
    ui_act_expandOnlyScale->setIcon(QIcon(PHOTONICONPATH + "iconExpandScale.png"));
    ui_act_expandOnlyScale->setCheckable(false);
    ui_act_expandOnlyScale->setText(tr("Scale expand only"));

    ui_menu_scaleMode = new QMenu(this);
    ui_menu_scaleMode->addAction(ui_act_autoScale);
    ui_menu_scaleMode->addAction(ui_act_fixCurrScale);
    ui_menu_scaleMode->addAction(ui_act_expandOnlyScale);

    ui_toolBtn_scaleMode = new QToolButton(this);
    ui_toolBtn_scaleMode->setText(tr("Select scale mode"));
    //ui_toolBtn_scaleMode->setIcon(QIcon(PHOTONICONPATH + "iconAutoScale.png"));
    ui_toolBtn_scaleMode->setMenu(ui_menu_scaleMode);
    ui_toolBtn_scaleMode->setPopupMode(QToolButton::InstantPopup);


    ui_actGru_channelNum = new QActionGroup(this);
    ui_menu_channelNum = new QMenu(this);
    ui_toolBtn_channelNum = new QToolButton(this);
    ui_toolBtn_channelNum->setText(tr("Change monitor number"));
    for(int i = 0; i < MAXTIMELINEPLOTNUM; ++i)
    {
        ui_act_channelNum[i] = new QAction(this);
        ui_act_channelNum[i]->setText(tr("%1 channel").arg(i + 1));
        ui_act_channelNum[i]->setCheckable(true);
        ui_actGru_channelNum->addAction(ui_act_channelNum[i]);
        ui_menu_channelNum->addAction(ui_act_channelNum[i]);
    }
    ui_toolBtn_channelNum->setMenu(ui_menu_channelNum);
    ui_act_channelNum[targetUnitNum-1]->setChecked(true);
    ui_toolBtn_channelNum->setPopupMode(QToolButton::InstantPopup);
    connect(ui_toolBtn_channelNum, &QToolButton::triggered, this, &VarMonitorTimeLine::act_changeChannelNum);


    ui_act_editDataTime = new QAction(this);
    ui_spin_dataTime = new QDoubleSpinBox();
    ui_spin_dataTime->setPrefix(tr("Total time  "));
    ui_spin_dataTime->setSuffix(tr("  sec"));
    ui_spin_dataTime->setRange(0.1,500);
    ui_spin_dataTime->setSingleStep(0.1);
    ui_spin_dataTime->setDecimals(1);
    ui_spin_dataTime->adjustSize();//because if didn't do this, ui_spin_dataTime won't show at first click ui_act_editDataTime, because act_dataTimeStartEdit() use the size of ui_spin_dataTime to calculate pupup pos
    ui_spin_dataTime->setWindowFlags(Qt::SplashScreen);
    void (QDoubleSpinBox::*spinBoxValChange)(double) = &QDoubleSpinBox::valueChanged;
    connect(ui_spin_dataTime, spinBoxValChange, this, &VarMonitorTimeLine::act_changeDataTime);
    connect(ui_spin_dataTime, &QDoubleSpinBox::editingFinished, this, &VarMonitorTimeLine::act_dataTimeChanged);
    connect(ui_act_editDataTime, &QAction::triggered, this, &VarMonitorTimeLine::act_dataTimeStartEdit);
    ui_spin_dataTime->setValue(DEFAULTTOTALDATATIME);


    toolBar = new QToolBar(this);

    toolBar->addAction(ui_act_monitorOnOff);
    toolBar->addAction(ui_act_paperMode);
    toolBar->addWidget(ui_toolBtn_scaleMode);
    toolBar->addWidget(ui_toolBtn_channelNum);
    toolBar->addAction(ui_act_editDataTime);
    toolBar->setOrientation(Qt::Vertical);
    toolBar->setIconSize(QSize(MONITORICONSIZE, MONITORICONSIZE));

    mainLayout->addWidget(toolBar,0,1);
    connect(ui_act_monitorOnOff, &QAction::toggled, this, &VarMonitorTimeLine::act_switchMoitorOnOff);
    connect(ui_act_paperMode, &QAction::toggled, this, &VarMonitorTimeLine::act_switchDisplayMode);
    connect(ui_toolBtn_scaleMode, &QToolButton::triggered, this, &VarMonitorTimeLine::act_switchScaleMode);


    ui_toolBtn_scaleMode->setIcon(QIcon(PHOTONICONPATH + "iconAutoScale.png"));//set toolbtn before/after add to toolbar will cause different icon size

    //-------------------create toolbar-----------------

    loadPlotUnitSetting();


    connect(updLabelTimer, &QTimer::timeout, this, &VarMonitorTimeLine::updValueLabel);

    updLabelTimer->start(200);

    this->resize(600,400);

    adjustPlotUnitNum();

}


VarMonitorTimeLine::~VarMonitorTimeLine(void)
{
    while(plotUnitStructList.size() > 0)
    {
        TimeLineUnitStruct unitstruct = plotUnitStructList.back();

        plotUnitStructList.pop_back();

        plotUnitLayout->removeWidget(unitstruct.plotUnit);
        plotUnitLayout->removeWidget(unitstruct.varNameEdit);
        plotUnitLayout->removeWidget(unitstruct.varValuelabel);
        plotUnitLayout->removeItem(unitstruct.layout);

        delete unitstruct.plotUnit;
        delete unitstruct.varNameEdit;
        delete unitstruct.varValuelabel;
        delete unitstruct.layout;

    }

    if(ui_spin_dataTime)
        delete ui_spin_dataTime;



}

bool VarMonitorTimeLine::canUpdate()
{
    return canUpdateFlag;
}


void VarMonitorTimeLine::updateMonitor()
{
    if(!commandManager)
        return;
    for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
    {
        double value;

        if(!canUpdateFlag)//for thread safety
            break;

#ifdef _DEBUG
        //qDebug() << "start get var in VarMonitorTimeLine::updateMonitor()";
#endif
        int result = commandManager->getVar((*it).varName, &value);


#ifdef _DEBUG
        //qDebug() << "finish get var in VarMonitorTimeLine::updateMonitor()";
#endif

        if(!result)
        {
            (*it).plotUnit->appendData(value);
            (*it).successGetVar = 1;
            (*it).value = value;
        }
        else
        {
            (*it).successGetVar = 0;
        }
    }



}

void VarMonitorTimeLine::setCommandManager(CommandManager *cmd)
{
    commandManager = cmd;
}


void VarMonitorTimeLine::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QBrush brush(Qt::black);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);

    for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
    {
        TimeLinePlotUnit *plotunit = (*it).plotUnit;
        QRect rect = QRect(plotunit->geometry().topLeft(), plotunit->geometry().bottomRight());
        rect = QRect(plotunit->parentWidget()->mapToParent(rect.topLeft()), plotunit->parentWidget()->mapToParent(rect.bottomRight()));
        painter.drawRect(rect);
    }



}


void VarMonitorTimeLine::loadPlotUnitSetting()
{
    QColor defaucolor[MAXTIMELINEPLOTNUM] = {QColor(255,180,180), QColor(180,255,180), QColor(180,180,255),
        QColor(240,240,180), QColor(180,240,240), QColor(240,180,240),
        QColor(230,230,230), QColor(255,230,210)};

    for(int i = 0; i < MAXTIMELINEPLOTNUM; ++i)
    {
        QColorDialog::setCustomColor(i*2, defaucolor[i]);
        plotUnitSetting[i].varName = "";
        plotUnitSetting[i].curveColor = defaucolor[i];
    }

}


void VarMonitorTimeLine::adjustPlotUnitNum()
{
    canUpdateFlag = false;
    unsigned int msec = (unsigned int)(ui_spin_dataTime->value() * 1000.0);


    QPixmap btnicon(MONITORICONSIZE,MONITORICONSIZE);
    btnicon.fill(Qt::transparent);
    QPainter iconpaint(&btnicon);
    iconpaint.drawPixmap(QRect(0,0,MONITORICONSIZE,MONITORICONSIZE),QPixmap(PHOTONICONPATH + "iconMonitorNum.png"));
    iconpaint.setFont(QFont("arial",MONITORICONSIZE*0.6,99));
    iconpaint.setPen(QPen(Qt::white));
    iconpaint.drawText(QRect(0,0,MONITORICONSIZE,MONITORICONSIZE),Qt::AlignCenter,QString("%1").arg(targetUnitNum));
    iconpaint.setFont(QFont("arial",MONITORICONSIZE*0.6,0));
    iconpaint.setPen(QPen(Qt::black));
    iconpaint.drawText(QRect(0,0,MONITORICONSIZE,MONITORICONSIZE),Qt::AlignCenter,QString("%1").arg(targetUnitNum));
    ui_toolBtn_channelNum->setIcon(QIcon(btnicon));



    for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
    {
        (*it).plotUnit->stopTimeLinePlot();
    }



    while(currUnitNum > targetUnitNum)
    {
        TimeLineUnitStruct unitstruct = plotUnitStructList.back();

        plotUnitStructList.pop_back();

        plotUnitLayout->removeWidget(unitstruct.curveColorBtn);
        plotUnitLayout->removeWidget(unitstruct.plotUnit);
        plotUnitLayout->removeWidget(unitstruct.varNameEdit);
        plotUnitLayout->removeWidget(unitstruct.varValuelabel);
        plotUnitLayout->removeItem(unitstruct.layout);

        delete unitstruct.curveColorBtn;
        delete unitstruct.plotUnit;
        delete unitstruct.varNameEdit;
        delete unitstruct.varValuelabel;
        delete unitstruct.layout;

        for(auto it = unitstruct.otherObjects.begin(); it != unitstruct.otherObjects.end(); ++it)
        {
            delete (*it);
        }
        unitstruct.otherObjects.clear();

        --currUnitNum;
    }

    while(currUnitNum < targetUnitNum)
    {
        TimeLineUnitStruct unitstruct;


        TimeLinePlotUnit *plotunit = new TimeLinePlotUnit(this);
        plotunit->resize(300,100);
        plotunit->changeTotalTime(msec);
        plotunit->setScaleMode(axisScaleMode);
        plotunit->setColor(plotUnitSetting[currUnitNum].curveColor);

        QFrame *frame = new QFrame(this);

        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Sunken);
        frame->setLineWidth(1);
        frame->setMidLineWidth(0);

        QVBoxLayout *tmplayout = new QVBoxLayout(this);
        tmplayout->addWidget(plotunit);
        tmplayout->setMargin(2);
        tmplayout->setSpacing(0);
        frame->setLayout(tmplayout);


        QLineEdit *varnameedit = new QLineEdit(this);
        varnameedit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        varnameedit->resize(varnameedit->height(), 50);
        varnameedit->setText(plotUnitSetting[currUnitNum].varName);

        QPushButton *colorbtn = new QPushButton(this);
        colorbtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        colorbtn->setFixedWidth(MONITORCOLORICONSIZE);
        colorbtn->setFixedHeight(MONITORCOLORICONSIZE);
        setBtnColorIcon(colorbtn, plotUnitSetting[currUnitNum].curveColor);


        QLabel *varvaluelabel = new QLabel(this);
        varvaluelabel->resize(varvaluelabel->height(), 50);
        varvaluelabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        varvaluelabel->setFrameShape(QFrame::Panel);
        varvaluelabel->setFrameShadow(QFrame::Sunken);
        varvaluelabel->setLineWidth(0);
        varvaluelabel->setMidLineWidth(0);



        QGridLayout *layout = new QGridLayout(this);
        layout->setMargin(0);
        layout->setSpacing(0);
        layout->addWidget(frame, 0, 0, 1, 5);
        layout->addWidget(colorbtn, 1, 0);
        layout->addWidget(varnameedit, 1, 1);
        layout->addWidget(varvaluelabel, 1, 2);

        plotUnitLayout->addLayout(layout);

        connect(varnameedit, &QLineEdit::textEdited, this, &VarMonitorTimeLine::varNameEditChange);
        connect(varnameedit, &QLineEdit::returnPressed, this, &VarMonitorTimeLine::varNameEditFinish);

        connect(colorbtn, &QPushButton::clicked, this, &VarMonitorTimeLine::curveColorStartEdit);

        unitstruct.successGetVar = 0;
        unitstruct.plotUnit = plotunit;
        unitstruct.curveColorBtn = colorbtn;
        unitstruct.varNameEdit = varnameedit;
        unitstruct.varValuelabel = varvaluelabel;
        unitstruct.layout = layout;
        unitstruct.varName = plotUnitSetting[currUnitNum].varName;

        //should add before frame, maybe because tmplayout will delete when delete frame
        unitstruct.otherObjects.push_back(tmplayout);
        unitstruct.otherObjects.push_back(frame);


        plotUnitStructList.push_back(unitstruct);
        ++currUnitNum;
    }


    if(ui_act_monitorOnOff->isChecked())
    {
        for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
        {
            (*it).plotUnit->startTimeLinePlot();
        }
        canUpdateFlag = true;
    }
}


void VarMonitorTimeLine::varNameEditChange()
{
    QLineEdit *sender = dynamic_cast<QLineEdit*>(QObject::sender());

    if(sender)
    {
        sender->setStyleSheet("QLineEdit{background: yellow;}");
        varNameEditing = true;
    }

}



void VarMonitorTimeLine::varNameEditFinish()
{

    QLineEdit *sender = dynamic_cast<QLineEdit*>(QObject::sender());

    if(sender)
    {
        varNameEditing = false;
        sender->setStyleSheet("QLineEdit{background: white;}");

        for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
        {
            (*it).plotUnit->stopTimeLinePlot();
            if((*it).varNameEdit == sender)
            {
                int indx = std::distance(plotUnitStructList.begin(), it);
                plotUnitSetting[indx].varName = sender->text();
                (*it).varName = sender->text();
            }
        }
        if(ui_act_monitorOnOff->isChecked())
        {
            for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
            {
                (*it).plotUnit->startTimeLinePlot();
            }
        }
    }




}


void VarMonitorTimeLine::curveColorStartEdit()
{
    QPushButton *sender = dynamic_cast<QPushButton *>(QObject::sender());

    if(sender)
    {

        for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
        {
            if((*it).curveColorBtn == sender)
            {		
                int indx = std::distance(plotUnitStructList.begin(), it);
                QColor color = QColorDialog::getColor(plotUnitSetting[indx].curveColor, this, tr("Please select color"), QColorDialog::DontUseNativeDialog);
                //QColor color = QColorDialog::getColor(QColor(180,255,180), this, tr("Please select color"));

                if(color.isValid())
                {
                    plotUnitSetting[indx].curveColor = color;
                    setBtnColorIcon(sender, color);
                    (*it).plotUnit->setColor(color);
                }
            }
        }
    }
}



void VarMonitorTimeLine::setBtnColorIcon(QPushButton *btn, QColor color)
{
    QPixmap pixicon(MONITORCOLORICONSIZE,MONITORCOLORICONSIZE);
    QPainter painter(&pixicon);
    painter.setBrush(color);
    painter.drawRect(0,0,MONITORCOLORICONSIZE,MONITORCOLORICONSIZE);
    btn->setIcon(QIcon(pixicon));
}


void VarMonitorTimeLine::updValueLabel()
{
    if(!canUpdateFlag)
        return;
    for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
    {
        QString value = QString("%1").arg((*it).value);
        if((*it).successGetVar)
        {
            (*it).varValuelabel->setText(value);

            if(!varNameEditing)
                (*it).varNameEdit->setStyleSheet("QLineEdit{background: white;}");
        }
        else
        {
            (*it).varValuelabel->setText("N.A.");
            if(!varNameEditing)
                (*it).varNameEdit->setStyleSheet("QLineEdit{background: red;}");

        }
    }

}


void VarMonitorTimeLine::act_switchMoitorOnOff()
{
    if(ui_act_monitorOnOff->isChecked())
    {
        for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
        {
            (*it).plotUnit->startTimeLinePlot();


        }

        canUpdateFlag = true;
    }
    else
    {
        canUpdateFlag = false;
        //here maybe need to wait a few second

        for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
        {
            (*it).plotUnit->pauseTimeLinePlot();
        }
    }





}


void VarMonitorTimeLine::act_switchDisplayMode()
{
    if(ui_act_paperMode->isChecked())
    {
        for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
        {
            (*it).plotUnit->setDisplayMode(DisplayMode_paper);
        }
    }
    else
    {
        for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
        {
            (*it).plotUnit->setDisplayMode(DisplayMode_normal);
        }
    }
}



void VarMonitorTimeLine::act_switchScaleMode(QAction *act)
{
    int scalemode;
    if(act == ui_act_autoScale)
    {
        ui_toolBtn_scaleMode->setIcon(QIcon(PHOTONICONPATH + "iconAutoScale.png"));
        scalemode = ScaleMode_autoScale;
    }
    else if(act == ui_act_fixCurrScale)
    {
        ui_toolBtn_scaleMode->setIcon(QIcon(PHOTONICONPATH + "iconFixScale.png"));
        scalemode = ScaleMode_fixToCurr;
    }
    else if(act == ui_act_expandOnlyScale)
    {
        ui_toolBtn_scaleMode->setIcon(QIcon(PHOTONICONPATH + "iconExpandScale.png"));
        scalemode = ScaleMode_expandOnly;
    }

    for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
    {
        (*it).plotUnit->setScaleMode(scalemode);
    }
    axisScaleMode = scalemode;
}

void VarMonitorTimeLine::act_changeChannelNum(QAction *act)
{
    for(int i = 0; i < MAXTIMELINEPLOTNUM; ++i)
    {
        if(ui_act_channelNum[i] == act)
        {
            targetUnitNum = i + 1;
            break;
        }
    }

    adjustPlotUnitNum();
}


void VarMonitorTimeLine::act_changeDataTime(double second)
{

    QPixmap btnicon(MONITORICONSIZE,MONITORICONSIZE);
    btnicon.fill(Qt::transparent);
    QPainter iconpaint(&btnicon);
    iconpaint.drawPixmap(QRect(0,0,MONITORICONSIZE,MONITORICONSIZE),QPixmap(PHOTONICONPATH + "iconDataTime.png"));


    QString secstr;

    if(second <= 1.0)
    {
        secstr = QString("%1").arg(second,0,'f',1);
        ui_spin_dataTime->setSingleStep(0.1);
        ui_spin_dataTime->setDecimals(1);
    }
    else if(second < 2.0)
    {
        secstr = QString("%1").arg(2.0,0,'f',0);
        ui_spin_dataTime->setValue(2.0);
        ui_spin_dataTime->setSingleStep(1);
        ui_spin_dataTime->setDecimals(0);
    }
    else
    {
        secstr = QString("%1").arg(second,0,'f',0);
        ui_spin_dataTime->setSingleStep(1);
        ui_spin_dataTime->setDecimals(0);
    }
    ui_spin_dataTime->setValue(secstr.toDouble());


    iconpaint.setPen(QPen(Qt::black));
    iconpaint.setFont(QFont("arial",MONITORICONSIZE*0.3,50));
    iconpaint.drawText(QRect(0,0,MONITORICONSIZE,MONITORICONSIZE*0.4),Qt::AlignCenter,secstr);
    iconpaint.setFont(QFont("arial",MONITORICONSIZE*0.3,50));
    iconpaint.drawText(QRect(0,MONITORICONSIZE*0.3,MONITORICONSIZE,MONITORICONSIZE),Qt::AlignCenter,QString("sec"));
    ui_act_editDataTime->setIcon(QIcon(btnicon));



}

void VarMonitorTimeLine::act_dataTimeStartEdit()
{
    //because ui_spin_dataTime has no parent
    //QPoint mousepos = this->mapFromGlobal(QCursor::pos());

    QPoint mousepos = QCursor::pos();
    QRect rec = QApplication::desktop()->screenGeometry();
    QPoint popuppos = mousepos;

    QSize debug = ui_spin_dataTime->size();

    if(mousepos.x() > rec.right() - ui_spin_dataTime->width())
        popuppos.rx() = mousepos.x() - ui_spin_dataTime->width();

    if(mousepos.y() > rec.bottom() - ui_spin_dataTime->height())
        popuppos.ry() = mousepos.y() - ui_spin_dataTime->height();



    ui_spin_dataTime->move(popuppos);
    ui_spin_dataTime->show();
    ui_spin_dataTime->setFocus();
    ui_spin_dataTime->raise();
}

void VarMonitorTimeLine::act_dataTimeChanged()
{
    canUpdateFlag = false;
    ui_spin_dataTime->hide();

    unsigned int msec = (unsigned int)(ui_spin_dataTime->value() * 1000.0);


    for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
    {
        (*it).plotUnit->stopTimeLinePlot();
    }

    for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
    {
        (*it).plotUnit->changeTotalTime(msec);
    }


    if(ui_act_monitorOnOff->isChecked())
    {
        for(auto it = plotUnitStructList.begin(); it != plotUnitStructList.end(); ++it)
        {
            (*it).plotUnit->startTimeLinePlot();
        }
        canUpdateFlag = true;
    }

}


//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------




VarMonitorControl::VarMonitorControl(CommandManager *cmd)
{
    varMonitorTimeLine = NULL;

    //if(!cmd)
    //	return;


    threadRunFlag = true;

    varMonitorTimeLine = new VarMonitorTimeLine(nullptr, cmd);

    varMonitorTimeLine->show();
    //varMonitorTimeLine->moveToThread(&varMonitorThread);
    //connect(&varMonitorThread, &QThread::finished, varMonitorTimeLine, &VarMonitorTimeLine::deleteLater);
    //connect(&varMonitorThread, &QThread::started, varMonitorTimeLine, &VarMonitorTimeLine::startRoutineThread);

    //varMonitorThread.start();

}



VarMonitorControl::~VarMonitorControl()
{
    if(varMonitorTimeLine)
        delete varMonitorTimeLine;
    //varMonitorTimeLine->stopRoutineThread();
    //varMonitorThread.quit();
    //varMonitorThread.wait();
}




void VarMonitorControl::startRoutineThread()
{
    threadRunFlag = true;

    while(threadRunFlag)
    {
        if(varMonitorTimeLine->canUpdate())
        {
            varMonitorTimeLine->updateMonitor();
        }
    }



}



void VarMonitorControl::stopRoutineThread()
{

    threadRunFlag = false;


}


void VarMonitorControl::showMonitor()
{
    if(varMonitorTimeLine)
    {
        varMonitorTimeLine->show();
        varMonitorTimeLine->raise();
    }
}


void VarMonitorControl::setCommandManager(CommandManager *cmd)
{
    if(varMonitorTimeLine)
    {
        varMonitorTimeLine->setCommandManager(cmd);
    }
}
