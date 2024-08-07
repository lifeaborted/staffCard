#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QListView>
#include <QStringListModel>
#include <QSqlQueryModel>
#include "card.h"
#include<QAbstractItemModel>
#include <QTableView>
#include<QStandardItemModel>
#include<QTimer>
#include<QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class StringListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    StringListModel(const QStringList &strings, QObject *parent = 0)
        : QAbstractListModel(parent), stringList(strings) {}

    int StringListModel::rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        if(parent.isValid())
            return 0;
        return stringList.count();
    }


    QVariant StringListModel::data(const QModelIndex &index, int role) const override
    {
        if (!index.isValid())
            return QVariant();

        if (index.row() >= stringList.size())
            return QVariant();

        if (role == Qt::DisplayRole)
            return stringList.at(index.row());
        else
            return QVariant();
    }

    Qt::ItemFlags StringListModel::flags(const QModelIndex &index) const
    {
        if (!index.isValid())
            return Qt::ItemIsEnabled;

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    bool StringListModel::setData(const QModelIndex& index,
                                  const QVariant &value, int role)
    {
        if (index.isValid() && role == Qt::EditRole) {

            stringList.replace(index.row(), value.toString());
            emit dataChanged(index, index);
            return true;
        }
        return false;
    }

    bool StringListModel::insertRows(int position, int rows, const QModelIndex &parent)
    {
        beginInsertRows(QModelIndex(), position, position+rows-1);

        for (int row = 0; row < rows; ++row) {
            stringList.insert(position, "");
        }

        endInsertRows();
        return true;
    }

    bool StringListModel::removeRows(int position, int rows, const QModelIndex &parent)
    {
        if(parent.isValid())
        {
            return false;
        }

        beginRemoveRows(QModelIndex(), position, position+rows - 1);
        for (int row = 0; row < rows; ++row){
            stringList.removeAt(position);
        }
        endRemoveRows();

        return true;
        }

    void StringListModel::resetInternalData()
    {
        beginResetModel();
        stringList.clear();
        endResetModel();
    }

    void StringListModel::updateItem(const QStringList& items){
        beginResetModel();
        stringList.clear();
        stringList = items;
        endResetModel();
    }

    QString StringListModel::itemData(int index) const{
        if(index >= 0 && index <stringList.size()){
            return stringList.at(index);
        }
        else
            return QString();
    }


private:
    QStringList stringList;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    //extern Widget id;
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    //void lineEdit();
    //void pushButton();
   // void listView();
    //void on_pushButton_clicked();
    //void handleItemClick(const QModelIndex &index);
//    void on_pushButton_clicked();
    void on_addButton_clicked();
//    void on_deleteButton_clicked();
    void onSearchTextChanged();
    void performSearch();
    //void on_selectButton_clicked();
    void fillComboBox();
   // void addNewPerson()
    void on_editButton_clicked();
    void itemsAreVisible(bool);
    void editMode(bool);
    void addMode(bool);
    //void on_saveButton_clicked();
    void onComboboxItemActivated(int index);
    bool compareStrings(QString, QString);
    void equipmentListData(QString);
    void on_saveEditButton_clicked();

    void on_saveAddButton_clicked();

private:
    QSqlQueryModel equipdModel;
    StringListModel *_model1;
    StringListModel *_filteredModel;
    Ui::Widget *ui;
    QSqlDatabase sdb;
    QSqlDatabase edb;
    card *secondWindow;
   // QLineEdit *lineEdit;
    QSqlQuery *qry;
    QSqlQuery *idQuery;
    QTimer *timer;
    QStringList result, eqList;
    QStringList reserveResult;
    QString stringReserve;
    QStringList sqlData(QString);
    QString sqlDataReserve(QString);
    QString request, eqRequest;
    QString searchString;
    int32_t idFinder(QString);
    QString Name, Surname, Patronymic, Post, Person;
    QMessageBox error;

    //QSqlQuery *qu;
    //QSqlQueryModel *model;
   // QSqlQueryModel *model1;
   // QListView *resultListView;
    //QListView *listView;
    //QStringListModel *model;

};
#endif // WIDGET_H
