#ifndef AXISDATA_H
#define AXISDATA_H

#include <QObject>

namespace LimeReport {
class AxisData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool reverseDirection READ reverseDirection WRITE setReverseDirection)
    Q_PROPERTY(bool calculateAxisScale READ calculateAxisScale WRITE setCalculateAxisScale)
    Q_PROPERTY(bool isStepAutomatic READ isStepAutomatic WRITE setIsStepAutomatic)
    Q_PROPERTY(bool isMinimumAutomatic READ isMinimumAutomatic WRITE setIsMinimumAutomatic)
    Q_PROPERTY(bool isMaximumAutomatic READ isMaximumAutomatic WRITE setIsMaximumAutomatic)
    Q_PROPERTY(qreal manualStep READ manualStep WRITE setManualStep)
    Q_PROPERTY(qreal manualMinimum READ manualMinimum WRITE setManualMinimum)
    Q_PROPERTY(qreal manualMaximum READ manualMaximum WRITE setManualMaximum)
public:
    enum AxisType {
        YAxis = 0,
        XAxis = 1
    };

    AxisData(AxisType type, QObject *parent = nullptr);

    QString toString() const;
    void copy(AxisData *other);
    void update();
    void update(qreal minValue, qreal maxValue);
    void updateForDesignMode();

    int segmentCount() const;

    qreal rangeMin() const;
    qreal rangeMax() const;

    qreal minValue() const;
    qreal maxValue() const;
    qreal step() const;

    qreal delta() const;

    bool reverseDirection() const;
    void setReverseDirection(bool newReverseDirection);
    bool calculateAxisScale() const;
    void setCalculateAxisScale(bool newCalculateAxisScale);

    qreal manualMaximum() const;
    void setManualMaximum(qreal newManualMaximum);

    qreal manualMinimum() const;
    void setManualMinimum(qreal newManualMinimum);

    qreal manualStep() const;
    void setManualStep(qreal newManualStep);

    bool isMaximumAutomatic() const;
    void setIsMaximumAutomatic(bool newIsMaximumAutomatic);

    bool isMinimumAutomatic() const;
    void setIsMinimumAutomatic(bool newIsMinimumAutomatic);

    bool isStepAutomatic() const;
    void setIsStepAutomatic(bool newIsStepAutomatic);

    AxisType type() const;

private:
    void calculateRoundedAxisScale();
    void calculateSimpleAxisScale();
    qreal calculateNewMinimum(qreal min, qreal step) const;
    qreal calculateNewMaximum(qreal max, qreal step) const;

    qreal m_rangeMin;
    qreal m_rangeMax;
    qreal m_minValue;
    qreal m_maxValue;
    qreal m_step;
    qreal m_delta;
    int m_segmentCount;
    bool m_calculateAxisScale;
    bool m_reverseDirection;
    qreal m_manualMaximum;
    qreal m_manualMinimum;
    qreal m_manualStep;
    bool m_isMaximumAutomatic;
    bool m_isMinimumAutomatic;
    bool m_isStepAutomatic;
    const AxisType m_type;
};
};

#endif // AXISDATA_H
