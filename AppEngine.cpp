#include "AppEngine.h"

#include "Mucha.h"

AppEngine::AppEngine(const AppSettings &settings, QObject *parent) :
    QObject(parent),
    m_settings(settings),
    m_plotModel(settings.plotSize, this)
{
    for (int i = 0; i < m_settings.plotSize.width(); ++i) {
        for (int j = 0; j < m_settings.plotSize.height(); ++j) {
            m_plot[QPoint(i, j)] = 0;
        }
    }
}

AppEngine::~AppEngine()
{
    stopSimulation();
}

bool AppEngine::flyToCell(const QPoint &from, const QPoint &to)
{
    if (m_settings.plotSize.width() <= to.x() ||
            m_settings.plotSize.height() <= to.y() ||
            to.x() < 0 ||
            to.y() < 0) {
        return false;
    }

    QMutexLocker lock(&m_mutexPlot);
    auto cell = m_plot.find(to);
    if (cell == m_plot.cend()) {
        return false;
    }

    if (m_settings.maxMuchaInCell <= cell.value()) {
        return false;
    }

    cell.value()++;
    m_plot[from]--;
    return true;
}

QSize AppEngine::plotSize() const
{
    return m_settings.plotSize;
}

PlotModel const* AppEngine::plotModel() const
{
    return &m_plotModel;
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
            auto mucha = new Mucha(MuchaSettings{
                                       m_settings.plotSize,
                                       m_settings.flightPlanningTimeSec,
                                       pos
                                   }, this);
            mucha->moveToThread(thread);
            m_plot[pos]++;
            m_poolThreadMucha.append(thread);
            //connect(thread, &QThread::finished, mucha, &QObject::deleteLater);
            connect(thread, &QThread::started, mucha, &Mucha::startFly);
            connect(this, &AppEngine::simulationStoped, mucha, &Mucha::stopFly, Qt::DirectConnection);
            connect(mucha, &Mucha::statusDeadChanged, [thread](bool isDead) {
                if (isDead) {
                    thread->quit();
                }
            });
            m_plotModel.addMucha(mucha);
        }
    }

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
    m_plotModel.takeAwayAllMucha();

}
