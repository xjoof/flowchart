#include "flowchartwidget.h"
#include "xmlflochatransfer.h"
#include "datainputdlg.h"


FlowchartWidget::FlowchartWidget(QWidget *parent,FlowchartsAssemble *inputFlocha)
    : QWidget(parent)
{
    button_flocha_map.clear();
    scrollOffsetPos = QPoint(0,0);

    selectAreaDrag = false;
    btnDragging = false;
    pushToUndoWhenRedo = false;

    commandManager = nullptr;
    selectedBtnList.clear();
    breakBtnList.clear();
    SuspendBtn = nullptr;

    flochaNeedPopMenu = nullptr;


    ui_act_editAssemSetting = nullptr;
    ui_act_openAssemInTab = nullptr;
    ui_act_putAssemToItemBox = nullptr;
    ui_act_dispelAssem = nullptr;


    pressedCtrlKey = false;

    tempPos = QPoint(50,50);
    tmpCnt = 0;

    operationMode = editItemMode;

    men_preButton = nullptr;
    if(inputFlocha == nullptr)
    {
        QMessageBox::warning(this,tr("error"),tr("inputFlocha == nullptr"));
        return;
    }
    else
    {
        flochaAssem = inputFlocha;
        connect(flochaAssem,&FlowchartsAssemble::hitBreakPointSignal,this,&FlowchartWidget::hitBreakPointSignal);
        connect(flochaAssem,&FlowchartsAssemble::runFinishedSignal,this,&FlowchartWidget::runFinishedSignal);
    }


    this->setAcceptDrops(true);









    /*
    button_addIf = new QPushButton(this);
    button_addSet = new QPushButton(this);
    button_addRun = new QPushButton(this);
    button_addSleep = new QPushButton(this);
    button_startRun = new QPushButton(this);
    button_saveFlocha = new QPushButton(this);

    button_addSet->move(100,0);
    button_addRun->move(200,0);
    button_addSleep->move(300,0);
    button_startRun->move(400,0);
    button_saveFlocha->move(500,0);
    button_addIf->setText(tr("if"));
    button_addSet->setText(tr("set"));
    button_addRun->setText(tr("run"));
    button_addSleep->setText(tr("sleep"));
    button_startRun->setText(tr("start"));
    button_saveFlocha->setText(tr("save"));

    connect(button_addIf,&QPushButton::clicked,this,&FlowchartWidget::addItemIf);
    connect(button_addSet,&QPushButton::clicked,this,&FlowchartWidget::addItemSet);
    connect(button_addRun,&QPushButton::clicked,this,&FlowchartWidget::addItemRun);
    connect(button_addSleep,&QPushButton::clicked,this,&FlowchartWidget::addItemSleep);
    connect(button_startRun,&QPushButton::clicked,flochaAssem,&FlowchartsAssemble::startTemplate);
    connect(button_saveFlocha,&QPushButton::clicked,this,&FlowchartWidget::saveFlocha);
    */
    /*
    startBtn = new QPushButton(this);
    endBtn  = new QPushButton(this);
    startBtn->move(200,50);
    endBtn->move(200,300);
    flochaAssem->getStartFlocha()->itemPos[0] = 200;
    flochaAssem->getStartFlocha()->itemPos[1] = 50;
    flochaAssem->getEndFlocha()->itemPos[0] = 200;
    flochaAssem->getEndFlocha()->itemPos[0] = 300;

    startBtn->setText(tr("start"));
    endBtn->setText(tr("end"));

    startBtn->installEventFilter(this);
    endBtn->installEventFilter(this);

    button_flocha_map.insert(std::make_pair(startBtn, flochaAssem->getStartFlocha()));
    button_flocha_map.insert(std::make_pair(endBtn, flochaAssem->getEndFlocha()));


    buttonList.push_back(startBtn);
    buttonList.push_back(endBtn);
    */






    //constructFromAssem(flochaAssem);
    constructFromAssemLoop(flochaAssem);
    linkButtonFromAssem();

}

FlowchartWidget::~FlowchartWidget()
{
    if(ui_act_editAssemSetting)
        delete ui_act_editAssemSetting;
    if(ui_act_openAssemInTab)
        delete ui_act_openAssemInTab;
    if(ui_act_putAssemToItemBox)
        delete ui_act_putAssemToItemBox;
    if(ui_act_dispelAssem)
        delete ui_act_dispelAssem;



    for(auto it = button_flocha_map.begin(); it != button_flocha_map.end(); ++it)
    {
        if(it->first != nullptr)
        {
            delete it->first;
            //it->first = nullptr;
        }
        if(it->second!=0)
        {

            //flocha should not delete when widget destruct
            //			delete it->second;
            //			it->second = nullptr;
        }
    }
    /*
    for(auto it = buttonLinkMap.begin(); it != buttonLinkMap.end(); ++it)
    {
    if(it->first!=0)
    delete it->first;
    if(it->second!=0)
    delete it->second;
    }
    */
    button_flocha_map.clear();
    buttonLinkMap.clear();
}



void FlowchartWidget::setFlochaCmdManager(CommandManager *cmd)
{
    //	flochaAssem->setMpiController(cmd);
    commandManager = cmd;
}



void FlowchartWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    bool paintprebtn = false;

    painter.save();
    painter.setPen(QPen(Qt::black,2));


    if(selectAreaDrag == true)
    {
        QRect selectarea(mouseStartPos, mousePos);
        QLinearGradient gradient(selectarea.topLeft(), selectarea.bottomRight());
        gradient.setColorAt(0, QColor(245,250,255));
        gradient.setColorAt(0.5, QColor(230,235,255));
        gradient.setColorAt(1, QColor(245,250,255));
        QBrush brush(gradient);
        painter.setBrush(brush);
        painter.setPen(QPen(QColor(230,230,255),2));
        painter.drawRect(QRect(mouseStartPos, mousePos));
    }



    drawSelectedBtn(&painter);



    for(auto it = buttonLinkMap.begin(); it != buttonLinkMap.end(); ++it)
    {


        QRect rect1 = it->first->geometry();

#ifdef _DEBUG
        //	auto debug_p = button_flocha_map.find(it->first)->second;
        //	qDebug() << "idname" << debug_p->idName;
#endif

        if(it->first == men_preButton)
        {
            if(button_flocha_map.find(it->first)->second->getflochaType() != flochaTypeIf)
                drawLinkLine(&painter,rect1,QRect(mousePos,mousePos));
            else
            {
                if((it->second.btnPtrIftrue != 0) && btnLinkElse)
                    drawLinkLine(&painter,rect1,it->second.btnPtrIftrue->geometry(),linkTypeIfTrue);

                if((it->second.btnPtrElse != 0) && (!btnLinkElse))
                    drawLinkLine(&painter,rect1,it->second.btnPtrElse->geometry(),linkTypeElse);


                if(btnLinkElse)
                    drawLinkLine(&painter,rect1,QRect(mousePos,mousePos), linkTypeElse);
                else
                    drawLinkLine(&painter,rect1,QRect(mousePos,mousePos), linkTypeIfTrue);
            }
            paintprebtn = true;
        }
        else
        {

            if(it->second.btnPtr != 0)
                drawLinkLine(&painter,rect1,it->second.btnPtr->geometry());

            if(it->second.btnPtrIftrue != 0)
                drawLinkLine(&painter,rect1,it->second.btnPtrIftrue->geometry(),linkTypeIfTrue);

            if(it->second.btnPtrElse != 0)
                drawLinkLine(&painter,rect1,it->second.btnPtrElse->geometry(),linkTypeElse);

        }

        /*
        QPoint line1P1 = rect1.topLeft() * 0.5 + rect1.bottomRight() * 0.5;
        QPoint line2P2 = rect2.topLeft() * 0.5 + rect2.bottomRight() * 0.5;

        QPoint line1P2 = QPoint(line1P1.x(),line2P2.y());
        QPoint line2P1 = line1P2;


        QPoint arrow1;
        QPoint arrow2_1;
        QPoint arrow2_2;

        if(line1P2.x() > line2P2.x())
        {
        arrow1 = QPoint(rect2.right(),line1P2.y());
        arrow2_1 = arrow1 + QPoint(10,5);
        arrow2_2 = arrow1 + QPoint(10,-5);
        }
        else
        {
        arrow1 = QPoint(rect2.left(),line1P2.y());
        arrow2_1 = arrow1 - QPoint(10,5);
        arrow2_2 = arrow1 - QPoint(10,-5);
        }

        painter.drawLine(arrow1,arrow2_1);
        painter.drawLine(arrow1,arrow2_2);
        painter.drawLine(line1P1,line1P2);
        painter.drawLine(line2P1,line2P2);
        */
    }





    if((men_preButton != nullptr) && !paintprebtn)
    {
        QRect rect = men_preButton->geometry();
        if(button_flocha_map.find(men_preButton)->second->getflochaType() != flochaTypeIf)
            drawLinkLine(&painter,rect,QRect(mousePos,mousePos));
        else
        {
            if(btnLinkElse)
                drawLinkLine(&painter,rect,QRect(mousePos,mousePos), linkTypeElse);
            else
                drawLinkLine(&painter,rect,QRect(mousePos,mousePos), linkTypeIfTrue);
        }
    }



    painter.restore();

}




