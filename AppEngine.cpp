#include "AppEngine.h"

#include "Mucha.h"

AppEngine::AppEngine(const AppSettings &settings, QObject *parent) :
    QAbstractTableModel(parent),
    m_settings(settings)
{
    for (int i = 0; i < m_settings.plotSize.width(); ++i) {
        for (int j = 0; j < m_settings.plotSize.height(); ++j) {
            m_plot[QPoint(i, j)] = QObjectList{};
        }
    }
}

AppEngine::~AppEngine()
{
    stopSimulation();
}

void AppEngine::flyMucha(const QPoint &diff)
{
    qDebug() << diff;
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

void AppEngine::startSimulation()
{
    QMutexLocker m(&m_mutexStart);
    qDebug() << "Start simulation";

    stopSimulation();

    for (auto const &pos: m_settings.startPositionInPlot.keys()) {
        auto const countMucha = m_settings.startPositionInPlot.value(pos);
        for (quint64 i = 0; i < countMucha; ++i) {
            auto thread = new QThread(this);
            auto mucha = new Mucha(MuchaSettings{m_settings.flightPlanningTimeSec});

            m_plot[pos].push_back(mucha);
            m_poolThreadMucha.append(thread);
            connect(mucha, &Mucha::positionChanged, this, &AppEngine::flyMucha, Qt::QueuedConnection);
            //connect(thread, &QThread::finished, mucha, &QObject::deleteLater);
            connect(thread, &QThread::started, mucha, &Mucha::startFly);
            connect(this, &AppEngine::simulationStoped, mucha, &Mucha::stopFly, Qt::DirectConnection);
            connect(mucha, &Mucha::statusDeadChanged, [thread](bool isDead) {
                if (isDead) {
                    thread->quit();
                }
            });

            connect(mucha, &Mucha::statusDeadChanged, [this, mucha]() {
                emit dataChanged(index(mucha->position().y(), mucha->position().x()),
                                 index(mucha->position().y(), mucha->position().x()));
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
    if (m_poolThreadMucha.isEmpty()) {
        return;
    }

    QMutexLocker m(&m_mutexStop);

    emit simulationStoped();

    for (auto const t: m_poolThreadMucha) {
        t->wait();
        t->deleteLater();
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
