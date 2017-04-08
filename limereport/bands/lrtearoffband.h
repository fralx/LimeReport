#ifndef TEAROFFBAND_H
#define TEAROFFBAND_H
#include "lrbanddesignintf.h"

namespace LimeReport {

class TearOffBand : public BandDesignIntf
{
    Q_OBJECT
public:
    TearOffBand(QObject* owner = 0, QGraphicsItem *parent=0);
    virtual BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0);
protected:
    QColor bandColor() const;
    virtual bool isUnique() const {return true;}
};

} // namespace LimeReport

#endif // TEAROFFBAND_H
