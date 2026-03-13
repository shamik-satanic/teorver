#include "StreetWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <algorithm>

namespace RandomWalk {

StreetWidget::StreetWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(130);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_timer = new QTimer(this);
    m_timer->setInterval(kTimerMs);
    connect(m_timer, &QTimer::timeout, this, &Str#include "StreetWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <algorithm>

namespace RandomWalk {

StreetWidget::StreetWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(130);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_timer = new QTimer(this);
    m_timer->setInterval(kTimerMs);
    connect(m_timer, &QTimer::timeout, this, &StreetWidget::advanceFrame);

    // Цвет фона
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(14, 16, 24));
    setPalette(pal);
    setAutoFillBackground(true);
}

QSize StreetWidget::sizeHint()        const { return { 800, 130 }; }
QSize StreetWidget::minimumSizeHint() const { return { 400, 130 }; }

// -------------------------------------------------------
void StreetWidget::loadTrajectory(QList<int> trajectory, int B, WalkResult result)
{
    m_timer->stop();
    m_trajectory  = std::move(trajectory);
    m_B           = B;
    m_result      = result;
    m_frameIdx    = 0;
    m_finished    = false;
    m_currentPos  = m_trajectory.isEmpty() ? B : m_trajectory.front();
    m_timer->start();
}

void StreetWidget::stop()   { m_timer->stop(); }
void StreetWidget::replay() { m_frameIdx = 0; m_finished = false; m_timer->start(); }

// -------------------------------------------------------
void StreetWidget::advanceFrame()
{
    if (m_trajectory.isEmpty()) return;

    // Прокручиваем несколько кадров за тик, чтобы длинные траектории
    // проигрывались за разумное время (≤ 4 сек для 10000 шагов).
    const int stride = std::max(1, static_cast<int>(m_trajectory.size()) / 100);
    m_frameIdx = std::min(m_frameIdx + stride,
                          static_cast<int>(m_trajectory.size()) - 1);
    m_currentPos = m_trajectory[m_frameIdx];

    update(); // перерисовать

    if (m_frameIdx >= static_cast<int>(m_trajectory.size()) - 1) {
        m_timer->stop();
        m_finished = true;
        emit animationFinished(m_result, static_cast<int>(m_trajectory.size()) - 1);
    }
}

// -------------------------------------------------------
void StreetWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int W = width();
    const int H = height();
    const int streetY = H / 2 + 10;

    // --- Сетка фона ---
    p.setPen(QColor(30, 34, 53, 120));
    for (int x = 0; x < W; x += 20) p.drawLine(x, 0, x, H);
    for (int y = 0; y < H; y += 20) p.drawLine(0, y, W, y);

    // --- Асфальт (полоса улицы) ---
    QLinearGradient road(0, streetY - 4, 0, streetY + 14);
    road.setColorAt(0, QColor(30, 34, 50));
    road.setColorAt(1, QColor(18, 20, 32));
    p.fillRect(0, streetY - 4, W, 18, road);

    // --- Определяем, сколько ячеек влезает ---
    const int visibleCells = std::min(m_B + 9, W / kCellSize);
    const int startX = 40;

    // Линия дороги
    p.setPen(QPen(QColor(50, 55, 80), 2));
    p.drawLine(startX, streetY + 5, W - 10, streetY + 5);

    // --- Ячейки ---
    for (int i = 0; i <= visibleCells; ++i) {
        const int x = startX + i * kCellSize;
        if (x > W - 10) break;

        if (i == 0) {
            drawCliff(p, x, streetY);
            continue;
        }
        if (i == m_B) {
            drawCafe(p, x, streetY, m_B);
        }

        // Засечка
        const bool isCafe = (i == m_B);
        p.setPen(QPen(isCafe ? QColor(79, 255, 176) : QColor(50, 55, 80), isCafe ? 2 : 1));
        p.drawLine(x, streetY - 6, x, streetY + 6);

        // Подпись
        p.setPen(isCafe ? QColor(79, 255, 176) : QColor(90, 98, 130));
        QFont f = font();
        f.setPixelSize(9);
        f.setBold(isCafe);
        p.setFont(f);
        p.drawText(QRect(x - 15, streetY + 10, 30, 16),
                   Qt::AlignCenter, QString::number(i));
    }

    // --- Человек ---
    if (!m_trajectory.isEmpty()) {
        const int personX = startX + m_currentPos * kCellSize;
        if (personX < W - 5) {
            const bool fallen = (m_finished && m_result == WalkResult::Cliff);
            drawPerson(p, personX, streetY, fallen);
        }
    }

    // --- Подписи краёв ---
    QFont lf = font(); lf.setPixelSize(9);
    p.setFont(lf);
    p.setPen(QColor(255, 79, 79));
    p.drawText(QRect(0, H - 22, 70, 16), Qt::AlignCenter, "← ОБРЫВ");
    p.setPen(QColor(90, 98, 130));
    p.drawText(QRect(W - 70, H - 22, 68, 16), Qt::AlignCenter, "УЛИЦА →");

