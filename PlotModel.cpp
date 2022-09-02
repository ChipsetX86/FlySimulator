#include "PlotModel.h"
#include "Mucha.h"

Q_DECLARE_METATYPE(CellInfo);

PlotModel::PlotModel(const QSize &size, QObject *parent):
    QAbstractTableModel(parent),
    m_plotSize(size)
{

}

QVariant PlotModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
        case Qt::DisplayRole: {
            CellInfo info;
            for (const auto m: listMucha) {

                if (m->position().x() == index.row() &&
                        m->position().y() == index.column()) {
                    auto isDead = m->isDead();
                    if (isDead) {
                        info.countDead++;
                    } else {
                        info.countLive++;
                    }


                }
            }

            return QVariant::fromValue(info);
        }
        default:
            break;
    }

    return QVariant();
}

QHash<int, QByteArray> PlotModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractTableModel::roleNames();
    roles[Qt::DisplayRole] = "display";
    return roles;
}

int PlotModel::rowCount(const QModelIndex &) const
{
    return m_plotSize.height();
}

int PlotModel::columnCount(const QModelIndex &) const
{
    return m_plotSize.width();
}

void PlotModel::addMucha(Mucha *m)
{
    listMucha.push_back(m);
    connect(m, &Mucha::statusDeadChanged, [this, m]() {
        emit dataChanged(index(m->position().x(), m->position().y()),
                         index(m->position().x(), m->position().y()));
    });
    connect(m, &Mucha::positionChanged, [this](QPoint from, QPoint to) {
        emit dataChanged(index(from.x(), from.y()),
                         index(from.x(), from.y()), {Qt::DisplayRole});
        emit dataChanged(index(to.x(), to.y()),
                         index(to.x(), to.y()), {Qt::DisplayRole});
    });

    auto pos = m->position();
    emit dataChanged(index(pos.x(), pos.y()),
                     index(pos.x(), pos.y()));
}

void PlotModel::takeAwayMucha(Mucha *m)
{
    listMucha.removeOne(m);
}

void PlotModel::takeAwayAllMucha()
{
    listMucha.clear();
}
