#include "lrgraphicsviewzoom.h"
#include <QMouseEvent>
#include <QApplication>
#include <qmath.h>

namespace LimeReport{

GraphicsViewZoomer::GraphicsViewZoomer(QGraphicsView* view)
  : QObject(view), m_view(view)
{
  m_view->viewport()->installEventFilter(this);
  m_view->setMouseTracking(true);
  m_modifiers = Qt::ControlModifier;
  m_zoomFactorBase = 1.0009;
}

void GraphicsViewZoomer::gentleZoom(double factor) {
  m_view->scale(factor, factor);
  m_view->centerOn(m_targetScenePos);
  QPointF delta_viewport_pos = m_targetViewportPos - QPointF(m_view->viewport()->width() / 2.0,
                                                             m_view->viewport()->height() / 2.0);
  QPointF viewport_center = m_view->mapFromScene(m_targetScenePos) - delta_viewport_pos;
  m_view->centerOn(m_view->mapToScene(viewport_center.toPoint()));
  emit zoomed(factor);
}

void GraphicsViewZoomer::setModifiers(Qt::KeyboardModifiers modifiers) {
  m_modifiers = modifiers;
}

void GraphicsViewZoomer::setZoomFactorBase(double value) {
    m_zoomFactorBase = value;
}

void GraphicsViewZoomer::setView(QGraphicsView *view)
{
    if (m_view!=view){
        m_view->viewport()->removeEventFilter(this);
        m_view = view;
        m_view->viewport()->installEventFilter(this);
        m_view->setMouseTracking(true);
    }
}

bool GraphicsViewZoomer::eventFilter(QObject *object, QEvent *event) {
  if (event->type() == QEvent::MouseMove) {
    QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
    QPointF delta = m_targetViewportPos - mouse_event->pos();
    if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5) {
      m_targetViewportPos = mouse_event->pos();
      m_targetScenePos = m_view->mapToScene(mouse_event->pos());
    }
  } else if (event->type() == QEvent::Wheel) {
    QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);
    if (QApplication::keyboardModifiers() == m_modifiers) {
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
      if (wheel_event->orientation() == Qt::Vertical) {
        double angle = wheel_event->delta();
        double factor = qPow(m_zoomFactorBase, angle);
        gentleZoom(factor);
        return true;
      }
#else
      if (wheel_event->angleDelta().y() != 0) {
        double angle = wheel_event->angleDelta().y();
        double factor = qPow(m_zoomFactorBase, angle);
        gentleZoom(factor);
        return true;
      }
#endif
    }
  }
  Q_UNUSED(object)
  return false;
}

} //namespace LimeReport
