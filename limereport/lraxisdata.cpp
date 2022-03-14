#include "lraxisdata.h"

#include <cmath>
#include <QDebug>

namespace LimeReport {
AxisData::AxisData(AxisType type, QObject *parent)
    : QObject(parent), m_rangeMin(0), m_rangeMax(0),
      m_minValue(0), m_maxValue(0), m_step(0),
      m_delta(0), m_segmentCount(4), m_calculateAxisScale(false),
      m_reverseDirection(false), m_manualMaximum(0),
      m_manualMinimum(0), m_manualStep(0), m_isMaximumAutomatic(true),
      m_isMinimumAutomatic(true), m_isStepAutomatic(true),
      m_type(type)
{
}

QString AxisData::toString() const
{
    // Just for debug purposes
    QString str;
    QTextStream stream(&str);
    stream << "{ "
           << "min: " << m_minValue << ", max: " << m_maxValue << ", step: " << m_step
           << ", range min: " << m_rangeMin << ", range max: " << m_rangeMax << ", segments: " << m_segmentCount
           << ", reverseDiection: " << m_reverseDirection << ", calculateAxisScale: " << m_calculateAxisScale
           << ", manualMaxEnabled: " << !m_isMaximumAutomatic << ", manualMinEnabled: " << !m_isMinimumAutomatic
           << ", manualStepEnabled: " << !m_isStepAutomatic << ", manualMax: " << m_manualMaximum
           << ", manualMin: " << m_manualMinimum << ", manualStep: " << m_manualStep
           << " }";
    return str;
}

void AxisData::copy(AxisData *other)
{
    m_calculateAxisScale = other->calculateAxisScale();
    m_reverseDirection = other->reverseDirection();
    m_manualMaximum = other->manualMaximum();
    m_manualMinimum = other->manualMinimum();
    m_manualStep = other->manualStep();
    m_isMaximumAutomatic = other->isMaximumAutomatic();
    m_isMinimumAutomatic = other->isMinimumAutomatic();
    m_isStepAutomatic = other->isStepAutomatic();
}

void AxisData::update()
{
    if (m_calculateAxisScale) {
        calculateRoundedAxisScale();
    } else {
        calculateSimpleAxisScale();
    }
    m_delta = m_step * m_segmentCount;

    // Update manual values if they are automatic
    if (m_isStepAutomatic) {
        m_manualStep = m_step;
    }
    if (m_isMinimumAutomatic) {
        m_manualMinimum = m_rangeMin;
    }
    if (m_isMaximumAutomatic) {
        m_manualMaximum = m_rangeMax;
    }
}

void AxisData::update(qreal minValue, qreal maxValue)
{
    m_minValue = minValue;
    m_maxValue = maxValue;
    update();
}

void AxisData::updateForDesignMode()
{
    m_minValue = 0;
    m_maxValue = 40;
    const bool tmp = m_calculateAxisScale;
    m_calculateAxisScale = false;
    update();
    m_calculateAxisScale = tmp;
}

int AxisData::segmentCount() const
{
    return m_segmentCount;
}

bool AxisData::calculateAxisScale() const
{
    return m_calculateAxisScale;
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

void AxisData::calculateRoundedAxisScale()
{
    const int maximumSegmentCount = 10;

    bool calculateStep = isStepAutomatic();
    const bool calculateMinimum = isMinimumAutomatic();
    const bool calculateMaximum = isMaximumAutomatic();

    qreal temporaryMin = 0;
    qreal temporaryMax = 0;
    if (calculateMinimum) {
        temporaryMin = qMin(0.0, minValue());
    } else {
        temporaryMin = qMin(manualMinimum(), minValue());
    }
    if (calculateMaximum) {
        temporaryMax = maxValue();
    } else {
        temporaryMax = qMax(manualMaximum(), maxValue());
    }
    m_step = calculateStep ? 0 : manualStep();

    if (temporaryMax == temporaryMin) {
        if (temporaryMax == 0) {
            temporaryMax = 1;
        } else {
            temporaryMax *= 2;
        }
    }

    const qreal minAndMaxSpacingOffset = 0.95;

    qreal stepMagnitude = 0.0;
    qreal normalizedStep = 0.0;
    bool isStepNormalized = false;
    bool isLoopFinished = false;

    // Calculate until segment count is below maximum
    while( !isLoopFinished ) {
        if (calculateStep) {
            if(isStepNormalized) {
                if( normalizedStep == 1.0 ) {
                    normalizedStep = 2.0;
                } else if( normalizedStep == 2.0 ) {
                    normalizedStep = 5.0;
                } else {
                    normalizedStep = 1.0;
                    stepMagnitude *= 10;
                }
            } else {
                const double startingStep = (temporaryMax - temporaryMin) / maximumSegmentCount;
                const int exponent = static_cast< int >( floor( log10( startingStep ) ) );
                stepMagnitude = pow(10.0, static_cast<double>(exponent));
                normalizedStep = startingStep / stepMagnitude;
                if( normalizedStep <= 1.0 ) {
                    normalizedStep = 1.0;
                } else if( normalizedStep <= 2.0 ) {
                    normalizedStep = 2.0;
                } else if( normalizedStep <= 5.0 ) {
                    normalizedStep = 5.0;
                } else {
                    normalizedStep = 1.0;
                    stepMagnitude *= 10;
                }
                isStepNormalized = true;
            }
            m_step = normalizedStep * stepMagnitude;
        }

        qreal currentAxisMinimum = temporaryMin;
        qreal currentAxisMaximum = temporaryMax;

        if (calculateMinimum) {
            currentAxisMinimum = calculateNewMinimum(currentAxisMinimum, m_step);
            const qreal currentDelta = currentAxisMaximum - currentAxisMinimum;
            const qreal actualDelta = currentAxisMaximum - minValue();
            if ((currentAxisMinimum != 0.0) && ((actualDelta / currentDelta) > minAndMaxSpacingOffset)) {
                currentAxisMinimum -= m_step;
            }
        }

        if (calculateMaximum) {
            currentAxisMaximum = calculateNewMaximum(currentAxisMaximum, m_step);
            const qreal currentDelta = currentAxisMaximum - currentAxisMinimum;
            const qreal actualDelta = maxValue() - currentAxisMinimum;
            if ((currentAxisMaximum != 0.0) && ((actualDelta / currentDelta) > minAndMaxSpacingOffset)) {
                currentAxisMaximum += m_step;
            }
        }

        m_segmentCount = static_cast<int>(round((currentAxisMaximum - currentAxisMinimum) / m_step));
        m_rangeMin = currentAxisMinimum;
        m_rangeMax = currentAxisMaximum;
        // Check also if step is correctly calucalted. It is possible for float steps that
        // there might be a difference. Recalculate the step in that case.
        const qreal tmpStep = (m_rangeMax - m_rangeMin) / m_segmentCount;
        isLoopFinished = m_segmentCount <= maximumSegmentCount && qFuzzyCompare(tmpStep, m_step);
        if (!isLoopFinished) {
            // Configured step may be invalid, calculating it automatically
            calculateStep = true;
        }
    }
}

void AxisData::calculateSimpleAxisScale()
{
    qreal min = 0;
    if (m_minValue < 0) {
        min = minValue();
    }
    m_segmentCount = 4;
    const int delta = maxValue() - min;
    int max = delta;
    while (max % m_segmentCount != 0){
        max++;
    }
    m_rangeMax = minValue() + max;
    m_step = max / m_segmentCount;
    m_rangeMin = minValue();
}

double AxisData::calculateNewMinimum(qreal min, qreal step) const
{
    if (step <= 0.0)
        return min;

    double ret = floor(min / step) * step;
    if (ret > min && !qFuzzyCompare(ret, min)) {
        ret -= step;
    }
    return ret;
}

double AxisData::calculateNewMaximum(qreal max, qreal step) const
{
    if (step <= 0.0)
        return max;

    double ret = floor(max / step) * step;
    if (ret < max && !qFuzzyCompare(ret, max)) {
        ret += step;
    }
    return ret;
}

void AxisData::setCalculateAxisScale(bool newCalculateAxisScale)
{
    m_calculateAxisScale = newCalculateAxisScale;
}

bool AxisData::reverseDirection() const
{
    return m_reverseDirection;
}

void AxisData::setReverseDirection(bool reverseDirection)
{
    m_reverseDirection = reverseDirection;
}

qreal AxisData::manualMaximum() const
{
    return m_manualMaximum;
}

void AxisData::setManualMaximum(qreal newManualMaximum)
{
    m_manualMaximum = newManualMaximum;
}

qreal AxisData::manualMinimum() const
{
    return m_manualMinimum;
}

void AxisData::setManualMinimum(qreal newManualMinimum)
{
    m_manualMinimum = newManualMinimum;
}

qreal AxisData::manualStep() const
{
    return m_manualStep;
}

void AxisData::setManualStep(qreal newManualStep)
{
    m_manualStep = newManualStep;
}

bool AxisData::isMaximumAutomatic() const
{
    return m_isMaximumAutomatic;
}

void AxisData::setIsMaximumAutomatic(bool newIsMaximumAutomatic)
{
    m_isMaximumAutomatic = newIsMaximumAutomatic;
}

bool AxisData::isMinimumAutomatic() const
{
    return m_isMinimumAutomatic;
}

void AxisData::setIsMinimumAutomatic(bool newIsMinimumAutomatic)
{
    m_isMinimumAutomatic = newIsMinimumAutomatic;
}

bool AxisData::isStepAutomatic() const
{
    return m_isStepAutomatic;
}

void AxisData::setIsStepAutomatic(bool newIsStepAutomatic)
{
    m_isStepAutomatic = newIsStepAutomatic;
}

AxisData::AxisType AxisData::type() const
{
    return m_type;
}

}
