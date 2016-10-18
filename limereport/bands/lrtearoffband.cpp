#include "lrtearoffband.h"
#include "lrdesignelementsfactory.h"
#include "lrglobal.h"

const QString xmlTag ="TearOffBand";

namespace{
LimeReport::BaseDesignIntf * createBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::TearOffBand(owner,parent);
}
bool VARIABLE_IS_NOT_USED registred = LimeReport::DesignElementsFactory::instance().registerCreator(
        xmlTag,
        LimeReport::ItemAttribs(QObject::tr("Tear-off Band"),LimeReport::Const::bandTAG),
        createBand
    );
}

namespace LimeReport{

TearOffBand::TearOffBand(QObject *owner, QGraphicsItem *parent)
    :BandDesignIntf(LimeReport::BandDesignIntf::TearOffBand,xmlTag,owner,parent)
{
    setBandTypeText(tr("Tear-off Band"));
    setMarkerColor(bandColor());
}

BaseDesignIntf *TearOffBand::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new TearOffBand(owner,parent);
}

QColor TearOffBand::bandColor() const
{
    return QColor(200,200,200);
}

} // namedpace LimeReport
