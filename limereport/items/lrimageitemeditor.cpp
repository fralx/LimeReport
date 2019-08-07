#include "lrimageitemeditor.h"
#include "ui_lrimageitemeditor.h"
#include "lrimageitem.h"

#include <QFileInfo>
#include <QFileDialog>

ImageItemEditor::ImageItemEditor(LimeReport::ImageItem *item, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageItemEditor), m_item(item)
{
    ui->setupUi(this);
    m_image = QPixmap::fromImage(m_item->image());
    ui->resourcePath->setText(m_item->resourcePath());
    updateImage();
}

ImageItemEditor::~ImageItemEditor()
{
    delete ui;
}

void ImageItemEditor::updateImage()
{
    ui->imageViewer->setPixmap(m_image);
    if (m_image.isNull() && !ui->resourcePath->text().isEmpty()){
        if (m_resourcePathImage.isNull())
            m_resourcePathImage = QPixmap(ui->resourcePath->text());
        ui->imageViewer->setPixmap(m_resourcePathImage);
    }
}

void ImageItemEditor::on_tbLoadImage_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select image file"));
    m_image = QPixmap(fileName);
    updateImage();
}

void ImageItemEditor::on_tbClearImage_clicked()
{
    m_image = QPixmap();
    updateImage();
}

void ImageItemEditor::on_buttonBox_accepted()
{
    m_item->setImage(m_image.toImage());
    m_item->setResourcePath(ui->resourcePath->text());
    this->close();
}

void ImageItemEditor::on_buttonBox_rejected()
{
    this->close();
}

void ImageItemEditor::on_toolButton_clicked()
{
    ui->resourcePath->setText(QFileDialog::getOpenFileName(this, tr("Select image file")));
    m_resourcePathImage = QPixmap(ui->resourcePath->text());
    if (!m_resourcePathImage.isNull() && m_image.isNull())
        ui->imageViewer->setPixmap(m_resourcePathImage);
}

void ImageItemEditor::on_tbResourcePath_clicked()
{
    ui->resourcePath->setText("");
    updateImage();
}
