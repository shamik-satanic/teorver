#include "MainWindow.h"
#include <QApplication>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QFrame>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QScrollArea>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

namespace PointMotion {

namespace C {
    const QColor BG      {10, 12, 20};
    const QColor Surface {16, 19, 30};
    const QColor Surface2{22, 26, 40};
    const QColor Border  {35, 42, 65};
    const QColor Accent  {79,255,176};
    const QColor Accent2 {79,159,255};
    const QColor Warn    {255,200,79};
    const QColor Danger  {255, 90, 90};
    const QColor TextDim {90, 98,130};
    const QColor Text    {200,210,235};
}

static QLabel* dimLabel(const QString& t, QWidget* p = nullptr)
{
    auto* l = new QLabel(t, p);
    l->setStyleSheet(QString("color:%1;font-size:9px;").arg(C::TextDim.name()));
    return l;
}

static QFrame* statCard(const QString& title, const QColor& col,
                        QLabel*& valOut, QWidget* parent)
{
    auto* card = new QFrame(parent);
    card->setStyleSheet(
        QString("QFrame{background:%1;border:1px solid %2;border-radius:5px;}")
        .arg(C::Surface2.name()).arg(C::Border.name()));
    auto* vl = new QVBoxLayout(card);
    vl->setContentsMargins(9,7,9,7);
    vl->setSpacing(2);
    auto* tl = new QLabel(title, card);
    tl->setStyleSheet(
        QString("color:%1;font-size:9px;letter-spacing:1px;").arg(C::TextDim.name()));
    valOut = new QLabel("---", card);
    valOut->setStyleSheet(
        QString("color:%1;font-size:18px;font-weight:bold;").arg(col.name()));
    vl->addWidget(tl);
    vl->addWidget(valOut);
    return card;
}

// -------------------------------------------------------
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Движение материальной точки");
    resize(1300, 840);

    qApp->setStyle("Fusion");
    QPalette pal;
    pal.setColor(QPalette::Window,          C::BG);
    pal.setColor(QPalette::WindowText,      C::Text);
    pal.setColor(QPalette::Base,            C::Surface);
    pal.setColor(QPalette::AlternateBase,   C::Surface2);
    pal.setColor(QPalette::Text,            C::Text);
    pal.setColor(QPalette::Button,          C::Surface2);
    pal.setColor(QPalette::ButtonText,      C::Text);
    pal.setColor(QPalette::Highlight,       C::Accent);
    pal.setColor(QPalette::HighlightedText, C::BG);
    qApp->setPalette(pal);
    QFont f("JetBrains Mono", 9);
    qApp->setFont(f);

    qApp->setStyleSheet(
        "QSpinBox,QDoubleSpinBox,QComboBox{"
        "  background:#0a0c14;border:1px solid #232840;border-radius:4px;"
        "  color:#c8d2eb;padding:4px 6px;}"
        "QSpinBox:focus,QDoubleSpinBox:focus,QComboBox:focus{border:1px solid #4fffb0;}"
        "QComboBox::drop-down{border:none;}"
        "QComboBox QAbstractItemView{background:#0e1018;color:#c8d2eb;border:1px solid #232840;}"
        "QPushButton{border-radius:5px;padding:8px 14px;}"
        "QPushButton#primary{background:#4fffb0;color:#000;font-weight:bold;font-size:10px;}"
        "QPushButton#primary:hover{background:#7fffc4;}"
        "QPushButton#secondary{background:#141622;border:1px solid #232840;"
        "  color:#5a6282;font-size:9px;}"
        "QPushButton#secondary:hover{border:1px solid #4fffb0;color:#4fffb0;}"
        "QGroupBox{border:1px solid #232840;border-radius:6px;margin-top:14px;"
        "  color:#4fffb0;font-size:9px;letter-spacing:2px;}"
        "QGroupBox::title{subcontrol-origin:margin;left:10px;}"
        "QTabWidget::pane{border:1px solid #232840;border-radius:4px;}"
        "QTabBar::tab{background:#0e1018;color:#5a6282;padding:6px 14px;"
        "  border:1px solid #232840;border-bottom:none;"
        "  border-radius:4px 4px 0 0;margin-right:2px;}"
        "QTabBar::tab:selected{background:#141622;color:#4fffb0;border-color:#4fffb0;}"
        "QTableWidget{background:#0a0c14;gridline-color:#1a1e2e;}"
        "QTableWidget::item{padding:4px;color:#c8d2eb;}"
        "QTableWidget::item:selected{background:#1a2a3a;}"
        "QHeaderView::section{background:#0e1018;color:#5a6282;"
        "  border:1px solid #1a1e2e;padding:4px;font-size:9px;letter-spacing:1px;}"
        "QSplitter::handle{background:#1a1e2e;}"
        "QScrollBar:vertical{background:#0a0c14;width:8px;}"
        "QScrollBar::handle:vertical{background:#232840;border-radius:4px;}"
    );

