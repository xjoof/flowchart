#include "vardisplaymanage.h"
#include "globalobjmanager.h"
extern GlobalObjManager *globalObjManager;

DisplayIconList::DisplayIconList()
{
    iconMap.clear();
    totalIconNum = 0;
}


DisplayIconList::~DisplayIconList()
{
    for(auto it = iconMap.begin(); it != iconMap.end(); ++it)
    {
        delete it->second;
    }
    iconMap.clear();

}

const QPixmap * const DisplayIconList::getIcon(int num) const
{
    if(totalIconNum == 0)
        return nullptr;
    auto it = iconMap.find(num);
    if(it != iconMap.end())
        return it->second;
#ifdef _DEBUG
    else
        QMessageBox::warning(nullptr, QString("error"), QString("can not find icon"));
#endif
    return nullptr;
}


const int DisplayIconList::iconCount() const
{
    return totalIconNum;
}

void DisplayIconList::insertIcon(const QString &path)
{
    QPixmap *pixmap = new QPixmap(path);
    if(pixmap->isNull())
    {
#ifdef _DEBUG
        QMessageBox::warning(NULL, QString("Warning"),QString("Can not load icon : %1").arg(path));
#endif
    }
    else
    {
        iconMap.insert(std::make_pair(totalIconNum, pixmap));
        ++totalIconNum;
    }



}
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------




VarDisplayUnit::VarDisplayUnit(QWidget *parent, CommandManager *cmd, QString varname, DisplayUnitSetting setting, 
                               const std::map<double, QString> &textmap,
                               const std::map<double, int> &iconmap)
                               : QWidget(parent),
                               unitSetting(setting),
                               valueTextMap(textmap),
                               valueIconIdxMap(iconmap),
                               commandManager(cmd),
                               varName(varname)
{
    mainLayout = new QVBoxLayout(this);
    displayLayout = new QHBoxLayout(this);
    editAreaLayout = new QGridLayout(this);
    //------------------------minimize the space------------
    mainLayout->setContentsMargins(0,0,0,0);
    displayLayout->setContentsMargins(0,0,0,0);
    editAreaLayout->setContentsMargins(0,0,0,0);
    this->setContentsMargins(0,0,0,0);
    //------------------------minimize the space------------

    unitSetting.bits.editMode = 0;

    updEditDelaytimer = nullptr;

    ui_label_varName = nullptr;
    ui_edit_varValue = nullptr;
    ui_label_icon = nullptr;
    ui_combo_textMap = nullptr;

    ui_combo_enable = nullptr;
    ui_combo_type = nullptr;
    ui_combo_readWrite = nullptr;
    ui_edit_varName = nullptr;

    ui_edit_newKey = nullptr;
    ui_edit_newText = nullptr;
    ui_combo_forNew = nullptr;

    ui_button_edit = nullptr;
    ui_button_ok = nullptr;
    ui_button_cancel = nullptr;

    needBackup = true;
    varValueEditing = false;
    varValueCanUpd = true;
    varNameInvalid = false;

    ctrlKeyPressed = false;

    valueTextMap_backup.clear();
    valueIconIdxMap_backup.clear();


    edit_combo_map.clear();
    edit_edit_map.clear();
    labelList.clear();

    ui_label_varName = new QLabel(varName, this);
    displayLayout->addWidget(ui_label_varName);
    mainLayout->addLayout(displayLayout);
    mainLayout->addLayout(editAreaLayout);
    this->setLayout(mainLayout);


#ifdef _DEBUG
    if(globalObjManager->getIconList() == nullptr)
        QMessageBox::warning(this, QString("error"), QString("icon list is null"));
    //unitSetting.bits.displayType = UNITDISPLAY_TEXTMAP;

#endif



    rebuildUnitDisplay();

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);


}

VarDisplayUnit::~VarDisplayUnit()
{

}


void VarDisplayUnit::setTextNeedTrans()
{
    deleteEditArea();

    rebuildUnitDisplay();

    if(unitSetting.bits.editMode)
    {
        unitSetting.bits.editMode = 0;
        startEditContents();
    }
}


void VarDisplayUnit::updateVar()
{
    double value;

#ifdef _DEBUG
    //	qDebug() << "getVar in VarDisplayUnit::updateVar()";
#endif
    if(commandManager->getVar(varName, &value) == -1)
    {
        varNameInvalid = true;
        update();
        return;
    }
    varNameInvalid = false;

    if(ui_edit_varValue)
    {
        if(varValueCanUpd)
            ui_edit_varValue->setText(QString("%1").arg(value));

        /*
        if(testcnt == 0)
        {
        ui_edit_varValue->setText(QString("%1").arg(value));
        testcnt = 20;
        }
        else
        --testcnt;
        */
    }
    else if(ui_combo_textMap)
    {
        auto it = valueTextMap.find(value);
        if(it != valueTextMap.end())
        {
            ui_combo_textMap->setCurrentText(it->second);
        }
#ifdef _DEBUG
        //else
        //QMessageBox::warning(this, QString("waring"), QString("%1 textMap can not find text").arg(varName));
#endif

    }
    else if(ui_label_icon)
    {
        auto it = valueIconIdxMap.find(value);
        if(it != valueIconIdxMap.end())
        {
            const QPixmap *pix = globalObjManager->getIconList()->getIcon(it->second);
            if(pix)
            {
                ui_label_icon->setPixmap(pix->scaled(30,30));
            }
        }
#ifdef _DEBUG
        //else
        //QMessageBox::warning(this, QString("waring"), QString("%1 iconMap can not find icon").arg(varName));
#endif

    }

}



