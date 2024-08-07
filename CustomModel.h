#ifndef CUSTOMMODEL_H
#define CUSTOMMODEL_H

#include <QAbstractTableModel>
#include <QSqlQueryModel>
#include <QColor>

class CustomModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    CustomModel(QSqlQueryModel *model);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);

private:

    QSqlQueryModel *model;
    QList<QList<QVariant> > list; // Двумерный список для хранения данных из QSqlQueryModel
    QVector<QHash<int, QVariant> > header; // "Шапка" таблицы

signals:

    void headerDataChanged(Qt::Orientation orientation, int first, int last);

};

#endif // CUSTOMMODEL_H