void FlowchartWidget::drawLinkLine(QPainter *painter,QRect startR,QRect endR,int type)
{
    painter->save();
    QPoint startP = startR.topLeft() * 0.5 + startR.bottomRight() * 0.5;
    QPoint endP = endR.topLeft() * 0.5 + endR.bottomRight() * 0.5;

    int arrowL = 5;
    int arrowW = 3;

    if(type == linkTypeIfTrue)
    {
        painter->setPen(QPen(QColor(50,255,50,150),3));
        painter->setFont(QFont("arial",15,99));
        startP = startP + QPoint(startR.width() * 0.25, 0);
        painter->drawText(QPoint(startP.x() + 5, startR.bottom() + 15), tr("Yes"));
    }
    else if(type == linkTypeElse)
    {
        painter->setPen(QPen(QColor(255,50,255,150),3));
        painter->setFont(QFont("arial",15,99));
        startP = startP - QPoint(startR.width() * 0.25, 0);
        painter->drawText(QPoint(startP.x() - 30, startR.bottom() + 15), tr("No"));
    }
    else
    {
        painter->setPen(QPen(QColor(50,50,255,150),3));
    }

    QPoint middle1;
    QPoint middle2;
    QPoint middle3;


    QPoint arrow1;
    QPoint arrow2_1;
    QPoint arrow2_2;

    if(startP.y() > endP.y() - endR.height()*0.5)
    {
        middle1 = startP + QPoint(0, endR.height());

        if(startP.x() < endP.x())
        {
            middle2 = QPoint(endR.right() + endR.width(), middle1.y());
            middle3 = QPoint(endR.right() + endR.width(), endP.y());

            arrow1 = QPoint(endR.right(),endP.y());
            arrow2_1 = arrow1 + QPoint(arrowL,arrowW);
            arrow2_2 = arrow1 + QPoint(arrowL,-arrowW);
        }
        else
        {
            middle2 = QPoint(endR.left() - endR.width(), middle1.y());
            middle3 = QPoint(endR.left() - endR.width(), endP.y());

            arrow1 = QPoint(endR.left(),endP.y());
            arrow2_1 = arrow1 - QPoint(arrowL,arrowW);
            arrow2_2 = arrow1 - QPoint(arrowL,-arrowW);
        }
    }
    else
    {
        middle1 = QPoint(startP.x(),endP.y());
        middle2 = middle1;
        middle3 = middle1;

        if(startP.x() > (endP.x() + startR.width() * 0.5 + endR.width() * 0.5))
        {
            arrow1 = QPoint(endR.right(),endP.y());
            arrow2_1 = arrow1 + QPoint(arrowL,arrowW);
            arrow2_2 = arrow1 + QPoint(arrowL,-arrowW);
        }
        else if(startP.x() < (endP.x() - startR.width() * 0.5 - endR.width() * 0.5))
        {
            arrow1 = QPoint(endR.left(),endP.y());
            arrow2_1 = arrow1 - QPoint(arrowL,arrowW);
            arrow2_2 = arrow1 - QPoint(arrowL,-arrowW);
        }
        else
        {
            middle1 = QPoint(startP.x(),(startP.y() + startR.height()*0.5 + endP.y() - endR.height()*0.5)/2);
            middle2 = QPoint(endP.x(), middle1.y());
            middle3 = middle2;

            arrow1 = QPoint(endP.x(),endR.top());
            arrow2_1 = arrow1 - QPoint(arrowW,arrowL);
            arrow2_2 = arrow1 - QPoint(-arrowW,arrowL);
        }

    }

    painter->drawLine(arrow1,arrow2_1);
    painter->drawLine(arrow1,arrow2_2);
    painter->drawLine(startP,middle1);
    painter->drawLine(middle1,middle2);
    painter->drawLine(middle2,middle3);
    painter->drawLine(middle3,endP);

    painter->restore();
}




void FlowchartWidget::drawSelectedBtn(QPainter *painter)
{
    painter->save();


    QPushButton *btn = SuspendBtn;



    for(auto it = selectedBtnList.begin(); it != selectedBtnList.end(); ++it)
    {
        QRect rect((*it)->geometry().topLeft() - QPoint(5,5), (*it)->geometry().bottomRight() + QPoint(5,5));
        QRadialGradient gradient(rect.center(), (rect.width() > rect.height() ? rect.width() : rect.height()) / 2 );
        gradient.setColorAt(0, QColor(255,0,0));
        gradient.setColorAt(1, QColor(255,255,255,0));
        QBrush brush(gradient);
        painter->setPen(Qt::NoPen);
        painter->setBrush(brush);
        painter->drawRect(rect);
        if(selectAreaDrag == true)
            (*it)->setDown(true);
        else
            (*it)->setDown(false);
    }


    for(auto it = breakBtnList.begin(); it != breakBtnList.end(); ++it)
    {
        QRect rect((*it)->geometry().topLeft() - QPoint(20,20), (*it)->geometry().topLeft());
        QRadialGradient gradient(rect.center(), (rect.width() > rect.height() ? rect.width() : rect.height()));
        gradient.setColorAt(0, QColor(255,0,0));
        gradient.setColorAt(1, QColor(255,255,255));
        QBrush brush(gradient);
        painter->setPen(QPen(Qt::black,1));
        painter->setBrush(brush);
        painter->drawEllipse(rect);
    }


    if(btn)
    {
        QRect rect(btn->geometry().topLeft() - QPoint(20,20), btn->geometry().topLeft()- QPoint(3,3));
        QPoint arrow_p[7];
        arrow_p[0] = rect.bottomRight();

        arrow_p[1] = arrow_p[0] - QPoint(0,rect.height()*0.5);
        arrow_p[2] = arrow_p[1] + QPoint(rect.height()*(-0.15),rect.width()*0.15);
        arrow_p[3] = arrow_p[2] - QPoint(rect.width()*0.5, rect.height()*0.5);

        arrow_p[6] = arrow_p[0] - QPoint(rect.width()*0.5,0);
        arrow_p[5] = arrow_p[6] + QPoint(rect.height()*0.15,rect.width()*(-0.15));
        arrow_p[4] = arrow_p[5] - QPoint(rect.width()*0.5, rect.height()*0.5);


        QRadialGradient gradient(rect.center(), (rect.width() > rect.height() ? rect.width() : rect.height()));
        gradient.setColorAt(0, QColor(255,255,0));
        gradient.setColorAt(1, QColor(255,255,255));
        QBrush brush(gradient);
        painter->setPen(QPen(Qt::black,1));
        painter->setBrush(brush);
        painter->drawPolygon(arrow_p,7);
    }



    painter->restore();
}




