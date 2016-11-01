#ifndef COLORINDICATOR_H
#define COLORINDICATOR_H

#include <QWidget>
#include <QPaintEvent>

class ColorIndicator : public QWidget{
    Q_OBJECT
public:
    ColorIndicator(QWidget* parent = 0);
    QColor color() const;
    void setColor(const QColor &color);
    QSize sizeHint() const;
protected:
    void paintEvent(QPaintEvent *event);
private:
    QColor m_color;
};

#endif // COLORINDICATOR_H
