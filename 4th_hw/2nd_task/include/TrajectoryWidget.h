#pragma once
#include <QWidget>
#include <QTimer>
#include "SimulationEngine.h"

namespace PointMotion {

class TrajectoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TrajectoryWidget(QWidget* parent = nullptr);
    QSize sizeHint() const override;

    void setTrajectory(const std::vector<Point2D>& traj, int crossings);
    void clear();

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private slots:
    void advanceAnim();

private:
    void   recalcTransform();
    QPointF toScreen(double x, double y) const;

    std::vector<Point2D> m_traj;
    int    m_crossings  = 0;
    int    m_animIdx    = 0;
    bool   m_animDone   = false;
    QTimer* m_timer     = nullptr;

    // Transform
    double m_xMin = 0, m_xMax = 1;
    double m_yMin = -5, m_yMax = 15;
    int    m_pad  = 50;
};

} // namespace PointMotion
