#pragma once

#include <QWidget>
#include <QTimer>
#include <QList>
#include "SimulationEngine.h"

namespace RandomWalk {

// -------------------------------------------------------
/// Виджет: анимированная улица с движущимся пешеходом.
/// Рисуется через QPainter без каких-либо внешних зависимостей.
// -------------------------------------------------------
class StreetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StreetWidget(QWidget* parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    /// Загрузить новую траекторию и запустить анимацию
    void loadTrajectory(QList<int> trajectory, int B, WalkResult result);

    /// Остановить анимацию
    void stop();

    /// Перезапустить анимацию с текущей траекторией
    void replay();

signals:
    void animationFinished(WalkResult result, int totalSteps);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void advanceFrame();

private:
    void drawCliff(QPainter& p, int x, int y);
    void drawCafe(QPainter& p, int x, int y, int B);
    void drawPerson(QPainter& p, int x, int y, bool fallen);

    QList<int>  m_trajectory;
    int         m_B          = 5;
    int         m_frameIdx   = 0;
    int         m_currentPos = 5;
    WalkResult  m_result     = WalkResult::Cafe;
    bool        m_finished   = false;

    QTimer*     m_timer;

    // Визуальные константы
    static constexpr int kCellSize   = 52;
    static constexpr int kPersonSize = 20;
    static constexpr int kStreetY    = 80;
    static constexpr int kTimerMs    = 40; 
};

} // namespace RandomWalk
