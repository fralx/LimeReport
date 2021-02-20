#ifndef SVGITEM_H
#define SVGITEM_H

#include "lritemdesignintf.h"
#include "lreditableimageitemintf.h"

namespace LimeReport{
class SVGItem: public ItemDesignIntf, public IEditableImageItem
{
    Q_OBJECT
    Q_PROPERTY(QString resourcePath READ resourcePath WRITE setResourcePath)
    Q_PROPERTY(QByteArray image READ image WRITE setImage)
    Q_PROPERTY(QString datasource READ datasource WRITE setDatasource)
    Q_PROPERTY(QString field READ field WRITE setField)
    Q_PROPERTY(int opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(QString variable READ variable WRITE setVariable)
    Q_PROPERTY(bool watermark READ isWatermark WRITE setWatermark)
public:
    SVGItem(QObject *owner, QGraphicsItem *parent);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QByteArray imageAsByteArray() const;
    void setImageAsByteArray(QByteArray image);
    QString fileFilter() const;

    void preparePopUpMenu(QMenu &menu);
    void processPopUpAction(QAction *action);
    QWidget* defaultEditor();

    QString resourcePath() const;
    void setResourcePath(const QString &resourcePath);
    QByteArray image() const;
    void setImage(const QByteArray &image);
    QString datasource() const;
    void setDatasource(const QString &datasource);
    QString field() const;
    void setField(const QString &field);
    QString variable() const;
    void setVariable(const QString &variable);
    bool isNeedUpdateSize(RenderPass) const;
protected:
    BaseDesignIntf *createSameTypeItem(QObject *owner, QGraphicsItem *parent);
    void updateItemSize(DataSourceManager *dataManager, RenderPass pass, int maxHeight);
    QByteArray imageFromResource(QString resourcePath);
private:
    QString m_resourcePath;
    QByteArray m_image;
    QString m_datasource;
    QString m_field;
    QString m_variable;
public:

};
} // namespace LimeReport
#endif // SVGITEM_H
