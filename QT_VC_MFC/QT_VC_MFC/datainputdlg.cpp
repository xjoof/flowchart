#include "datainputdlg.h"


FloDataInputDlg::FloDataInputDlg(QWidget *parent, FlowchartsTemplate *flocha) 
    : QDialog(parent),
    m_flocha(flocha)
{

    okBtn = nullptr;
    cancelBtn = nullptr;

    label1 = nullptr;
    label2 = nullptr;
    label3 = nullptr;

    combotype1 = nullptr;
    combotype2 = nullptr;
    combooper = nullptr;


    lineedit1 = nullptr;
    lineedit2 = nullptr;
    lineedit3 = nullptr;

    dataBackup_edit.clear();
    dataBackup_combo.clear();


    createDlg();

    if(m_flocha == nullptr)
    {
        QMessageBox::warning(this,tr("error"),tr("flocha is null"));
        return;
    }

}


FloDataInputDlg::~FloDataInputDlg(void)
{
    delete okBtn;
    delete cancelBtn;

    if(label1 == nullptr)
    {
        delete label1;
        label1 = nullptr;
    }

    if(label2 == nullptr)
    {
        delete label2;
        label2 = nullptr;
    }

    if(label3 == nullptr)
    {
        delete label3;
        label3 = nullptr;
    }



    if(combotype1 == nullptr)
    {
        delete combotype1;
        combotype1 = nullptr;
    }

    if(combotype2 == nullptr)
    {
        delete combotype2;
        combotype2 = nullptr;
    }

    if(combooper == nullptr)
    {
        delete combooper;
        combooper = nullptr;
    }

    if(lineedit1 == nullptr)
    {
        delete lineedit1;
        lineedit1 = nullptr;
    }
    if(lineedit2 == nullptr)
    {
        delete lineedit2;
        lineedit2 = nullptr;
    }
    if(lineedit3 == nullptr)
    {
        delete lineedit3;
        lineedit3 = nullptr;
    }



}



void FloDataInputDlg::createDlg()
{
    row = 0;

    okBtn = new QPushButton(this);
    cancelBtn = new QPushButton(this);
    std::vector<std::pair<int, QString > > oplist;

    okBtn->setText(tr("OK"));
    cancelBtn->setText(tr("Cancel"));

    connect(okBtn, &QPushButton::clicked, this, &FloDataInputDlg::act_triggerOK);
    connect(cancelBtn, &QPushButton::clicked, this, &FloDataInputDlg::act_triggerCancel);

    //FlowchartsAssemble *flochaassem = dynamic_cast<FlowchartsAssemble*>(m_flocha);

    QDialog flodlg;

    grid = new QGridLayout(this);

    labelname = new QLabel(tr("ID name :"), this);
    //	labelList.push_back(labelname);

    editname = new QLineEdit(this);
    //	lineEditList.push_back(editname);
    editname->setText(m_flocha->idName);
    connect(editname, &QLineEdit::textChanged, this, &FloDataInputDlg::updIdName);


    switch(m_flocha->getflochaType())
    {
    case flochaTypeIf:
        createIfDlg();

        break;
    case flochaTypeSet:
        createSetDlg();

        break;
    case flochaTypeRun:
        createRunDlg();
        break;
    case flochaTypeSleep:

        createSleepDlg();

        break;
    case flochaTypeAssem:
        createAssemDlg();

        break;
    case flochaTypeTemplate:

        break;
    default:
        QMessageBox::warning(this, tr("error"), tr("setBtnTextByflocha error"));
        break;
    }

    grid->addWidget(okBtn,row,3);
    grid->addWidget(cancelBtn,row,4);




}

