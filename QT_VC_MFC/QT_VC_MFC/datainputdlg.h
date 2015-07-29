#ifndef DATAINPUTDLG_H
#define DATAINPUTDLG_H
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QComboBox>
#include <QGridLayout>
#include "flowchartsitems.h"


class FloDataInputDlg : public QDialog
{
    Q_OBJECT
public:
    FloDataInputDlg(QWidget *parent, FlowchartsTemplate *flocha);
    ~FloDataInputDlg(void);


private:




    void createDlg();


    void createIfDlg();
    void createSetDlg();
    void createRunDlg();
    void createSleepDlg();
    void createAssemDlg();


    void closeEvent(QCloseEvent *closeeve);

    void act_triggerOK();
    void act_triggerCancel();

    //----------general------------------
    void updIdName(QString str);
    //----------general------------------

    //----------if------------------
    void updIfStatement1(QString str);
    void updIfStatement2(QString str);
    void updIfOperator();
    //----------if------------------

    //----------set------------------
    void updSetVarName(QString str);
    void updSetValueStatement(QString str);
    //----------set------------------




    //----------run------------------
    void updRunPDLName(QString str);
    //----------run------------------


    //----------sleep------------------
    void updSleepTime(QString str);
    //----------sleep------------------


    //----------assem------------------
    void updLoopRunTimes(QString str);
    void updSleepTimePerLoop(QString str);
    void updLoopRunCntStr(QString str);


    //----------assem------------------



    QLabel *labelname;
    QLineEdit *editname;

    QLabel *label1;
    QLabel *label2;
    QLabel *label3;

    QComboBox *combotype1;
    QComboBox *combotype2;
    QComboBox *combooper;


    QLineEdit *lineedit1;
    QLineEdit *lineedit2;
    QLineEdit *lineedit3;

    FlowchartsTemplate *m_flocha;

    QPushButton *okBtn;
    QPushButton *cancelBtn;

    //	std::vector<QLabel*> labelList;
    //	std::vector<QLineEdit *> lineEditList;
    //	std::vector<QComboBox *> comboList;
    std::list<std::pair<QLineEdit*,QString> > dataBackup_edit;
    std::list<std::pair<QComboBox*,QString> > dataBackup_combo;


    int row;
    QGridLayout *grid;


};

#endif