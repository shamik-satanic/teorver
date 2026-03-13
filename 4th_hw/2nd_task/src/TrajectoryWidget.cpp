#include "TrajectoryWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <algorithm>
#include <cmath>

namespace PointMotion {

TrajectoryWidget::TrajectoryWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(400, 300);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(10, 12, 20));
    setPalette(pal);
    setAutoFillBackground(true);

    m_timer = new QTimer(this);
    m_timer->setInterval(16); // ~60fps
    connect(m_timer, &QTimer::timeout, this, &TrajectoryWidget::advanceAnim);
}

QSize TrajectoryWidget::sizeHint() const { return {700, 400}; }

void TrajectoryWidget::clear()
{
    m_timer->stop();
    m_traj.clear();
    m_crossings = 0;
    m_animIdx   = 0;
    m_animDone  = false;
    update();
}

void TrajectoryWidget::setTrajectory(const std::vector<Point2D>& traj, int crossings)
{
    m_timer->stop();
    m_traj      = traj;
    m_crossings = crossings;
    m_animIdx   = 0;
    m_animDone  = false;
    recalcTransform();
    m_timer->start();
}

void TrajectoryWidget::recalcTransform()
{
    if (m_traj.empty()) return;
    m_xMin = m_traj.front().x;
    m_xMax = m_traj.back().x;
    m_yMin = m_traj.front().y;
    m_yMax = m_traj.front().y;
    for (auto& pt : m_traj) {
        m_yMin = std::min(m_yMin, pt.y);
        m_yMax = std::max(m_yMax, pt.y);
    }
    // Pad
    double dy = (m_yMax - m_yMin) * 0.15;
    if (dy < 2.0) dy = 2.0;
    m_yMin -= dy;
    m_yMax += dy;
    double dx = (m_xMax - m_xMin) * 0.02;
    if (dx < 1.0) dx = 1.0;
    m_xMin -= dx;
    m_xMax += dx;
}

void TrajectoryWidget::resizeEvent(QResizeEvent*) { update(); }

QPointF TrajectoryWidget::toScreen(double x, double y) const
{
    int W = width()  - 2 * m_pad;
    int H = height() - 2 * m_pad;
    double sx = m_pad + (x - m_xMin) / (m_xMax - m_xMin) * W;
    double sy = m_pad + (1.0 - (y - m_yMin) / (m_yMax - m_yMin)) * H;
    return {sx, sy};
}

void TrajectoryWidget::advanceAnim()
{
    if (m_traj.empty()) { m_timer->stop(); return; }
    int stride = std::max(1, static_cast<int>(m_traj.size()) / 120);
    m_animIdx = std::min(m_animIdx + stride, static_cast<int>(m_traj.size()) - 1);
    update();
    if (m_animIdx >= static_cast<int>(m_traj.size()) - 1) {
        m_timer->stop();
        m_animDone = true;
    }
}

void TrajectoryWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int W = width();
    const int H = height();

    // Background grid
    p.setPen(QColor(20, 24, 38));
    for (int x = 0; x < W; x += 30) p.drawLine(x, 0, x, H);
    for (int y = 0; y < H; y += 30) p.drawLine(0, y, W, y);

    if (m_traj.empty()) {
        p.setPen(QColor(60, 70, 100));
        p.setFont(QFont("JetBrains Mono", 11));
        p.drawText(rect(), Qt::AlignCenter, "Нет данных. Нажмите \"Запустить\".");
        return;
    }

    // X axis (y=0 line)
    QPointF ax0 = toScreen(m_xMin, 0.0);
    QPointF ax1 = toScreen(m_xMax, 0.0);
    QPen axisPen(QColor(60, 200, 120, 180), 1.5, Qt::DashLine);
    p.setPen(axisPen);
    p.drawLine(ax0, ax1);

    // Axis label
    p.setPen(QColor(60, 200, 120, 120));
    QFont sf("JetBrains Mono", 8);
    p.setFont(sf);
    p.drawText(QPointF(ax1.x() - 50, ax1.y() - 5), "y = 0");

    // Y axis
    QPointF ay0 = toScreen(0.0, m_yMin);
    QPointF ay1 = toScreen(0.0, m_yMax);
    p.setPen(QPen(QColor(80, 90, 120, 140), 1));
    p.drawLine(ay0, ay1);

    // Tick marks and labels
    p.setPen(QColor(70, 80, 110));
    p.setFont(QFont("JetBrains Mono", 8));

    // Y ticks
    double yStep = std::pow(10.0, std::floor(std::log10(m_yMax - m_yMin)) - 0.5);
    if (yStep < 0.5) yStep = 0.5;
    for (double yv = std::ceil(m_yMin / yStep) * yStep; yv <= m_yMax; yv += yStep) {
        QPointF pt = toScreen(m_xMin, yv);
        p.drawLine(QPointF(pt.x() - 4, pt.y()), QPointF(pt.x() + 4, pt.y()));
        p.drawText(QPointF(m_pad - 38, pt.y() + 4),
                   QString::number(static_cast<int>(yv)));
    }

    // Trajectory path (up to animIdx)
    int drawTo = m_animDone ? static_cast<int>(m_traj.size()) - 1 : m_animIdx;

    // Colour segments by sign of y
    if (drawTo >= 1) {
        for (int i = 1; i <= drawTo; ++i) {
            double y0 = m_traj[static_cast<size_t>(i-1)].y;
            double y1 = m_traj[static_cast<size_t>(i)].y;
            QColor segColor = (y0 >= 0 && y1 >= 0) ? QColor(79, 159, 255)
                            : (y0 <= 0 && y1 <= 0) ? QColor(255, 100, 100)
                            :                         QColor(255, 200, 79);
            p.setPen(QPen(segColor, 2.0));
            p.drawLine(toScreen(m_traj[static_cast<size_t>(i-1)].x,
                                m_traj[static_cast<size_t>(i-1)].y),
                       toScreen(m_traj[static_cast<size_t>(i)].x,
                                m_traj[static_cast<size_t>(i)].y));
        }
    }

    // Mark crossing points
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255, 220, 79));
    for (int i = 1; i < static_cast<int>(m_traj.size()) && i <= drawTo; ++i) {
        double yp = m_traj[static_cast<size_t>(i-1)].y;
        double yc = m_traj[static_cast<size_t>(i)].y;
        bool isCross = (yc == 0.0) ||
                       ((yp > 0 && yc < 0) || (yp < 0 && yc > 0));
        if (isCross) {
            QPointF sc = toScreen(m_traj[static_cast<size_t>(i)].x, yc);
            p.drawEllipse(sc, 5.0, 5.0);
        }
    }

    // Start point
    if (!m_traj.empty()) {
        QPointF sp = toScreen(m_traj[0].x, m_traj[0].y);
        p.setBrush(QColor(79, 255, 176));
        p.setPen(Qt::NoPen);
        p.drawEllipse(sp, 6.0, 6.0);
    }

    // Current point (animated)
    if (!m_animDone && drawTo >= 0 && drawTo < static_cast<int>(m_traj.size())) {
        QPointF cp = toScreen(m_traj[static_cast<size_t>(drawTo)].x,
                              m_traj[static_cast<size_t>(drawTo)].y);
        QRadialGradient glow(cp, 12);
        glow.setColorAt(0, QColor(79, 159, 255, 180));
        glow.setColorAt(1, Qt::transparent);
        p.setBrush(glow);
        p.setPen(Qt::NoPen);
        p.drawEllipse(cp, 12.0, 12.0);
        p.setBrush(QColor(200, 220, 255));
        p.drawEllipse(cp, 4.0, 4.0);
    }

    // Info overlay
    p.setPen(QColor(79, 255, 176));
    p.setFont(QFont("JetBrains Mono", 9));
    QString info = QString("Шагов: %1   Пересечений оси X: %2")
                   .arg(m_traj.size() - 1)
                   .arg(m_crossings);
    p.drawText(QRect(m_pad, 6, W - 2*m_pad, 18), Qt::AlignLeft, info);

    // Legend
    p.setFont(QFont("JetBrains Mono", 8));
    int lx = W - 160, ly = 8;
    auto drawLeg = [&](const QColor& c, const QString& t) {
        p.setPen(Qt::NoPen);
        p.setBrush(c);
        p.drawRect(lx, ly, 14, 3);
        p.setPen(QColor(130, 140, 170));
        p.drawText(lx + 18, ly + 5, t);
        ly += 14;
    };
    drawLeg(QColor(79,159,255), "y > 0");
    drawLeg(QColor(255,100,100), "y < 0");
    drawLeg(QColor(255,220,79), "пересечение");
}

} // namespace PointMotion