bool FlowchartWidget::eventFilter(QObject* watched, QEvent* event)
{

    if(event->type()==QEvent::MouseButtonPress) {

        if (dynamic_cast<QMouseEvent*>(event)->buttons() & Qt::MiddleButton)
        {
            return QWidget::eventFilter(watched, event);
        }


        if (dynamic_cast<QMouseEvent*>(event)->buttons() & Qt::RightButton)
        {
            setMouseTracking(false);
            men_preButton = nullptr;
            QPushButton *btn = dynamic_cast<QPushButton*>(watched);
            if(btn)
            {
                FlowchartsTemplate *flocha = button_flocha_map.find(btn)->second;
                int type = flocha->getflochaType();
                if((type == flochaTypeIf) || (type == flochaTypeSet) || (type == flochaTypeRun)
                    || (type == flochaTypeSleep))
                {

                    //FlowchartsIf *floIf = dynamic_cast<FlowchartsIf*>(flocha);

                    FloDataInputDlg inputdlg(this, flocha);
                    inputdlg.exec();

                }

                else if(type == flochaTypeAssem)
                {
                    flochaNeedPopMenu = flocha;
                    flochaPopupMenu(btn->mapToParent(dynamic_cast<QMouseEvent*>(event)->pos()));
                }

                setBtnTextByflocha(btn, flocha);
                emit flochaIsChanged(flocha);
            }



        }


        if(operationMode == editItemMode)
        {

            if((men_preButton != nullptr))
            {
                QPushButton *btn = dynamic_cast<QPushButton*>(watched);
                linkTwoBtn_flocha(men_preButton, btn);

                emit flochaIsChanged(button_flocha_map.find(men_preButton)->second);

                setMouseTracking(false);
                men_preButton = nullptr;

                update();
            }

            QPushButton *btn = dynamic_cast<QPushButton*>(watched);
            if(btn)
            {
                bool noselected = true;
                for(auto it = selectedBtnList.begin(); it != selectedBtnList.end(); ++it)
                    if((*it) == btn)
                    {
                        noselected = false;
                        break;
                    }

                    if(noselected)
                    {
                        if(!pressedCtrlKey)
                            selectedBtnList.clear();

                        selectedBtnList.push_back(btn);
                    }


                    update();

                    mouseStartPos = btn->mapToParent(dynamic_cast<QMouseEvent*>(event)->pos());
                    posInButton = dynamic_cast<QMouseEvent*>(event)->pos();
                    posOfPushButton = btn->pos();
            }

        }




    }
    else if(event->type()==QEvent::MouseButtonDblClick)
    {

        if(operationMode == editItemMode)
        {

            if((men_preButton == nullptr))
            {
                QPushButton *btn = dynamic_cast<QPushButton*>(watched);//avoid paint before choose if or else
                //men_preButton = dynamic_cast<QPushButton*>(watched);//avoid paint before choose if or else
                setMouseTracking(true);


                if(btn)
                {
                    if(button_flocha_map.find(btn)->second->getflochaType() == flochaTypeIf)
                    {
                        QStringList strlist;
                        QString iftruestr = tr("if true");
                        QString ifelsestr = tr("else");
                        strlist << iftruestr << ifelsestr;

                        bool ok;
                        btnLinkElse = false;
                        QString item = QInputDialog::getItem(this, tr("if true or false ?"),
                            tr("select"), strlist, 0, false, &ok);
                        if (ok && !item.isEmpty())
                        {
                            if(item == iftruestr)
                                btnLinkElse = false;
                            else
                                btnLinkElse = true;
                        }
                    }
                    men_preButton = btn;//avoid paint before choose if or else
                }
            }
        }


        //mouseStartPos=posInButton+posOfPushButton;

        update();

    }
    else if(event->type()==QEvent::MouseMove) {

        btnDragging = true;

        if (dynamic_cast<QMouseEvent*>(event)->buttons() & Qt::MiddleButton)
        {
            return QWidget::eventFilter(watched, event);
        }


        if(operationMode == editItemMode)
        {
            //posInButton=dynamic_cast<QMouseEvent*>(event)->pos();
            QPushButton *btn = dynamic_cast<QPushButton*>(watched);

            posOfPushButton=dynamic_cast<QWidget*>(watched)->pos();
            QMouseEvent tmpevt(QEvent::MouseMove, btn->mapToParent(dynamic_cast<QMouseEvent*>(event)->pos()), Qt::LeftButton,Qt::LeftButton, Qt::NoModifier);
            mouseMoveEvent(&tmpevt);


            return true;//avoid the event continue deliver, return true.

        }

#ifdef _DEBUG
        ++tmpCnt;
        if(tmpCnt>10)
            tmpCnt = 0;
#endif

    }
    if(event->type()==QEvent::MouseButtonRelease)
    {
        //		if (dynamic_cast<QMouseEvent*>(event)->buttons() & Qt::LeftButton)
        {
            if(!(pressedCtrlKey || btnDragging))
            {
                selectedBtnList.clear();

                QPushButton *btn = dynamic_cast<QPushButton*>(watched);
                if(btn)
                {
                    bool noselected = true;
                    for(auto it = selectedBtnList.begin(); it != selectedBtnList.end(); ++it)
                        if((*it) == btn)
                        {
                            noselected = false;
                            break;
                        }

                        if(noselected)
                            selectedBtnList.push_back(btn);
                }
                update();
            }
            if(pressedCtrlKey)
                return QWidget::eventFilter(watched, event);
        }


        QMouseEvent* releasevt = dynamic_cast<QMouseEvent*>(event);
        if(releasevt)
            mouseReleaseEvent(releasevt);

        btnDragging = false;
    }


    return QWidget::eventFilter(watched, event);


}
void FlowchartWidget::mousePressEvent(QMouseEvent *event)
{

#ifdef _DEBUG
    ++tmpCnt;
    if(tmpCnt>10)
        tmpCnt = 0;
#endif

    if (event->buttons() & Qt::MiddleButton)
    {
        mouseStartPos = event->pos();
    }
    else if(event->buttons() & Qt::LeftButton)
    {
        mouseStartPos = event->pos();
        mousePos = mouseStartPos;
        if(men_preButton != nullptr)
        {

            bool noselected = true;
            for(auto it = selectedBtnList.begin(); it != selectedBtnList.end(); ++it)
                if((*it) == men_preButton)
                {
                    noselected = false;
                    break;
                }

                if(noselected)
                    selectedBtnList.push_back(men_preButton);

        }
        else
            selectedBtnList.clear();

        selectAreaDrag = true;
    }
    men_preButton = nullptr;
    setMouseTracking(false);
    update();

}



void FlowchartWidget::mouseMoveEvent(QMouseEvent *event)
{
    //	qDebug() << event->pos().x() << event->pos().y();

    mousePos = event->pos();



    if (event->buttons() & Qt::LeftButton) {
        men_preButton = nullptr;

        int distance = (mousePos - mouseStartPos).manhattanLength();
        if (distance >= 5)//QApplication::startDragDistance())
        {
            if(operationMode == editItemMode)
            {
                if(btnDragging == true)
                {
                    if(this->geometry().contains(mousePos))
                    {
                        QPoint movevector = mousePos - posOfPushButton - posInButton;

                        for(auto it = selectedBtnList.begin(); it != selectedBtnList.end(); it++)
                        {
#ifdef _DEBUG
                            qDebug() << "idName" << button_flocha_map.find(*it)->second->idName << "btnpos before" << (*it)->geometry().topLeft();
                            qDebug() << "event->globalPos()" << event->globalPos() << "mousePos" << mousePos << "mouseStartPos" << mouseStartPos;
#endif
                            QPoint btnpos = (*it)->geometry().topLeft();
                            (*it)->move(movevector + btnpos);

#ifdef _DEBUG
                            qDebug() << "idName" << button_flocha_map.find(*it)->second->idName << "btnpos after" << (*it)->geometry().topLeft();
#endif


                        }
                        mouseStartPos = mousePos;
                        //child->move(event->pos() - posInButton + posOfPushButton);

                    }

                }
                else
                    selectedBtnList = findBtnListInRect(QRect(mouseStartPos, mousePos));

                //posInButton = event->pos();
            }
        }
    }
    else if (event->buttons() & Qt::MiddleButton)
    {
        scroll(event->pos().x() - mouseStartPos.x(), event->pos().y() - mouseStartPos.y());

        scrollOffsetPos += (event->pos() - mouseStartPos);

#ifdef _DEBUG
        qDebug() << "offset x" << scrollOffsetPos.x() << "offset y" << scrollOffsetPos.y();
        //qDebug() << "scroll x" << event->pos().x() - mouseStartPos.x() << "scroll y" << event->pos().y() - mouseStartPos.y();
        //qDebug() << "x" << geometry().left() << "y" << geometry().top();
#endif


        mouseStartPos = event->pos();

    }
    update();
}


void FlowchartWidget::mouseReleaseEvent(QMouseEvent *event)
{

    int test = event->button();

    if (event->button() & Qt::LeftButton) {
        //		men_preButton = nullptr;

        if(operationMode == editItemMode)
        {
            QPushButton *child = dynamic_cast<QPushButton *>(this->childAt(event->pos() + posOfPushButton));

            if(child != 0)
            {
                std::list<FlowchartsTemplate *> flochalist;
                flochalist.clear();

                for(auto it = selectedBtnList.begin(); it != selectedBtnList.end(); it++)
                {
                    FlowchartsTemplate *flocha = button_flocha_map.find(*it)->second;
                    flochalist.push_back(flocha);
                }
                //
                //floActStackForUndo.push_back(std::make_pair(flochalist,flochaChangeData));
                //do not push to container directly

                pushFlochaDataToReUndo(flochalist, pushToUndoStack, flochaChangeData);


                for(auto it = selectedBtnList.begin(); it != selectedBtnList.end(); it++)
                {
                    FlowchartsTemplate *flocha = button_flocha_map.find(*it)->second;
                    if(flocha)
                    {
                        flocha->itemPos[0] = (*it)->geometry().left() - scrollOffsetPos.x();
                        flocha->itemPos[1] = (*it)->geometry().top() - scrollOffsetPos.y();
                        emit flochaIsChanged(flocha);
                    }
                }
            }
            else
            {
                int test = 0;
            }

        }

    }




    selectAreaDrag = false;



    update();



}



void FlowchartWidget::keyPressEvent(QKeyEvent * event)
{
    //int test = event->key();
    if(event->key() == Qt::Key_Control)
        pressedCtrlKey = true;

}

void FlowchartWidget::keyReleaseEvent (QKeyEvent * event)
{
    pressedCtrlKey = false;
}


void FlowchartWidget::dragEnterEvent(QDragEnterEvent *event)
{
    const ItemBoxMimeData *itemmime = dynamic_cast<const ItemBoxMimeData*>(event->mimeData());

    if (itemmime)
        event->acceptProposedAction();

}


void FlowchartWidget::dragMoveEvent(QDragMoveEvent *event)
{
    const ItemBoxMimeData *itemmime = dynamic_cast<const ItemBoxMimeData*>(event->mimeData());
    if (itemmime) 
    {

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }

}
void FlowchartWidget::dropEvent(QDropEvent *event)
{
    const ItemBoxMimeData *itemmime = dynamic_cast<const ItemBoxMimeData*>(event->mimeData());

    if(itemmime)
    {
        FlowchartsTemplate *dropinassem = itemmime->getFlochaAssemPtr();
        if(dropinassem)
        {
            mousePos = event->pos();
            std::list<FlowchartsTemplate*> srcflolist;
            std::list<FlowchartsTemplate*> dstflolist;
            srcflolist.clear();
            dstflolist.clear();
            srcflolist.push_back(dropinassem);

            dropinassem->itemPos[0] = mousePos.x() - scrollOffsetPos.x();
            dropinassem->itemPos[1] = mousePos.y() - scrollOffsetPos.y();


            dstflolist = fullCopyFlochaList(srcflolist,flochaAssem,NULL);

            updWidgetAfterFloChange(dstflolist, flochaChangeAdd);

            for(auto it = dstflolist.begin(); it != dstflolist.end(); ++it)
            {
                emit flochaIsChanged(*it);
            }


        }
        else
        {
            mousePos = event->pos();
            switch (itemmime->getNewFlochaType())
            {
            case flochaTypeIf:
                addItemIf();
                break;
            case flochaTypeSet:
                addItemSet();
                break;
            case flochaTypeRun:
                addItemRun();
                break;
            case flochaTypeSleep:
                addItemSleep();
                break;
            case flochaTypeAssem:
                addItemAssem();
                break;

            default:
                break;
            }

        }


        event->setDropAction(Qt::MoveAction);
        event->accept();
    }



}