    qRegisterMetaType<PointMotion::SimParams>();
    qRegisterMetaType<PointMotion::SimResult>();
    qRegisterMetaType<PointMotion::EmpiricalResult>();

    m_log    = new LogStorage(50, this);
    m_worker = new SimulationWorker();
    m_thread = new QThread(this);
    m_worker->moveToThread(m_thread);
    m_thread->start();

    connect(m_worker, &SimulationWorker::fullReady,
            this,     &MainWindow::onFullReady);
    connect(m_worker, &SimulationWorker::singleReady,
            this,     &MainWindow::onSingleReady);
    connect(m_worker, &SimulationWorker::errorOccurred,
            this,     &MainWindow::onWorkerError);
    connect(m_log, &LogStorage::entriesChanged,
            this,  &MainWindow::refreshLogTable);

    QSplitter* split = new QSplitter(Qt::Horizontal, this);
    split->addWidget(buildControlPanel());
    split->addWidget(buildMainArea());
    split->setSizes({310, 990});
    setCentralWidget(split);
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
    auto* panel = new QWidget();
    panel->setFixedWidth(310);
    panel->setStyleSheet(QString("background:%1;").arg(C::Surface.name()));

    auto* scroll = new QScrollArea();
    scroll->setWidget(panel);
    scroll->setWidgetResizable(true);
    scroll->setFixedWidth(312);
    scroll->setStyleSheet("border:none;background:" + C::Surface.name());

    auto* vl = new QVBoxLayout(panel);
    vl->setContentsMargins(14,14,14,14);
    vl->setSpacing(10);

    auto* ttl = new QLabel("// Параметры задачи");
    ttl->setStyleSheet(
        QString("color:%1;font-size:9px;letter-spacing:3px;"
                "border-bottom:1px solid %2;padding-bottom:7px;")
        .arg(C::Accent.name()).arg(C::Border.name()));
    vl->addWidget(ttl);

    // --- Движение ---
    auto* baseGroup = new QGroupBox("ДВИЖЕНИЕ");
    auto* fl = new QFormLayout(baseGroup);
    fl->setSpacing(7);
    fl->setLabelAlignment(Qt::AlignLeft);

    m_y0Spin = new QDoubleSpinBox();
    m_y0Spin->setRange(-1e6, 1e6);
    m_y0Spin->setValue(10.0);
    m_y0Spin->setDecimals(2);
    fl->addRow(dimLabel("Y0 — нач. позиция"), m_y0Spin);

    m_hSpin = new QDoubleSpinBox();
    m_hSpin->setRange(0.01, 1000);
    m_hSpin->setValue(1.0);
    m_hSpin->setDecimals(3);
    fl->addRow(dimLabel("h — шаг по X"), m_hSpin);

    m_nSpin = new QSpinBox();
    m_nSpin->setRange(1, 100000);
    m_nSpin->setValue(100);
    fl->addRow(dimLabel("n — число шагов"), m_nSpin);

    vl->addWidget(baseGroup);

