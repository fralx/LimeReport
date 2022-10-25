#include "borderframeeditor.h"
#include "ui_borderframeeditor.h"
#include <QPainter>
#include <QGraphicsLineItem>
#include <QDebug>
#include <QMouseEvent>
#include "lrbasedesignintf.h"
#include "lrbordereditor.h"
using namespace LimeReport;
BorderFrameEditor::BorderFrameEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BorderFrameEditor)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(ui->graphicsView);

    QRect vRect = rect();

    //Draw corder lines
    //topLeft
    scene->addLine(10,5, 10,10,QPen(Qt::gray));
    scene->addLine(5,10, 10,10,QPen(Qt::gray));
    //bottomLeft
    scene->addLine(10,vRect.bottom() -5, 10,vRect.bottom()-10,QPen(Qt::gray));
    scene->addLine(5,vRect.bottom()-10,10,vRect.bottom()-10,QPen(Qt::gray));
    //bottomRight
    scene->addLine(vRect.right()-10,vRect.bottom() -5,vRect.right()- 10,vRect.bottom()-10,QPen(Qt::gray));
    scene->addLine(vRect.right()-5,vRect.bottom()-10,vRect.right()-10,vRect.bottom()-10,QPen(Qt::gray));
    //topRight
    scene->addLine(vRect.width()-10,5,vRect.width()- 10,10,QPen(Qt::gray));
    scene->addLine(vRect.width()-5,10, vRect.width()-10,10,QPen(Qt::gray));
    scene->setSceneRect(vRect);
    ui->graphicsView->setScene(scene);
    QGraphicsSimpleTextItem * io = new QGraphicsSimpleTextItem();
    io->setAcceptedMouseButtons(Qt::LeftButton);
    io->setPos(scene->sceneRect().center());
    io->setText(tr("Text"));
    scene->addItem(io);

    QRectF bR = io->sceneBoundingRect();
    io->setPos( scene->sceneRect().center().x() - bR.width()/2, scene->sceneRect().center().y() - bR.height()/2 );
    connect(this,SIGNAL(borderSideClicked(int, bool)),this,SLOT(SlotBorderSideClicked(int,bool)));

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

        topLine = scene->addLine(QLineF(10,10,rect().width() - 10,10),m_pen);


        leftLine = scene->addLine(QLineF(10,10,10,rect().height() - 10),m_pen);

        bottomLine = scene->addLine(QLineF(10,rect().bottom() -10,rect().width() - 10
                                           ,rect().bottom() - 10),m_pen);



        rightLine = scene->addLine(QLineF(rect().width() - 10,10
                                          ,rect().width() - 10,rect().height() - 10),m_pen);


}

void BorderFrameEditor::unSetAllLines()
{
    if(topLine)
    {
        scene->removeItem(topLine);

    }
    if(leftLine)
    {
        scene->removeItem(leftLine);

    }
    if(bottomLine)
    {
        scene->removeItem(bottomLine);



    }
    if(rightLine)
    {
        scene->removeItem(rightLine);

    }

}

void BorderFrameEditor::mousePressEvent(QMouseEvent *event)
{

    if(event->x() >= 10 && event->y() <30)//Draw top border
    {

        if(!topLine)
        {

            emit borderSideClicked(1,true);

        }
        else
        {

            emit borderSideClicked(1,false);
        }
    }

    if((event->x() >= 10 && event->x() < 30) && (event->y() > 10)) //Draw border left
    {
        if(!leftLine)
        {

            emit borderSideClicked(4,true);

        }
        else
        {

            emit borderSideClicked(4,false);

        }
    }

    if(event->x() >= 10 && (event->y() >80 && event->y() < rect().bottom())) //Draw bottom border
    {
        if(!bottomLine)
        {

            emit borderSideClicked(2,true);
        }

        else
        {

            emit borderSideClicked(2,false);

        }
    }

    if((event->x() >= 130 && event->x() < rect().width()) && event->y() > 10) //Draw border right
    {
        if(!rightLine)
        {

            emit borderSideClicked(8,true);

        }
        else
        {

            emit borderSideClicked(8,false);

        }

    }


}

void BorderFrameEditor::SlotBorderSideClicked(int side, bool show)
{

    switch(side)
    {
    case BaseDesignIntf::BorderSide::TopLine:
    {
        if(show)
        {
            topLine = scene->addLine(QLineF(10,10,rect().width() - 10,10),m_pen);


        }
        else
        {
            scene->removeItem(topLine);
            topLine = NULL;

        }
    }break;
    case BaseDesignIntf::LeftLine:
    {
        if(show)
        {
            leftLine = scene->addLine(QLineF(10,10,10,rect().height() - 10),m_pen);


        }
        else
        {
            scene->removeItem(leftLine);
            leftLine = NULL;


        }
    }break;
    case BaseDesignIntf::BottomLine:
    {
        if(show)
        {
            bottomLine = scene->addLine(QLineF(10,rect().bottom() -10,rect().width() - 10
                                               ,rect().bottom() - 10),m_pen);

        }

        else
        {
            scene->removeItem(bottomLine);
            bottomLine = NULL;


        }
    }break;
    case BaseDesignIntf::RightLine:
    {
        if(show)
        {
            rightLine = scene->addLine(QLineF(rect().width() - 10,10
                                              ,rect().width() - 10,rect().height() - 10),m_pen);

        }
        else
        {
            scene->removeItem(rightLine);
            rightLine = NULL;

        }

    }

    }
    updateBorders();
}

void BorderFrameEditor::updateBorders()
{
    //if a line is set we redraw it
    if(topLine)
    {
        scene->removeItem(topLine);
        topLine = scene->addLine(QLineF(10,10,rect().width() - 10,10),m_pen);

    }
    if(leftLine)
    {
        scene->removeItem(leftLine);
        leftLine = scene->addLine(QLineF(10,10,10,rect().height() - 10),m_pen);

    }
    if(bottomLine)
    {
        scene->removeItem(bottomLine);

        bottomLine = scene->addLine(QLineF(10,rect().bottom() -10,rect().width() - 10
                                           ,rect().bottom() - 10),m_pen);

    }
    if(rightLine)
    {
        scene->removeItem(rightLine);

        rightLine = scene->addLine(QLineF(rect().width() - 10,10
                                          ,rect().width() - 10,rect().height() - 10),m_pen);

    }

}

