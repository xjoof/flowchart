#include "commandmanager.h"


//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------

PCVariablePool::PCVariablePool(PCVariablePool* parent)
{
    if(parent != nullptr)
        globlePool = parent;
    else
        globlePool = nullptr;

    if (!InitializeCriticalSectionAndSpinCount(&getVarCritiSect, 
        0x00000000) ) 
        return;

}


PCVariablePool::~PCVariablePool()
{
    allVarMap.clear();
    DeleteCriticalSection(&getVarCritiSect);
}



int PCVariablePool::getVar(const QString &varname, double *value)
{
#ifdef _DEBUG
    //qDebug() << "start PCVariablePool::getVar";
#endif
    int rtn = -1;
    std::map<QString, double>::iterator it;
    EnterCriticalSection(&getVarCritiSect); 
    it = allVarMap.find(varname);
    if(it != allVarMap.end())
    {
        *value = it->second;
        rtn = 0;
    }
    else if(globlePool != nullptr)
    {
        rtn = globlePool->getVar(varname, value);
    }

    LeaveCriticalSection(&getVarCritiSect);
#ifdef _DEBUG
    //qDebug() << "enf of PCVariablePool::getVar";
#endif

    return rtn;
}



int PCVariablePool::setVar(const QString &varname, const double &value)
{
    int rtn = -1;
    std::map<QString, double>::iterator it;
    EnterCriticalSection(&getVarCritiSect);
    it = allVarMap.find(varname);

    if(it != allVarMap.end())
    {
        it->second = value;
        rtn = 0;
    }
    else if(globlePool != nullptr)
    {
        rtn = globlePool->setVar(varname, value);
    }
    LeaveCriticalSection(&getVarCritiSect);
    return rtn;
}


int PCVariablePool::setArrayValue(const QString &arrayname, unsigned int idx, const double &value)
{
    int rtn = -1;
    std::map<QString, std::pair<ArrayDimensInfo, double*> >::iterator it;
    EnterCriticalSection(&getVarCritiSect);
    it = arrayMap.find(arrayname);
    if(it != arrayMap.end())
    {
        double *tmparray = it->second.second;
        if(idx < it->second.first.totalSize - 1)
        {
            tmparray[idx] = value;
            rtn = 0;
        }
    }
    LeaveCriticalSection(&getVarCritiSect);
    return rtn;
}


int PCVariablePool::getArrayValue(const QString &arrayname, unsigned int idx, double *value)
{
    int rtn = -1;
    std::map<QString, std::pair<ArrayDimensInfo, double*> >::iterator it;
    EnterCriticalSection(&getVarCritiSect);
    it = arrayMap.find(arrayname);
    if(it != arrayMap.end())
    {
        double *tmparray = it->second.second;
        if(idx < it->second.first.totalSize - 1)
        {
            *value = tmparray[idx];
            rtn = 0;
        }
    }
    LeaveCriticalSection(&getVarCritiSect);
    return rtn;
}


int PCVariablePool::isArrayExist(const QString &arrayname, ArrayDimensInfo *arrayinfo)
{
    int rtn = -1;
    std::map<QString, std::pair<ArrayDimensInfo, double*> >::iterator it;
    EnterCriticalSection(&getVarCritiSect);
    it = arrayMap.find(arrayname);
    if(arrayMap.find(arrayname) != arrayMap.end())
    {
        *arrayinfo = it->second.first;
        rtn = 0;
    }
    LeaveCriticalSection(&getVarCritiSect);
    return rtn;
}


int PCVariablePool::insertVarMap(const QString &varname, const double &value)
{
    int rtn = -1;

    EnterCriticalSection(&getVarCritiSect);
    if(allVarMap.find(varname) == allVarMap.end())
    {
        allVarMap.insert(std::make_pair(varname, value));
        rtn = 0;
    }
    LeaveCriticalSection(&getVarCritiSect);

    return rtn;
}

int PCVariablePool::eraseVarMap(const QString &varname)
{
    int rtn = -1;
    EnterCriticalSection(&getVarCritiSect);
    auto it = allVarMap.find(varname);
    if(it != allVarMap.end())
    {
        allVarMap.erase(it);
        rtn = 0;
    }
    LeaveCriticalSection(&getVarCritiSect);
    return rtn;
}


