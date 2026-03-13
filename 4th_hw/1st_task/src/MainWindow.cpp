#include "MainWindow.h"

#include <QApplication>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QFrame>
#include <QFont>
#include <QMessageBox>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <algorithm>

namespace RandomWalk {

// -------------------------------------------------------
namespace Colors {
    const QColor BG        { 14,  16,  24 };
    const QColor Surface   { 18,  20,  32 };
    const QColor Surface2  { 22,  25,  40 };
    const QColor Border    { 35,  40,  65 };
    const QColor Accent    { 79, 255, 176 };
    const QColor Cliff     {255,  79,  79 };
    const QColor Cafe      { 79, 255, 176 };
    const QColor Theory    {255, 208,  79 };
    const QColor Blue      { 79, 159, 255 };
    const QColor TextDim   { 90,  98, 130 };
    const QColor Text      {200, 208, 232 };
}

// -------------------------------------------------------
// Returns a QFrame* card; also sets valueOut to the value QLabel inside it.
// Caller adds the QFrame* to a layout directly.
// -------------------------------------------------------
static QFrame* makeStatCard(const QString& labelText,
                            const QColor&  valueColor,
                            QLabel*&       valueOut,
                            QWidget*       parent)
{
    QFrame* card = new QFrame(parent);
    card->setFrameShape(QFrame::StyledPanel);
    card->setStyleSheet(QString(
        "QFrame { background: %1; border: 1px solid %2; border-radius: 6px; }")
        .arg(Colors::Surface2.name())
        .arg(Colors::Border.name()));

    QVBoxLayout* l = new QVBoxLayout(card);
    l->setContentsMargins(10, 8, 10, 8);
    l->setSpacing(2);

    QLabel* title = new QLabel(labelText, card);
    title->setStyleSheet(QString("color:%1;font-size:9px;letter-spacing:1px;")
                         .arg(Colors::TextDim.name()));

    valueOut = new QLabel("---", card);
    valueOut->setStyleSheet(QString("color:%1;font-size:20px;font-weight:bold;")
                            .arg(valueColor.name()));

    l->addWidget(title);
    l->addWidget(valueOut);
    return card;
}

// -------------------------------------------------------
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Случайное блуждание — Нетрезвый пешеход");
    resize(1280, 820);

    qApp->setStyle("Fusion");
    QPalette pal;
    pal.setColor(QPalette::Window,          Colors::BG);
    pal.setColor(QPalette::WindowText,      Colors::Text);
    pal.setColor(QPalette::Base,            Colors::Surface);
    pal.setColor(QPalette::AlternateBase,   Colors::Surface2);
    pal.setColor(QPalette::Text,            Colors::Text);
    pal.setColor(QPalette::Button,          Colors::Surface2);
    pal.setColor(QPalette::ButtonText,      Colors::Text);
    pal.setColor(QPalette::Highlight,       Colors::Accent);
    pal.setColor(QPalette::HighlightedText, Colors::BG);
    qApp->setPalette(pal);

    QFont appFont("JetBrains Mono", 9);
    qApp->setFont(appFont);

    qApp->setStyleSheet(
        "QSlider::groove:horizontal { height:2px; background:#1e2235; border-radius:1px; }"
        "QSlider::handle:horizontal { width:14px; height:14px; margin:-6px 0; "
        "  background:#4fffb0; border-radius:7px; }"
        "QSlider::sub-page:horizontal { background:#4fffb0; border-radius:1px; }"
        "QSpinBox, QDoubleSpinBox { background:#0e1018; border:1px solid #1e2235; "
        "  border-radius:4px; color:#c8d0e8; padding:4px 6px; }"
        "QSpinBox:focus, QDoubleSpinBox:focus { border:1px solid #4fffb0; }"
        "QPushButton { border-radius:5px; padding:8px 16px; font-size:10px; }"
        "QPushButton#primary { background:#4fffb0; color:#000; font-weight:bold; }"
        "QPushButton#primary:hover { background:#7fffc4; }"
        "QPushButton#primary:pressed { background:#2eff98; }"
        "QPushButton#secondary { background:#141622; border:1px solid #1e2235; color:#5a6282; }"
        "QPushButton#secondary:hover { border:1px solid #4fffb0; color:#4fffb0; }"
        "QProgressBar { border:1px solid #1e2235; border-radius:3px; background:#0e1018; "
        "  color:transparent; max-height:4px; }"
        "QProgressBar::chunk { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "  stop:0 #4fffb0, stop:1 #4f9fff); border-radius:3px; }"
        "QGroupBox { border:1px solid #1e2235; border-radius:6px; margin-top:16px; "
        "  color:#4fffb0; font-size:9px; letter-spacing:2px; }"
        "QGroupBox::title { subcontrol-origin:margin; left:10px; }"
        "QSplitter::handle { background:#1e2235; }"
        "QScrollArea { border:none; }"
    );

