#ifndef CHARTWRAPPER_H
#define CHARTWRAPPER_H

#include <QWidget>
#include <QDateTime>
#include <QList>
#include <QTimer>
#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

class ChartWrapper : public QObject {
    Q_OBJECT

public:

    explicit ChartWrapper(QWidget *parentWidget);
    void setChartData(const QList<QDateTime> &datetimes, const QList<double> &values);

private:

    QWidget *m_parentWidget;
    QChartView *m_chartView;
    QChart *m_chart;
    QList<int> m_lHovered;

    QBarSet *set, *setHighlight;
    QList<double> m_lValues;
    QList<QDate> m_lDates;
    QTimer *m_timerHover;
    int m_nLastHoveredIndex {-1};

signals:
    void sig_dateHovered(QDate date);
};

#endif // CHARTWRAPPER_H