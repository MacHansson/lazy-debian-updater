#include "chartwrapper.h"
#include <QGraphicsLayout>
#include <QApplication>
#include <QPalette>

#include <algorithm>

#include <debugflag.h>

ChartWrapper::ChartWrapper(QWidget *parentWidget)
    : QObject(parentWidget), m_parentWidget(parentWidget)
{
    m_chart = new QChart();
    m_chart->setTheme(QChart::ChartThemeQt);
    m_chart->legend()->setVisible(false);
    m_chart->setBackgroundVisible(false);
    m_chart->setPlotAreaBackgroundVisible(false);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
    m_chart->setAnimationOptions(QChart::NoAnimation);
    ///m_chart->setAnimationOptions(QChart::SeriesAnimations);
    m_chart->setMargins(QMargins(0, 0, 0, 0));
    m_chart->layout()->setContentsMargins(0, 0, 0, 0);

    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    if (!m_parentWidget->layout()) {
        m_parentWidget->setLayout(new QVBoxLayout());
        m_parentWidget->layout()->setContentsMargins(0, 0, 0, 0);
        m_parentWidget->layout()->setSpacing(0);
    }
    m_parentWidget->layout()->addWidget(m_chartView);

    m_timerHover = new QTimer(this);
    QObject::connect(m_timerHover, &QTimer::timeout, [this] () {
        int remove = qRound(0.33 * (double)m_lHovered.size());
        if(remove) {
            for(int i=0; i<remove; i++) {
                set->replace(m_lHovered.first(), m_lValues[m_lHovered.first()]);
                setHighlight->replace(m_lHovered.first(), 0);
                m_lHovered.pop_front();
            }
        }
        if(m_lHovered.size() == 1) {
            if(m_lHovered.first() != m_nLastHoveredIndex) {
                m_nLastHoveredIndex = m_lHovered.first();
                emit sig_dateHovered(m_lDates.at(m_nLastHoveredIndex));
                if(DEBUG) qDebug() << "Selected graph item:" << m_lDates.at(m_nLastHoveredIndex);
            }
        }
    });
    m_timerHover->start(33);
}

void ChartWrapper::setChartData(const QList<QDateTime> &datetimes, const QList<double> &values) {

    m_chart->removeAllSeries();
    for (auto axis : m_chart->axes())
        m_chart->removeAxis(axis);
    m_lValues.clear();
    m_lDates.clear();

    if (datetimes.size() != values.size() || datetimes.isEmpty())
        return;

    for(int i=datetimes.size()-1; i>=0; i--) {
        m_lDates.append(datetimes.at(i).date());
        m_lValues.append(values.at(i));
    }

    if(DEBUG) qDebug().noquote() << "Received new dataset with a size of" << m_lDates.size() << "and a time span of" << m_lDates.first() << "to" << m_lDates.last() << "->" << m_lDates.first().daysTo(m_lDates.last()) << "days)";

    QDate current, before;
    for(int i = m_lDates.size() - 1; i > 0; ) {
        current = m_lDates.at(i);
        before = m_lDates.at(i-1);

        if(current > before.addDays(1)) {
            // Gap detected: Insert missing day
            m_lDates.insert(i, current.addDays(-1));
            m_lValues.insert(i, 0.000001);
            // Do NOT decrement i; check the new inserted date against before
        } else if(current == before) {
            // Duplicate detected: Merge into i-1 and remove i
            m_lValues.replace(i-1, m_lValues.at(i-1) + m_lValues.at(i));
            m_lDates.removeAt(i);
            m_lValues.removeAt(i);
            // i naturally points to the next pair now
        } else {
            i--;
        }
    }

    if(DEBUG) qDebug().noquote() << "After removing duplicate days and filling gaps in time line ...";
    if(DEBUG) qDebug().noquote() << "Set new chart with a dataset size of" << m_lDates.size() << "and a time span of" << m_lDates.first() << "to" << m_lDates.last() << "->" << m_lDates.first().daysTo(m_lDates.last()) << "days)";
    if(DEBUG) qDebug() << "Dates:" << m_lDates;
    if(DEBUG) qDebug() << "Values:" << m_lValues;

    QStackedBarSeries *series = new QStackedBarSeries();
    series->setBarWidth(0.8);

    set = new QBarSet("Base");
    setHighlight = new QBarSet("Highlight");
    QStringList categories;

    for (int i = 0; i < m_lDates.size(); ++i) {
        *set << m_lValues[i];
        *setHighlight << 0; // Highlight is invisible (0 height) initially
        categories << m_lDates[i].toString("yyyy.MM.dd");
    }

    series->append(set);
    series->append(setHighlight);
    m_chart->addSeries(series);

    // 2. Updated Hover Logic: "The Swap"
    // We capture the values list to know the original numbers
    connect(series, &QStackedBarSeries::hovered, [this](bool status, int index) {
        if (index < 0 || index >= m_lValues.size())
            return;

        if(m_lValues.at(index) < 1)
            return;

        if (status) {
            if(m_lHovered.contains(index)) { return; }
            else {m_lHovered.append(index); }

            set->replace(index, 0);
            setHighlight->replace(index, m_lValues[index]);
        }
    });

    // 3. Axes Setup
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setVisible(false);
    m_chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    double maxVal = *std::max_element(m_lValues.begin(), m_lValues.end());
    if(DEBUG) qDebug().noquote() << "Found max. value of" << maxVal;
    axisY->setRange(0, maxVal * 1.0);
    axisY->setVisible(false);
    m_chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // 4. Styling (System Palette)
    QColor systemHighlight = QApplication::palette().color(QPalette::Highlight);

    // Normal state: System highlight
    set->setBrush(systemHighlight);
    set->setPen(Qt::NoPen);

    // Hover state: Slightly lighter/different version of system highlight
    // setHighlight->setBrush(systemHighlight.lighter(130));

    // Hover state: Complementary color
    float h, s, l, a; // Get HSL values
    systemHighlight.getHslF(&h, &s, &l, &a);
    h = fmod(h + 0.5, 1.0); // Rotate hue by 0.5 (180 degrees in 0.0-1.0 range)
    setHighlight->setBrush(QColor::fromHslF(h, s, l, a));
    setHighlight->setPen(Qt::NoPen);
}