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
    m_countMovement(0),
    m_appEngine(appEngine),
    m_random(reinterpret_cast<uint64_t>(this))
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
    emit statusDeadChanged(m_isDead);
    emit iconChanged();
}

void Mucha::startFly()
{

    while (!isDead() && m_flightPlanningTimeSec * m_plotSize.width() > ageSec()) {

        QThread::sleep(m_random.bounded(m_flightPlanningTimeSec));
        QPoint toPos = m_currentPos;

        toPos.setX(toPos.x() + m_random.bounded(3) - 1);
        toPos.setY(toPos.y() + m_random.bounded(3) - 1);

        if (m_appEngine->flyToCell(m_currentPos, toPos)) {
            auto last = m_currentPos;
            m_currentPos = toPos;
            m_countMovement++;
            emit positionChanged(last, toPos);
        }
    }

    setIsDead(true);
}

QString Mucha::icon() const
{
    return isDead() ? QStringLiteral("mucha_dead.svg") :
                      ("mucha_live" + QString::number(m_randomIcon) + ".svg");
}

void Mucha::stopFly()
{
    setIsDead(true);
}

bool Mucha::isDead() const
{
    return m_isDead;
}

void Mucha::setPosition(const QPoint &point)
{
    QMutexLocker lock(&m_mutexPos);
    m_currentPos = point;
}

QPoint Mucha::position()
{
    QMutexLocker lock(&m_mutexPos);
    return m_currentPos;
}

quint64 Mucha::ageSec() const
{
    return qMax(1, m_createDate.secsTo(QDateTime::currentDateTime()));
}

float Mucha::meanSpeedCellsInSec() const
{
    return m_countMovement / ageSec();
}