const FlowchartsAssemble* FlowchartWidget::getMainFlochaAssem()
{
    return flochaAssem;
}

int FlowchartWidget::constructFromAssem(FlowchartsAssemble* fromAssem)
{
    int rtn = -1;


    if(fromAssem != nullptr)
    {
        delete flochaAssem;
        flochaAssem = fromAssem;
    }
    for(auto it = button_flocha_map.begin(); it != button_flocha_map.end(); ++it)
        delete it->second;

    for(auto it = buttonList.begin(); it != buttonList.end(); ++it)
        delete *it;

    button_flocha_map.clear();
    buttonLinkMap.clear();
    buttonList.clear();








    return rtn;
}


int FlowchartWidget::constructFromAssemLoop(FlowchartsAssemble* flochaIn)
{
    int rtn = -1;
    for(auto it = flochaIn->getFlochaList().begin(); it != flochaIn->getFlochaList().end(); ++it)
    {

        //		if((*it)->getflochaType() == flochaTypeTemplate)
        //			continue;

        QPushButton *btn = new QPushButton(this);
        btn->move((*it)->itemPos[0],(*it)->itemPos[1]);
        btn->show();

        button_flocha_map.insert(std::make_pair(btn,*it));

        btn->installEventFilter(this);
        buttonList.push_back(btn);

        //		if((*it)->getflochaType() == flochaTypeAssem)
        //		{
        //			FlowchartsAssemble *assem = dynamic_cast<FlowchartsAssemble*>(*it);
        //			constructFromAssemLoop(assem);
        //		}
        setBtnTextByflocha(btn, *it);
    }

    return rtn;
}

int FlowchartWidget::linkButtonFromAssem()
{
    int rtn = -1;
    for(auto it = button_flocha_map.begin(); it != button_flocha_map.end(); ++it)
    {
        auto it_btn = buttonLinkMap.find(it->first);
        if(it_btn != buttonLinkMap.end())
        {
            if(it->second->getflochaType() == flochaTypeIf)
            {
                it_btn->second.btnPtr = 0;
                it_btn->second.btnPtrElse = findButtonByFlocha(it->second->getElsePtr());
                it_btn->second.btnPtrIftrue = findButtonByFlocha(it->second->getNextPtr());
            }
            else
            {
                it_btn->second.btnPtr = findButtonByFlocha(it->second->getNextPtr());
                it_btn->second.btnPtrElse = 0;
                it_btn->second.btnPtrIftrue = 0;
            }

        }
        else if(it->second->getflochaType() == flochaTypeIf)
        {
            buttonLinkAttri attri;
            attri.btnPtr = 0;

#ifdef _DEBUG
            qDebug() << "link Button From Assem() idname" << it->second->idName;

            if(it->second->getElsePtr())
                qDebug() << "link Button From Assem() p_else" << it->second->getElsePtr()->idName;
            if(it->second->getNextPtr())
                qDebug() << "link Button From Assem() p_next" << it->second->getNextPtr()->idName;

#endif

            attri.btnPtrElse = findButtonByFlocha(it->second->getElsePtr());
            attri.btnPtrIftrue = findButtonByFlocha(it->second->getNextPtr());

            buttonLinkMap.insert(std::make_pair(it->first,attri));
        }
        else
        {
            buttonLinkAttri attri;
            attri.btnPtr = findButtonByFlocha(it->second->getNextPtr());
            attri.btnPtrElse = 0;
            attri.btnPtrIftrue = 0;

            buttonLinkMap.insert(std::make_pair(it->first,attri));
        }
    }
    return rtn;
}

void FlowchartWidget::deleteAll()
{

    for(auto it = button_flocha_map.begin(); it != button_flocha_map.end(); ++it)
        delete it->second;

    //do not delete start & end button

    for(auto it = buttonList.begin(); it != buttonList.end(); ++it)
        delete *it;



    button_flocha_map.clear();
    buttonLinkMap.clear();
    buttonList.clear();
}







QPushButton *FlowchartWidget::findButtonByFlocha(FlowchartsTemplate* flocha)
{
    for(auto it = button_flocha_map.begin(); it != button_flocha_map.end(); ++it)
    {
        if(it->second == flocha)
            return it->first;
    }
    return nullptr;
}

void FlowchartWidget::setBtnTextByflocha(QPushButton *btn,FlowchartsTemplate* flocha)
{
    FlowchartsIf *flochaif = dynamic_cast<FlowchartsIf *>(flocha);
    FlowchartsSetVar* flochaset = dynamic_cast<FlowchartsSetVar*>(flocha);
    FlowchartsSleep *flochasleep = dynamic_cast<FlowchartsSleep*>(flocha);
    FlowchartsRunPDL *flocharun = dynamic_cast<FlowchartsRunPDL*>(flocha);
    FlowchartsAssemble *flochaassem = dynamic_cast<FlowchartsAssemble*>(flocha);


    std::pair<QString,QString> ifoppair;
    if(flochaif != nullptr)
        ifoppair = IfOperatorClass::getOperStr(flochaif->getOperator());

    switch(flocha->getflochaType())
    {
    case flochaTypeIf:

        btn->setText(QString("%1\n%2\n%3\n%4 %5 %6\n%7")
            .arg(flochaif->idName).arg("IF")
            .arg(flochaif->getStatement1())
            .arg(ifoppair.second)
            .arg(ifoppair.first)
            .arg(ifoppair.second)
            .arg(flochaif->getStatement2())
            );


        break;
    case flochaTypeSet:
        btn->setText(QString("%1\n%2\n%3\n%4")
            .arg(flochaset->idName).arg("SET")
            .arg(flochaset->getVarName())
            .arg(flochaset->getStatement())
            );

        break;
    case flochaTypeRun:
        btn->setText(QString("%1\n%2\n%3")
            .arg(flocharun->idName).arg("RUN")
            .arg(flocharun->getPDLName())
            );
        break;
    case flochaTypeSleep:

        btn->setText(QString("%1\n%2\n%3")
            .arg(flochasleep->idName).arg("SLEEP")
            .arg(flochasleep->getSleepTime())
            );

        break;
    case flochaTypeAssem:

        btn->setText(QString("%1\n%2")
            .arg(flochaassem->idName)
            .arg("ASSEM\n")
            );



        break;
    case flochaTypeTemplate:

        if(QString("%1").arg(flocha->idName) == XmlEleTagValueStart)
            btn->setText(QString("\nstart\n"));


        if(QString("%1").arg(flocha->idName) == XmlEleTagValueEnd)
            btn->setText(QString("\nend\n"));

        break;
    default:
        QMessageBox::warning(this, tr("error"), tr("setBtnTextByflocha error"));
        break;
    }



    btn->adjustSize();

}

void FlowchartWidget::unlinkBtn(QPushButton *btn)
{
    for(auto it = buttonLinkMap.begin(); it != buttonLinkMap.end(); ++it)
    {
        if(it->first == btn)
        {
            it->second.btnPtr = nullptr;
            it->second.btnPtrElse = nullptr;
            it->second.btnPtrIftrue = nullptr;

        }

        if(it->second.btnPtr == btn)
            it->second.btnPtr = nullptr;

        if(it->second.btnPtrElse == btn)
            it->second.btnPtrElse = nullptr;

        if(it->second.btnPtrIftrue == btn)
            it->second.btnPtrIftrue = nullptr;
    }

}


