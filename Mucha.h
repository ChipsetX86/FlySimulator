#ifndef MUCHA_H
#define MUCHA_H

#include <QObject>
#include <QPoint>
#include <QSize>
#include <QRandomGenerator>
#include <QDateTime>
#include <QMutexLocker>
#include <QMutex>
#include <QtQml/qqml.h>

class AppEngine;

struct MuchaSettings {
    QSize plotSize;
    quint64 flightPlanningTimeSec;
    QPoint startPos;
};

class Mucha : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isDead READ isDead NOTIFY statusDeadChanged)
    Q_PROPERTY(QPoint position READ position NOTIFY positionChanged)
    Q_PROPERTY(quint64 ageSec READ ageSec)
    Q_PROPERTY(float meanSpeedCellsInSec READ meanSpeedCellsInSec)
    Q_PROPERTY(QString icon READ icon CONSTANT)
    QML_ELEMENT
public:
    explicit Mucha(const MuchaSettings& settings, AppEngine *appEngine, QObject *parent = nullptr);
    virtual ~Mucha();

    bool isDead() const;
    QPoint position();
    quint64 ageSec() const;
    float meanSpeedCellsInSec() const;
    QString icon() const;

public slots:
    void startFly();
    void stopFly();
signals:
    void statusDeadChanged(bool) const;
    void positionChanged(QPoint from, QPoint to) const;
    void iconChanged() const;

private:
     QSize m_plotSize;
     QPoint m_currentPos;
     quint32 m_flightPlanningTimeSec;
     bool m_isDead;
     QDateTime m_createDate;
     quint64 m_countMovement;
     AppEngine *m_appEngine;
     QRandomGenerator m_random;
     uint m_randomIcon;
     void setIsDead(const bool);
     void setPosition(const QPoint &point);
     QMutex m_mutexPos;
};

#endif // MUCHA_H