    // --- Статус ---
    QString statusText;
    if (m_finished) {
        if      (m_result == WalkResult::Cliff)   statusText = "💀 Упал в обрыв!";
        else if (m_result == WalkResult::Cafe)    statusText = "☕ Вернулся в кафе!";
        else                                      statusText = "⏰ Достигнут лимит шагов";
    } else if (!m_trajectory.isEmpty()) {
        statusText = QString("Шаг %1 / %2   Позиция: %3")
                     .arg(m_frameIdx)
                     .arg(m_trajectory.size() - 1)
                     .arg(m_currentPos);
    }
    p.setPen(QColor(90, 98, 130));
    QFont sf = font(); sf.setPixelSize(10);
    p.setFont(sf);
    p.drawText(QRect(10, 4, W - 20, 18), Qt::AlignLeft | Qt::AlignVCenter, statusText);
}

// -------------------------------------------------------
void StreetWidget::drawCliff(QPainter& p, int x, int y)
{
    // Красная зона обрыва
    QLinearGradient g(x - 30, 0, x + 5, 0);
    g.setColorAt(0, QColor(255, 79, 79, 60));
    g.setColorAt(1, Qt::transparent);
    p.fillRect(x - 30, 20, 36, height() - 40, g);

    // Иконка
    p.setPen(QColor(255, 79, 79));
    QFont f = font(); f.setPixelSize(22);
    p.setFont(f);
    p.drawText(QRect(x - 18, y - 36, 36, 30), Qt::AlignCenter, "⛰");

    QFont lf = font(); lf.setPixelSize(8); lf.setBold(true);
    p.setFont(lf);
    p.drawText(QRect(x - 14, y + 10, 28, 14), Qt::AlignCenter, "0");
}

void StreetWidget::drawCafe(QPainter& p, int x, int y, int)
{
    // Зелёное свечение над кафе
    QRadialGradient g(x, y - 20, 26);
    g.setColorAt(0, QColor(79, 255, 176, 40));
    g.setColorAt(1, Qt::transparent);
    p.fillRect(x - 26, y - 46, 52, 40, g);

    p.setPen(QColor(79, 255, 176));
    QFont f = font(); f.setPixelSize(20);
    p.setFont(f);
    p.drawText(QRect(x - 14, y - 44, 28, 28), Qt::AlignCenter, "☕");

    QFont lf = font(); lf.setPixelSize(8); lf.setBold(true);
    p.setFont(lf);
    p.drawText(QRect(x - 14, y + 24, 28, 12), Qt::AlignCenter, "Кафе");
}

void StreetWidget::drawPerson(QPainter& p, int x, int y, bool fallen)
{
    // Свечение под пешеходом
    QRadialGradient glow(x, y - 10, 22);
    glow.setColorAt(0, fallen ? QColor(255,79,79,80) : QColor(79,159,255,80));
    glow.setColorAt(1, Qt::transparent);
    p.fillRect(x - 22, y - 32, 44, 36, glow);

    // Иконка
    QFont f = font(); f.setPixelSize(kPersonSize);
    p.setFont(f);
    p.drawText(QRect(x - 14, y - 34, 28, 28),
               Qt::AlignCenter,
               fallen ? "💀" : "🚶");
}

} // namespace RandomWalk
eetWidget::advanceFrame);

    // Цвет фона
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(14, 16, 24));
    setPalette(pal);
    setAutoFillBackground(true);
}

QSize StreetWidget::sizeHint()        const { return { 800, 130 }; }
QSize StreetWidget::minimumSizeHint() const { return { 400, 130 }; }

void StreetWidget::loadTrajectory(QList<int> trajectory, int B, WalkResult result)
{
    m_timer->stop();
    m_trajectory  = std::move(trajectory);
    m_B           = B;
    m_result      = result;
    m_frameIdx    = 0;
    m_finished    = false;
    m_currentPos  = m_trajectory.isEmpty() ? B : m_trajectory.front();
    m_timer->start();
}

void StreetWidget::stop()   { m_timer->stop(); }
void StreetWidget::replay() { m_frameIdx = 0; m_finished = false; m_timer->start(); }

void StreetWidget::advanceFrame()
{
    if (m_trajectory.isEmpty()) return;


    const int stride = std::max(1, static_cast<int>(m_trajectory.size()) / 100);
    m_frameIdx = std::min(m_frameIdx + stride,
                          static_cast<int>(m_trajectory.size()) - 1);
    m_currentPos = m_trajectory[m_frameIdx];

    update(); 

    if (m_frameIdx >= static_cast<int>(m_trajectory.size()) - 1) {
        m_timer->stop();
        m_finished = true;
        emit animationFinished(m_result, static_cast<int>(m_trajectory.size()) - 1);
    }
}


void StreetWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int W = width();
    const int H = height();
    const int streetY = H / 2 + 10;


    p.setPen(QColor(30, 34, 53, 120));
    for (int x = 0; x < W; x += 20) p.drawLine(x, 0, x, H);
    for (int y = 0; y < H; y += 20) p.drawLine(0, y, W, y);


    QLinearGradient road(0, streetY - 4, 0, streetY + 14);
    road.setColorAt(0, QColor(30, 34, 50));
    road.setColorAt(1, QColor(18, 20, 32));
    p.fillRect(0, streetY - 4, W, 18, road);


    const int visibleCells = std::min(m_B + 9, W / kCellSize);
    const int startX = 40;


    p.setPen(QPen(QColor(50, 55, 80), 2));
    p.drawLine(startX, streetY + 5, W - 10, streetY + 5);


    for (int i = 0; i <= visibleCells; ++i) {
        const int x = startX + i * kCellSize;
        if (x > W - 10) break;

        if (i == 0) {
            drawCliff(p, x, streetY);
            continue;
        }
        if (i == m_B) {
            drawCafe(p, x, streetY, m_B);
        }


        const bool isCafe = (i == m_B);
        p.setPen(QPen(isCafe ? QColor(79, 255, 176) : QColor(50, 55, 80), isCafe ? 2 : 1));
        p.drawLine(x, streetY - 6, x, streetY + 6);


        p.setPen(isCafe ? QColor(79, 255, 176) : QColor(90, 98, 130));
        QFont f = font();
        f.setPixelSize(9);
        f.setBold(isCafe);
        p.setFont(f);
        p.drawText(QRect(x - 15, streetY + 10, 30, 16),
                   Qt::AlignCenter, QString::number(i));
    }


    if (!m_trajectory.isEmpty()) {
        const int personX = startX + m_currentPos * kCellSize;
        if (personX < W - 5) {
            const bool fallen = (m_finished && m_result == WalkResult::Cliff);
            drawPerson(p, personX, streetY, fallen);
        }
    }


    QFont lf = font(); lf.setPixelSize(9);
    p.setFont(lf);
    p.setPen(QColor(255, 79, 79));
    p.drawText(QRect(0, H - 22, 70, 16), Qt::AlignCenter, "← ОБРЫВ");
    p.setPen(QColor(90, 98, 130));
    p.drawText(QRect(W - 70, H - 22, 68, 16), Qt::AlignCenter, "УЛИЦА →");


    QString statusText;
    if (m_finished) {
        if      (m_result == WalkResult::Cliff)   statusText = "💀 Упал в обрыв!";
        else if (m_result == WalkResult::Cafe)    statusText = "☕ Вернулся в кафе!";
        else                                      statusText = "⏰ Достигнут лимит шагов";
    } else if (!m_trajectory.isEmpty()) {
        statusText = QString("Шаг %1 / %2   Позиция: %3")
                     .arg(m_frameIdx)
                     .arg(m_trajectory.size() - 1)
                     .arg(m_currentPos);
    }
    p.setPen(QColor(90, 98, 130));
    QFont sf = font(); sf.setPixelSize(10);
    p.setFont(sf);
    p.drawText(QRect(10, 4, W - 20, 18), Qt::AlignLeft | Qt::AlignVCenter, statusText);
}

void StreetWidget::drawCliff(QPainter& p, int x, int y)
{

    QLinearGradient g(x - 30, 0, x + 5, 0);
    g.setColorAt(0, QColor(255, 79, 79, 60));
    g.setColorAt(1, Qt::transparent);
    p.fillRect(x - 30, 20, 36, height() - 40, g);


    p.setPen(QColor(255, 79, 79));
    QFont f = font(); f.setPixelSize(22);
    p.setFont(f);
    p.drawText(QRect(x - 18, y - 36, 36, 30), Qt::AlignCenter, "⛰");

    QFont lf = font(); lf.setPixelSize(8); lf.setBold(true);
    p.setFont(lf);
    p.drawText(QRect(x - 14, y + 10, 28, 14), Qt::AlignCenter, "0");
}

void StreetWidget::drawCafe(QPainter& p, int x, int y, int)
{

    QRadialGradient g(x, y - 20, 26);
    g.setColorAt(0, QColor(79, 255, 176, 40));
    g.setColorAt(1, Qt::transparent);
    p.fillRect(x - 26, y - 46, 52, 40, g);

    p.setPen(QColor(79, 255, 176));
    QFont f = font(); f.setPixelSize(20);
    p.setFont(f);
    p.drawText(QRect(x - 14, y - 44, 28, 28), Qt::AlignCenter, "☕");

    QFont lf = font(); lf.setPixelSize(8); lf.setBold(true);
    p.setFont(lf);
    p.drawText(QRect(x - 14, y + 24, 28, 12), Qt::AlignCenter, "Кафе");
}

void StreetWidget::drawPerson(QPainter& p, int x, int y, bool fallen)
{

    QRadialGradient glow(x, y - 10, 22);
    glow.setColorAt(0, fallen ? QColor(255,79,79,80) : QColor(79,159,255,80));
    glow.setColorAt(1, Qt::transparent);
    p.fillRect(x - 22, y - 32, 44, 36, glow);

    QFont f = font(); f.setPixelSize(kPersonSize);
    p.setFont(f);
    p.drawText(QRect(x - 14, y - 34, 28, 28),
               Qt::AlignCenter,
               fallen ? "💀" : "🚶");
}

} // namespace RandomWalk
