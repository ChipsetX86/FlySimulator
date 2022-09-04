#include "AppEngine.h"

#include <QDebug>

#include "Mucha.h"

AppEngine::AppEngine(const AppSettings &settings, QObject *parent) :
    QAbstractTableModel(parent),
    m_settings(settings)
{

}

AppEngine::~AppEngine()
{
    stopSimulation();
}

void AppEngine::flyMucha(const QPoint &diff)
{
    if (diff.x() < -1 || diff.y() < -1 || diff.x() > 1 || diff.y() > 1) {
        return;
    }

    auto m = qobject_cast<Mucha *>(sender());
    if (!m) {
        return;
    }

    QMutexLocker lock(&m_mutexPlot);
    QPoint to;

    auto fromCell = m_plot.end();
    for (auto t = m_plot.begin(); t != m_plot.end(); t++) {
        if (t.value().contains(m)) {
            fromCell = t;
            to.setX(t.key().x() + diff.x());
            to.setY(t.key().y() + diff.y());
            break;
        }
    }

    if (m_settings.plotSize.width() <= to.x() ||
            m_settings.plotSize.height() <= to.y() ||
            to.x() < 0 ||
            to.y() < 0) {
        return;
    }

    if (m_settings.maxMuchaInCell <= fromCell.value().count()) {
        return;
    }


    fromCell.value().removeOne(m);
    m_plot[to].append(m);
    m->increaseMovement();
    emit dataChanged(index(fromCell.key().y(), fromCell.key().x()),
                     index(fromCell.key().y(), fromCell.key().x()), {Qt::DisplayRole});
    emit dataChanged(index(to.y(), to.x()),
                     index(to.y(), to.x()), {Qt::DisplayRole});
}

QSize AppEngine::plotSize() const
{
    return m_settings.plotSize;
}

void AppEngine::setPlotSize(QSize s)
{
    m_settings.plotSize = s;
}

void AppEngine::startSimulation()
{
    QMutexLocker m(&m_mutexStart);
    qDebug() << "Start simulation" << m_settings.plotSize;

    stopSimulation();

    for (auto& m: m_plot) {
        qDeleteAll(m);
        m.clear();
    }

    for (int i = 0; i < m_settings.plotSize.width(); ++i) {
        for (int j = 0; j < m_settings.plotSize.height(); ++j) {
            m_plot[QPoint(i, j)] = QObjectList{};
        }
    }

    for (auto const &pos: m_settings.startPositionInPlot.keys()) {
        auto const countMucha = m_settings.startPositionInPlot.value(pos);
        for (quint64 i = 0; i < countMucha; ++i) {
            auto thread = new QThread(this);
            auto mucha = new Mucha(m_settings.flightPlanningTimeSec,
                                   m_settings.flightPlanningTimeSec * m_settings.plotSize.width());

            m_plot[pos].push_back(mucha);
            m_poolThreadMucha.append(thread);
            connect(mucha, &Mucha::positionChanged, this, &AppEngine::flyMucha, Qt::QueuedConnection);

            connect(thread, &QThread::started, mucha, &Mucha::startFly);
            connect(this, &AppEngine::simulationStoped, mucha, &Mucha::stopFly, Qt::DirectConnection);
            connect(mucha, &Mucha::flyStoped, thread, &QThread::quit);

            connect(mucha, &Mucha::statusDeadChanged, [this](const bool isDead) {
                if (isDead) {
                    for (auto t = m_plot.begin(); t != m_plot.end(); t++) {
                        emit dataChanged(index(t.key().y(), t.key().x()),
                                         index(t.key().y(), t.key().x()));
                    }
                }
            });

            mucha->moveToThread(thread);

        }
    }

    emit dataChanged(index(0, 0), index(rowCount(), columnCount()), {Qt::DisplayRole});

    for (auto const t: m_poolThreadMucha) {
        t->start();
    }

    emit simulationStarted();
}

void AppEngine::stopSimulation()
{
    QMutexLocker m(&m_mutexStop);
    emit simulationStoped();

    if (m_poolThreadMucha.isEmpty()) {
        return;
    }

   for (auto const t: m_poolThreadMucha) {
       t->quit();
       t->wait();
    }

    m_poolThreadMucha.clear();
}

QVariant AppEngine::data(const QModelIndex &index, int role) const
{
    switch (role) {
        case Qt::DisplayRole: {
            return QVariant::fromValue(m_plot[QPoint(index.column(), index.row())]);
        }
        default:
            break;
    }

    return QVariant();
}

QHash<int, QByteArray> AppEngine::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractTableModel::roleNames();
    roles[Qt::DisplayRole] = "display";
    return roles;
}

int AppEngine::rowCount(const QModelIndex &) const
{
    return m_settings.plotSize.height();
}

int AppEngine::columnCount(const QModelIndex &) const
{
    return m_settings.plotSize.width();
}


quint64 AppEngine::flightPlanningTimeSec() const {
    return m_settings.flightPlanningTimeSec;
}

void AppEngine::setFlightPlanningTimeSec(const quint64 s)
{
    m_settings.flightPlanningTimeSec = s;
}

bool AppEngine::isStoped() const
{
    return m_poolThreadMucha.isEmpty();
}