void FloDataInputDlg::createIfDlg()
{
    setWindowTitle(tr("Edit If flowchart data"));
    FlowchartsIf *flochaif = dynamic_cast<FlowchartsIf *>(m_flocha);

    //because alse have void (QComboBox:: *indexChangedSignal)(double)
    void (QComboBox:: *indexChangedSignal)(int) = &QComboBox::currentIndexChanged;






    label1 = new QLabel(tr("Statement1"), this);
    label2 = new QLabel(tr("Statement2"), this);


    combotype1 = new QComboBox(this);
    combotype2 = new QComboBox(this);
    combooper = new QComboBox(this);

    auto oplist = IfOperatorClass::getOperNumStrList();
    for(auto it = oplist.begin(); it != oplist.end(); ++it)
    {
        combooper->addItem(it->second,it->first);
    }
    oplist = IfOperatorClass::getCompNumTypeList();
    for(auto it = oplist.begin(); it != oplist.end(); ++it)
    {
        combotype1->addItem(it->second,it->first);
        combotype2->addItem(it->second,it->first);
    }

    connect(combotype1, indexChangedSignal, combotype2, &QComboBox::setCurrentIndex);
    connect(combotype2, indexChangedSignal, combotype1, &QComboBox::setCurrentIndex);



    lineedit1 = new QLineEdit(this);
    lineedit2 = new QLineEdit(this);






    lineedit1->setText(flochaif->getStatement1());
    lineedit2->setText(flochaif->getStatement2());
    std::pair<QString, QString> oppair = IfOperatorClass::getOperStr(flochaif->getOperator());
    combotype1->setCurrentText(oppair.second);
    combooper->setCurrentText(oppair.first);

    dataBackup_edit.push_back(std::make_pair(lineedit1,flochaif->getStatement1()));
    dataBackup_edit.push_back(std::make_pair(lineedit2,flochaif->getStatement2()));
    dataBackup_combo.push_back(std::make_pair(combotype1,oppair.second));
    dataBackup_combo.push_back(std::make_pair(combooper,oppair.first));



    connect(lineedit1, &QLineEdit::textChanged, this, &FloDataInputDlg::updIfStatement1);
    connect(lineedit2, &QLineEdit::textChanged, this, &FloDataInputDlg::updIfStatement2);
    connect(combotype1, indexChangedSignal, this, &FloDataInputDlg::updIfOperator);
    connect(combooper, indexChangedSignal, this, &FloDataInputDlg::updIfOperator);



    grid->addWidget(labelname,row,0);
    grid->addWidget(editname,row,1,1,2);

    ++row;


    grid->addWidget(label1,row,0);
    grid->addWidget(label2,row,3);

    grid->addWidget(combotype1,row,1);
    grid->addWidget(combotype2,row,4);
    grid->addWidget(combooper,row,2,2,1);
    ++row;
    grid->addWidget(lineedit1,row,0,1,2);
    grid->addWidget(lineedit2,row,3,1,2);
    ++row;

    /*
    labelList.push_back(label1);
    labelList.push_back(label2);

    comboList.push_back(combotype1);
    comboList.push_back(combotype2);
    comboList.push_back(combooper);

    lineEditList.push_back(lineedit1);
    lineEditList.push_back(lineedit2);
    */
}

void FloDataInputDlg::createSetDlg()
{
    setWindowTitle(tr("Edit SetVar flowchart data"));
    FlowchartsSetVar* flochaset = dynamic_cast<FlowchartsSetVar*>(m_flocha);


    label1 = new QLabel(tr("Var name"), this);
    label2 = new QLabel(tr("Value statement"), this);


    lineedit1 = new QLineEdit(this);
    lineedit2 = new QLineEdit(this);


    lineedit1->setText(flochaset->getVarName());
    lineedit2->setText(flochaset->getStatement());

    dataBackup_edit.push_back(std::make_pair(lineedit1,flochaset->getVarName()));
    dataBackup_edit.push_back(std::make_pair(lineedit2,flochaset->getStatement()));


    connect(lineedit1, &QLineEdit::textChanged, this, &FloDataInputDlg::updSetVarName);
    connect(lineedit2, &QLineEdit::textChanged, this, &FloDataInputDlg::updSetValueStatement);




    grid->addWidget(labelname,row,0);
    grid->addWidget(editname,row,1,1,4);

    ++row;


    grid->addWidget(label1,row,0);
    grid->addWidget(lineedit1,row,1,1,4);


    ++row;
    grid->addWidget(label2,row,0);
    grid->addWidget(lineedit2,row,1,1,4);

    ++row;




}