std::map<double, QString> VarDisplayUnit::getValueTextMap() const
{
    return valueTextMap;
}

std::map<double, int> VarDisplayUnit::getValueIconIdxMap() const
{
    return valueIconIdxMap;
}


QString VarDisplayUnit::getVarName() const
{
    return varName;
}



DisplayUnitSetting VarDisplayUnit::getUnitSetting() const
{
    return unitSetting;
}


void VarDisplayUnit::paintEvent(QPaintEvent *paintevent)
{
    QPainter painter(this);

    if(ui_edit_varValue)
    {
        if(varValueEditing || !varValueCanUpd)
            ui_edit_varValue->setStyleSheet("QLineEdit{background: yellow;}");
        else if(varNameInvalid)
            ui_edit_varValue->setStyleSheet("QLineEdit{background: rgb(255,150,150);}");
        else
            ui_edit_varValue->setStyleSheet("QLineEdit{background: white;}");

    }
    else if(ui_combo_textMap)
    {

    }
    else if(ui_label_icon)
    {

    }


    if(unitSetting.bits.editMode)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(255,255,50), Qt::DiagCrossPattern));
        painter.drawRect(QRect(0, 0, this->width(), this->height()));
    }

}


bool VarDisplayUnit::eventFilter(QObject* watched, QEvent* evt)
{
    return QWidget::eventFilter(watched, evt);
}


void VarDisplayUnit::keyPressEvent(QKeyEvent *keyevt)
{

    int keycode = keyevt->key();
    if((keyevt->key() == Qt::CTRL) || (keyevt->key() == Qt::Key_Control))
    {
        ctrlKeyPressed = true;
        return;
    }
    if(ctrlKeyPressed)
    {
        if((keyevt->key() == Qt::Key_Return) || (keyevt->key() == Qt::Key_Enter))
        {
            if(unitSetting.bits.editMode)
            {
                ctrlKeyPressed = false;
                editFinishOK();
            }
        }
    }
}


void VarDisplayUnit::keyReleaseEvent(QKeyEvent *keyevt)
{
    ctrlKeyPressed = false;
}


void VarDisplayUnit::editingVarValue()
{
    varValueEditing = true;
    varValueCanUpd = false;
}

void VarDisplayUnit::editVarValueFinish()
{
    commandManager->setVar(ui_label_varName->text(), ui_edit_varValue->text().toDouble());
    varValueCanUpd = true;
    varValueEditing = false;
}

void VarDisplayUnit::varValueSelectChange()
{
    if(ui_edit_varValue->hasSelectedText())
        varValueCanUpd = false;
    else if(varValueEditing == false)
        varValueCanUpd = true;
}


void VarDisplayUnit::editVarNameStart()
{

}

void VarDisplayUnit::editVarNameFinish()
{

    varName = ui_edit_varName->text();
    ui_label_varName->setText(varName);

}


