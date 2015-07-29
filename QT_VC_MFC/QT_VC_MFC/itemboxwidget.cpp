#include "itemboxwidget.h"
#include "globalobjmanager.h"

extern GlobalObjManager *globalObjManager;



ItemBoxMimeData::ItemBoxMimeData()
{
    mimeAssem = NULL;
    flochaType = -1;
}

ItemBoxMimeData::~ItemBoxMimeData()
{

}

FlowchartsAssemble *ItemBoxMimeData::getFlochaAssemPtr() const
{
    return mimeAssem;
}


int ItemBoxMimeData::getNewFlochaType() const
{
    return flochaType;
}

void ItemBoxMimeData::setFlochaAssemPtr(FlowchartsAssemble *assem)
{
    mimeAssem = assem;
}

void ItemBoxMimeData::setNewFlochaType(int type)
{
    flochaType = type;
}

//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------


ItemListWidget::ItemListWidget(QWidget *parent)
    :QListWidget(parent)
{

    this->setDragEnabled(true);

}


ItemListWidget::~ItemListWidget()
{

}

void ItemListWidget::addFlochaItem(QString itemtext, FlowchartsAssemble *assem, int basictype)
{
    if((!assem) && (basictype == -1))
    {
#ifdef _DEBUG
        QMessageBox::warning(this, QString("error"), QString("assem is null && basictype is invalid"));
#endif
        return;
    }

    QPushButton iconbtn;

    if(assem)
    {
        iconbtn.setText(QString("%1\n%2\n").arg(itemtext).arg("Assem"));
    }
    else
    {
        iconbtn.setText(QString("\n%1\n").arg(itemtext));
    }

    QIcon icon(iconbtn.grab());

    QListWidgetItem *item = new QListWidgetItem(icon, itemtext, this);


    this->addItem(item);

    listitem_mimedata_map.insert(std::make_pair(item, std::make_pair(assem, basictype)));

}


void ItemListWidget::mousePressEvent(QMouseEvent *event)
{
    mouseStartP = event->pos();
    mouseP = mouseStartP;
}

void ItemListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        int distance = (event->pos() - mouseP).manhattanLength();

        if (distance >= 5)//QApplication::startDragDistance())
        {
            performDrag();
            mouseP = event->pos();

        }
    }
}


void ItemListWidget::performDrag()
{
    QListWidgetItem *curritem = this->itemAt(mouseP);

    if(!curritem)
        return;

    auto it = listitem_mimedata_map.find(curritem);
    if(it == listitem_mimedata_map.end())
        return;

    FlowchartsAssemble *assem = it->second.first;
    int type = it->second.second;

    if((!assem) && (type == -1))
        return;

    ItemBoxMimeData *mimeData = new ItemBoxMimeData;

    if(assem)
        mimeData->setFlochaAssemPtr(assem);
    else
        mimeData->setNewFlochaType(type);


    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(curritem->icon().pixmap(QSize(64,64)));
    if (drag->exec(Qt::CopyAction | Qt::MoveAction) == Qt::MoveAction) {
        selectionModel()->clearSelection();
    }

}


//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------



ItemBoxWidget::ItemBoxWidget(QWidget *parent)
    : QWidget(parent)
{
    btn_ListWidget_map.clear();
    expandIcon = NULL;
    collapseIcon = NULL;


    createBtnIcon();


    mainLayout = new QVBoxLayout(this);

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    this->setLayout(mainLayout);


    createBox_basic();
    createBox_tool();
    createBox_driveFunc();
    createBox_userDefine();
    createBox_test();




}

ItemBoxWidget::~ItemBoxWidget()
{


    for(auto it = btn_ListWidget_map.begin(); it != btn_ListWidget_map.end(); ++it)
    {
        delete it->first;
        delete it->second;
    }

    if(expandIcon)
        delete expandIcon;

    if(collapseIcon)
        delete collapseIcon;

    delete mainLayout;

}


void ItemBoxWidget::setTextNeedTrans()
{

}

/*
bool ItemBoxWidget::eventFilter(QObject* watched, QEvent* event)
{
auto eventtype = event->type();
qDebug() << eventtype;
if(event->type()==QEvent::MouseButtonPress) {

QListWidget *listwidget = dynamic_cast<QListWidget*>(watched);

if(listwidget)
{
QListWidgetItem *curritem = listwidget->currentItem();



}



}
QListWidget *listwidget = dynamic_cast<QListWidget*>(watched);

if(listwidget)
{
QListWidgetItem *curritem = listwidget->currentItem();

if(curritem)
{
int test = 0;
}

}
return QWidget::eventFilter(watched, event);
}
*/