void FloDataInputDlg::createRunDlg()
{
    setWindowTitle(tr("Edit RunPDL flowchart data"));

    FlowchartsRunPDL *flocharun = dynamic_cast<FlowchartsRunPDL*>(m_flocha);

    label1 = new QLabel(tr("PDL name"), this);


    lineedit1 = new QLineEdit(this);


    lineedit1->setText(flocharun->getPDLName());

    dataBackup_edit.push_back(std::make_pair(lineedit1,flocharun->getPDLName()));



    connect(lineedit1, &QLineEdit::textChanged, this, &FloDataInputDlg::updRunPDLName);




    grid->addWidget(labelname,row,0);
    grid->addWidget(editname,row,1,1,4);

    ++row;


    grid->addWidget(label1,row,0);
    grid->addWidget(lineedit1,row,1,1,4);


    ++row;




}

void FloDataInputDlg::createSleepDlg()
{
    setWindowTitle(tr("Edit Sleep flowchart data"));
    FlowchartsSleep *flochasleep = dynamic_cast<FlowchartsSleep*>(m_flocha);


    label1 = new QLabel(tr("Sleep time"), this);


    lineedit1 = new QLineEdit(this);


    lineedit1->setText(flochasleep->getSleepTime());

    dataBackup_edit.push_back(std::make_pair(lineedit1,flochasleep->getSleepTime()));

    connect(lineedit1, &QLineEdit::textChanged, this, &FloDataInputDlg::updSleepTime);


    grid->addWidget(labelname,row,0);
    grid->addWidget(editname,row,1,1,4);

    ++row;

    grid->addWidget(label1,row,0);
    grid->addWidget(lineedit1,row,1,1,4);

    ++row;



}


void FloDataInputDlg::createAssemDlg()
{

    setWindowTitle(tr("Edit Assem flowchart data"));
    FlowchartsAssemble *flochaassem = dynamic_cast<FlowchartsAssemble*>(m_flocha);




    label1 = new QLabel(tr("Loop run times"), this);
    label2 = new QLabel(tr("Sleep time per loop"), this);
    label3 = new QLabel(tr("Loop run count variable name"), this);


    lineedit1 = new QLineEdit(this);
    lineedit2 = new QLineEdit(this);
    lineedit3 = new QLineEdit(this);

    FlochaAssemAttr attr;
    flochaassem->getAssemAttr(&attr);
    
    lineedit1->setText(QString("%1").arg(attr.runLoopTimes));
    lineedit2->setText(QString("%1").arg(attr.sleepPerLoop));
    lineedit3->setText(QString("%1").arg(attr.loopRunCntVar));

    dataBackup_edit.push_back(std::make_pair(lineedit1,QString("%1").arg(attr.runLoopTimes)));
    dataBackup_edit.push_back(std::make_pair(lineedit2,QString("%1").arg(attr.sleepPerLoop)));
    dataBackup_edit.push_back(std::make_pair(lineedit2,QString("%1").arg(attr.loopRunCntVar)));


    connect(lineedit1, &QLineEdit::textChanged, this, &FloDataInputDlg::updLoopRunTimes);
    connect(lineedit2, &QLineEdit::textChanged, this, &FloDataInputDlg::updSleepTimePerLoop);
    connect(lineedit3, &QLineEdit::textChanged, this, &FloDataInputDlg::updLoopRunCntStr);




    grid->addWidget(labelname,row,0);
    grid->addWidget(editname,row,1,1,4);

    ++row;


    grid->addWidget(label1,row,0);
    grid->addWidget(lineedit1,row,1,1,4);
    ++row;


    grid->addWidget(label2,row,0);
    grid->addWidget(lineedit2,row,1,1,4);
    ++row;

    grid->addWidget(label3,row,0);
    grid->addWidget(lineedit3,row,1,1,4);
    ++row;



}