    // --- Распределение ---
    auto* distGroup = new QGroupBox("РАСПРЕДЕЛЕНИЕ s");
    auto* dl = new QFormLayout(distGroup);
    dl->setSpacing(7);
    dl->setLabelAlignment(Qt::AlignLeft);

    m_distCombo = new QComboBox();
    m_distCombo->addItem("Равномерное",
        static_cast<int>(Distribution::Uniform));
    m_distCombo->addItem("Биномиальное",
        static_cast<int>(Distribution::Binomial));
    m_distCombo->addItem("Конечное геометрическое",
        static_cast<int>(Distribution::FiniteGeometric));
    m_distCombo->addItem("Дискретное треугольное",
        static_cast<int>(Distribution::DiscreteTriangle));
    dl->addRow(dimLabel("Закон"), m_distCombo);

    m_sMinSpin = new QDoubleSpinBox();
    m_sMinSpin->setRange(-1000, 1000);
    m_sMinSpin->setValue(-3.0);
    m_sMinSpin->setDecimals(2);
    dl->addRow(dimLabel("s_min"), m_sMinSpin);

    m_sMaxSpin = new QDoubleSpinBox();
    m_sMaxSpin->setRange(-1000, 1000);
    m_sMaxSpin->setValue(3.0);
    m_sMaxSpin->setDecimals(2);
    dl->addRow(dimLabel("s_max"), m_sMaxSpin);

    m_binomGroup = new QGroupBox("ПАРАМЕТРЫ БИНОМИАЛЬНОГО/ГЕОМ.");
    auto* bl = new QFormLayout(m_binomGroup);
    bl->setSpacing(6);
    m_nTrialsSpin = new QSpinBox();
    m_nTrialsSpin->setRange(1, 200);
    m_nTrialsSpin->setValue(6);
    bl->addRow(dimLabel("n_trials"), m_nTrialsSpin);
    m_probSpin = new QDoubleSpinBox();
    m_probSpin->setRange(0.001, 0.999);
    m_probSpin->setValue(0.5);
    m_probSpin->setDecimals(3);
    m_probSpin->setSingleStep(0.05);
    bl->addRow(dimLabel("prob"), m_probSpin);
    dl->addRow(m_binomGroup);

    vl->addWidget(distGroup);

    connect(m_distCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onDistChanged);
    onDistChanged(0);

    // --- Статистика ---
    auto* statGroup = new QGroupBox("ЭМПИРИЧЕСКАЯ СТАТИСТИКА");
    auto* sl = new QFormLayout(statGroup);
    sl->setSpacing(7);
    sl->setLabelAlignment(Qt::AlignLeft);

    m_runsSpin = new QSpinBox();
    m_runsSpin->setRange(10, 100000);
    m_runsSpin->setValue(1000);
    sl->addRow(dimLabel("runs — прогонов"), m_runsSpin);

    m_lSpin = new QSpinBox();
    m_lSpin->setRange(0, 10000);
    m_lSpin->setValue(3);
    sl->addRow(dimLabel("l — целевых пересечений"), m_lSpin);

    vl->addWidget(statGroup);

    // --- Лог ---
    auto* logGroup = new QGroupBox("ЛОГ");
    auto* ll = new QFormLayout(logGroup);
    ll->setSpacing(7);
    ll->setLabelAlignment(Qt::AlignLeft);
    m_kSpin = new QSpinBox();
    m_kSpin->setRange(1, 500);
    m_kSpin->setValue(50);
    connect(m_kSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            m_log,  &LogStorage::setMaxEntries);
    ll->addRow(dimLabel("K — хранить последних"), m_kSpin);
    vl->addWidget(logGroup);

    // Buttons
    m_runBtn = new QPushButton(">  ЗАПУСТИТЬ (полный)");
    m_runBtn->setObjectName("primary");
    m_singleBtn = new QPushButton("Одна траектория");
    m_singleBtn->setObjectName("secondary");
    connect(m_runBtn,    &QPushButton::clicked, this, &MainWindow::onRun);
    connect(m_singleBtn, &QPushButton::clicked, this, &MainWindow::onSingleRun);
    vl->addWidget(m_runBtn);
    vl->addWidget(m_singleBtn);