    // --- Worker setup ---
    qRegisterMetaType<BStats>();
    qRegisterMetaType<SimParams>();
    qRegisterMetaType<WalkResult>();
    qRegisterMetaType<QList<BStats>>();
    qRegisterMetaType<QList<int>>();

    m_worker = new SimulationWorker();
    m_thread = new QThread(this);
    m_worker->moveToThread(m_thread);
    m_thread->start();

    connect(m_worker, &SimulationWorker::progressChanged,
            this,     &MainWindow::onProgressChanged);
    connect(m_worker, &SimulationWorker::stepCompleted,
            this,     &MainWindow::onStepCompleted);
    connect(m_worker, &SimulationWorker::finished,
            this,     &MainWindow::onSimulationFinished);
    connect(m_worker, &SimulationWorker::trajectoryReady,
            this,     &MainWindow::onTrajectoryReady);
    connect(m_worker, &SimulationWorker::errorOccurred,
            this,     &MainWindow::onWorkerError);

    // --- Layout ---
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(buildControlPanel());
    splitter->addWidget(buildChartsArea());
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({300, 980});

    setCentralWidget(splitter);
    setupCharts();
}

MainWindow::~MainWindow()
{
    m_thread->quit();
    m_thread->wait();
    delete m_worker;
}

// -------------------------------------------------------
QWidget* MainWindow::buildControlPanel()
{
    QWidget* panel = new QWidget();
    panel->setFixedWidth(300);
    panel->setStyleSheet(QString("background:%1;").arg(Colors::Surface.name()));

    QVBoxLayout* vl = new QVBoxLayout(panel);
    vl->setContentsMargins(16, 16, 16, 16);
    vl->setSpacing(12);

    QLabel* title = new QLabel("// Параметры");
    title->setStyleSheet(QString("color:%1;font-size:9px;letter-spacing:3px;"
                                 "border-bottom:1px solid %2;padding-bottom:8px;")
                         .arg(Colors::Accent.name())
                         .arg(Colors::Border.name()));
    vl->addWidget(title);

    // --- p slider ---
    QGroupBox* paramGroup = new QGroupBox("ВЕРОЯТНОСТИ");
    QVBoxLayout* pg = new QVBoxLayout(paramGroup);
    pg->setSpacing(10);

    {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel("p (вперёд)");
        lbl->setStyleSheet(QString("color:%1;").arg(Colors::TextDim.name()));
        m_pLabel = new QLabel("0.40");
        m_pLabel->setStyleSheet(QString("color:%1;font-weight:bold;font-size:12px;")
                                .arg(Colors::Accent.name()));
        row->addWidget(lbl);
        row->addStretch();
        row->addWidget(m_pLabel);
        pg->addLayout(row);
    }

    m_pSlider = new QSlider(Qt::Horizontal);
    m_pSlider->setRange(1, 99);
    m_pSlider->setValue(40);
    pg->addWidget(m_pSlider);
    connect(m_pSlider, &QSlider::valueChanged, this, &MainWindow::onPChanged);

    {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel("q = 1 - p (назад)");
        lbl->setStyleSheet(QString("color:%1;").arg(Colors::TextDim.name()));
        m_qLabel = new QLabel("0.60");
        m_qLabel->setStyleSheet(QString("color:%1;font-weight:bold;font-size:12px;")
                                .arg(Colors::Cliff.name()));
        row->addWidget(lbl);
        row->addStretch();
        row->addWidget(m_qLabel);
        pg->addLayout(row);
    }
    vl->addWidget(paramGroup);

    // --- Sim params ---
    QGroupBox* simGroup = new QGroupBox("ПАРАМЕТРЫ СИМУЛЯЦИИ");
    QFormLayout* fl = new QFormLayout(simGroup);
    fl->setSpacing(8);
    fl->setLabelAlignment(Qt::AlignLeft);

    auto makeLabel = [](const QString& t) {
        QLabel* l = new QLabel(t);
        l->setStyleSheet(QString("color:%1;").arg(Colors::TextDim.name()));
        return l;
    };

    m_bMaxSpin = new QSpinBox();
    m_bMaxSpin->setRange(2, 50);
    m_bMaxSpin->setValue(15);
    m_bMaxSpin->setSuffix("  (1...N)");
    fl->addRow(makeLabel("Макс. позиция B"), m_bMaxSpin);

    m_simSpin = new QSpinBox();
    m_simSpin->setRange(100, 10000);
    m_simSpin->setValue(2000);
    m_simSpin->setSingleStep(100);
    fl->addRow(makeLabel("Симуляций на точку"), m_simSpin);

    m_maxStepsSpin = new QSpinBox();
    m_maxStepsSpin->setRange(1000, 1000000);
    m_maxStepsSpin->setValue(50000);
    m_maxStepsSpin->setSingleStep(5000);
    fl->addRow(makeLabel("Лимит шагов"), m_maxStepsSpin);

    vl->addWidget(simGroup);

    // --- Buttons ---
    m_runBtn = new QPushButton(">  ЗАПУСТИТЬ СИМУЛЯЦИЮ");
    m_runBtn->setObjectName("primary");
    m_walkBtn = new QPushButton("Показать одно блуждание");
    m_walkBtn->setObjectName("secondary");
    connect(m_runBtn,  &QPushButton::clicked, this, &MainWindow::onRunSimulation);
    connect(m_walkBtn, &QPushButton::clicked, this, &MainWindow::onRunSingleWalk);
    vl->addWidget(m_runBtn);
    vl->addWidget(m_walkBtn);

    m_progress = new QProgressBar();
    m_progress->setRange(0, 100);
    m_progress->setValue(0);
    vl->addWidget(m_progress);

    m_statusLabel = new QLabel("");
    m_statusLabel->setStyleSheet(QString("color:%1;font-size:9px;")
                                 .arg(Colors::TextDim.name()));
    m_statusLabel->setWordWrap(true);
    vl->addWidget(m_statusLabel);

    // --- Stat cards ---
    QGroupBox* statGroup = new QGroupBox("СТАТИСТИКА (средний B)");
    QGridLayout* sg = new QGridLayout(statGroup);
    sg->setSpacing(6);

    sg->addWidget(makeStatCard("P(ОБРЫВ)",  Colors::Cliff,  m_lCliff,  statGroup), 0, 0);
    sg->addWidget(makeStatCard("P(КАФЕ)",   Colors::Cafe,   m_lCafe,   statGroup), 0, 1);
    sg->addWidget(makeStatCard("СР.ШАГИ",   Colors::Blue,   m_lSteps,  statGroup), 1, 0);
    sg->addWidget(makeStatCard("P(ТЕОРИЯ)", Colors::Theory, m_lTheory, statGroup), 1, 1);
    vl->addWidget(statGroup);

    // --- Theory box ---
    QLabel* theory = new QLabel(
        "<b style='color:#ffd04f'>Теория (N->inf):</b><br>"
        "p &gt; q:  P(обрыв|B) = <b>(q/p)^B</b><br>"
        "p &lt; q:  P(обрыв|B) = <b>1</b><br>"
        "p = q:  P(обрыв|B) = <b>1</b><br>"
        "<span style='color:#5a6282;font-size:8px'>"
        "Симметричное блуждание рекуррентно<br>в 1D: пешеход всегда упадёт.</span>"
    );
    theory->setStyleSheet(
        QString("background:%1;border:1px solid %2;border-radius:5px;"
                "padding:10px;color:%3;font-size:9px;")
        .arg("#ffd04f12").arg("#ffd04f33").arg(Colors::TextDim.name()));
    theory->setTextFormat(Qt::RichText);
    theory->setWordWrap(true);
    vl->addWidget(theory);

    vl->addStretch();
    return panel;
}