void FloDataInputDlg::closeEvent(QCloseEvent *closeeve)
{
    act_triggerCancel();
}

void FloDataInputDlg::act_triggerOK()
{
    return  done(1);
}


void FloDataInputDlg::act_triggerCancel()
{
    for(auto it = dataBackup_edit.begin(); it != dataBackup_edit.end(); ++it)
    {
        it->first->setText(it->second);
    }


    for(auto it = dataBackup_combo.begin(); it != dataBackup_combo.end(); ++it)
    {
        it->first->setCurrentText(it->second);
    }

    return  done(0);
}


//----------general------------------
void FloDataInputDlg::updIdName(QString str)
{

    memcpy(m_flocha->idName, str.toStdString().c_str(), str.size());
}
//----------general------------------

//----------if------------------
void FloDataInputDlg::updIfStatement1(QString str)
{
    FlowchartsIf *flochaif = dynamic_cast<FlowchartsIf *>(m_flocha);

    flochaif->setStatement1(str);
}


void FloDataInputDlg::updIfStatement2(QString str)
{
    FlowchartsIf *flochaif = dynamic_cast<FlowchartsIf *>(m_flocha);
    flochaif->setStatement2(str);
}


void FloDataInputDlg::updIfOperator()
{
    FlowchartsIf *flochaif = dynamic_cast<FlowchartsIf *>(m_flocha);
    int op = combotype1->currentData().toInt() | combooper->currentData().toInt();
    flochaif->setOperator(op);
}
//----------if------------------

//----------set------------------
void FloDataInputDlg::updSetVarName(QString str)
{
    FlowchartsSetVar* flochaset = dynamic_cast<FlowchartsSetVar*>(m_flocha);

    flochaset->setVarName(str);
}

void FloDataInputDlg::updSetValueStatement(QString str)
{
    FlowchartsSetVar* flochaset = dynamic_cast<FlowchartsSetVar*>(m_flocha);
    flochaset->setStatement(str);
}
//----------set------------------




//----------run------------------
void FloDataInputDlg::updRunPDLName(QString str)
{
    FlowchartsRunPDL *flocharun = dynamic_cast<FlowchartsRunPDL*>(m_flocha);
    flocharun->setPDLName(str);
}
//----------run------------------



//----------run------------------
void FloDataInputDlg::updSleepTime(QString str)
{
    FlowchartsSleep *flochasleep = dynamic_cast<FlowchartsSleep*>(m_flocha);

    flochasleep->setSleepTime(str);
}
//----------run------------------



//----------assem------------------
void FloDataInputDlg::updLoopRunTimes(QString str)
{
    
    FlowchartsAssemble *flochaassem = dynamic_cast<FlowchartsAssemble*>(m_flocha);
    FlochaAssemAttr attr;
    flochaassem->getAssemAttr(&attr);
    attr.runLoopTimes = str.toUInt();
    flochaassem->setAssemAttr(attr);
}


void FloDataInputDlg::updSleepTimePerLoop(QString str)
{
    FlowchartsAssemble *flochaassem = dynamic_cast<FlowchartsAssemble*>(m_flocha);
    FlochaAssemAttr attr;
    flochaassem->getAssemAttr(&attr);
    attr.sleepPerLoop = str.toUInt();
    flochaassem->setAssemAttr(attr);
    
}


void FloDataInputDlg::updLoopRunCntStr(QString str)
{
    FlowchartsAssemble *flochaassem = dynamic_cast<FlowchartsAssemble*>(m_flocha);
    FlochaAssemAttr attr;
    flochaassem->getAssemAttr(&attr);
    attr.loopRunCntVar = str;
    flochaassem->setAssemAttr(attr);
    
}
//----------assem------------------