void VarDisplayUnit::startEditContents()
{
    if(unitSetting.bits.editMode)
    {
        editFinishCancel();
        return;
    }
    else
    {
        unitSetting.bits.editMode = 1;
        ui_button_edit->setText("Cancel");
    }

    if(needBackup)
    {
        valueTextMap_backup = valueTextMap;
        valueIconIdxMap_backup = valueIconIdxMap;
        needBackup = false;
    }

    //this is important, let this widget update sizehint
    this->setHidden(true);
    //this is important, let this widget update sizehint


    int currrow = 0;
    void (QComboBox:: *indexChangedSignal)(int) = &QComboBox::currentIndexChanged;

    ui_edit_varName = new QLineEdit(this);
    ui_combo_enable = new QComboBox(this);
    ui_combo_type = new QComboBox(this);
    ui_combo_readWrite = new QComboBox(this);



    ui_edit_varName->setText(ui_label_varName->text());
    connect(ui_edit_varName, &QLineEdit::textEdited, this, &VarDisplayUnit::editVarNameStart);
    connect(ui_edit_varName, &QLineEdit::returnPressed, this, &VarDisplayUnit::editVarNameFinish);


    QLabel *varname_label = new QLabel(tr("Variable name"),this);
    labelList.push_back(varname_label);
    editAreaLayout->addWidget(varname_label, currrow, 1, 1, 2);
    editAreaLayout->addWidget(ui_edit_varName, currrow, 3, 1, 2);
    ++currrow;



    //------sequence must like this--------
    ui_combo_enable->addItem(tr("Enable"));
    ui_combo_enable->addItem(tr("Disable"));
    //------sequence must like this--------
    ui_combo_enable->setCurrentIndex(unitSetting.bits.disable);
    connect(ui_combo_enable, indexChangedSignal, this, &VarDisplayUnit::updEditState);
    QLabel *enable_label = new QLabel(tr("Enable?"),this);
    labelList.push_back(enable_label);
    editAreaLayout->addWidget(enable_label, currrow, 1, 1, 1);
    editAreaLayout->addWidget(ui_combo_enable, currrow, 2, 1, 1);




    ui_combo_type->addItem(tr("Direct show"), UNITDISPLAY_DIRECT);
    ui_combo_type->addItem(tr("Show text list"), UNITDISPLAY_TEXTMAP);
    ui_combo_type->addItem(tr("Show icon list"), UNITDISPLAY_ICONMAP);

    for(int indx = 0; indx < ui_combo_type->count(); ++indx)
    {
        if(unitSetting.bits.displayType == ui_combo_type->itemData(indx).toInt())
        {
            ui_combo_type->setCurrentIndex(indx);
            break;
        }
    }
    connect(ui_combo_type, indexChangedSignal, this, &VarDisplayUnit::updEditState);
    QLabel *type_label = new QLabel(tr("Type"),this);
    labelList.push_back(type_label);
    editAreaLayout->addWidget(type_label, currrow, 3, 1, 1);
    editAreaLayout->addWidget(ui_combo_type, currrow, 4, 1, 1);
    ++currrow;


    //------sequence must like this--------
    ui_combo_readWrite->addItem(tr("Read and write"));
    ui_combo_readWrite->addItem(tr("Read only"));
    //------sequence must like this--------
    ui_combo_readWrite->setCurrentIndex(unitSetting.bits.isReadOnly);
    connect(ui_combo_readWrite, indexChangedSignal, this, &VarDisplayUnit::updEditState);
    QLabel *r_w_label = new QLabel(tr("Read/write?"),this);
    labelList.push_back(r_w_label);
    editAreaLayout->addWidget(r_w_label, currrow, 1, 1, 1);
    editAreaLayout->addWidget(ui_combo_readWrite, currrow, 2, 1, 1);
    ++currrow;


    if(unitSetting.bits.displayType == UNITDISPLAY_TEXTMAP)
    {

        QLabel *keylabel = new QLabel(tr("Key"), this);
        QLabel *textlabel = new QLabel(tr("Text"), this);

        labelList.push_back(keylabel);
        labelList.push_back(textlabel);

        editAreaLayout->addWidget(keylabel, currrow, 1, 1, 2);
        editAreaLayout->addWidget(textlabel, currrow, 3, 1, 2);
        ++currrow;


        for(auto it = valueTextMap.begin(); it != valueTextMap.end(); ++it)
        {
            QLineEdit *keyedit = new QLineEdit(this);
            QLineEdit *valueedit = new QLineEdit(this);
            edit_edit_map.insert(std::make_pair(keyedit, valueedit));


            editAreaLayout->addWidget(keyedit, currrow, 1, 1, 2);
            editAreaLayout->addWidget(valueedit, currrow, 3, 1, 2);
            ++currrow;

            keyedit->setText(QString("%1").arg(it->first));
            valueedit->setText(it->second);
        }

        ui_edit_newKey = new QLineEdit(this);
        ui_edit_newText = new QLineEdit(this);


        editAreaLayout->addWidget(ui_edit_newKey, currrow, 1, 1, 2);
        editAreaLayout->addWidget(ui_edit_newText, currrow, 3, 1, 2);
        ++currrow;
        connect(ui_edit_newKey, &QLineEdit::editingFinished, this, &VarDisplayUnit::updEditState);

    }
    else if(unitSetting.bits.displayType == UNITDISPLAY_ICONMAP)
    {
        QLabel *keylabel = new QLabel(tr("Key"), this);
        QLabel *iconlabel = new QLabel(tr("Icon"), this);

        labelList.push_back(keylabel);
        labelList.push_back(iconlabel);

        editAreaLayout->addWidget(keylabel, currrow, 1, 1, 2);
        editAreaLayout->addWidget(iconlabel, currrow, 3, 1, 2);
        ++currrow;


        for(auto it = valueIconIdxMap.begin(); it != valueIconIdxMap.end(); ++it)
        {
            QLineEdit *keyedit = new QLineEdit(this);
            QComboBox *iconcombo = new QComboBox(this);
            edit_combo_map.insert(std::make_pair(keyedit, iconcombo));



            editAreaLayout->addWidget(keyedit, currrow, 1, 1, 2);
            editAreaLayout->addWidget(iconcombo, currrow, 3, 1, 2);
            ++currrow;

            keyedit->setText(QString("%1").arg(it->first));


            for(int idx = 0; idx < globalObjManager->getIconList()->iconCount(); ++idx)
            {
                const QPixmap *pix = globalObjManager->getIconList()->getIcon(idx);
                if(pix)
                    iconcombo->addItem(QIcon(*pix), tr("%1").arg(idx),idx);
            }

            int curridx = it->second;
            if((curridx >= 0) && (curridx < iconcombo->count()))
                iconcombo->setCurrentIndex(curridx);
        }

        ui_edit_newKey = new QLineEdit(this);
        ui_combo_forNew = new QComboBox(this);

        for(int idx = 0; idx < globalObjManager->getIconList()->iconCount(); ++idx)
        {
            const QPixmap *pix = globalObjManager->getIconList()->getIcon(idx);
            if(pix)
                ui_combo_forNew->addItem(QIcon(*pix), tr("%1").arg(idx),idx);
        }


        editAreaLayout->addWidget(ui_edit_newKey, currrow, 1, 1, 2);
        editAreaLayout->addWidget(ui_combo_forNew, currrow, 3, 1, 2);
        ++currrow;
        connect(ui_edit_newKey, &QLineEdit::editingFinished, this, &VarDisplayUnit::updEditState);

    }

    if(!ui_button_ok)
    {
        ui_button_ok = new QPushButton(tr("OK (ctrl+enter)"),this);
        connect(ui_button_ok, &QPushButton::clicked, this, &VarDisplayUnit::editFinishOK);
        editAreaLayout->addWidget(ui_button_ok, currrow, 3, 1, 1);
    }
    if(!ui_button_cancel)
    {
        ui_button_cancel = new QPushButton(tr("Cancel"),this);
        connect(ui_button_cancel, &QPushButton::clicked, this, &VarDisplayUnit::editFinishCancel);
        editAreaLayout->addWidget(ui_button_cancel, currrow, 4, 1, 1);
    }


    ++currrow;



    this->setHidden(false);
    this->adjustSize();

    ui_edit_varName->setFocus();
    ui_edit_varName->selectAll();

}