void FlowchartWidget::updWidgetAfterFloChange(std::list<FlowchartsTemplate *> &flochalist, int type)
{

    if(type == flochaChangeData || type == flochaChangeDelete_unlink)
    {
        for(auto it_flo = flochalist.begin(); it_flo != flochalist.end(); ++it_flo)
        {
            QPushButton *btn = findButtonByFlocha(*it_flo);

            setBtnTextByflocha(btn, *it_flo);

            auto it_btn = buttonLinkMap.find(btn);

#ifdef _DEBUG
            qDebug() << "idname" << (*it_flo)->idName;

            if(it_btn == buttonLinkMap.end())
                QMessageBox::warning(this,tr("error"),tr("not find in button link map"));
            if((*it_flo)->getElsePtr())
                qDebug() << "updWidgetAfterFloChange  p_else" << (*it_flo)->getElsePtr()->idName;
            if((*it_flo)->getNextPtr())
                qDebug() << "updWidgetAfterFloChange  p_next" << (*it_flo)->getNextPtr()->idName;
#endif

            btn->move((*it_flo)->itemPos[0] + scrollOffsetPos.x(), (*it_flo)->itemPos[1] + scrollOffsetPos.y());



            if((*it_flo)->getflochaType() == flochaTypeIf)
            {
                it_btn->second.btnPtrElse = findButtonByFlocha((*it_flo)->getElsePtr());
                it_btn->second.btnPtrIftrue = findButtonByFlocha((*it_flo)->getNextPtr());
            }
            else
            {
                it_btn->second.btnPtr = findButtonByFlocha((*it_flo)->getNextPtr());
                it_btn->second.btnPtrElse = 0;
                it_btn->second.btnPtrIftrue = 0;
            }

        }
    }
    else if(type == flochaChangeAdd)
    {
        for(auto it_flo = flochalist.begin(); it_flo != flochalist.end(); ++it_flo)
        {


#ifdef _DEBUG
            qDebug() << "print debug msg";
            printFlochaDebugMsg(*it_flo);
#endif

            QPushButton *btn = findButtonByFlocha(*it_flo);
            if(btn == nullptr)
                btn = createBtnForFlocha(*it_flo);

            /*
            buttonLinkAttri attri;
            if((*it_flo)->getflochaType() == flochaTypeIf)
            {
            attri.btnPtr = 0;
            attri.btnPtrElse = findButtonByFlocha((*it_flo)->p_else);
            attri.btnPtrIftrue = findButtonByFlocha((*it_flo)->p_next);
            }
            else
            {
            attri.btnPtr = findButtonByFlocha((*it_flo)->p_next);
            attri.btnPtrElse = 0;
            attri.btnPtrIftrue = 0;
            }
            buttonLinkMap.insert(std::make_pair(btn,attri));
            */

            btn->move((*it_flo)->itemPos[0] + scrollOffsetPos.x(), (*it_flo)->itemPos[1] + scrollOffsetPos.y());
            setBtnTextByflocha(btn, *it_flo);
        }
    }
    else if(type == flochaChangeDelete)
    {
        for(auto it_flo = flochalist.begin(); it_flo != flochalist.end(); ++it_flo)
        {
            QPushButton *btn = findButtonByFlocha(*it_flo);
            button_flocha_map.erase(btn);
            buttonLinkMap.erase(btn);
            buttonList.remove(btn);
            selectedBtnList.remove(btn);

            delete btn;
            btn = nullptr;
        }
    }

    linkButtonFromAssem();

    update();

}



QPushButton* FlowchartWidget::createBtnForFlocha(FlowchartsTemplate *flocha)
{
#ifdef _DEBUG
    qDebug() << "add btn";
#endif

    QPushButton *btn = new QPushButton(this);
    buttonList.push_back(btn);
    btn->installEventFilter(this);

    btn->move(flocha->itemPos[0] + scrollOffsetPos.x(),flocha->itemPos[1] + scrollOffsetPos.y());
    btn->show();


    button_flocha_map.insert(std::make_pair(btn, flocha));


    return btn;
}

void FlowchartWidget::drawBtn(QPushButton *btn)
{


}


void FlowchartWidget::setNewflochaIdName(FlowchartsTemplate *newflocha)
{
    QString typestr;
    QString idname;
    QString tmpstr;
    QStringList strlist;

    int num = 0;

    switch (newflocha->getflochaType())
    {
    case flochaTypeIf:
        typestr = QString("if");
        break;
    case flochaTypeSet:
        typestr = QString("set");
        break;
    case flochaTypeRun:
        typestr = QString("run");
        break;
    case flochaTypeSleep:
        typestr = QString("sleep");
        break;
    case flochaTypeAssem:
        typestr = QString("assem");
        break;
    default:
        typestr = QString("error");
        break;
    }

    for(auto it = button_flocha_map.begin(); it != button_flocha_map.end(); ++it)
    {

        tmpstr = QString(it->second->idName);
        strlist = tmpstr.split("_");
        if(strlist.size() >1)
        {
            if(typestr == strlist.at(0))
                num = (strlist.at(1).toInt() >= num) ? (strlist.at(1).toInt() + 1) : num;
        }
    }

    idname = typestr + QString("_%1").arg(num);
    memcpy(newflocha->idName, idname.toStdString().c_str(), idname.size());

}


void FlowchartWidget::linkTwoBtn_flocha(QPushButton *src, QPushButton *dst)
{
    if((!src) || (!dst))
    {
#ifdef _DEBUG
        QMessageBox::warning(this,tr("warning"),tr("btn src or dst is null"));
#endif
        return;
    }


    std::list<FlowchartsTemplate *> flochalist;
    flochalist.clear();
    flochalist.push_back(button_flocha_map.find(src)->second);

    //do not push to container directly
    //floActStackForUndo.push_back(std::make_pair(flochalist,flochaChangeData));
    //

    pushFlochaDataToReUndo(flochalist, pushToUndoStack, flochaChangeData);

    if((dst != src) && (button_flocha_map.find(src)->second->getflochaType() != flochaTypeIf))
    {
        //buttonLinkMap.erase(men_preButton);

        if(buttonLinkMap.find(src) == buttonLinkMap.end())
        {
            buttonLinkAttri tmpBtnAttri2;
            tmpBtnAttri2.btnPtr = dst;
            tmpBtnAttri2.btnPtrElse = 0;
            tmpBtnAttri2.btnPtrIftrue = 0;

            buttonLinkMap.insert(std::make_pair(src, tmpBtnAttri2));
        }
        else
        {
            auto it = buttonLinkMap.find(src);
            it->second.btnPtr = dst;
        }

        flochaAssem->unlinkTemplate(button_flocha_map.find(src)->second,UNLINKNEXT);
        flochaAssem->linkTwoTemplate(button_flocha_map.find(src)->second,button_flocha_map.find(dst)->second);
    }
    else if((button_flocha_map.find(src)->second->getflochaType() == flochaTypeIf))
    {



        if(buttonLinkMap.find(src) == buttonLinkMap.end())
        {
            buttonLinkAttri tmpBtnAttri2;

            tmpBtnAttri2.btnPtr = 0;
            if(btnLinkElse)
            {
                tmpBtnAttri2.btnPtrIftrue = 0;
                tmpBtnAttri2.btnPtrElse = dst;
                flochaAssem->linkTwoTemplate(button_flocha_map.find(src)->second,button_flocha_map.find(dst)->second, true);

            }
            else
            {
                tmpBtnAttri2.btnPtrIftrue = dst;
                tmpBtnAttri2.btnPtrElse = 0;
                flochaAssem->linkTwoTemplate(button_flocha_map.find(src)->second,button_flocha_map.find(dst)->second, false);

            }
            buttonLinkMap.insert(std::make_pair(src, tmpBtnAttri2));
        }
        else
        {
            auto it = buttonLinkMap.find(src);
            if(btnLinkElse)
            {
                it->second.btnPtrElse = dst;
                flochaAssem->unlinkTemplate(button_flocha_map.find(src)->second,UNLINKELSE);
                flochaAssem->linkTwoTemplate(button_flocha_map.find(src)->second,button_flocha_map.find(dst)->second,true);
            }
            else
            {
                it->second.btnPtrIftrue = dst;
                flochaAssem->unlinkTemplate(button_flocha_map.find(src)->second,UNLINKNEXT);
                flochaAssem->linkTwoTemplate(button_flocha_map.find(src)->second,button_flocha_map.find(dst)->second,false);
            }
        }

    }

}


std::list<QPushButton*> FlowchartWidget::findBtnListInRect(QRect rect)
{
    std::list<QPushButton*> btnlist;
    btnlist.clear();
    for(auto it = buttonList.begin(); it != buttonList.end(); ++it)
    {
        //-----------------------------to avoid btn no setDown(false)--------------------------
        (*it)->setDown(false);
        //-----------------------------to avoid btn no setDown(false)--------------------------

        if(rect.contains((*it)->geometry().center()))
        {
            btnlist.push_back(*it);
        }
    }
    return btnlist;
}