    auto* cfgRow = new QHBoxLayout();
    auto* loadBtn = new QPushButton("Загрузить JSON");
    loadBtn->setObjectName("secondary");
    auto* saveBtn = new QPushButton("Сохранить JSON");
    saveBtn->setObjectName("secondary");
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::onLoadConfig);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveConfig);
    cfgRow->addWidget(loadBtn);
    cfgRow->addWidget(saveBtn);
    vl->addLayout(cfgRow);

    // Stat cards
    auto* cardGroup = new QGroupBox("РЕЗУЛЬТАТЫ");
    auto* cg = new QGridLayout(cardGroup);
    cg->setSpacing(6);
    cg->addWidget(statCard("P(>= l)", C::Accent,  m_lPAtLeastL, cardGroup), 0, 0);
    cg->addWidget(statCard("P(== l)", C::Accent2, m_lPExactL,   cardGroup), 0, 1);
    cg->addWidget(statCard("P(0)",    C::Warn,    m_lPZero,     cardGroup), 1, 0);
    cg->addWidget(statCard("E[l]",    C::Danger,  m_lMeanC,     cardGroup), 1, 1);
    vl->addWidget(cardGroup);

    vl->addStretch();
    return scroll;
}

// -------------------------------------------------------
QWidget* MainWindow::buildMainArea()
{
    auto* area = new QWidget();
    area->setStyleSheet(QString("background:%1;").arg(C::BG.name()));
    auto* vl = new QVBoxLayout(area);
    vl->setContentsMargins(10,10,10,10);
    vl->setSpacing(10);

    auto* hdr = new QLabel("Движение материальной точки");
    hdr->setFont(QFont("JetBrains Mono", 15, QFont::Bold));
    hdr->setStyleSheet(QString("color:%1;").arg(C::Text.name()));
    vl->addWidget(hdr);

    auto* sub = new QLabel("x(t) = x(t-1) + h,   y(t) = y(t-1) + s,   старт: (0, Y0)");
    sub->setStyleSheet(
        QString("color:%1;font-size:9px;").arg(C::TextDim.name()));
    vl->addWidget(sub);

    m_tabs = new QTabWidget();

    m_trajWidget = new TrajectoryWidget();
    m_tabs->addTab(m_trajWidget, "Траектория");

    // Histogram tab
    auto* histWrap = new QWidget();
    histWrap->setStyleSheet(
        QString("background:%1;").arg(C::Surface.name()));
    auto* hvl = new QVBoxLayout(histWrap);
    hvl->setContentsMargins(8,8,8,8);
    auto* hlbl = new QLabel("// Гистограмма числа пересечений оси X");
    hlbl->setStyleSheet(
        QString("color:%1;font-size:9px;letter-spacing:2px;").arg(C::Accent.name()));
    hvl->addWidget(hlbl);
    m_histView = new QChartView();
    m_histView->setRenderHint(QPainter::Antialiasing);
    m_histView->setStyleSheet("background:transparent;border:none;");
    hvl->addWidget(m_histView);
    m_tabs->addTab(histWrap, "Гистограмма");

    m_tabs->addTab(buildLogPanel(), "Лог");

    vl->addWidget(m_tabs, 1);
    return area;
}

