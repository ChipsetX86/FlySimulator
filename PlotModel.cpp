#include "PlotModel.h"

#include <QList>

#include "Mucha.h"

PlotModel::PlotModel(const QSize &size, QObject *parent):
    QAbstractTableModel(parent),
    m_plotSize(size)
{

}

QVariant PlotModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
        case Qt::DisplayRole: {
            QObjectList list;
            for (const auto m: listMucha) {
                auto pos = m->position();
                if (pos.x() == index.column() && pos.y() == index.row()) {
                     list.append(m);
                }
            }

            return QVariant::fromValue(list);
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
        emit dataChanged(index(m->position().y(), m->position().x()),
                         index(m->position().y(), m->position().x()));
    });
    connect(m, &Mucha::positionChanged, [this](QPoint from, QPoint to) {
        emit dataChanged(index(from.y(), from.x()),
                         index(from.y(), from.x()), {Qt::DisplayRole});
        emit dataChanged(index(to.y(), to.x()),
                         index(to.y(), to.x()), {Qt::DisplayRole});
    });

    auto pos = m->position();
    emit dataChanged(index(pos.y(), pos.x()),
                     index(pos.y(), pos.x()));
}

void PlotModel::takeAwayMucha(Mucha *m)
{
    listMucha.removeOne(m);
}

void PlotModel::takeAwayAllMucha()
{
    listMucha.clear();
}