int PCVariablePool::insertArrayMap(const QString &arrayname, const ArrayDimensInfo &arrayinfo)
{
    int rtn = -1;

    EnterCriticalSection(&getVarCritiSect);
    if(arrayMap.find(arrayname) == arrayMap.end())
    {
        double *parray = new double(arrayinfo.totalSize);
        if(parray)
        {
            arrayMap.insert(std::make_pair(arrayname, std::make_pair(arrayinfo, parray)));
            rtn = 0;
        }
    }
    LeaveCriticalSection(&getVarCritiSect);
    return rtn;
}


int PCVariablePool::eraseArrayMap(const QString &arrayname)
{
    int rtn = -1;
    EnterCriticalSection(&getVarCritiSect);
    auto it = arrayMap.find(arrayname);
    if(it != arrayMap.end())
    {
        delete [] it->second.second;
        arrayMap.erase(it);
        rtn = 0;
    }
    LeaveCriticalSection(&getVarCritiSect);
    return rtn;
}

//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------

PCVarDimensEdit::PCVarDimensEdit(QWidget *parent, ArrayDimensInfo initinfo)
    : QWidget(parent)
{
    dimensInfo = initinfo;
    dimensInfo_backup = initinfo;

    mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    
    void (QSpinBox::*spinvaluechange)(int) = &QSpinBox::valueChanged;

    ui_spin_dimens = new QSpinBox(this);
    ui_spin_dimens->setSuffix("-D");
    ui_spin_dimens->setSingleStep(1);
    ui_spin_dimens->setMinimum(0);
    ui_spin_dimens->setMaximum(ARRAYDIMENSION);
    ui_spin_dimens->setValue(dimensInfo.dimension);
    ui_spin_dimens->setButtonSymbols(QAbstractSpinBox::NoButtons);
    connect(ui_spin_dimens, &QSpinBox::editingFinished, this, &PCVarDimensEdit::judgeEditFinish);
    connect(ui_spin_dimens, spinvaluechange, this, &PCVarDimensEdit::dimemsSpinChange);

    //ui_spin_dimens->setFixedWidth(150);
    //ui_spin_dimens->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    mainLayout->addWidget(ui_spin_dimens);

    for(int i = 0; i < ARRAYDIMENSION; ++i)
    {
        ui_spin_dimensSize[i] = new QSpinBox(this);
        ui_spin_dimensSize[i]->setPrefix("[");
        ui_spin_dimensSize[i]->setSuffix("]");
        ui_spin_dimensSize[i]->setSingleStep(1);
        ui_spin_dimensSize[i]->setMinimum(0);//will change in dimemsSpinChange(dimensInfo.dimension);
        ui_spin_dimensSize[i]->setMaximum(10000);//will change dynamic
        ui_spin_dimensSize[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);

        ui_spin_dimensSize[i]->setValue(dimensInfo.dimensionSize[i]);

        connect(ui_spin_dimensSize[i], &QSpinBox::editingFinished, this, &PCVarDimensEdit::judgeEditFinish);
        connect(ui_spin_dimensSize[i], spinvaluechange, this, &PCVarDimensEdit::dimemsSizeSpinChange);

        mainLayout->addWidget(ui_spin_dimensSize[i]);
    }

    dimemsSpinChange(dimensInfo.dimension);
    

}

PCVarDimensEdit::~PCVarDimensEdit()
{

}



ArrayDimensInfo PCVarDimensEdit::getDimensInfo()
{
    return dimensInfo;
}



void PCVarDimensEdit::dimemsSpinChange(int dimens)
{
    switch (dimens)
    {
    case 0:
        for(int i = 0; i < ARRAYDIMENSION; ++i)
        {
            ui_spin_dimensSize[i]->setMinimum(0);
            ui_spin_dimensSize[i]->setValue(0);
            ui_spin_dimensSize[i]->setEnabled(false);
        }


        break;

    case 1: 
    case 2:
    case 3:

        for(int i = 0; i < dimens; ++i)
        {
            ui_spin_dimensSize[i]->setMinimum(2);
            ui_spin_dimensSize[i]->setValue(dimensInfo.dimensionSize[i]);
            ui_spin_dimensSize[i]->setEnabled(true);
        }


    default:
        break;
    }
    dimensInfo.dimension = dimens;

}
void PCVarDimensEdit::dimemsSizeSpinChange(int size)
{
    QSpinBox *sender = dynamic_cast<QSpinBox*>(QObject::sender());
    
    if(ui_spin_dimens->hasFocus())
    {
        return;
    }

    for(int i = 0; i < ARRAYDIMENSION; ++i)
    {
        if(sender == ui_spin_dimensSize[i])
        {
            dimensInfo.dimensionSize[i] = size;
            return;
        }

    }

}