void VarDisplayUnit::updEditState()
{
    updEditCaller = dynamic_cast<QWidget*>(QObject::sender());

    if(updEditDelaytimer ==nullptr)
        updEditDelaytimer = new QTimer(this);

    updEditDelaytimer->setSingleShot(true);
    connect(updEditDelaytimer, &QTimer::timeout, this, &VarDisplayUnit::updEditStateDelay);
    updEditDelaytimer->start(10);
}

void VarDisplayUnit::updEditStateDelay()
{
    if(updEditDelaytimer)
    {
        delete updEditDelaytimer;
        updEditDelaytimer = nullptr;
    }


    if(!updEditCaller)
        return;


    if(updEditCaller == ui_combo_type)
    {
        int curridx = ui_combo_type->currentIndex();
        unitSetting.bits.displayType = ui_combo_type->itemData(curridx).toInt();

        //qApp->processEvents();
        deleteEditArea();
        unitSetting.bits.editMode = 0;
        startEditContents();

    }
    else if(updEditCaller == ui_edit_newKey)
    {
        if(!ui_edit_newKey->text().isEmpty())
        {

            if(unitSetting.bits.displayType == UNITDISPLAY_TEXTMAP)
                valueTextMap.insert(std::make_pair(ui_edit_newKey->text().toDouble(),ui_edit_newText->text()));

            if(unitSetting.bits.displayType == UNITDISPLAY_ICONMAP)
                valueIconIdxMap.insert(std::make_pair(ui_edit_newKey->text().toDouble(),ui_combo_forNew->currentData().toInt()));

            deleteEditArea();
            unitSetting.bits.editMode = 0;
            startEditContents();
        }
    }

    updEditCaller = nullptr;

}

void VarDisplayUnit::editFinishOK()
{
    if(edit_edit_map.size() != 0)
        valueTextMap.clear();
    for(auto it = edit_edit_map.begin(); it != edit_edit_map.end(); ++it)
    {
        bool ok;
        double key = it->first->text().toDouble(&ok);
        if(ok)
        {
            valueTextMap.insert(std::make_pair(key, it->second->text()));
        }
    }

    if(edit_combo_map.size() != 0)
        valueIconIdxMap.clear();
    for(auto it = edit_combo_map.begin(); it != edit_combo_map.end(); ++it)
    {
        bool ok;
        double key = it->first->text().toDouble(&ok);
        if(ok)
        {
            valueIconIdxMap.insert(std::make_pair(key, it->second->currentData().toInt()));
        }
    }


    varName = ui_edit_varName->text();
    ui_label_varName->setText(varName);


    valueTextMap_backup.clear();
    valueIconIdxMap_backup.clear();
    needBackup = true;

    deleteEditArea();
    unitSetting.bits.editMode = 0;


    rebuildUnitDisplay();
    ui_button_edit->setText("Edit");
}