// -------------------------------------------------------
QWidget* MainWindow::buildChartsArea()
{
    QWidget* area = new QWidget();
    area->setStyleSheet(QString("background:%1;").arg(Colors::BG.name()));
    QVBoxLayout* vl = new QVBoxLayout(area);
    vl->setContentsMargins(12, 12, 12, 12);
    vl->setSpacing(12);

    QLabel* header = new QLabel("Нетрезвый пешеход на одномерной улице");
    QFont hf("JetBrains Mono", 16, QFont::Bold);
    header->setFont(hf);
    header->setStyleSheet(QString("color:%1;").arg(Colors::Text.name()));
    vl->addWidget(header);

    QLabel* sub = new QLabel(
        "Задача о разорении игрока - случайное блуждание с поглощающими барьерами (обрыв=0, кафе=B)");
    sub->setStyleSheet(QString("color:%1;font-size:9px;").arg(Colors::TextDim.name()));
    vl->addWidget(sub);

    // Street widget
    m_street = new StreetWidget();
    connect(m_street, &StreetWidget::animationFinished,
            this,     &MainWindow::onAnimationFinished);
    vl->addWidget(m_street);

    QSplitter* chartSplit = new QSplitter(Qt::Vertical);
    chartSplit->setStyleSheet("QSplitter::handle{background:#1e2235;height:2px;}");

    // Chart 1
    {
        QWidget* wrap = new QWidget();
        wrap->setStyleSheet(QString("background:%1;border:1px solid %2;border-radius:6px;")
                            .arg(Colors::Surface.name()).arg(Colors::Border.name()));
        QVBoxLayout* wl = new QVBoxLayout(wrap);
        wl->setContentsMargins(12, 10, 12, 10);

        QLabel* ct = new QLabel("// ВЕРОЯТНОСТИ - P(обрыв|B) и P(кафе|B)");
        ct->setStyleSheet(QString("color:%1;font-size:9px;letter-spacing:2px;")
                          .arg(Colors::Accent.name()));
        wl->addWidget(ct);

        QHBoxLayout* leg = new QHBoxLayout();
        auto addLeg = [&](const QColor& c, const QString& t) {
            QLabel* dot = new QLabel();
            dot->setFixedSize(24, 3);
            dot->setStyleSheet(QString("background:%1;border-radius:1px;").arg(c.name()));
            QLabel* txt = new QLabel(t);
            txt->setStyleSheet(QString("color:%1;font-size:9px;").arg(Colors::TextDim.name()));
            leg->addWidget(dot);
            leg->addWidget(txt);
            leg->addSpacing(8);
        };
        addLeg(Colors::Cliff,  "P(обрыв) - эмпир.");
        addLeg(Colors::Theory, "P(обрыв) - теория");
        addLeg(Colors::Cafe,   "P(кафе)  - эмпир.");
        leg->addStretch();
        wl->addLayout(leg);

        m_probView = new QChartView();
        m_probView->setRenderHint(QPainter::Antialiasing);
        m_probView->setStyleSheet("background:transparent;border:none;");
        wl->addWidget(m_probView);
        chartSplit->addWidget(wrap);
    }

    // Chart 2
    {
        QWidget* wrap = new QWidget();
        wrap->setStyleSheet(QString("background:%1;border:1px solid %2;border-radius:6px;")
                            .arg(Colors::Surface.name()).arg(Colors::Border.name()));
        QVBoxLayout* wl = new QVBoxLayout(wrap);
        wl->setContentsMargins(12, 10, 12, 10);

        QLabel* ct = new QLabel("// ТРАЕКТОРИЯ - позиция пешехода во времени");
        ct->setStyleSheet(QString("color:%1;font-size:9px;letter-spacing:2px;")
                          .arg(Colors::Accent.name()));
        wl->addWidget(ct);

        m_trajView = new QChartView();
        m_trajView->setRenderHint(QPainter::Antialiasing);
        m_trajView->setStyleSheet("background:transparent;border:none;");
        wl->addWidget(m_trajView);
        chartSplit->addWidget(wrap);
    }

    vl->addWidget(chartSplit, 1);
    return area;
}

