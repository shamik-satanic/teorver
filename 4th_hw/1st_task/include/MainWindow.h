#pragma once

#include <QMainWindow>
#include <QThread>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QLabel>
#include <QProgressBar>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include <QList>

#include "SimulationEngine.h"
#include "SimulationWorker.h"
#include "StreetWidget.h"

namespace RandomWalk {

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onRunSimulation();
    void onRunSingleWalk();
    void onStepCompleted(BStats stats);
    void onSimulationFinished(QList<BStats> allStats);
    void onTrajectoryReady(QList<int> traj, int B, WalkResult result);
    void onAnimationFinished(WalkResult result, int steps);
    void onWorkerError(QString msg);
    void onProgressChanged(int current, int total);
    void onPChanged(int value);

private:
    QWidget*    buildControlPanel();
    QWidget*    buildChartsArea();
    void        setupCharts();
    void        clearCharts();
    void        updateStats(const QList<BStats>& stats);
    void        setUiRunning(bool running);

    // Worker
    QThread*           m_thread  = nullptr;
    SimulationWorker*  m_worker  = nullptr;

    // Controls
    QSlider*       m_pSlider      = nullptr;
    QLabel*        m_pLabel       = nullptr;
    QLabel*        m_qLabel       = nullptr;
    QSpinBox*      m_bMaxSpin     = nullptr;
    QSpinBox*      m_simSpin      = nullptr;
    QSpinBox*      m_maxStepsSpin = nullptr;
    QPushButton*   m_runBtn       = nullptr;
    QPushButton*   m_walkBtn      = nullptr;
    QProgressBar*  m_progress     = nullptr;
    QLabel*        m_statusLabel  = nullptr;

    // Stat value labels (populated by makeStatCard)
    QLabel* m_lCliff  = nullptr;
    QLabel* m_lCafe   = nullptr;
    QLabel* m_lSteps  = nullptr;
    QLabel* m_lTheory = nullptr;

    // Charts
    QChartView*   m_probView    = nullptr;
    QChartView*   m_trajView    = nullptr;
    QLineSeries*  m_cliffSeries  = nullptr;
    QLineSeries*  m_cafeSeries   = nullptr;
    QLineSeries*  m_theorySeries = nullptr;
    QLineSeries*  m_trajSeries   = nullptr;
    QValueAxis*   m_probAxisX   = nullptr;
    QValueAxis*   m_probAxisY   = nullptr;
    QValueAxis*   m_trajAxisX   = nullptr;
    QValueAxis*   m_trajAxisY   = nullptr;

    // Street widget
    StreetWidget* m_street = nullptr;

    // State
    QList<BStats> m_lastStats;
    int           m_trajB = 5;
};

} // namespace RandomWalk
