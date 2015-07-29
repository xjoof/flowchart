#ifndef ITEMBOXWIDGET_H
#define ITEMBOXWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QIcon>
#include <QEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QDrag>
#include <QPixmap>


#ifdef _DEBUG
#include <QDebug>
#endif


#include "flowchartsitems.h"
//#include "globalobjmanager.h"

//extern GlobalObjManager *globalObjManager;


#define EXPAN_COLAP_ICONSIZE 64

class ItemBoxMimeData : public QMimeData
{
public:
    ItemBoxMimeData();
    ~ItemBoxMimeData();

    FlowchartsAssemble *getFlochaAssemPtr() const;
    int getNewFlochaType() const;

    void setFlochaAssemPtr(FlowchartsAssemble *assem);
    void setNewFlochaType(int type);

private:

    FlowchartsAssemble *mimeAssem;
    int flochaType;

};

//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------



class ItemListWidget : public QListWidget
{
    Q_OBJECT

public:
    ItemListWidget(QWidget *parent);
    ~ItemListWidget();

    void addFlochaItem(QString itemtext, FlowchartsAssemble *assem, int basictype = -1);

private:

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    //	void mouseReleaseEvent(QMouseEvent *event);

    //	void dragEnterEvent(QDragEnterEvent *event);
    //   void dragMoveEvent(QDragMoveEvent *event);
    //    void dropEvent(QDropEvent *event);
    void performDrag();


    std::map<QListWidgetItem*, std::pair<FlowchartsAssemble*, int> > listitem_mimedata_map;

    QPoint mouseStartP;
    QPoint mouseP;
};


//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------

class ItemBoxWidget : public QWidget
{
    Q_OBJECT

public:
    ItemBoxWidget(QWidget *parent);
    ~ItemBoxWidget();

    void setTextNeedTrans();

private:

    //----------------------drag and drop----------------------
    //	bool eventFilter(QObject* watched, QEvent* event);
    //    void mousePressEvent(QMouseEvent *event);
    //	void mouseMoveEvent(QMouseEvent *event);
    //	void mouseReleaseEvent(QMouseEvent *event);

    //	void dragEnterEvent(QDragEnterEvent *event);
    //  void dragMoveEvent(QDragMoveEvent *event);
    //    void dropEvent(QDropEvent *event);
    //----------------------drag and drop----------------------
    void createBox_basic();
    void createBox_tool();
    void createBox_driveFunc();
    void createBox_userDefine();
    void createBox_test();

    void createBtnIcon();
    void expand_collapse_list();

    ItemListWidget *createItemListWidget(const QString &category);


    QPixmap *expandIcon;
    QPixmap *collapseIcon;

    QVBoxLayout *mainLayout;

    std::map<QPushButton*, ItemListWidget*> btn_ListWidget_map;



};

#endif // ITEMBOXWIDGET_H