// -------------------------------------------------------
QWidget* MainWindow::buildLogPanel()
{
    auto* w = new QWidget();
    auto* vl = new QVBoxLayout(w);
    vl->setContentsMargins(8,8,8,8);
    vl->setSpacing(8);

    auto* lbl = new QLabel("// Лог последних K моделирований");
    lbl->setStyleSheet(
        QString("color:%1;font-size:9px;letter-spacing:2px;").arg(C::Accent.name()));
    vl->addWidget(lbl);

    QSplitter* sp = new QSplitter(Qt::Vertical);

    m_logTable = new QTableWidget(0, 6);
    m_logTable->setHorizontalHeaderLabels(
        {"Время","Распр.","n","l","P(>=l)","Пересеч. avg"});
    m_logTable->horizontalHeader()->setStretchLastSection(true);
    m_logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_logTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_logTable->verticalHeader()->setVisible(false);

    // currentCellChanged(row, col, prevRow, prevCol)
    connect(m_logTable, &QTableWidget::currentCellChanged,
            this, [this](int row, int, int, int) {
                onLogSelectionChanged(row);
            });

    sp->addWidget(m_logTable);

    auto* detailW = new QWidget();
    detailW->setStyleSheet(
        QString("background:%1;").arg(C::Surface.name()));
    auto* dl = new QVBoxLayout(detailW);
    dl->setContentsMargins(8,8,8,8);
    dl->setSpacing(6);

    m_logDetailLabel = new QLabel(
        "Выберите запись в таблице для просмотра деталей");
    m_logDetailLabel->setStyleSheet(
        QString("color:%1;font-size:9px;padding:6px;").arg(C::TextDim.name()));
    m_logDetailLabel->setWordWrap(true);
    dl->addWidget(m_logDetailLabel);

    m_logTrajWidget = new TrajectoryWidget();
    m_logTrajWidget->setMinimumHeight(200);
    dl->addWidget(m_logTrajWidget, 1);

    sp->addWidget(detailW);
    sp->setSizes({200, 280});
    vl->addWidget(sp, 1);
    return w;
}

// -------------------------------------------------------
SimParams MainWindow::paramsFromUI() const
{
    SimParams p;
    p.Y0       = m_y0Spin->value();
    p.h        = m_hSpin->value();
    p.n        = m_nSpin->value();
    p.runs     = m_runsSpin->value();
    p.l_target = m_lSpin->value();
    p.distParams.dist =
        static_cast<Distribution>(m_distCombo->currentData().toInt());
    p.distParams.s_min    = m_sMinSpin->value();
    p.distParams.s_max    = m_sMaxSpin->value();
    p.distParams.n_trials = m_nTrialsSpin->value();
    p.distParams.prob     = m_probSpin->value();
    return p;
}

void MainWindow::applyParamsToUI(const SimParams& p)
{
    m_y0Spin->setValue(p.Y0);
    m_hSpin->setValue(p.h);
    m_nSpin->setValue(p.n);
    m_runsSpin->setValue(p.runs);
    m_lSpin->setValue(p.l_target);
    m_sMinSpin->setValue(p.distParams.s_min);
    m_sMaxSpin->setValue(p.distParams.s_max);
    m_nTrialsSpin->setValue(p.distParams.n_trials);
    m_probSpin->setValue(p.distParams.prob);
    int idx = m_distCombo->findData(static_cast<int>(p.distParams.dist));
    if (idx >= 0) m_distCombo->setCurrentIndex(idx);
}

void MainWindow::setRunning(bool r)
{
    m_runBtn->setEnabled(!r);
    m_singleBtn->setEnabled(!r);
    m_runBtn->setText(r ? "Расчёт..." : ">  ЗАПУСТИТЬ (полный)");
}

// -------------------------------------------------------
void MainWindow::onRun()
{
    setRunning(true);
    m_log->setMaxEntries(m_kSpin->value());
    SimParams p = paramsFromUI();
    QMetaObject::invokeMethod(m_worker, "runFull",
                              Qt::QueuedConnection,
                              Q_ARG(PointMotion::SimParams, p));
}

void MainWindow::onSingleRun()
{
    setRunning(true);
    SimParams p = paramsFromUI();
    QMetaObject::invokeMethod(m_worker, "runSingle",
                              Qt::QueuedConnection,
                              Q_ARG(PointMotion::SimParams, p));
}