void PCVarDimensEdit::judgeEditFinish()
{
    if(ui_spin_dimens->hasFocus())
    {
        return;
    }

    for(int i = 0; i < ARRAYDIMENSION; ++i)
    {
        if(ui_spin_dimensSize[i]->hasFocus())
        {
            return;
        }
    }


    dimensInfo.dimension = ui_spin_dimens->value();
    for(int i = 0; i < ARRAYDIMENSION; ++i)
    {
        dimensInfo.dimensionSize[i] = ui_spin_dimensSize[i]->value();
    }

    if(memcmp(&dimensInfo_backup, &dimensInfo, sizeof(ArrayDimensInfo)))
        emit dimensEditFinish();

    dimensInfo_backup = dimensInfo;
}


//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------



PCVarEditWidget::PCVarEditWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vlayout = new QVBoxLayout(this);

    varTableWidget = new QTableWidget(1,3, this);

    varTableWidget->setColumnHidden(2, true);

    vlayout->addWidget(varTableWidget);
    this->setLayout(vlayout);
    varTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(varTableWidget, &QTableWidget::cellDoubleClicked, this, &PCVarEditWidget::dealCellDoubleClick);
    connect(varTableWidget, &QTableWidget::cellChanged, this, &PCVarEditWidget::varContextChange);
    varTableWidget->installEventFilter(this);
}


PCVarEditWidget::~PCVarEditWidget()
{
}

void PCVarEditWidget::setTextNeedTrans()
{

}

void PCVarEditWidget::addVarToTable(unsigned int indx, QString &varname, const ArrayDimensInfo &info)
{
    int rowcount = varTableWidget->rowCount() - 1;
    editOrAddVar = TABLECHANGE_ADDNEWPCVAR;
    varTableWidget->insertRow(rowcount);
    QTableWidgetItem *varnameitem = new QTableWidgetItem(varname);
    //QTableWidgetItem *valueitem = new QTableWidgetItem(QString("%1").arg(value));
    PCVarDimensEdit *dimensedit = new PCVarDimensEdit(this, info);
    QTableWidgetItem *indxitem = new QTableWidgetItem(QString("%1").arg(indx));

    connect(dimensedit, &PCVarDimensEdit::dimensEditFinish, this, &PCVarEditWidget::varDimensEditFinish);

    varTableWidget->setItem(rowcount, 0, varnameitem);
    //varTableWidget->setItem(rowcount, 1, valueitem);
    varTableWidget->setCellWidget(rowcount, 1, dimensedit);
    varTableWidget->setItem(rowcount, 2, indxitem);
    varTableWidget->setCurrentCell(rowcount, 0);

    indexVarNameMap.insert(std::make_pair(indx, varname));
}

bool PCVarEditWidget::eventFilter(QObject* watched, QEvent* event)
{
    QKeyEvent *keyevent = dynamic_cast<QKeyEvent*>(event);


    if(keyevent)
        keyPressEvent(keyevent);

    return QWidget::eventFilter(watched, event);
}


void PCVarEditWidget::keyPressEvent(QKeyEvent * event)
{
    if(event->key() == Qt::Key_Delete)
    {
        QList<QTableWidgetSelectionRange> range = varTableWidget->selectedRanges();
        if(range.size() == 0)
            return;

        for(auto it = range.begin(); it != range.end(); ++it)
        {
            if((it->leftColumn() <= 0) && (it->rightColumn() >= 1))
            {
                for(int i = it->topRow(); i <= it->bottomRow(); ++i)
                {
                    removeRow(i);
                }
            }
        }
    }

}

