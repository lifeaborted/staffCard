#include "CustomModel.h"

CustomModel::CustomModel(QSqlQueryModel *model)
{
    if (model->rowCount() > 0)
    {
        // Заполнение QList строками
        QList<QVariant> emptyRow;
        for(int i=0; i<model->rowCount(); i++)
        {
            list.append(emptyRow);
        }

        // Заполнение QList по столбцам
        for(int i=0; i<list.size(); i++)
        {
            for(int j=0; j < model->columnCount(); j++)
            {
                list[i].append(model->data(model->index(i,j)));
            }
        }
    }
}

QModelIndex CustomModel::index(int row, int column, const QModelIndex &parent) const
{
    if(hasIndex(row, column, parent))
    {
        return createIndex(row, column);
    }
    return QModelIndex();
}

int CustomModel::rowCount(const QModelIndex &parent) const
{
    return list.size();
}

int CustomModel::columnCount(const QModelIndex &parent) const
{
    //return list.at(0).size();
    // Тут неверно
    return list[0].size();
}

QVariant CustomModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        QVariant headerValue = header.value(section).value(role);
        if (!headerValue.isValid()) {
            headerValue = header.value(section).value(Qt::EditRole);
        }
        if (!headerValue.isValid())
        {
            headerValue = QVariant(section).toString();
        }
        return headerValue;
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool CustomModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    int numColumns = columnCount();
    if (section < 0 || section >= numColumns)
    {
        return false;
    }

    if (orientation != Qt::Horizontal)
    {
        return false;
    }

    if (header.size() != numColumns)
    {
        header.resize(numColumns);
    }

    header[section][role] = value;
    emit(headerDataChanged(orientation, section, section));
    return true;
}

QVariant CustomModel::data(const QModelIndex &index, int role) const
{
    QVariant value = list[index.row()][index.column()];

    if (value.isValid() && role == Qt::DisplayRole)
    {
        return value;
    }

    if (value.isValid() && role == Qt::TextColorRole)
    {
        if (index.column() == 5)
        {
            return QVariant::fromValue(QColor(0,150,35));
        }
    }

    if (value.isValid() && role == Qt::TextAlignmentRole)
    {
        if(index.column() != 4 && index.column() != 5)
        {
            return Qt::AlignCenter;
        }
        else
        {
            return Qt::AlignVCenter;
        }
    }
    return QVariant();
}

bool CustomModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (value.type() != QVariant::String)
    {
        return false;
    }

    if (role != Qt::EditRole)
    {
        return false;
    }

    list[index.row()][index.column()] = value.toString();

    emit dataChanged(createIndex(index.row(), index.column()), createIndex(index.row(), index.column()));

    return true;
}
