#include "lrsvgeditor.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include "lrimageeditor.h"

namespace LimeReport{

SvgEditor::SvgEditor(QWidget* parent)
    :QWidget(parent)
{
    m_button.setIcon(QIcon(":items/ImageItem"));
    m_clearButton.setIcon(QIcon(":items/clear.png"));
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(&m_button);
    layout->addWidget(&m_clearButton);
    layout->setSpacing(1);
    layout->setContentsMargins(1,0,1,1);
    setLayout(layout);
    setFocusProxy(&m_button);
    setAutoFillBackground(true);
    connect(&m_button,SIGNAL(clicked()),this,SLOT(slotButtonClicked()));
    connect(&m_clearButton,SIGNAL(clicked()),this,SLOT(slotClearButtonClicked()));
}

QByteArray SvgEditor::image()
{
    return m_image;
}

void SvgEditor::slotButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select image file"), "", "SVG (*.svg)");
    if (!fileName.isEmpty()){
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)){
            m_image = file.readAll();
        }
    }
    emit editingFinished();
}

void SvgEditor::slotClearButtonClicked()
{
    m_image = QByteArray();
    emit editingFinished();
}

}