void PCVarEditWidget::dealCellDoubleClick(int row, int column)
{
    if(row != (varTableWidget->rowCount() - 1))
    {
        editOrAddVar = TABLECHANGE_EDITPCVARNAME;
        varTableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
        return;
    }
    editOrAddVar = TABLECHANGE_ADDNEWPCVAR;
    varTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    addNewRow();

}


void PCVarEditWidget::addNewRow()
{
    unsigned int indx = -1;//indx = -1 for new local var
    QString varname;
    ArrayDimensInfo info;
    memset(&info, 0, sizeof(ArrayDimensInfo));

    int rowcount = varTableWidget->rowCount() - 1;

    varname = QInputDialog::getText(this, tr("Add new local variable"), tr("Please enter variable name"));

    if(varname.isEmpty())
        return;

    for(int i = 0; i < rowcount; ++i)
    {
        if(varname == varTableWidget->item(i,0)->text())
        {
            QMessageBox::warning(this, tr("warnig"), tr("%1 is used").arg(varname));
            varTableWidget->setCurrentCell(i, 0);
            return;
        }
    }

    emit changeVarSignal(&indx, varname, info);//*indx = -1 for new local var

#ifdef _DEBUG
    if(indx == -1)
    {
        QMessageBox::warning(this, "error","indx == -1");
    }
#endif

    addVarToTable(indx,varname, info);

}

void PCVarEditWidget::removeRow(int row)
{
    unsigned int indx = varTableWidget->item(row,2)->text().toUInt();

    emit removeVarSignal(indx);

    varTableWidget->removeRow(row);

}

void PCVarEditWidget::varContextChange(int row, int column)
{
#ifdef _DEBUG
    qDebug() << "varTableWidget->rowCount" << varTableWidget->rowCount();
#endif


    if(editOrAddVar == TABLECHANGE_ADDNEWPCVAR)
        return;

    unsigned int indx = varTableWidget->item(row,2)->text().toUInt();
    QString varname = varTableWidget->item(row,0)->text();
    PCVarDimensEdit *dimensedit = dynamic_cast<PCVarDimensEdit *>(varTableWidget->cellWidget(row,1));
    ArrayDimensInfo info = dimensedit->getDimensInfo();
    memset(&info, 0, sizeof(ArrayDimensInfo));
    if(dimensedit)
        info = dimensedit->getDimensInfo();
    emit changeVarSignal(&indx, varname, info);

}


void PCVarEditWidget::varDimensEditFinish()
{
    PCVarDimensEdit *sender = dynamic_cast<PCVarDimensEdit*>(QObject::sender());
    for(int i = 0; i < varTableWidget->rowCount(); ++i)
    {
        if(sender == varTableWidget->cellWidget(i, 1))
        {
            editOrAddVar = TABLECHANGE_EDITPCVARNAME;
            varContextChange(i, 1);
            return;
        }
    }
}

//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------





CommandManager::CommandManager(CommandManager *parent)
{
    localVariableIndex = 1;
    varPool = nullptr;
    driveCtrl = nullptr;

    indexVarNameMap.clear();
    arrayIDMap_forRun.clear();
    arrayID_forRun = 1;
    if(parent != nullptr)
    {
        parentCmdManager = parent;
        varPool = new PCVariablePool(parentCmdManager->varPool);
        driveCtrl = parentCmdManager->driveCtrl;
    }
    else
    {
        QMessageBox::warning(nullptr, tr("error"),tr("parent are null"));
    }


}


CommandManager::CommandManager(PCVariablePool *varpool, DriveCtrlBase *drivectrl)
{
    localVariableIndex = 1;
    parentCmdManager = nullptr;
    varPool = varpool;
    driveCtrl = drivectrl;
}


CommandManager::~CommandManager(void)
{
    if(parentCmdManager)
        if(varPool)
        {
            delete varPool;
            varPool = nullptr;
        }
}

int CommandManager::getVar(const QString &varname, double *data)
{
    int rtn = varPool->getVar(varname, data);

    if(rtn)
    {
        rtn = driveCtrl->getVar(varname, data);
    }
    
    if(rtn && parentCmdManager)
    {
        rtn = parentCmdManager->getVar(varname, data);
    }


    return rtn;

}

