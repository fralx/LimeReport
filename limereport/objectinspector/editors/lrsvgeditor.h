#ifndef SVGEDITOR_H
#define SVGEDITOR_H
#include <QWidget>
#include <QPushButton>

namespace LimeReport{

class SvgEditor : public QWidget
{
    Q_OBJECT

public:
    SvgEditor(QWidget *parent=0);
    QByteArray image();
    void setImage(const QByteArray& image){m_image=image;}
signals:
    void editingFinished();
private slots:
    void slotButtonClicked();
    void slotClearButtonClicked();
private:
    QPushButton m_button;
    QPushButton m_clearButton;
    QByteArray  m_image;
};

} // namespace LimeReport

#endif // SVGEDITOR_H