// -------------------------------------------------------
void MainWindow::onFullReady(SimResult sample, EmpiricalResult emp)
{
    setRunning(false);
    m_trajWidget->setTrajectory(sample.trajectory, sample.crossings);
    m_tabs->setCurrentIndex(0);

    SimParams p = paramsFromUI();
    updateStatsLabels(emp, p);
    updateHistogramChart(emp, p.l_target);

    LogEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.params    = p;
    entry.empirical = emp;
    entry.sampleRun = sample;
    entry.summary   =
        QString("[%1] %2 | n=%3 | P(>=%4)=%5")
        .arg(entry.timestamp.toString("hh:mm:ss"))
        .arg(QString::fromStdString(distName(p.distParams.dist)))
        .arg(p.n)
        .arg(p.l_target)
        .arg(emp.pAtLeastL, 0, 'f', 3);
    m_log->addEntry(entry);
}

void MainWindow::onSingleReady(SimResult result)
{
    setRunning(false);
    m_trajWidget->setTrajectory(result.trajectory, result.crossings);
    m_tabs->setCurrentIndex(0);
}

void MainWindow::onWorkerError(QString msg)
{
    setRunning(false);
    QMessageBox::warning(this, "Ошибка", msg);
}

// -------------------------------------------------------
void MainWindow::updateStatsLabels(const EmpiricalResult& emp,
                                   const SimParams& /*p*/)
{
    m_lPAtLeastL->setText(
        QString("%1%").arg(emp.pAtLeastL * 100.0, 0, 'f', 2));
    m_lPExactL->setText(
        QString("%1%").arg(emp.pExactL   * 100.0, 0, 'f', 2));
    m_lPZero->setText(
        QString("%1%").arg(emp.pZero     * 100.0, 0, 'f', 2));
    m_lMeanC->setText(
        QString("%1").arg(emp.meanCrossings, 0, 'f', 2));
}

// -------------------------------------------------------
void MainWindow::updateHistogramChart(const EmpiricalResult& emp,
                                      int l_target)
{
    if (emp.histogram.empty()) return;

    int totalRuns = 0;
    for (int v : emp.histogram) totalRuns += v;

    QStringList cats;
    for (int i = 0; i < static_cast<int>(emp.histogram.size()); ++i)
        cats << QString::number(i);

    // Main series (blue)
    auto* setMain = new QBarSet("Прогоны");
    setMain->setColor(QColor(79, 159, 255, 180));
    setMain->setBorderColor(QColor(79, 159, 255));

    // Highlight series (yellow) — separate BarSet for l_target bar
    auto* setHL = new QBarSet("l_target");
    setHL->setColor(QColor(255, 200, 79, 220));
    setHL->setBorderColor(QColor(255, 200, 79));

    for (int i = 0; i < static_cast<int>(emp.histogram.size()); ++i) {
        double pct = totalRuns > 0
            ? emp.histogram[static_cast<size_t>(i)] * 100.0 / totalRuns
            : 0.0;
        if (i == l_target) {
            *setMain << 0.0;
            *setHL   << pct;
        } else {
            *setMain << pct;
            *setHL   << 0.0;
        }
    }

    auto* series = new QBarSeries();
    series->append(setMain);
    series->append(setHL);

    auto* chart = new QChart();
    chart->setBackgroundBrush(QBrush(C::Surface));
    chart->setPlotAreaBackgroundBrush(QBrush(QColor(10,12,20)));
    chart->setPlotAreaBackgroundVisible(true);
    chart->addSeries(series);
    chart->setMargins(QMargins(4,4,4,4));
    chart->legend()->setVisible(false);

    auto* axX = new QBarCategoryAxis();
    axX->append(cats);
    axX->setTitleText("Число пересечений l");
    axX->setTitleBrush(QBrush(C::TextDim));
    axX->setLabelsBrush(QBrush(C::TextDim));
    axX->setGridLineColor(QColor(30,34,52));
    chart->addAxis(axX, Qt::AlignBottom);
    series->attachAxis(axX);

    auto* axY = new QValueAxis();
    axY->setTitleText("% прогонов");
    axY->setTitleBrush(QBrush(C::TextDim));
    axY->setLabelsBrush(QBrush(C::TextDim));
    axY->setGridLineColor(QColor(30,34,52));
    chart->addAxis(axY, Qt::AlignLeft);
    series->attachAxis(axY);

    m_histView->setChart(chart);
}

