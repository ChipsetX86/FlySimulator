#ifndef APPENGINE_H
#define APPENGINE_H

#include <QObject>
#include <QList>
#include <QThread>
#include <QMutexLocker>
#include <QMutex>
#include <QMap>

#include "PlotModel.h"
#include "Mucha.h"

struct AppSettings {
    QSize plotSize;
    quint64 maxMuchaInCell;
    quint64 flightPlanningTimeSec;
    QMap<QPoint, quint64> startPositionInPlot;
};

class AppEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSize plotSize READ plotSize CONSTANT)
    Q_PROPERTY(PlotModel const* plotTableModel READ plotModel CONSTANT)
    QML_ELEMENT
    QML_ADDED_IN_MINOR_VERSION(1)
    QML_SINGLETON
public:
    explicit AppEngine(const AppSettings &settings, QObject *parent = nullptr);
    virtual ~AppEngine();

    bool flyToCell(const QPoint &from, const QPoint &to);
    Q_INVOKABLE void startSimulation();
    Q_INVOKABLE void stopSimulation();
    QSize plotSize() const;
    PlotModel const* plotModel() const;

signals:
    void simulationStarted() const;
    void simulationStoped() const;
private:
    QList<QThread*> m_poolThreadMucha;
    QMutex m_mutexPlot;
    using Plot = QMap<QPoint, quint64>;
    Plot m_plot;
    AppSettings m_settings;
    PlotModel m_plotModel;
    QMutex m_mutexStart;
    QMutex m_mutexStop;
};


inline bool operator<(const QPoint &p1, const QPoint &p2)
{
    return p1.x() + p1.y() < p2.x() + p2.y();
}


#endif // APPENGINE_H