FlowchartsTemplate* FlowchartWidget::paste_copyFlochaData(FlowchartsTemplate *flodst,FlowchartsTemplate *flodata, FlowchartsAssemble* dstparent, FlowchartsAssemble* srcparent, bool copylink, bool copyfullassem)
{
    CommandManager *cmd = commandManager;

    if((copylink == true) && (copyfullassem == false))//should be carefour!!!!!! this is to speed up copy and paste
        cmd = nullptr;

    FlowchartsTemplate* rtnflo = nullptr;
    if(flodst)
        rtnflo = flodst;
    else
    {
        switch(flodata->getflochaType())
        {
        case flochaTypeIf:
            rtnflo = new FlowchartsIf(dstparent,cmd);
            break;
        case flochaTypeSet:
            rtnflo = new FlowchartsSetVar(dstparent,cmd);
            break;
        case flochaTypeRun:
            rtnflo = new FlowchartsRunPDL(dstparent, cmd);
            break;
        case flochaTypeSleep:
            rtnflo = new FlowchartsSleep(dstparent);
            break;
        case flochaTypeAssem:
            rtnflo = new FlowchartsAssemble(dstparent, cmd);
            break;
        default:
            rtnflo = new FlowchartsTemplate(dstparent);
            break;
        }
    }

    if(rtnflo->getflochaType() != flodata->getflochaType())
    {
        delete rtnflo;
        rtnflo = nullptr;
        return rtnflo;
    }


    rtnflo->itemPos[0] = flodata->itemPos[0];
    rtnflo->itemPos[1] = flodata->itemPos[1];
    memcpy(rtnflo->idName,flodata->idName,sizeof(flodata->idName));
    if(copylink)
    {
        rtnflo->setNextPtr(flodata->getNextPtr());
        rtnflo->setElsePtr(flodata->getElsePtr());
    }


    FlowchartsIf *flochaif = dynamic_cast<FlowchartsIf *>(rtnflo);
    FlowchartsSetVar* flochaset = dynamic_cast<FlowchartsSetVar*>(rtnflo);
    FlowchartsSleep *flochasleep = dynamic_cast<FlowchartsSleep*>(rtnflo);
    FlowchartsRunPDL *flocharun = dynamic_cast<FlowchartsRunPDL*>(rtnflo);
    FlowchartsAssemble *flochaassem = dynamic_cast<FlowchartsAssemble*>(rtnflo);

    FlowchartsIf *flochaif_data = dynamic_cast<FlowchartsIf *>(flodata);
    FlowchartsSetVar* flochaset_data = dynamic_cast<FlowchartsSetVar*>(flodata);
    FlowchartsSleep *flochasleep_data = dynamic_cast<FlowchartsSleep*>(flodata);
    FlowchartsRunPDL *flocharun_data = dynamic_cast<FlowchartsRunPDL*>(flodata);
    FlowchartsAssemble *flochaassem_data = dynamic_cast<FlowchartsAssemble*>(flodata);




    switch(flodata->getflochaType())
    {
    case flochaTypeIf:

        flochaif->setStatement1(flochaif_data->getStatement1());
        flochaif->setStatement2(flochaif_data->getStatement2());
        flochaif->setOperator(flochaif_data->getOperator());

        break;
    case flochaTypeSet:
        flochaset->setVarName(flochaset_data->getVarName());
        flochaset->setStatement(flochaset_data->getStatement());

        break;
    case flochaTypeRun:

        flocharun->setPDLName(flocharun_data->getPDLName());
        break;
    case flochaTypeSleep:

        flochasleep->setSleepTime(flochasleep_data->getSleepTime());

        break;
    case flochaTypeAssem:
        //sub flocha should not undo redo in this widget
        if(copyfullassem)
            fullCopyFlochaList(flochaassem_data->getFlochaList(),flochaassem, flochaassem_data);
        //{
        //	std::list<FlowchartsTemplate*> copylist;
        //	copylist.clear();
        //	copylist.push_back(flochaassem_data);
        //}
        //flochaassem->(flochaassem_data->getSleepTime());

        break;

    default:
        break;
    }


#ifdef _DEBUG
    if(rtnflo == nullptr)
        QMessageBox::warning(this,tr("error"), tr("clone or paste flocha error"));
#endif

    return rtnflo;
}



void FlowchartWidget::pushFlochaDataToReUndo(std::list<FlowchartsTemplate*> flochalist, int reundo, int type)
{
    //
    //should clear redo stack and data
    //
    std::list<FlowchartsTemplate*> tmpflochalist;
    tmpflochalist.clear();

    if((reundo == pushToUndoStack) && (!pushToUndoWhenRedo))
        deleteRedoStack();



    if((type == flochaChangeData) || (type == flochaChangeDelete) || (type == flochaChangeDelete_unlink))
    {
        for(auto it = flochalist.begin(); it != flochalist.end(); ++it)
        {
            FlowchartsTemplate *recordflo = paste_copyFlochaData(nullptr,*it, flochaAssem, flochaAssem);

#ifdef _DEBUG
            printFlochaDebugMsg(*it);
            printFlochaDebugMsg(recordflo);
#endif

            std::map<FlowchartsTemplate*,std::vector<FlowchartsTemplate*> >::iterator it_data;

            bool found = false;

            if(reundo == pushToUndoStack)
            {
                it_data = undoFlochaData.find(*it);
                if(it_data != undoFlochaData.end())
                    found = true;
            }
            else
            {
                it_data = redoFlochaData.find(*it);
                if(it_data != redoFlochaData.end())
                    found = true;
            }


            if(found)
            {
                it_data->second.push_back(recordflo);
            }
            else
            {
                std::vector<FlowchartsTemplate*> flochadata;
                flochadata.push_back(recordflo);

                if(reundo == pushToUndoStack)
                    undoFlochaData.insert(std::make_pair(*it,flochadata));
                else
                    redoFlochaData.insert(std::make_pair(*it,flochadata));
            }

        }

    }
    else if(type == flochaChangeAdd)
    {
    }

    if(reundo == pushToUndoStack)
        floActStackForUndo.push_back(std::make_pair(flochalist,type));
    else
        floActStackForRedo.push_back(std::make_pair(flochalist,type));

}

void FlowchartWidget::deleteRedoStack()
{
    if(floActStackForRedo.size() == 0)
        return;

    for(auto it_map = redoFlochaData.begin(); it_map != redoFlochaData.end(); ++it_map)
    {
        for(auto it_vect = it_map->second.begin(); it_vect != it_map->second.end(); ++it_vect)
        {
            delete (*it_vect);
        }
    }

    redoFlochaData.clear();
    floActStackForRedo.clear();

}

void FlowchartWidget::undoFlochaChange()
{
    if(floActStackForUndo.size() == 0)
        return;

    std::list<FlowchartsTemplate*> &tmplist = floActStackForUndo.back().first;

    //int redotype = floActStackForUndo.back().second;//redo type is same as flocha act stack


    int undotype;

    switch (floActStackForUndo.back().second)
    {

    case flochaChangeData:
        undotype = flochaChangeData;
        break;
    case flochaChangeAdd:
        undotype = flochaChangeDelete;
        break;
    case flochaChangeDelete:
        undotype = flochaChangeAdd;
        break;
    case flochaChangeDelete_unlink:
        undotype = flochaChangeDelete_unlink;
    default:
        break;
    }

    if(undotype != flochaChangeDelete_unlink)
        pushFlochaDataToReUndo(tmplist,pushToRedoStack,undotype);


    if((undotype == flochaChangeData) || (undotype == flochaChangeAdd) || (undotype == flochaChangeDelete_unlink))
    {
        for(auto it = tmplist.begin(); it != tmplist.end(); ++it)
        {

#ifdef _DEBUG
            printFlochaDebugMsg(*it);
            printFlochaDebugMsg(undoFlochaData.find(*it)->second.back());
#endif


            paste_copyFlochaData(*it,undoFlochaData.find(*it)->second.back(), flochaAssem, flochaAssem);

            if(undotype == flochaChangeAdd)
            {
                flochaAssem->addTemplate(*it);
            }


#ifdef _DEBUG
            printFlochaDebugMsg(*it);
#endif


            emit flochaIsChanged(*it);

            delete undoFlochaData.find(*it)->second.back();
            undoFlochaData.find(*it)->second.pop_back();
        }

    }


    updWidgetAfterFloChange(tmplist,undotype);
    floActStackForUndo.pop_back();


    if(undotype == flochaChangeAdd)
        undoFlochaChange();

}

void FlowchartWidget::redoFlochaChange()
{
    if(floActStackForRedo.size() == 0)
        return;


    std::list<FlowchartsTemplate*> &tmplist = floActStackForRedo.back().first;

    //int redotype = floActStackForUndo.back().second;//redo type is same as flocha act stack


    int redotype;

    pushToUndoWhenRedo = true;

    switch (floActStackForRedo.back().second)
    {

    case flochaChangeData:
        redotype = flochaChangeData;
        break;
    case flochaChangeAdd:
        redotype = flochaChangeDelete;
        break;
    case flochaChangeDelete:
        redotype = flochaChangeAdd;

        break;
    default:
        break;
    }

    if(redotype != flochaChangeDelete)
        pushFlochaDataToReUndo(tmplist,pushToUndoStack,redotype);


    if((redotype == flochaChangeData) || (redotype == flochaChangeAdd))
    {
        for(auto it = tmplist.begin(); it != tmplist.end(); ++it)
        {

            int test = 0;

            paste_copyFlochaData(*it,redoFlochaData.find(*it)->second.back(), flochaAssem, flochaAssem);


            emit flochaIsChanged(*it);
            delete redoFlochaData.find(*it)->second.back();
            redoFlochaData.find(*it)->second.pop_back();
        }
    }
    else
    {
        selectedBtnList.clear();
        for(auto it = tmplist.begin(); it != tmplist.end(); ++it)
        {
            selectedBtnList.push_back(findButtonByFlocha(*it));

        }
        removeSelectFlochas();

    }


    updWidgetAfterFloChange(tmplist,redotype);
    floActStackForRedo.pop_back();
    pushToUndoWhenRedo = false;
}


void FlowchartWidget::startAutoRun()
{
    FloRunFlagStuct runflag;
    runflag.bits.debug = 0;
    runflag.bits.stepRun = 0;
    flochaAssem->setRunFlag(runflag);

    SuspendBtn = nullptr;
    flochaAssem->startRunAssem();
    update();
}

