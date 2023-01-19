#ifndef WIDGET
#define WIDGET

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include "lrbasedesignintf.h"
namespace LimeReport{

namespace Ui { class BorderFrameEditor; }

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
    void borderSideClicked(LimeReport::BaseDesignIntf::BorderSide side, bool show);
private slots:
    void slotBorderSideClicked(LimeReport::BaseDesignIntf::BorderSide side, bool show);

private:
    QGraphicsLineItem *createSideLine(LimeReport::BaseDesignIntf::BorderSide side);
    void updateBorders();
private:
    Ui::BorderFrameEditor *ui;
    QGraphicsScene *scene;
    QGraphicsLineItem *topLine = NULL
                ,*bottomLine = NULL
                ,*leftLine = NULL
                ,*rightLine = NULL;
    QPen m_pen;



};
} // namespace LimeReport
#endif // WIDGET