// -------------------------------------------------------
static QChart* makeChart(const QColor& bg)
{
    QChart* ch = new QChart();
    ch->setBackgroundBrush(QBrush(bg));
    ch->setPlotAreaBackgroundBrush(QBrush(QColor(10, 12, 20)));
    ch->setPlotAreaBackgroundVisible(true);
    ch->legend()->setVisible(false);
    ch->setMargins(QMargins(4, 4, 4, 4));
    return ch;
}

static QValueAxis* makeAxis(Qt::Orientation ori,
                             const QString& title,
                             QChart* ch,
                             const QColor& textColor)
{
    QValueAxis* ax = new QValueAxis();
    ax->setTitleText(title);
    ax->setTitleBrush(QBrush(textColor));
    ax->setLabelsBrush(QBrush(textColor));
    ax->setGridLineColor(QColor(30, 34, 52));
    ax->setLinePen(QPen(QColor(35, 40, 65)));
    ax->setShadesBrush(Qt::NoBrush);
    ch->addAxis(ax, ori == Qt::Horizontal ? Qt::AlignBottom : Qt::AlignLeft);
    return ax;
}

void MainWindow::setupCharts()
{
    // Prob chart
    QChart* pc = makeChart(Colors::Surface);
    m_cliffSeries  = new QLineSeries();
    m_cafeSeries   = new QLineSeries();
    m_theorySeries = new QLineSeries();

    m_cliffSeries->setPen(QPen(Colors::Cliff,  2.0));
    m_cafeSeries->setPen(QPen(Colors::Cafe,    2.0));
    QPen dashPen(Colors::Theory, 1.5);
    dashPen.setStyle(Qt::DashLine);
    m_theorySeries->setPen(dashPen);

    pc->addSeries(m_cliffSeries);
    pc->addSeries(m_cafeSeries);
    pc->addSeries(m_theorySeries);

    m_probAxisX = makeAxis(Qt::Horizontal, "Начальная позиция B", pc, Colors::TextDim);
    m_probAxisY = makeAxis(Qt::Vertical,   "Вероятность",         pc, Colors::TextDim);
    m_probAxisX->setRange(1, m_bMaxSpin->value());
    m_probAxisY->setRange(0, 1.0);
    m_probAxisY->setTickCount(6);

    for (auto* s : {m_cliffSeries, m_cafeSeries, m_theorySeries}) {
        s->attachAxis(m_probAxisX);
        s->attachAxis(m_probAxisY);
    }
    m_probView->setChart(pc);

    // Traj chart
    QChart* tc = makeChart(Colors::Surface);
    m_trajSeries = new QLineSeries();
    m_trajSeries->setPen(QPen(Colors::Blue, 1.5));
    tc->addSeries(m_trajSeries);

    m_trajAxisX = makeAxis(Qt::Horizontal, "Шаг",     tc, Colors::TextDim);
    m_trajAxisY = makeAxis(Qt::Vertical,   "Позиция", tc, Colors::TextDim);
    m_trajSeries->attachAxis(m_trajAxisX);
    m_trajSeries->attachAxis(m_trajAxisY);
    m_trajView->setChart(tc);
}

