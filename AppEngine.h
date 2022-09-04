#ifndef APPENGINE_H
#define APPENGINE_H

#include <QObject>
#include <QList>
#include <QObjectList>
#include <QHash>
#include <QThread>
#include <QMutexLocker>
#include <QMutex>
#include <QMap>
#include <QAbstractTableModel>

#include "Mucha.h"

struct AppSettings {
    QSize plotSize;
    qsizetype maxMuchaInCell;
    quint64 flightPlanningTimeSec;
    QMap<QPoint, quint64> startPositionInPlot;
};

class AppEngine: public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QSize plotSize READ plotSize WRITE setPlotSize)
    Q_PROPERTY(bool isStoped READ isStoped NOTIFY simulationStoped)
    Q_PROPERTY(quint64 flightPlanningTimeSec READ flightPlanningTimeSec WRITE setFlightPlanningTimeSec)
    QML_ELEMENT
    QML_ADDED_IN_MINOR_VERSION(1)
public:
    explicit AppEngine(const AppSettings &settings = AppSettings(), QObject *parent = nullptr);
    virtual ~AppEngine();

    Q_INVOKABLE void startSimulation();
    Q_INVOKABLE void stopSimulation();
    QSize plotSize() const;
    void setPlotSize(QSize s);
    bool isStoped() const;

    quint64 flightPlanningTimeSec() const;
    void setFlightPlanningTimeSec(const quint64 s);

    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
public slots:
    void flyMucha(const QPoint &diff);
signals:
    void simulationStarted() const;
    void simulationStoped() const;
private:
    QList<QThread*> m_poolThreadMucha;
    QMutex m_mutexPlot;
    using Plot = QHash<QPoint, QObjectList>;
    Plot m_plot;
    AppSettings m_settings;
    QMutex m_mutexStart;
    QMutex m_mutexStop;
};


inline bool operator<(const QPoint &p1, const QPoint &p2)
{
    return p1.x() * 2 + p1.y() < p2.x() * 2 + p2.y();
}


#endif // APPENGINE_H
