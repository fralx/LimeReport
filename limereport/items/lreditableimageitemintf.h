#ifndef LREDITABLEIMAGEITEMINTF_H
#define LREDITABLEIMAGEITEMINTF_H

#include <QByteArray>

namespace LimeReport {

class IEditableImageItem{
public:
    virtual QByteArray imageAsByteArray() const = 0;
    virtual void setImageAsByteArray(QByteArray image) = 0;
    virtual QString resourcePath() const  = 0;
    virtual void setResourcePath(const QString &value) = 0;
    virtual QString fileFilter() const = 0;
};

}


#endif // LREDITABLEIMAGEITEMINTF_H