void VarDisplayUnit::editFinishCancel()
{
    valueTextMap = valueTextMap_backup;
    valueIconIdxMap = valueIconIdxMap_backup;
    valueTextMap_backup.clear();
    valueIconIdxMap_backup.clear();
    needBackup = true;

    deleteEditArea();
    unitSetting.bits.editMode = 0;
    ui_button_edit->setText("Edit");
}


void VarDisplayUnit::deleteEditArea()
{
    void (QComboBox:: *indexChangedSignal)(int) = &QComboBox::currentIndexChanged;
    this->setHidden(true);
    if(ui_combo_enable)
    {
        disconnect(ui_combo_enable, indexChangedSignal, this, &VarDisplayUnit::updEditState);
        editAreaLayout->removeWidget(ui_combo_enable);
        delete ui_combo_enable;
        ui_combo_enable = nullptr;
    }
    if(ui_combo_type)
    {
        disconnect(ui_combo_type, indexChangedSignal, this, &VarDisplayUnit::updEditState);
        editAreaLayout->removeWidget(ui_combo_type);
        delete ui_combo_type;
        ui_combo_type = nullptr;
    }
    if(ui_combo_readWrite)
    {
        disconnect(ui_combo_readWrite, indexChangedSignal, this, &VarDisplayUnit::updEditState);
        editAreaLayout->removeWidget(ui_combo_readWrite);
        delete ui_combo_readWrite;
        ui_combo_readWrite = nullptr;
    }
    if(ui_edit_varName)
    {
        editAreaLayout->removeWidget(ui_edit_varName);
        delete ui_edit_varName;
        ui_edit_varName = nullptr;
    }





    if(ui_edit_newKey)
    {
        disconnect(ui_edit_newKey, &QLineEdit::editingFinished, this, &VarDisplayUnit::updEditState);
        editAreaLayout->removeWidget(ui_edit_newKey);
        delete ui_edit_newKey;
        ui_edit_newKey = nullptr;
    }
    if(ui_edit_newText)
    {
        editAreaLayout->removeWidget(ui_edit_newText);
        delete ui_edit_newText;
        ui_edit_newText = nullptr;
    }
    if(ui_combo_forNew)
    {
        editAreaLayout->removeWidget(ui_combo_forNew);
        delete ui_combo_forNew;
        ui_combo_forNew = nullptr;
    }




    for(auto it = labelList.begin(); it != labelList.end(); ++it)
    {
        editAreaLayout->removeWidget(*it);
        delete *it;
    }
    labelList.clear();

    for(auto it = edit_combo_map.begin(); it != edit_combo_map.end(); ++it)
    {
        editAreaLayout->removeWidget(it->first);
        editAreaLayout->removeWidget(it->second);
        delete it->first;
        delete it->second;
    }
    edit_combo_map.clear();
    for(auto it = edit_edit_map.begin(); it != edit_edit_map.end(); ++it)
    {
        editAreaLayout->removeWidget(it->first);
        editAreaLayout->removeWidget(it->second);
        delete it->first;
        delete it->second;
    }
    edit_edit_map.clear();



    //	editAreaLayout->removeWidget(ui_button_edit);
    if(ui_button_ok)
    {
        editAreaLayout->removeWidget(ui_button_ok);
        delete ui_button_ok;
        ui_button_ok = nullptr;
    }
    if(ui_button_cancel)
    {
        editAreaLayout->removeWidget(ui_button_cancel);
        delete ui_button_cancel;
        ui_button_cancel = nullptr;
    }


    this->setHidden(false);
}

void VarDisplayUnit::rebuildUnitDisplay()
{
    this->setHidden(true);

    if(ui_edit_varValue)
    {
        displayLayout->removeWidget(ui_edit_varValue);
        delete ui_edit_varValue;
        ui_edit_varValue = nullptr;
    }

    if(ui_label_icon)
    {
        displayLayout->removeWidget(ui_label_icon);
        delete ui_label_icon;
        ui_label_icon = nullptr;
    }

    if(ui_combo_textMap)
    {
        displayLayout->removeWidget(ui_combo_textMap);
        delete ui_combo_textMap;
        ui_combo_textMap = nullptr;
    }


    if(unitSetting.bits.displayType == UNITDISPLAY_DIRECT)
    {
        ui_edit_varValue = new QLineEdit(this);
        displayLayout->insertWidget(1, ui_edit_varValue);
        connect(ui_edit_varValue, &QLineEdit::textEdited, this, &VarDisplayUnit::editingVarValue);
        connect(ui_edit_varValue, &QLineEdit::returnPressed, this, &VarDisplayUnit::editVarValueFinish);
        connect(ui_edit_varValue, &QLineEdit::selectionChanged, this, &VarDisplayUnit::varValueSelectChange);
        ui_edit_varValue->setReadOnly(unitSetting.bits.isReadOnly);
    }
    else if(unitSetting.bits.displayType == UNITDISPLAY_TEXTMAP)
    {
        ui_combo_textMap = new QComboBox(this);
        displayLayout->insertWidget(1, ui_combo_textMap);

        for(auto it = valueTextMap.begin(); it != valueTextMap.end(); ++it)
        {
            ui_combo_textMap->addItem(it->second);
        }

        ui_combo_textMap->setEditable(unitSetting.bits.isReadOnly);
    }
    else if(unitSetting.bits.displayType == UNITDISPLAY_ICONMAP)
    {
        ui_label_icon = new QLabel(this);
        ui_label_icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        displayLayout->insertWidget(1, ui_label_icon);
    }

    if(!ui_button_edit)
    {
        ui_button_edit = new QPushButton(tr("Edit"),this);
        connect(ui_button_edit, &QPushButton::clicked, this, &VarDisplayUnit::startEditContents);
        displayLayout->addWidget(ui_button_edit);
    }
#ifdef _DEBUG
    qDebug() << "this width height" << this->geometry().width() << this->geometry().height();
    qDebug() << "ui_button_edit width height" << ui_button_edit->width() << ui_button_edit->height();
#endif


    this->setHidden(false);
    this->adjustSize();
    //displayLayout

}