void ItemBoxWidget::createBox_basic()
{

    ItemListWidget *listwidget = createItemListWidget(tr("Basic item"));


    listwidget->addFlochaItem(tr("IF"), NULL, flochaTypeIf);
    listwidget->addFlochaItem(tr("RUN"), NULL, flochaTypeRun);
    listwidget->addFlochaItem(tr("SET"), NULL, flochaTypeSet);
    listwidget->addFlochaItem(tr("SLEEP"), NULL, flochaTypeSleep);
    listwidget->addFlochaItem(tr("ASSEM"), NULL, flochaTypeAssem);

}


void ItemBoxWidget::createBox_tool()
{



}


void ItemBoxWidget::createBox_driveFunc()
{




}



void ItemBoxWidget::createBox_userDefine()
{

    ItemListWidget *listwidget = createItemListWidget(tr("User define"));
    std::list<FlowchartsAssemble*> itemlist;

    itemlist.clear();


    globalObjManager->getItemBoxItem(itemlist);


    for(auto it = itemlist.begin(); it != itemlist.end(); ++it)
    {
        listwidget->addFlochaItem((*it)->idName, *it);
    }

}


void ItemBoxWidget::createBox_test()
{

    ItemListWidget *listwidget = createItemListWidget(tr("Test"));


    FlowchartsAssemble *assem = new FlowchartsAssemble(NULL, nullptr);


    FlowchartsTemplate* flo1 = new FlowchartsIf(assem, nullptr);
    FlowchartsTemplate* flo2 = new FlowchartsSetVar(assem, nullptr);
    FlowchartsTemplate* flo3 = new FlowchartsSetVar(assem, nullptr);

    flo1->itemPos[0] = 100;
    flo1->itemPos[1] = 100;

    flo2->itemPos[0] = 50;
    flo2->itemPos[1] = 200;

    flo3->itemPos[0] = 150;
    flo3->itemPos[1] = 300;


    assem->addTemplate(flo1);
    assem->addTemplate(flo2);
    assem->addTemplate(flo3);

    assem->linkTwoTemplate(flo1, flo2);
    assem->linkTwoTemplate(flo2, flo3);




    listwidget->addFlochaItem(tr("Test"), assem);

}

void ItemBoxWidget::createBtnIcon()
{
    if(expandIcon == NULL)
    {
        int iconsize = EXPAN_COLAP_ICONSIZE;
        expandIcon = new QPixmap(iconsize,iconsize);
        expandIcon->fill(Qt::transparent);
        QPainter painter(expandIcon);

        painter.setPen(QPen(Qt::black,2));

        QPoint expantri[3];

        expantri[0] = QPoint(iconsize * 0.5, iconsize * 0.75);
        expantri[1] = QPoint(iconsize * 0.25, iconsize * 0.25);
        expantri[2] = QPoint(iconsize * 0.75, iconsize * 0.25);


        painter.drawPolygon(expantri,3);
    }

    if(collapseIcon == NULL)
    {
        int iconsize = EXPAN_COLAP_ICONSIZE;
        collapseIcon = new QPixmap(iconsize,iconsize);
        collapseIcon->fill(Qt::transparent);
        QPainter painter(collapseIcon);

        painter.setPen(QPen(Qt::black,2));
        painter.setBrush(QBrush(Qt::black));
        QPoint expantri[3];

        expantri[0] = QPoint(iconsize * 0.75, iconsize * 0.5);
        expantri[1] = QPoint(iconsize * 0.25, iconsize * 0.25);
        expantri[2] = QPoint(iconsize * 0.25, iconsize * 0.75);


        painter.drawPolygon(expantri,3);



    }

}


void ItemBoxWidget::expand_collapse_list()
{
    QPushButton *sender = dynamic_cast<QPushButton*>(QObject::sender());
    auto it = btn_ListWidget_map.find(sender);
    if(it == btn_ListWidget_map.end())
        return;


    QListWidget *listwidget = it->second;

    if(listwidget->isHidden())
    {
        listwidget->setHidden(false);
        sender->setIcon(QIcon(*expandIcon));
    }
    else
    {
        listwidget->setHidden(true);
        sender->setIcon(QIcon(*collapseIcon));
    }

}


ItemListWidget *ItemBoxWidget::createItemListWidget(const QString &category)
{

    ItemListWidget *listwidget = new ItemListWidget(this);
    QPushButton *btn = new QPushButton(this);


    btn_ListWidget_map.insert(std::make_pair(btn, listwidget));
    connect(btn, &QPushButton::clicked, this, &ItemBoxWidget::expand_collapse_list);
    btn->setText(category);
    btn->setIcon(QIcon(*expandIcon));
    //	btn->setStyleSheet("background-position:left;");
    mainLayout->addWidget(btn);
    mainLayout->addWidget(listwidget);


    return listwidget;
}
