#include "lraxisdata.h"

namespace LimeReport {
AxisData::AxisData()
    : m_rangeMin(0), m_rangeMax(0),
      m_minValue(0), m_maxValue(0), m_step(0),
      m_delta(0), m_segmentCount(4)
{

}

AxisData::AxisData(qreal minValue, qreal maxValue)
    : AxisData()
{
    m_minValue = minValue;
    m_maxValue = maxValue;
    calculateValuesAboveMax(minValue, maxValue, 4);
    m_delta = m_step * m_segmentCount;
}

int AxisData::segmentCount() const
{
    return m_segmentCount;
}

qreal AxisData::rangeMin() const
{
    return m_rangeMin;
}

qreal AxisData::rangeMax() const
{
    return m_rangeMax;
}

qreal AxisData::minValue() const
{
    return m_minValue;
}

qreal AxisData::maxValue() const
{
    return m_maxValue;
}

qreal AxisData::step() const
{
    return m_step;
}

qreal AxisData::delta() const
{
    return m_delta;
}

void AxisData::calculateValuesAboveMax(qreal minValue, qreal maxValue, int segments)
{
    const int delta = maxValue - minValue;
    int max = delta;
    while (max % segments != 0){
        max++;
    }
    m_rangeMax = max;
    m_step = max / segments;
    m_rangeMin = minValue;
    m_segmentCount = segments;
}
}