//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

DisplayUnitsPlane::DisplayUnitsPlane(QWidget* parent, CommandManager *cmd)
    : QWidget(parent),
    commandManager(cmd)
{
    mainLayout = new QVBoxLayout(this);
    unitLayout = new QVBoxLayout(this);
    spaceLayout = new QGridLayout(this);
    unitCount = 0;




    mainLayout->addLayout(unitLayout);
    mainLayout->addLayout(spaceLayout);

    QSpacerItem *space = new QSpacerItem(0, 2048, QSizePolicy::Expanding, QSizePolicy::Maximum);//2048 is just a large number, that more than most lcd screen
    ui_button_newUnit = new QPushButton(this);
    ui_button_removeUnit = new QPushButton(this);
    
    spaceLayout->addWidget(ui_button_newUnit, 0, 0);
    spaceLayout->addWidget(ui_button_removeUnit, 0, 1);
    spaceLayout->addItem(space,1,0,1,2);
    this->setLayout(mainLayout);

    connect(ui_button_newUnit, &QPushButton::clicked, this, &DisplayUnitsPlane::btn_addNewUnit);
    connect(ui_button_removeUnit, &QPushButton::clicked, this, &DisplayUnitsPlane::btn_removeUnit);

    setTextNeedTrans();
}



DisplayUnitsPlane::~DisplayUnitsPlane()
{
    btn_removeAllUnit();
}

void DisplayUnitsPlane::addDisplayUnit(VarDisplayUnit *unit)
{
    //	void *test1 = unitLayout->takeAt(unitCount);
    //	QSpacerItem *test = dynamic_cast<QSpacerItem *>(mainLayout->takeAt(unitCount));
    //	if(test)
    //		delete test;
    this->setHidden(true);
    unitLayout->addWidget(unit);
    displayUnitList.push_back(unit);
    //	QSpacerItem *space = new QSpacerItem(0, this->geometry().height());
    //	mainLayout->addItem(space);
    this->setHidden(false);
    ++unitCount;
}



void DisplayUnitsPlane::setTextNeedTrans()
{
    ui_button_newUnit->setText(tr("Add unit"));
    ui_button_removeUnit->setText(tr("Remove unit"));

    for(auto it = displayUnitList.begin(); it != displayUnitList.end(); ++it)
    {
        (*it)->setTextNeedTrans();
    }


}


std::list<VarDisplayUnit*> DisplayUnitsPlane::getDisplayUnitList() const
{
    return displayUnitList;
}


#ifdef _DEBUG
void DisplayUnitsPlane::paintEvent(QPaintEvent *paintevent)
{
    //	QPainter painter(this);
    //	painter.setPen(QPen(Qt::blue,5));

    //	painter.drawRect(QRect(0, 0, 100, 30));
    QRect test = this->geometry();
}
#endif



void DisplayUnitsPlane::btn_addNewUnit()
{
    DisplayUnitSetting setting;
    std::map<double, QString> textmap;
    std::map<double, int> iconmap;
    setting.all = 0;


    VarDisplayUnit *unit = new VarDisplayUnit(this, commandManager, QString("default"), setting, textmap, iconmap);

    globalObjManager->getVarMonitorManager()->addLowPriorityUnit(unit);
    this->addDisplayUnit(unit);
}

void DisplayUnitsPlane::btn_removeUnit()
{
    VarDisplayUnit *unit = displayUnitList.back();
    displayUnitList.pop_back();

    unitLayout->removeWidget(unit);

    globalObjManager->getVarMonitorManager()->removeDisplayUnit(unit);
    delete unit;
}



