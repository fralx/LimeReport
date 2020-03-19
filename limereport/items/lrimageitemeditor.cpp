#include "lrimageitemeditor.h"
#include "ui_lrimageitemeditor.h"
#include "lrimageitem.h"

#include <QFileInfo>
#include <QFileDialog>

ImageItemEditor::ImageItemEditor(LimeReport::IEditableImageItem *item, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageItemEditor), m_item(item)
{
    ui->setupUi(this);
    m_image = item->imageAsByteArray();
    ui->resourcePath->setText(m_item->resourcePath());
    updateImage();
}

ImageItemEditor::~ImageItemEditor()
{
    delete ui;
}

void ImageItemEditor::updateImage()
{
    QPixmap image;
    if (m_image.isEmpty() && !ui->resourcePath->text().isEmpty()){
        image.load(ui->resourcePath->text());
    } else {
        image.loadFromData(m_image);
    }
    ui->imageViewer->setPixmap(image);
}

void ImageItemEditor::on_tbLoadImage_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select image file"), "", m_item->fileFilter());
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)){
        m_image = file.readAll();
    }
    updateImage();
}

void ImageItemEditor::on_tbClearImage_clicked()
{
    m_image.clear();
    updateImage();
}

void ImageItemEditor::on_buttonBox_accepted()
{
    QImage image;
    m_item->setImageAsByteArray(m_image);
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
