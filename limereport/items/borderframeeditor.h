#ifndef WIDGET
#define WIDGET

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include "lrbasedesignintf.h"
QT_BEGIN_NAMESPACE
namespace Ui { class BorderFrameEditor; }
QT_END_NAMESPACE

class BorderFrameEditor : public QWidget
{
    Q_OBJECT

public:
    BorderFrameEditor(QWidget *parent = nullptr);
    ~BorderFrameEditor();
    void setPen(QPen pen);
    QPen pen();
    void setAllLines();
    void unSetAllLines();
protected:
    void mousePressEvent(QMouseEvent *event);
signals:
    void borderSideClicked(int side,bool show);
private slots:
    void SlotBorderSideClicked(int side, bool show);

private:
    Ui::BorderFrameEditor *ui;
        QGraphicsScene *scene;
        QGraphicsLineItem *topLine = NULL
                ,*bottomLine = NULL
                ,*leftLine = NULL
                ,*rightLine = NULL;
        QPen m_pen;
        void updateBorders();


};
#endif // WIDGET