int CommandManager::setVar(const QString &varname, const double &data)
{
    int rtn = varPool->setVar(varname, data);
    
    if(rtn)
    {
        rtn = driveCtrl->setVar(varname, data);
    }


    if(rtn && parentCmdManager)
    {
        rtn = parentCmdManager->setVar(varname, data);
    }

    return rtn;
}


int CommandManager::getVarOfArray(unsigned int arrayid, unsigned int idx, double *value)
{
    int rtn = -1;
    auto it = arrayIDMap_forRun.find(arrayid);
    if(it != arrayIDMap_forRun.end())
    {
        rtn = varPool->getArrayValue(it->second.first, idx, value);
        if(rtn == -1)
        {
            rtn = driveCtrl->getVar(it->second.first + "[" + QString("%1").arg(idx) + "]", value);
        }
    }
    return rtn;
}


int CommandManager::setVarOfArray(unsigned int arrayid, unsigned int idx, const double &value)
{
    int rtn = -1;
    auto it = arrayIDMap_forRun.find(arrayid);
    if(it != arrayIDMap_forRun.end())
    {
        rtn = varPool->setArrayValue(it->second.first, idx, value);
        if(rtn == -1)
        {
            rtn = driveCtrl->setVar(it->second.first + "[" + QString("%1").arg(idx) + "]", value);
        }
    }
    return rtn;
}


int CommandManager::getArrayIdx_Info(const QString &arrayname, unsigned int *idx, ArrayDimensInfo *info)
{
    int rtn = -1;

    memset(info, 0, sizeof(ArrayDimensInfo));

    for(auto it = arrayIDMap_forRun.begin(); it != arrayIDMap_forRun.end(); ++it)
    {
        if(it->second.first == arrayname)
        {
            *idx = it->first;
            *info = it->second.second;
            rtn = 0;
        }
    }
    if(rtn == -1)
    {
        rtn = varPool->isArrayExist(arrayname, info);
        if(rtn == -1)
        {
            int size = 0;
            int type = 0;
            rtn = driveCtrl->getVarOrArrayInfo(arrayname, &size, &type);

            if(rtn == 0)
            {
                info->dimension = 1;
                info->totalSize = (unsigned int)size;
                info->dimensionSize[0] = (unsigned int)size; 
            }
        }
        if(rtn == 0)
        {
            arrayIDMap_forRun.insert(std::make_pair(arrayID_forRun, std::make_pair(arrayname, *info)));
            *idx = arrayID_forRun;
            ++arrayID_forRun;
            if(arrayID_forRun == (unsigned int)-1)
            {
                QMessageBox::warning(NULL, tr("error"), tr("arrayID_forRun overflow"));
            }
        }
    }

    return rtn;
}


void CommandManager::delIDFromMapForRun(const QString &arrayname)
{
    for(auto it = arrayIDMap_forRun.begin(); it != arrayIDMap_forRun.end(); ++it)
    {
        if(it->second.first == arrayname)
        {
            arrayIDMap_forRun.erase(it);
            break;
        }
    }
}


int CommandManager::connectDrive()
{
    int rtn = 0;
    driveCtrl->connectDrive();
    return rtn;
}

int CommandManager::runHiwinPDLFunc(const QString &funcname)
{
    int rtn = -1;
    rtn = driveCtrl->runFunction(funcname);

    return rtn;
}



int CommandManager::editVarMap(unsigned int *indx, const QString &varname, const ArrayDimensInfo &info)
{
    int rtn = -1;
    auto it = indexVarNameMap.find(*indx);

    if(it == indexVarNameMap.end())
    {
        indexVarNameMap.insert(std::make_pair(localVariableIndex, varname));

        if(info.dimension == 0)
        {
            rtn = varPool->insertVarMap(varname, 0);
        }
        else
        {
            rtn = varPool->insertArrayMap(varname, info);
        }

        *indx = localVariableIndex;
        ++localVariableIndex;
    }
    else
    {
        varPool->eraseVarMap(it->second);
        varPool->eraseArrayMap(it->second);
        it->second = varname;
        if(info.dimension == 0)
        {
            rtn = varPool->insertVarMap(varname, 0);
        }
        else
        {
            rtn = varPool->insertArrayMap(varname, info);
        }
    }

    return rtn;
}

int CommandManager::eraseVarMap(unsigned int indx)
{
    return varPool->eraseVarMap(indexVarNameMap.find(indx)->second);
}