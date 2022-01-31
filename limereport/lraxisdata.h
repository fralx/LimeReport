#ifndef AXISDATA_H
#define AXISDATA_H

#include <QtGlobal>

namespace LimeReport {
class AxisData
{
public:
    AxisData();
    AxisData(qreal minValue, qreal maxValue);

    int segmentCount() const;

    qreal rangeMin() const;
    qreal rangeMax() const;

    qreal minValue() const;
    qreal maxValue() const;
    qreal step() const;

    qreal delta() const;

private:
    void calculateValuesAboveMax(qreal minValue, qreal maxValue, int segments);

    qreal m_rangeMin;
    qreal m_rangeMax;
    qreal m_minValue;
    qreal m_maxValue;
    qreal m_step;
    qreal m_delta;
    int m_segmentCount;
};
};

#endif // AXISDATA_H
