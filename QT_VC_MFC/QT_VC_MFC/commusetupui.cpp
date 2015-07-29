#include "commusetupui.h"

CommuSetupUI::CommuSetupUI(QWidget *parent, DriveCtrlBase *drivectrl)
    : QWidget(parent)
{
    driveCtrl = drivectrl;

    ui.setupUi(this);

    connect(ui.connectBtn, &QPushButton::clicked, this, &CommuSetupUI::connectDrive);
    connect(ui.disconnectBtn, &QPushButton::clicked, this, &CommuSetupUI::disconnectDrive);


    void (QComboBox:: *indexChangedSignal)(int) = &QComboBox::currentIndexChanged;
    for(int i = 0; i < 150; ++i)
    {
        ui.portNumCombo->addItem(QString("%1").arg(i+1));
    }

    std::vector<QString> typelist;
    typelist.clear();
    if(driveCtrl)
        typelist = driveCtrl->getDriveTypeList();

    for(auto it = typelist.begin(); it != typelist.end(); ++it)
    {
        ui.driveTypeCombo->addItem(*it);
    }

    connect(ui.driveTypeCombo, indexChangedSignal, this, &CommuSetupUI::driveTypeChange);
    connect(ui.portNumCombo, indexChangedSignal, this, &CommuSetupUI::comPortNumChange);
}

CommuSetupUI::~CommuSetupUI()
{

}


void CommuSetupUI::connectDrive()
{
    if(driveCtrl)
        driveCtrl->connectDrive();
}



void CommuSetupUI::disconnectDrive()
{
    if(driveCtrl)
        driveCtrl->disconnectDrive();
}


void CommuSetupUI::driveTypeChange(int type)
{
    if(driveCtrl)
        driveCtrl->changeDriveType(type);
}



void CommuSetupUI::comPortNumChange(int port)
{
    if(driveCtrl)
        driveCtrl->changePortNum(port + 1);
}