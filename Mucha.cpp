#include "Mucha.h"

#include <QRandomGenerator>
#include <QDebug>
#include <QDateTime>

#include "AppEngine.h"

Mucha::Mucha(const MuchaSettings& settings, AppEngine *appEngine, QObject *parent) :
    QObject(parent),
    m_plotSize(settings.plotSize),
    m_currentPos(settings.startPos),
    m_flightPlanningTimeSec(settings.flightPlanningTimeSec),
    m_isDead(false),
    m_createDate(QDateTime::currentDateTime()),
    m_appEngine(appEngine),
    m_random(reinterpret_cast<uint64_t>(this))
{

}

Mucha::~Mucha()
{
    setIsDead(true);
}

void Mucha::setIsDead(const bool value)
{
    m_isDead = value;
    emit statusDeadChanged(m_isDead);
}

void Mucha::startFly()
{

    while (!isDead() && m_flightPlanningTimeSec * m_plotSize.width() > ageSec()) {
            qDebug() << m_flightPlanningTimeSec * m_plotSize.width() << ageSec();

        QThread::sleep(m_random.bounded(m_flightPlanningTimeSec));
        QPoint toPos = m_currentPos;
        auto p = m_random.bounded(8);
        switch (p) {
            case 0: toPos.setX(toPos.x() + 1);
                    break;
            case 1: toPos.setX(toPos.x() - 1);
                    break;
            case 2: toPos.setX(toPos.y() + 1);
                    break;
            case 3: toPos.setX(toPos.y() - 1);
                    break;
            case 4: toPos.setX(toPos.x() - 1);
                    toPos.setX(toPos.y() - 1);
                    break;
            case 5: toPos.setX(toPos.x() + 1);
                    toPos.setX(toPos.y() + 1);
                    break;
            case 6: toPos.setX(toPos.x() + 1);
                    toPos.setX(toPos.y() - 1);
                    break;
            case 7: toPos.setX(toPos.x() - 1);
                    toPos.setX(toPos.y() + 1);
                    break;
        }

        if (m_appEngine->flyToCell(m_currentPos, toPos)) {
            auto last = m_currentPos;
            m_currentPos = toPos;
            m_countMovement++;
            emit positionChanged(last, toPos);
        }
    }

    setIsDead(true);
}

void Mucha::stopFly()
{
    setIsDead(true);
}

bool Mucha::isDead() const
{
    return m_isDead;
}

QPoint Mucha::position() const
{
    return m_currentPos;
}

quint64 Mucha::ageSec() const
{
    return m_createDate.secsTo(QDateTime::currentDateTime());
}

float Mucha::meanSpeedCellsInSec() const
{
    return m_countMovement / ageSec();
}
