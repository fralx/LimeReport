#ifndef LRIMAGEITEMEDITOR_H
#define LRIMAGEITEMEDITOR_H

#include <QWidget>

namespace Ui {
class ImageItemEditor;
}

namespace LimeReport {
class ImageItem;
}

class ImageItemEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ImageItemEditor(LimeReport::ImageItem* item, QWidget *parent = NULL);
    ~ImageItemEditor();
private:
    void updateImage();
private:
    Ui::ImageItemEditor *ui;
    LimeReport::ImageItem* m_item;
    QPixmap m_image;
    QPixmap m_resourcePathImage;
private slots:
    void on_tbLoadImage_clicked();
    void on_tbClearImage_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_toolButton_clicked();
    void on_tbResourcePath_clicked();
};

#endif // LRIMAGEITEMEDITOR_H
