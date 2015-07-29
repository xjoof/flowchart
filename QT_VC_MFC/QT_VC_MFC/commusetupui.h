#ifndef COMMUSETUPUI_H
#define COMMUSETUPUI_H

#include <QWidget>


#include "commandmanager.h"
#include "ui_commusetupui.h"

class CommuSetupUI : public QWidget
{
    Q_OBJECT

public:
    CommuSetupUI(QWidget *parent, DriveCtrlBase *drivectrl);
    ~CommuSetupUI();

private:
    Ui::CommuSetupUI ui;

    void connectDrive();
    void disconnectDrive();


    void driveTypeChange(int type);
    void comPortNumChange(int port);


    DriveCtrlBase *driveCtrl;


};

#endif // COMMUSETUPUI_H