void DisplayUnitsPlane::btn_removeAllUnit()
{
    int unitcnt = displayUnitList.size();

    VarDisplayUnit *unit;
    for(int i = 0; i < unitcnt; ++i)
    {
        unit = displayUnitList.back();
        displayUnitList.pop_back();

        unitLayout->removeWidget(unit);

        if(globalObjManager)
        {
            VarMonitorManager *varmonitor = globalObjManager->getVarMonitorManager();
            if(varmonitor)
                varmonitor->removeDisplayUnit(unit);
        }
        delete unit;
    }
}


//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------


VarMonitorManager::VarMonitorManager()
{
    //	threadHandle = NULL;
    //	highListCopyEvent = NULL;
    timerEndEvent = NULL;
    timerRunFlag = true;
    highListChanged = false;
    lowListChanged = false;
    updateLowPriorty = true;

    std::list<VarDisplayUnit*>::iterator it_high = this->highPriorityUnitList.begin();
    std::list<VarDisplayUnit*>::iterator it_low = this->lowPriorityUnitList.begin();

    if(timerEndEvent == NULL)
        timerEndEvent = CreateEvent(NULL,false,false,NULL);
    //	if(lowListCopyEvent == NULL)
    //		lowListCopyEvent = CreateEvent(NULL,false,false,NULL);

    //	SetEvent(highListCopyEvent);
    //	SetEvent(lowListCopyEvent);


    routineTimer = new QTimer;
    connect(routineTimer, &QTimer::timeout, this, &VarMonitorManager::VarMonitorRoutine);
    routineTimer->start(50);
}

VarMonitorManager::~VarMonitorManager()
{
#ifdef _DEBUG
    qDebug() << "start ~VarMonitorManager()";
#endif
    timerRunFlag = false;
    routineTimer->stop();

    delete routineTimer;

    WaitForSingleObject(timerEndEvent, INFINITE );
    //	if(threadHandle)
    //	{
    //		WaitForSingleObject(timerEndEvent, INFINITE );
    //		CloseHandle(threadHandle);
    //		threadHandle = NULL;
    //	}
#ifdef _DEBUG
    qDebug() << "finish ~VarMonitorManager()";
#endif
}


void VarMonitorManager::addHighPriorityUnit(VarDisplayUnit *unit)
{
    //	if(timerRunFlag)
    //		WaitForSingleObject(highListCopyEvent,INFINITE);

    //	if(highListChanged)
    //	{
    //#ifdef _DEBUG
    //			QMessageBox::warning(nullptr,QString("error"),QString("VarMonitorThread timming error"));
    //#endif
    //			return;
    //	}




    for(auto it = highPriorityUnitList.begin(); it != highPriorityUnitList.end(); ++it)
    {
        if(*it == unit)
        {
#ifdef _DEBUG
            QMessageBox::warning(nullptr,QString("warning"),QString("VarDisplayUnit already exist"));
#endif
            return;
        }
    }
    if(highListChanged)
    {
        highPriorityList_buff.push_back(unit);
    }
    if(timerRunFlag)
    {
        highPriorityList_buff.clear();
        highPriorityList_buff = highPriorityUnitList;
        highPriorityList_buff.push_back(unit);

        highListChanged = true;
    }
    else
    {
        highPriorityUnitList.push_back(unit);
    }

}
void VarMonitorManager::addLowPriorityUnit(VarDisplayUnit *unit)
{
    //	if(timerRunFlag && updateLowPriorty)
    //		WaitForSingleObject(lowListCopyEvent,INFINITE);
    //	if(lowListChanged)
    //	{
    //#ifdef _DEBUG
    //			QMessageBox::warning(nullptr,QString("error"),QString("VarMonitorThread timming error"));
    //#endif
    //			return;
    //	}

    for(auto it = lowPriorityUnitList.begin(); it != lowPriorityUnitList.end(); ++it)
    {
        if(*it == unit)
        {
#ifdef _DEBUG
            QMessageBox::warning(nullptr,QString("warning"),QString("VarDisplayUnit already exist"));
#endif
            return;
        }
    }
    if(lowListChanged)
    {
        lowPriorityList_buff.push_back(unit);
    }
    else if(timerRunFlag && updateLowPriorty)
    {
        lowPriorityList_buff.clear();
        lowPriorityList_buff = lowPriorityUnitList;
        lowPriorityList_buff.push_back(unit);

        lowListChanged = true;
    }
    else
    {
        lowPriorityUnitList.push_back(unit);
    }
}

void VarMonitorManager::removeDisplayUnit(VarDisplayUnit *unit)
{
    for(auto it = highPriorityUnitList.begin(); it != highPriorityUnitList.end(); ++it)
    {
        if(*it == unit)
        {
            highPriorityUnitList.erase(it);
            break;
        }
    }
    for(auto it = lowPriorityUnitList.begin(); it != lowPriorityUnitList.end(); ++it)
    {
        if(*it == unit)
        {
            lowPriorityUnitList.erase(it);
            break;
        }
    }
}

int VarMonitorManager::startVarMonitorThread()
{
    if(!timerRunFlag)
        return 0;

    timerRunFlag = true;
    routineTimer->start(50);

    return 0;
}