void FlowchartWidget::startDebugRun()
{
    FloRunFlagStuct runflag;
    runflag.bits.debug = 1;
    runflag.bits.stepRun = 0;
    flochaAssem->setRunFlag(runflag);

    SuspendBtn = nullptr;

    if(flochaAssem->whetherRunning())
        flochaAssem->setContinueRunEvent();
    else
        flochaAssem->startRunAssem();

    update();
}

void FlowchartWidget::runNextStep()
{
    if((flochaAssem->whetherRunning()))
    {
        FloRunFlagStuct runflag;
        runflag.bits.debug = 1;
        runflag.bits.stepRun = 1;
        flochaAssem->setRunFlag(runflag);
        flochaAssem->setContinueRunEvent();
    }
    SuspendBtn = nullptr;
    update();
}

void FlowchartWidget::setBreakPoint()
{
    for(auto it = selectedBtnList.begin(); it != selectedBtnList.end(); ++it)
    {
        bool found = false;
        button_flocha_map.find(*it)->second->setbreakPointFlag(true);
        for(auto it_brk = breakBtnList.begin(); it_brk != breakBtnList.end(); ++it_brk)
        {	
            if(*it_brk == *it)
            {
                found = true;
                break;
            }
        }

        if(!found)
        {
            breakBtnList.push_back(*it);
        }

    }
    update();
}

void FlowchartWidget::resetBreakPoint()
{
    for(auto it = selectedBtnList.begin(); it != selectedBtnList.end(); ++it)
    {
        bool found = false;
        button_flocha_map.find(*it)->second->setbreakPointFlag(false);

        auto it_brk = breakBtnList.begin();

        while(it_brk != breakBtnList.end())
        {
            if(*it_brk == *it)
            {
                it_brk = breakBtnList.erase(it_brk);
            }
            else
                ++it_brk;
        }
    }
    update();
}


void FlowchartWidget::pauseRun()
{
    FloRunFlagStuct runflag;
    runflag.bits.debug = 1;
    runflag.bits.stepRun = 1;
    flochaAssem->setRunFlag(runflag);
}

void FlowchartWidget::stopRun()
{
    flochaAssem->stopRunning();
    SuspendBtn = nullptr;
    update();
}


void FlowchartWidget::setSuspendBtn(FlowchartsTemplate *flocha)
{
    SuspendBtn = findButtonByFlocha(flocha);
    update();
}


void FlowchartWidget::copySelectFlochas()
{
    copyflochaList.clear();
    if(selectedBtnList.size() == 0)
        return;

    for(auto it = selectedBtnList.begin(); it != selectedBtnList.end(); ++it)
    {
        FlowchartsTemplate *flocha = button_flocha_map.find(*it)->second;
        if(flocha->getflochaType() == flochaTypeTemplate)
            continue;

        copyflochaList.push_back(flocha);
    }



}
void FlowchartWidget::pasteCopyFlochas()
{

    if(copyflochaList.size() == 0)
        return;

    selectedBtnList.clear();
#ifdef _DEBUG
    qDebug() << "1<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << ::GetTickCount();
#endif

    std::list<FlowchartsTemplate*> resultlist = fullCopyFlochaList(copyflochaList, flochaAssem, flochaAssem);

#ifdef _DEBUG
    qDebug() << "2<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << ::GetTickCount();
#endif

    QPoint offset = mouseStartPos - QPoint(resultlist.front()->itemPos[0], resultlist.front()->itemPos[1]) - scrollOffsetPos;

    for(auto it = resultlist.begin(); it != resultlist.end(); ++it)
    {
        (*it)->itemPos[0] = (*it)->itemPos[0] + offset.x();
        (*it)->itemPos[1] = (*it)->itemPos[1] + offset.y();
        setNewflochaIdName(*it);
        
        emit flochaIsChanged(*it);
        //flochaAssem->addTemplate(*it);
    }

#ifdef _DEBUG
    qDebug() << "3<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << ::GetTickCount();
#endif

    //
    pushFlochaDataToReUndo(resultlist, pushToUndoStack, flochaChangeAdd);
    //

#ifdef _DEBUG
    qDebug() << "4<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << ::GetTickCount();
#endif

    updWidgetAfterFloChange(resultlist,flochaChangeAdd);

#ifdef _DEBUG
    qDebug() << "5<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << ::GetTickCount();
#endif

    //--------should push to select btn list after updWidgetAfterFloChange, because updWidgetAfterFloChange insert the button_flocha_map-----------
    for(auto it = resultlist.begin(); it != resultlist.end(); ++it)
    {
        QPushButton *btn = findButtonByFlocha(*it);
        if(btn)
            selectedBtnList.push_back(btn);
    }

}


void FlowchartWidget::removeSelectFlochas()
{
    std::list<FlowchartsTemplate*> unlinklist;

    for(auto it = selectedBtnList.begin(); it != selectedBtnList.end(); ++it)
    {
        FlowchartsTemplate *flo = button_flocha_map.find(*it)->second;
        if(flo->getflochaType() == flochaTypeTemplate)
            continue;

        for(auto it_all = button_flocha_map.begin(); it_all != button_flocha_map.end(); ++it_all)
        {
            if((it_all->second->getNextPtr() == flo) || (it_all->second->getElsePtr() == flo))
                unlinklist.push_back(it_all->second);
        }
    }

    pushFlochaDataToReUndo(unlinklist, pushToUndoStack, flochaChangeDelete_unlink);


    std::list<FlowchartsTemplate*> flochalist;

    for(auto it = selectedBtnList.begin(); it != selectedBtnList.end(); ++it)
    {
        QPushButton *btn = *it;
        FlowchartsTemplate *flocha = button_flocha_map.find(btn)->second;

        if(flocha->getflochaType() == flochaTypeTemplate)
            continue;

        button_flocha_map.erase(btn);
        buttonLinkMap.erase(btn);
        buttonList.remove(btn);

        unlinkBtn(btn);

        delete btn;

        flochalist.push_back(flocha);

        emit flochaIsRemoved(flocha);

        if(flocha->getflochaType() == flochaTypeAssem)
        {
            FlowchartsAssemble *subassem = dynamic_cast<FlowchartsAssemble*>(flocha);
            emit subFlochaAssemDeleteSignal(subassem);
        }
        //flochaAssem->removeTemplate(flocha);

    }


    //floActStackForUndo.push_back(std::make_pair(flochalist,flochaChangeDelete));
    //do not push to container directly

    pushFlochaDataToReUndo(flochalist, pushToUndoStack, flochaChangeDelete);



    //should removeTemplate after push to record data--------------------------------
    for(auto it = flochalist.begin(); it != flochalist.end(); ++it)
    {
        flochaAssem->removeTemplate(*it);
    }
    //should removeTemplate after push to record data--------------------------------


    selectedBtnList.clear();


    update();
}


void FlowchartWidget::assembleSelectFlochas()
{
    if(selectedBtnList.size() == 0)
        return;

    std::list<FlowchartsTemplate *> resultassem;
    std::list<FlowchartsTemplate*> toassemlist;
    toassemlist.clear();
    resultassem.clear();

    for(auto it = selectedBtnList.begin(); it != selectedBtnList.end(); ++it)
    {
        FlowchartsTemplate *flocha = button_flocha_map.find(*it)->second;
        //		if(flocha->getflochaType() == flochaTypeTemplate)
        //			continue;
        toassemlist.push_back(flocha);
    }



    FlowchartsAssemble *assem = new FlowchartsAssemble(flochaAssem, commandManager);


    flochaAssem->addTemplate(assem);
    setNewflochaIdName(assem);
    assem->itemPos[0] = mouseStartPos.x();
    assem->itemPos[1] = mouseStartPos.y();


    fullCopyFlochaList(toassemlist, assem, flochaAssem);

    resultassem.push_back(assem);
    pushFlochaDataToReUndo(resultassem, pushToUndoStack, flochaChangeAdd);
    updWidgetAfterFloChange(resultassem, flochaChangeAdd);

    emit flochaIsChanged(assem);
    removeSelectFlochas();

}


void FlowchartWidget::addItemIf()
{
    qDebug() << "addif";
    QPushButton *btn = new QPushButton(this);
    buttonList.push_back(btn);
    btn->installEventFilter(this);
    btn->move(mousePos);
    btn->show();
    FlowchartsTemplate *flo = new FlowchartsIf(flochaAssem,commandManager);
    //	if(commandManager != nullptr)
    //		flo->setMpiController(commandManager);
    flochaAssem->addTemplate(flo);
    button_flocha_map.insert(std::make_pair(btn, flo));
    flo->itemPos[0] = mousePos.x();
    flo->itemPos[1] = mousePos.y();

    setNewflochaIdName(flo);
    setBtnTextByflocha(btn,flo);


    std::list<FlowchartsTemplate *> flochalist;
    flochalist.clear();
    flochalist.push_back(flo);
    //floActStackForUndo.push_back(std::make_pair(flochalist,flochaChangeAdd));
    //do not push to container directly

    pushFlochaDataToReUndo(flochalist, pushToUndoStack, flochaChangeAdd);


    emit flochaIsChanged(flo);
}


