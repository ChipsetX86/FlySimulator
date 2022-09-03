#ifndef PLOTMODEL_H
#define PLOTMODEL_H

#include <QtQml/qqml.h>
#include <QAbstractTableModel>
#include <QObject>
#include <QSize>

class Mucha;

class PlotModel: public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_ADDED_IN_MINOR_VERSION(1)
public:
    explicit PlotModel(const QSize& size, QObject *parent = nullptr);
    virtual ~PlotModel() {}

    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addMucha(Mucha *);
    void takeAwayMucha(Mucha *);
    void takeAwayAllMucha();
private:
    QSize m_plotSize;
    using ListMucha = QList<Mucha *>;
    ListMucha listMucha;
};
#endif // PLOTMODEL_H
