#ifndef LRGRAPHICSVIEWZOOM_H
#define LRGRAPHICSVIEWZOOM_H

#include <QObject>
#include <QGraphicsView>

namespace LimeReport {

class GraphicsViewZoomer : public QObject {
  Q_OBJECT
public:
  GraphicsViewZoomer(QGraphicsView* view);
  void gentleZoom(double factor);
  void setModifiers(Qt::KeyboardModifiers modifiers);
  void setZoomFactorBase(double value);
  void setView(QGraphicsView* view);
private:
  QGraphicsView* m_view;
  Qt::KeyboardModifiers m_modifiers;
  double m_zoomFactorBase;
  QPointF m_targetScenePos, m_targetViewportPos;
  bool eventFilter(QObject* object, QEvent* event);
signals:
  void zoomed(double factor);
};

} // namespace LimeReport

#endif // GRAPHICSVIEWZOOM_H
