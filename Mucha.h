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

class Mucha : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isDead READ isDead NOTIFY statusDeadChanged)
    Q_PROPERTY(quint64 ageSec READ ageSec CONSTANT)
    Q_PROPERTY(float meanSpeedCellsInSec READ meanSpeedCellsInSec CONSTANT)
    Q_PROPERTY(QString icon READ icon CONSTANT)
    QML_ELEMENT
public:
    explicit Mucha(const uint flightPlanningTimeSec,
                   const uint maxLifeTimeSec,
                   QObject *parent = nullptr);
    virtual ~Mucha();

    bool isDead() const;
    quint64 ageSec() const;
    float meanSpeedCellsInSec() const;
    QString icon() const;
    void increaseMovement();

public slots:
    Q_INVOKABLE void startFly();
    Q_INVOKABLE void stopFly();
signals:
    void statusDeadChanged(bool) const;
    void positionChanged(const QPoint diff) const;
    void iconChanged() const;
    void flyStoped() const;
private:
     quint32 m_maxFlightPlanningTimeSec;
     bool m_isDead;
     QDateTime m_createDate;
     quint64 m_countMovement;
     QRandomGenerator m_random;
     uint m_randomIcon;
     void setIsDead(const bool);
     void setPosition(const QPoint &point);
     QMutex m_mutexPos;
     bool m_stopFlag;
     uint m_maxLifeTimeSec;
};

#endif // MUCHA_H
