#include "Mucha.h"

#include <QRandomGenerator>
#include <QDateTime>

#include "AppEngine.h"

Mucha::Mucha(const uint flightPlanningTimeSec, const uint maxLifeTimeSec, QObject *parent) :
    QObject(parent),
    m_maxFlightPlanningTimeSec(flightPlanningTimeSec),
    m_isDead(false),
    m_createDate(QDateTime::currentDateTime()),
    m_countMovement(0),
    m_random(reinterpret_cast<uint64_t>(this)),
    m_stopFlag(false),
    m_maxLifeTimeSec(maxLifeTimeSec)
{
    m_randomIcon = m_random.bounded(5);
}

Mucha::~Mucha()
{
    setIsDead(true);
}

void Mucha::setIsDead(const bool value)
{
    m_isDead = value;
    emit iconChanged();
    emit statusDeadChanged(m_isDead);
}

void Mucha::startFly()
{
    m_stopFlag = false;
    while (!m_stopFlag && !isDead()) {
        if (m_maxLifeTimeSec < ageSec()) {
            setIsDead(true);
            m_stopFlag = true;
            break;
        }
        QThread::sleep(m_random.bounded(m_maxFlightPlanningTimeSec + 1));
        QPoint toDiff;
        toDiff.setX(m_random.bounded(3) - 1);
        toDiff.setY(m_random.bounded(3) - 1);
        if (toDiff == QPoint(0, 0)) {
            toDiff.setX(1);
        }

        emit positionChanged(toDiff);
    }

    emit flyStoped();
}

QString Mucha::icon() const
{
    return isDead() ? QStringLiteral("mucha_dead.svg") :
                      ("mucha_live" + QString::number(m_randomIcon) + ".svg");
}

void Mucha::increaseMovement()
{
    m_countMovement++;
}

void Mucha::stopFly()
{
   m_stopFlag = true;
}

bool Mucha::isDead() const
{
    return m_isDead;
}

quint64 Mucha::ageSec() const
{
    return qMax(1, m_createDate.secsTo(QDateTime::currentDateTime()));
}

float Mucha::meanSpeedCellsInSec() const
{
    return m_countMovement / (ageSec() * 1.f);
}
