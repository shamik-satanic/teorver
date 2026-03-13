#pragma once
#include <QMainWindow>
#include <QThread>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <QGroupBox>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>

#include "SimulationEngine.h"
#include "SimulationWorker.h"
#include "TrajectoryWidget.h"
#include "LogStorage.h"
#include "ConfigManager.h"

namespace PointMotion {

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onRun();
    void onSingleRun();
    void onFullReady(PointMotion::SimResult sample, PointMotion::EmpiricalResult emp);
    void onSingleReady(PointMotion::SimResult result);
    void onWorkerError(QString msg);
    void onLogSelectionChanged(int row);
    void onLoadConfig();
    void onSaveConfig();
    void onDistChanged(int idx);
    void refreshLogTable();

private:
    QWidget*  buildControlPanel();
    QWidget*  buildMainArea();
    QWidget*  buildLogPanel();
    void      applyParamsToUI(const SimParams& p);
    SimParams paramsFromUI() const;
    void      updateStatsLabels(const EmpiricalResult& emp, const SimParams& p);
    void      updateHistogramChart(const EmpiricalResult& emp, int l_target);
    void      setRunning(bool r);

    QThread*           m_thread = nullptr;
    SimulationWorker*  m_worker = nullptr;
    LogStorage*        m_log    = nullptr;

    QDoubleSpinBox* m_y0Spin      = nullptr;
    QDoubleSpinBox* m_hSpin       = nullptr;
    QSpinBox*       m_nSpin       = nullptr;
    QSpinBox*       m_runsSpin    = nullptr;
    QSpinBox*       m_lSpin       = nullptr;
    QSpinBox*       m_kSpin       = nullptr;
    QComboBox*      m_distCombo   = nullptr;
    QDoubleSpinBox* m_sMinSpin    = nullptr;
    QDoubleSpinBox* m_sMaxSpin    = nullptr;
    QSpinBox*       m_nTrialsSpin = nullptr;
    QDoubleSpinBox* m_probSpin    = nullptr;
    QGroupBox*      m_binomGroup  = nullptr;
    QPushButton*    m_runBtn      = nullptr;
    QPushButton*    m_singleBtn   = nullptr;

    QLabel* m_lPAtLeastL = nullptr;
    QLabel* m_lPExactL   = nullptr;
    QLabel* m_lPZero     = nullptr;
    QLabel* m_lMeanC     = nullptr;

    QTabWidget*       m_tabs           = nullptr;
    TrajectoryWidget* m_trajWidget     = nullptr;
    QChartView*       m_histView       = nullptr;
    QTableWidget*     m_logTable       = nullptr;
    TrajectoryWidget* m_logTrajWidget  = nullptr;
    QLabel*           m_logDetailLabel = nullptr;
};

} // namespace PointMotion