// -------------------------------------------------------
void MainWindow::clearCharts()
{
    m_cliffSeries->clear();
    m_cafeSeries->clear();
    m_theorySeries->clear();
}

void MainWindow::setUiRunning(bool running)
{
    m_runBtn->setEnabled(!running);
    m_walkBtn->setEnabled(!running);
    m_runBtn->setText(running ? "Расчёт..." : ">  ЗАПУСТИТЬ СИМУЛЯЦИЮ");
}

// -------------------------------------------------------
void MainWindow::onPChanged(int value)
{
    double p = value / 100.0;
    m_pLabel->setText(QString::number(p, 'f', 2));
    m_qLabel->setText(QString::number(1.0 - p, 'f', 2));
}

void MainWindow::onRunSimulation()
{
    SimParams params;
    params.p        = m_pSlider->value() / 100.0;
    params.bMin     = 1;
    params.bMax     = m_bMaxSpin->value();
    params.simCount = m_simSpin->value();
    params.maxSteps = static_cast<uint64_t>(m_maxStepsSpin->value());

    clearCharts();
    m_progress->setValue(0);
    m_probAxisX->setRange(1, params.bMax);
    setUiRunning(true);

    QMetaObject::invokeMethod(m_worker, "run",
                              Qt::QueuedConnection,
                              Q_ARG(RandomWalk::SimParams, params));
}

void MainWindow::onRunSingleWalk()
{
    const double   p    = m_pSlider->value() / 100.0;
    const int      B    = std::min(5, m_bMaxSpin->value());
    const uint64_t maxS = static_cast<uint64_t>(m_maxStepsSpin->value());
    m_trajB = B;
    setUiRunning(true);
    QMetaObject::invokeMethod(m_worker, "runSingleTrajectory",
                              Qt::QueuedConnection,
                              Q_ARG(int,      B),
                              Q_ARG(double,   p),
                              Q_ARG(uint64_t, maxS));
}

