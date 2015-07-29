#ifndef TRANSFLOCHATOCODE_H
#define TRANSFLOCHATOCODE_H

#include <queue>


#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QTextEdit>
#include <QToolBar>
#include <QAction>

#include "flowchartsitems.h"

#define PDLLABELPREFIX QString("flochaLabel_")

#define BINTREENODEARRAYNUM 4095
#define FLOATTRARRAYNUM 8191

#define GETNEXTBINSIBLING(node) if(node->parent->rightNdoeP == node) node = node->parent->leftNodeP; else node = nullptr;


enum
{
    CodeType_C,
    CodeType_CPP,
    CodeType_JAVA,
    CodeType_PDL,



};




typedef struct
{

}CodeSemantics;



typedef struct
{
}CodeSyntax;


typedef struct
{
    int varNameMaxSize;


    bool hasGoto;//c/c++/PDL
    bool hasPointer;//c/c++
    bool hasReference;//c++
    bool hasBreakContinu;//c/c++/java, not include PDL
    bool hasBrkContiLable;//break and continue label(java)




}CodeRuleStruct;



typedef struct __BinTreeNodeForTrans BinTreeNodeForTrans;
typedef struct __FlochaAttriForTrans FlochaAttriForTrans;


struct __BinTreeNodeForTrans
{
    int flochaNum;
    
    FlochaAttriForTrans *startFloAttr;
    FlochaAttriForTrans *endFloAttr;

    BinTreeNodeForTrans *rightNodeP;//true
    BinTreeNodeForTrans *leftNodeP;//false
    BinTreeNodeForTrans *parent;
};





struct __FlochaAttriForTrans
{

    bool needLabel;
    bool twoPreAreSibling;
    //unsigned int nodeNum;
    QString label;

    FlowchartsTemplate *flocha;

    FlochaAttriForTrans *nextAttr;
    FlochaAttriForTrans *elseAttr;

    int idxInNode;
    BinTreeNodeForTrans *treeNode;
    FlochaAttriForTrans *subAttr;

    std::list<FlochaAttriForTrans*> preList;
    

};



class FlochaCodeTransfer : public QObject
{
    Q_OBJECT

public:
    FlochaCodeTransfer(QObject *parent);
    ~FlochaCodeTransfer();

    void startTransToCode(FlowchartsAssemble *assem, QStringList &code);
    void transToRtxRunStruct(FlowchartsAssemble *assem);

    void setCodeType(int type);
    void setCustomCodeStyle(CodeRuleStruct rule);

private:
    
    void buildAttrArray_prevList(FlowchartsAssemble* assem);

    void initTreeNodeStruct();
    
    void buildDataForTrans(FlochaAttriForTrans *floattr);

    void addToPreMap(FlowchartsTemplate *pre, FlowchartsTemplate *flocha, bool iselse);//use in buildAttrArray_prevList(...)

    //void parseFlochaAtrri(FlowchartsAssemble* assem);




#ifdef _DEBUG
    void printParseAttriResult();
#endif
    void transToPdlCode(BinTreeNodeForTrans *treenode);

    void transToPsudoCode(FlowchartsAssemble* assem);
    void transToActualCode();



    int codeType;

    int labelNum;


    BinTreeNodeForTrans treeNodeArray[BINTREENODEARRAYNUM];
    int treeNodeIdx;
    BinTreeNodeForTrans *treeRoot;

    FlochaAttriForTrans floAttrArray[FLOATTRARRAYNUM];
    FlochaAttriForTrans *rootFloAttr;
    int floAttrIdx;
    //std::map<FlowchartsTemplate*, FlochaAttriForCode> flocahAttriMap;//every flocha will insert to this map, because FlowchartsTemplate did not has previous flocha infomation

    //std::map<FlowchartsTemplate*, QString> flochaLabelMap;

    //std::map<FlowchartsTemplate*, std::list<FlowchartsTemplate*> > flocahLoopMap;//because FlowchartsTemplate did not has previous flocha infomation

    FlowchartsAssemble *objectAssem;
    
    QStringList psudoCode;
    QStringList actualCode;
    

    CodeRuleStruct codeRule;
};


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


class CodeToHiliHtml: public QObject
{
    Q_OBJECT
public:
    CodeToHiliHtml(QObject *parent);
    ~CodeToHiliHtml();

    void startCodeToHiliHtml(const QStringList &src, QStringList &dest);

private:

    int replaceString(QString &srcstr, const QString &fromstr, const QString &tostr);

    void addStringHtmlColor(QString &srcstr);



    std::map<QString, QString> specialCharMap;
    
    std::vector<QString> keywordList;
    std::vector<QString> typeClaimList;

    QString keywordColor;
    QString typeClaimColor;
    QString commentColor;
    QString digitColor;
    QString stringColor;
    QString fontEnd;
};

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


class CodeEditorParser: public QTextEdit
{
    Q_OBJECT
public:
    CodeEditorParser(QWidget *parent);
    ~CodeEditorParser();

    void setCodeThenDraw(const QStringList &code);

private:



};



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


class FlochaToCodeWidget : public QWidget
{
    Q_OBJECT

public:
    FlochaToCodeWidget(QWidget *parent);
    ~FlochaToCodeWidget();

    void setFlochaAssem(FlowchartsAssemble *assem);


    void setTextNeedTrans();

private:

    void createUI();//only call in constructor

    void startGenCode();



    ///////ui

    QGridLayout *mainLayout;


    QAction *ui_act_genCode;

    QToolBar *ui_toolBar;

    CodeEditorParser *ui_textEdit_Code;
    



    ///////ui


    FlowchartsAssemble *objectAssem;

    FlochaCodeTransfer *codeTranslator;

};


#endif // TRANSFLOCHATOCODE_H