// -------------------------------------------------------
void MainWindow::refreshLogTable()
{
    m_logTable->setRowCount(0);
    for (int i = m_log->count() - 1; i >= 0; --i) {
        const LogEntry& e = m_log->entry(i);
        int row = m_logTable->rowCount();
        m_logTable->insertRow(row);
        m_logTable->setItem(row, 0,
            new QTableWidgetItem(e.timestamp.toString("hh:mm:ss")));
        m_logTable->setItem(row, 1,
            new QTableWidgetItem(
                QString::fromStdString(distName(e.params.distParams.dist))));
        m_logTable->setItem(row, 2,
            new QTableWidgetItem(QString::number(e.params.n)));
        m_logTable->setItem(row, 3,
            new QTableWidgetItem(QString::number(e.params.l_target)));
        m_logTable->setItem(row, 4,
            new QTableWidgetItem(
                QString("%1%").arg(e.empirical.pAtLeastL * 100.0, 0, 'f', 2)));
        m_logTable->setItem(row, 5,
            new QTableWidgetItem(
                QString("%1").arg(e.empirical.meanCrossings, 0, 'f', 2)));
    }
    m_logTable->resizeColumnsToContents();
}

// -------------------------------------------------------
void MainWindow::onLogSelectionChanged(int row)
{
    int idx = m_log->count() - 1 - row;
    if (idx < 0 || idx >= m_log->count()) return;

    const LogEntry&       e   = m_log->entry(idx);
    const EmpiricalResult& emp = e.empirical;
    const SimParams&       p   = e.params;

    QString detail =
        QString("Время: %1\n"
                "Распределение: %2  s_min=%3  s_max=%4\n"
                "Y0=%5  h=%6  n=%7  runs=%8  l=%9\n"
                "P(>=%9) = %10%   P(==%9) = %11%\n"
                "P(0 пересечений) = %12%   E[l] = %13\n"
                "Пример: %14 шагов, %15 пересечений")
        .arg(e.timestamp.toString("dd.MM.yyyy hh:mm:ss"))
        .arg(QString::fromStdString(distName(p.distParams.dist)))
        .arg(p.distParams.s_min, 0, 'f', 2)
        .arg(p.distParams.s_max, 0, 'f', 2)
        .arg(p.Y0,  0, 'f', 1)
        .arg(p.h,   0, 'f', 2)
        .arg(p.n)
        .arg(p.runs)
        .arg(p.l_target)
        .arg(emp.pAtLeastL * 100.0, 0, 'f', 3)
        .arg(emp.pExactL   * 100.0, 0, 'f', 3)
        .arg(emp.pZero     * 100.0, 0, 'f', 3)
        .arg(emp.meanCrossings, 0, 'f', 2)
        .arg(e.sampleRun.trajectory.size() - 1)
        .arg(e.sampleRun.crossings);

    m_logDetailLabel->setText(detail);
    m_logTrajWidget->setTrajectory(
        e.sampleRun.trajectory, e.sampleRun.crossings);
    m_tabs->setCurrentIndex(2);
}

// -------------------------------------------------------
void MainWindow::onLoadConfig()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Загрузить конфигурацию", "",
        "JSON (*.json);;Все файлы (*)");
    if (path.isEmpty()) return;
    applyParamsToUI(ConfigManager::loadFromFile(path));
}

void MainWindow::onSaveConfig()
{
    QString path = QFileDialog::getSaveFileName(
        this, "Сохранить конфигурацию", "config.json",
        "JSON (*.json);;Все файлы (*)");
    if (path.isEmpty()) return;
    ConfigManager::saveToFile(path, paramsFromUI());
}

void MainWindow::onDistChanged(int /*idx*/)
{
    Distribution d =
        static_cast<Distribution>(m_distCombo->currentData().toInt());
    bool need = (d == Distribution::Binomial ||
                 d == Distribution::FiniteGeometric);
    m_binomGroup->setVisible(need);
}

} // namespace PointMotion