void FlowchartWidget::addItemSet()
{
    qDebug() << "addset";
    QPushButton *btn = new QPushButton(this);
    buttonList.push_back(btn);
    btn->installEventFilter(this);
    btn->move(mousePos);
    btn->show();
    FlowchartsTemplate *flo = new FlowchartsSetVar(flochaAssem,commandManager);
    //	flo->setMpiController(commandManager);
    flochaAssem->addTemplate(flo);
    button_flocha_map.insert(std::make_pair(btn, flo));

    flo->itemPos[0] = mousePos.x();
    flo->itemPos[1] = mousePos.y();

    setNewflochaIdName(flo);
    setBtnTextByflocha(btn,flo);


    std::list<FlowchartsTemplate *> flochalist;
    flochalist.clear();
    flochalist.push_back(flo);

    //
    //floActStackForUndo.push_back(std::make_pair(flochalist,flochaChangeAdd));
    //do not push to container directly

    pushFlochaDataToReUndo(flochalist, pushToUndoStack, flochaChangeAdd);


    emit flochaIsChanged(flo);
}


void FlowchartWidget::addItemRun()
{
    qDebug() << "addrun";
    QPushButton *btn = new QPushButton(this);
    buttonList.push_back(btn);
    btn->installEventFilter(this);
    btn->move(mousePos);
    btn->show();
    FlowchartsTemplate *flo = new FlowchartsRunPDL(flochaAssem, commandManager);
    //	flo->setMpiController(commandManager);
    flochaAssem->addTemplate(flo); 
    button_flocha_map.insert(std::make_pair(btn, flo));

    flo->itemPos[0] = mousePos.x();
    flo->itemPos[1] = mousePos.y();

    setNewflochaIdName(flo);
    setBtnTextByflocha(btn,flo);


    std::list<FlowchartsTemplate *> flochalist;
    flochalist.clear();
    flochalist.push_back(flo);

    //
    //floActStackForUndo.push_back(std::make_pair(flochalist,flochaChangeAdd));
    //do not push to container directly

    pushFlochaDataToReUndo(flochalist, pushToUndoStack, flochaChangeAdd);


    emit flochaIsChanged(flo);
}



void FlowchartWidget::addItemSleep()
{
    qDebug() << "addrun";
    QPushButton *btn = new QPushButton(this);
    buttonList.push_back(btn);
    btn->installEventFilter(this);
    btn->move(mousePos);
    btn->show();
    FlowchartsTemplate *flo = new FlowchartsSleep(flochaAssem);
    //	flo->setMpiController(commandManager);
    flochaAssem->addTemplate(flo); 
    button_flocha_map.insert(std::make_pair(btn, flo));

    flo->itemPos[0] = mousePos.x();
    flo->itemPos[1] = mousePos.y();

    setNewflochaIdName(flo);
    setBtnTextByflocha(btn,flo);


    std::list<FlowchartsTemplate *> flochalist;
    flochalist.clear();
    flochalist.push_back(flo);

    //
    //floActStackForUndo.push_back(std::make_pair(flochalist,flochaChangeAdd));
    //do not push to container directly

    pushFlochaDataToReUndo(flochalist, pushToUndoStack, flochaChangeAdd);


    emit flochaIsChanged(flo);
}



void FlowchartWidget::addItemAssem()
{
    qDebug() << "addassem";
    QPushButton *btn = new QPushButton(this);
    buttonList.push_back(btn);
    btn->installEventFilter(this);
    btn->move(mousePos);
    btn->show();
    FlowchartsTemplate *flo = new FlowchartsAssemble(flochaAssem, commandManager);
    //	flo->setMpiController(commandManager);
    flochaAssem->addTemplate(flo); 
    button_flocha_map.insert(std::make_pair(btn, flo));

    flo->itemPos[0] = mousePos.x();
    flo->itemPos[1] = mousePos.y();

    setNewflochaIdName(flo);
    setBtnTextByflocha(btn,flo);

    std::list<FlowchartsTemplate *> flochalist;
    flochalist.clear();
    flochalist.push_back(flo);

    //
    //floActStackForUndo.push_back(std::make_pair(flochalist,flochaChangeAdd));
    //do not push to container directly

    pushFlochaDataToReUndo(flochalist, pushToUndoStack, flochaChangeAdd);


    emit flochaIsChanged(flo);
}



//FlowchartsAssemble* FlowchartWidget::copyFlochaAssem(FlowchartsAssemble *src)
//{
//	return NULL;
//}


void FlowchartWidget::flochaPopupMenu(const QPoint &point)
{
    if(!flochaNeedPopMenu)
        return;

    if(!ui_act_editAssemSetting)
    {
        ui_act_editAssemSetting = new QAction(this);
        ui_act_editAssemSetting->setText(tr("Edit assem setting"));
        connect(ui_act_editAssemSetting, &QAction::triggered, this, &FlowchartWidget::act_editAssemSetting);
    }
    if(!ui_act_openAssemInTab)
    {
        ui_act_openAssemInTab = new QAction(this);
        ui_act_openAssemInTab->setText(tr("Open assem in new tab"));
        connect(ui_act_openAssemInTab, &QAction::triggered, this, &FlowchartWidget::act_openAssemInTab);
    }
    if(!ui_act_putAssemToItemBox)
    {
        ui_act_putAssemToItemBox = new QAction(this);
        ui_act_putAssemToItemBox->setText(tr("Put assem to item box"));
        connect(ui_act_putAssemToItemBox, &QAction::triggered, this, &FlowchartWidget::act_putAssemToItemBox);
    }
    if(!ui_act_dispelAssem)
    {
        ui_act_dispelAssem = new QAction(this);
        ui_act_dispelAssem->setText(tr("Dispel assem"));
        connect(ui_act_dispelAssem, &QAction::triggered, this, &FlowchartWidget::act_dispelAssem);
    }


    QMenu menu(this);


    menu.addAction(ui_act_editAssemSetting);
    menu.addSeparator();
    menu.addAction(ui_act_openAssemInTab);
    menu.addSeparator();
    menu.addAction(ui_act_putAssemToItemBox);
    menu.addSeparator();
    menu.addAction(ui_act_dispelAssem);
    menu.addSeparator();

    QPoint ptPop=this->mapToGlobal(point);

    menu.exec(ptPop);

}

void FlowchartWidget::act_editAssemSetting()
{
    if(flochaNeedPopMenu)
    {
        FloDataInputDlg inputdlg(this, flochaNeedPopMenu);
        inputdlg.exec();
    }


}



void FlowchartWidget::act_openAssemInTab()
{
    FlowchartsAssemble *floassem = dynamic_cast<FlowchartsAssemble*>(flochaNeedPopMenu);
    if(floassem)
        emit subFlochaAssemOpenSignal(floassem);
}



void FlowchartWidget::act_putAssemToItemBox()
{


}



void FlowchartWidget::act_dispelAssem()
{


}


std::list<FlowchartsTemplate*> FlowchartWidget::fullCopyFlochaList(const std::list<FlowchartsTemplate*> &copyflochaList,FlowchartsAssemble*dstparent, FlowchartsAssemble*srcparent)
{
    std::list<FlowchartsTemplate*> rtnlist;
    std::map<FlowchartsTemplate*, FlowchartsTemplate*> src_dst_map;
    rtnlist.clear();
    src_dst_map.clear();

    for(auto it = copyflochaList.begin(); it != copyflochaList.end(); ++it)
    {
        FlowchartsTemplate* dstflocha;
        if((*it)->getflochaType() == flochaTypeTemplate)
        {
            if(QString("%1").arg((*it)->idName) == XmlEleTagValueStart)
            {
                dstflocha = dstparent->getStartFlocha();
            }
            else
            {
                dstflocha = dstparent->getEndFlocha();
            }
            paste_copyFlochaData(dstflocha, *it, dstparent, srcparent, false,true);
        }
        else
        {
            dstflocha = paste_copyFlochaData(NULL, *it, dstparent, srcparent, false,true);
            dstparent->addTemplate(dstflocha);
        }
        src_dst_map.insert(std::make_pair(*it, dstflocha));
        rtnlist.push_back(dstflocha);
    }

    for(auto it = copyflochaList.begin(); it != copyflochaList.end(); ++it)
    {

        auto it_next = src_dst_map.find((*it)->getNextPtr());
        auto it_else = src_dst_map.find((*it)->getElsePtr());

        if(it_next != src_dst_map.end())
            dstparent->linkTwoTemplate(src_dst_map.find(*it)->second, it_next->second);

        if(it_else != src_dst_map.end())
            dstparent->linkTwoTemplate(src_dst_map.find(*it)->second, it_else->second,true);
    }



    return rtnlist;

}









#ifdef _DEBUG
void FlowchartWidget::printFlochaDebugMsg(FlowchartsTemplate *flocha)
{
    qDebug() << "printFlochaDebugMsg flocha idname:" << flocha->idName;
    qDebug() << "printFlochaDebugMsg flocha itempos x:" << flocha->itemPos[0] << "flocha itempos y:" << flocha->itemPos[1];
    qDebug() << "printFlochaDebugMsg  flocha type:" << flocha->getflochaType();

    qDebug() << "printFlochaDebugMsg  p_next:" << flocha->getNextPtr();
    qDebug() << "printFlochaDebugMsg  p_else:" << flocha->getElsePtr();

    //switch(flocha->getflochaType())

}

#endif