/*
void VarMonitorManager::startVarMonitorThread()
{
if(threadRunFlag)
return;
if(highListCopyEvent == NULL)
highListCopyEvent = CreateEvent(NULL,false,false,NULL);
if(lowListCopyEvent == NULL)
lowListCopyEvent = CreateEvent(NULL,false,false,NULL);


if(threadHandle == NULL)
threadHandle = (HANDLE)_beginthreadex(NULL,0,  (unsigned (_stdcall *)(void *))VarMonitorThread, (LPVOID)this,CREATE_SUSPENDED,NULL); 

if(threadHandle)
{
SetEvent(highListCopyEvent);
SetEvent(lowListCopyEvent);
threadRunFlag = true;
ResumeThread(threadHandle);
}
else
{
QMessageBox::warning(nullptr,QString("error"), QString("can not start VarMonitorThread"));
}
}
*/
void VarMonitorManager::stopVarMonitorThread()
{
    timerRunFlag = false;
    routineTimer->stop();
    //	WaitForSingleObject( threadHandle, INFINITE );
    //	CloseHandle(threadHandle);
    //	threadHandle = NULL;
}


void VarMonitorManager::setUpdateLowPriorty(bool yesno)
{
    if(updateLowPriorty == yesno)
        return;

    if(updateLowPriorty)
    {


    }
    else
    {



    }


    updateLowPriorty = yesno;
}


void VarMonitorManager::VarMonitorRoutine()
{
    //	if(threadRunFlag)
    //		return;


    //	int sleeptime = 50;
    //	VarMonitorManager *owner = (VarMonitorManager *)(ownerobject);

    //	if(owner == nullptr)
    //		return 0;
    ResetEvent(timerEndEvent);

    //	this->threadRunFlag = 1;
    if(this->timerRunFlag)
    {
        if(this->highListChanged)
        {
            //-------------------not necessary. but for security----------------------
            //			ResetEvent(this->highListCopyEvent);
            //-------------------not necessary. but for security----------------------
            this->highPriorityUnitList = this->highPriorityList_buff;
            this->highListChanged = false;
            //			SetEvent(this->highListCopyEvent);
        }

        for(it_high = this->highPriorityUnitList.begin(); it_high != this->highPriorityUnitList.end(); ++it_high)
        {
            (*it_high)->updateVar();
        }

        if(this->updateLowPriorty)
        {
            if(this->lowListChanged)
            {
                //-------------------not necessary. but for security----------------------
                //				ResetEvent(this->lowListCopyEvent);
                //-------------------not necessary. but for security----------------------
                this->lowPriorityUnitList = this->lowPriorityList_buff;
                this->lowListChanged = false;
                //				SetEvent(this->lowListCopyEvent);
            }
            for(it_low = this->lowPriorityUnitList.begin(); it_low != this->lowPriorityUnitList.end(); ++it_low)
            {
                (*it_low)->updateVar();
            }

        }
    }

    SetEvent(timerEndEvent);
}
//---------------------------------
//---------------------------------
//---------------------------------
//---------------------------------
//---------------------------------
//---------------------------------

/*

unsigned int WINAPI VarMonitorThread(void *ownerobject)
{
int sleeptime = 50;
VarMonitorManager *owner = (VarMonitorManager *)(ownerobject);

if(owner == nullptr)
return 0;

std::list<VarDisplayUnit*>::iterator it_high = owner->highPriorityUnitList.begin();
std::list<VarDisplayUnit*>::iterator it_low = owner->lowPriorityUnitList.begin();
while(owner->threadRunFlag)
{
if(owner->highListChanged)
{
//-------------------not necessary. but for security----------------------
ResetEvent(owner->highListCopyEvent);
//-------------------not necessary. but for security----------------------
owner->highPriorityUnitList = owner->highPriorityList_buff;
owner->highListChanged = false;
SetEvent(owner->highListCopyEvent);
}

for(it_high = owner->highPriorityUnitList.begin(); it_high != owner->highPriorityUnitList.end(); ++it_high)
{
(*it_high)->updateVar();
}

if(owner->updateLowPriorty)
{
if(owner->lowListChanged)
{
//-------------------not necessary. but for security----------------------
ResetEvent(owner->lowListCopyEvent);
//-------------------not necessary. but for security----------------------
owner->lowPriorityUnitList = owner->lowPriorityList_buff;
owner->lowListChanged = false;
SetEvent(owner->lowListCopyEvent);
}
for(it_low = owner->lowPriorityUnitList.begin(); it_low != owner->lowPriorityUnitList.end(); ++it_low)
{
(*it_low)->updateVar();
}

}

sleeptime = 100 - owner->lowPriorityUnitList.size() - owner->highPriorityUnitList.size();
sleeptime = (sleeptime > 0) ? (sleeptime >> 1) : 0;
Sleep(sleeptime);//ui_edit_varValue->setText() can not call too high frequancy in another thread. 
}
return 0;
}
*/