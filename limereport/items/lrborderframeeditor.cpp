#include "lrborderframeeditor.h"
#include "ui_lrborderframeeditor.h"
#include <QPainter>
#include <QGraphicsLineItem>
#include <QDebug>
#include <QMouseEvent>
#include "lrbasedesignintf.h"
#include "lrbordereditor.h"

namespace LimeReport{

BorderFrameEditor::BorderFrameEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BorderFrameEditor)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(ui->graphicsView);

    QRect vRect = rect();

    //Draw corder lines
    //topLeft
    scene->addLine(10, 5, 10, 10, QPen(Qt::gray));
    scene->addLine(5, 10, 10, 10, QPen(Qt::gray));
    //bottomLeft
    scene->addLine(10,vRect.bottom() -5, 10, vRect.bottom()-10, QPen(Qt::gray));
    scene->addLine(5,vRect.bottom()-10, 10, vRect.bottom()-10, QPen(Qt::gray));
    //bottomRight
    scene->addLine(vRect.right() - 10, vRect.bottom() - 5, vRect.right()- 10, vRect.bottom() - 10, QPen(Qt::gray));
    scene->addLine(vRect.right() - 5, vRect.bottom() - 10, vRect.right() - 10, vRect.bottom() - 10, QPen(Qt::gray));
    //topRight
    scene->addLine(vRect.width() - 10, 5, vRect.width() - 10, 10, QPen(Qt::gray));
    scene->addLine(vRect.width() - 5, 10, vRect.width() - 10, 10, QPen(Qt::gray));
    scene->setSceneRect(vRect);
    ui->graphicsView->setScene(scene);
    QGraphicsSimpleTextItem * io = new QGraphicsSimpleTextItem();
    io->setAcceptedMouseButtons(Qt::LeftButton);
    io->setPos(scene->sceneRect().center());
    io->setText(tr("Text"));
    scene->addItem(io);

    QRectF bR = io->sceneBoundingRect();
    io->setPos( scene->sceneRect().center().x() - bR.width()/2, scene->sceneRect().center().y() - bR.height()/2 );
    connect(
        this, SIGNAL(borderSideClicked(LimeReport::BaseDesignIntf::BorderSide, bool)),
        this, SLOT(slotBorderSideClicked(LimeReport::BaseDesignIntf::BorderSide, bool))
    );

}

BorderFrameEditor::~BorderFrameEditor()
{
    delete ui;
}

void BorderFrameEditor::setPen(QPen pen)
{
    m_pen = pen;
    updateBorders();
}

QPen BorderFrameEditor::pen()
{
    return m_pen;
}

void BorderFrameEditor::setAllLines()
{
    if (!topLine)
        topLine = createSideLine(LimeReport::BaseDesignIntf::BorderSide::TopLine);
    if (!leftLine)
        leftLine = createSideLine(LimeReport::BaseDesignIntf::BorderSide::LeftLine);
    if (!bottomLine)
        bottomLine = createSideLine(LimeReport::BaseDesignIntf::BorderSide::BottomLine);
    if (!rightLine)
        rightLine = createSideLine(LimeReport::BaseDesignIntf::BorderSide::RightLine);
}

void BorderFrameEditor::unSetAllLines()
{
    if (topLine){
        scene->removeItem(topLine);
        topLine = NULL;
    }
    if (leftLine){
        scene->removeItem(leftLine);
        leftLine = NULL;
    }
    if (bottomLine){
        scene->removeItem(bottomLine);
        bottomLine = NULL;
    }
    if (rightLine){
        scene->removeItem(rightLine);
        rightLine = NULL;
    }

}

void BorderFrameEditor::mousePressEvent(QMouseEvent *event)
{
    if (event->x() >= 10 && event->y() <30)
        emit borderSideClicked(BaseDesignIntf::BorderSide::TopLine, !topLine);

    if ((event->x() >= 10 && event->x() < 30) && (event->y() > 10))
        emit borderSideClicked(BaseDesignIntf::BorderSide::LeftLine, !leftLine);

    if (event->x() >= 10 && (event->y() >80 && event->y() < rect().bottom()))
        emit borderSideClicked(BaseDesignIntf::BorderSide::BottomLine, !bottomLine);

    if ((event->x() >= 130 && event->x() < rect().width()) && event->y() > 10)
        emit borderSideClicked(BaseDesignIntf::BorderSide::RightLine, !rightLine);
}

void BorderFrameEditor::slotBorderSideClicked(BaseDesignIntf::BorderSide side, bool show)
{

    switch(side){

    case BaseDesignIntf::BorderSide::TopLine:
        if (show){
            if (!topLine) topLine = createSideLine(side);
        } else {
            if (topLine) scene->removeItem(topLine);
            topLine = NULL;
        }
        break;

    case BaseDesignIntf::LeftLine:
        if (show){
            if (!leftLine) leftLine = createSideLine(side);
        } else {
            if (leftLine) scene->removeItem(leftLine);
            leftLine = NULL;
        }
        break;

    case BaseDesignIntf::BottomLine:
        if (show){
            if (!bottomLine) bottomLine = createSideLine(side);
        } else {
            if (bottomLine) scene->removeItem(bottomLine);
            bottomLine = NULL;
        }
        break;

    case BaseDesignIntf::RightLine:
        if (show){
            if (!rightLine) rightLine = createSideLine(side);
        } else {
            if(rightLine) scene->removeItem(rightLine);
            rightLine = NULL;
        }

    }
    updateBorders();
}

QGraphicsLineItem *BorderFrameEditor::createSideLine(LimeReport::BaseDesignIntf::BorderSide side)
{
    switch(side){
    case BaseDesignIntf::BorderSide::TopLine:
        return scene->addLine(QLineF(10, 10, rect().width() - 10, 10), m_pen);
    case BaseDesignIntf::BorderSide::LeftLine:
        return scene->addLine(QLineF(10, 10, 10, rect().height() - 10), m_pen);
    case BaseDesignIntf::BorderSide::RightLine:
        return scene->addLine(QLineF(rect().width() - 10, 10 ,rect().width() - 10, rect().height() - 10), m_pen);
    case BaseDesignIntf::BorderSide::BottomLine:
        return scene->addLine(QLineF(10, rect().bottom() - 10, rect().width() - 10, rect().bottom() - 10), m_pen);
    }
}

void BorderFrameEditor::updateBorders()
{
    if (topLine) topLine->setPen(m_pen);
    if (leftLine) leftLine->setPen(m_pen);
    if (bottomLine) bottomLine->setPen(m_pen);
    if (rightLine) rightLine->setPen(m_pen);
}

} //namespace LimeReport