void MainWindow::onProgressChanged(int current, int total)
{
    int pct = (total > 0) ? (current * 100 / total) : 0;
    m_progress->setValue(pct);
    m_statusLabel->setText(QString("B = %1 / %2").arg(current).arg(total));
}

void MainWindow::onStepCompleted(BStats stats)
{
    m_cliffSeries->append(stats.B, stats.pCliff);
    m_cafeSeries->append(stats.B,  stats.pCafe);
    m_theorySeries->append(stats.B, stats.pTheory);
}

void MainWindow::onSimulationFinished(QList<BStats> allStats)
{
    m_lastStats = allStats;
    setUiRunning(false);
    m_progress->setValue(100);
    m_statusLabel->setText(
        QString("Готово - %1 точек x %2 симуляций")
        .arg(allStats.size())
        .arg(allStats.isEmpty() ? 0 : allStats.first().simCount));

    if (!allStats.isEmpty()) updateStats(allStats);

    // Show trajectory for B=5
    const double   p    = m_pSlider->value() / 100.0;
    const int      B    = std::min(5, m_bMaxSpin->value());
    const uint64_t maxS = static_cast<uint64_t>(m_maxStepsSpin->value());
    m_trajB = B;
    QMetaObject::invokeMethod(m_worker, "runSingleTrajectory",
                              Qt::QueuedConnection,
                              Q_ARG(int,      B),
                              Q_ARG(double,   p),
                              Q_ARG(uint64_t, maxS));
}

void MainWindow::updateStats(const QList<BStats>& stats)
{
    const int midIdx = stats.size() / 2;
    const BStats& s = stats[midIdx];
    m_lCliff->setText(QString("%1%").arg(s.pCliff  * 100.0, 0, 'f', 1));
    m_lCafe->setText( QString("%1%").arg(s.pCafe   * 100.0, 0, 'f', 1));
    m_lSteps->setText(QString::number(static_cast<int>(s.avgSteps)));
    m_lTheory->setText(QString("%1%").arg(s.pTheory * 100.0, 0, 'f', 1));
}

void MainWindow::onTrajectoryReady(QList<int> traj, int B, WalkResult result)
{
    setUiRunning(false);

    // Animate street
    m_street->loadTrajectory(traj, B, result);

    // Draw trajectory chart
    m_trajSeries->clear();
    // Fix: cast both sides to int to avoid qsizetype vs int mismatch
    const int maxShow = std::min(static_cast<int>(traj.size()), 2000);
    int minPos = B, maxPos = B;
    for (int i = 0; i < maxShow; ++i) {
        m_trajSeries->append(i, traj[i]);
        minPos = std::min(minPos, traj[i]);
        maxPos = std::max(maxPos, traj[i]);
    }
    m_trajAxisX->setRange(0, maxShow - 1);
    m_trajAxisY->setRange(std::max(0, minPos - 1), maxPos + 1);

    QString res;
    if      (result == WalkResult::Cliff)   res = "Упал в обрыв";
    else if (result == WalkResult::Cafe)    res = "Вернулся в кафе";
    else                                    res = "Лимит шагов";

    m_trajView->chart()->setTitle(
        QString("Траектория (B=%1, %2 шагов) - %3")
        .arg(B).arg(traj.size() - 1).arg(res));

    QFont tf; tf.setPixelSize(10);
    m_trajView->chart()->setTitleFont(tf);
    m_trajView->chart()->setTitleBrush(QBrush(Colors::TextDim));
}

void MainWindow::onAnimationFinished(WalkResult result, int steps)
{
    QString msg;
    if      (result == WalkResult::Cliff) msg = QString("Упал в обрыв за %1 шагов").arg(steps);
    else if (result == WalkResult::Cafe)  msg = QString("Вернулся в кафе за %1 шагов").arg(steps);
    else                                  msg = QString("Превышен лимит (%1 шагов)").arg(steps);
    m_statusLabel->setText(msg);
}

void MainWindow::onWorkerError(QString message)
{
    setUiRunning(false);
    QMessageBox::warning(this, "Ошибка симуляции", message);
}

} // namespace RandomWalk
