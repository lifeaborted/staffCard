#include "widget.h"
#include "./ui_widget.h"
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QDebug>
#include "card.h"
#include "ui_card.h"
#include <QSqlQuery>
#include<QMessageBox>
#include <QSqlQueryModel>
#include<QVector>
#include<QByteArray>
#include <QSqlRecord>
#include <QTableView>
#include<QComboBox>
#include<QListView>
#include <QAbstractItemModel>
//import StringListModel
#include <QCompleter>
#include"CustomModel.h"
#include<QTimer>
#include<QPixmap>
#include<QTabWidget>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    QMessageBox error;

    sdb = QSqlDatabase::addDatabase("QSQLITE", "connection1");
    edb = QSqlDatabase::addDatabase("QSQLITE", "connection2");
    sdb.setDatabaseName("C://Users//user//Documents//test2//staff.sqlite");
    edb.setDatabaseName("C://Users//user//Documents//test2//equipment.sqlite");

    if(!sdb.open()){
        error.setText("sdb.lastError()");
        error.exec();
        qDebug()<<sdb.lastError();
    }
    if(!edb.open())
    {
        error.setText("edb.lastError()");
        error.exec();
        qDebug()<<edb.lastError();
    }

    QString Name, Surname, Patronymic, Post, Person;
    QPixmap profilePic = QPixmap (":/tri.jpg");
    ui->label->setPixmap(profilePic);

    itemsAreVisible(false);

    ui->tabWidget->setVisible(false);
    ui->tabWidget->setDisabled(true);
    ui->deleteButton->setDisabled(true);
    ui->addButton->setDisabled(false);
    ui->saveAddButton->setVisible(false);
    ui->saveEditButton->setVisible(false);
    ui->saveAddButton->setDisabled(true);
    ui->saveEditButton->setDisabled(true);
    ui->tableView->setVisible(false);

    ui->personData->setReadOnly(true);
    ui->dataBox->setHidden(true);;
    timer = new QTimer(this);
    timer->setSingleShot(true);
    ui->dataText->activateWindow();
    ui->dataBox->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->dataText, SIGNAL(textEdited(QString)), SLOT(onSearchTextChanged()));
    connect (timer, SIGNAL(timeout()), SLOT(performSearch()));
    connect(ui->dataBox, SIGNAL(doubleClicked(QModelIndex)), SLOT(onComboboxItemActivated(QModelIndex)));

    generalRequest = "select name, surname, patronymic from staff";
    eqRequest = "select * from current";
    ui->dataText->setFocus();
    QTimer::singleShot(0, ui->dataText, SLOT(setFocus()));
    sqlDataReserve(generalRequest);
    fillComboBox();
    performSearch();
}

QString Widget::sqlDataReserve(QString req)
{
    stringReserve = "";
    qry = new QSqlQuery(sdb);
    qry->exec(generalRequest);

    while (qry->next()){
        Name = qry->value(0).toString();
        Surname = qry->value(1).toString();
        Patronymic = qry->value(2).toString();
        Post = qry->value(3).toString();
        reserveResult.append(Name+' '+Surname+' '+Patronymic);
        stringReserve += Name+' '+Surname+' '+Patronymic+' ';
    }

    _filteredModel = new StringListModel(reserveResult);
    return stringReserve;
}

bool Widget::compareStrings(QString inputString, QString fullName)
{
    bool b = false;
    int i=0;

    QStringList strIn = inputString.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QStringList strOut = fullName.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    for(i = 0; i < strIn.size(); ++i)
    {
        for(int j = 0; j < strOut.size(); ++j)
        {
            if (strOut[j].contains(strIn[i]))
            {
                b = true;
                break;
            }
        }
    }
    return b;
}

void Widget::itemsAreVisible(bool mode)
{
    ui->deleteButton->setVisible(mode);
    ui->label->setVisible(mode);
    ui->editButton->setVisible(mode);
    ui->personData->setVisible(mode);
    ui->postData->setVisible(mode);
    ui->nspLable->setVisible(mode);
    ui->postLabel->setVisible(mode);
    ui->equipmentList->setVisible(mode);
    ui->tableView->setVisible(mode);
}

QStringList Widget::sqlData(QString req)
{
    QStringList result;
    qry = new QSqlQuery(sdb);
    qry->exec(req);
    while (qry->next()){
        Name = qry->value(0).toString();
        Surname = qry->value(1).toString();
        Patronymic = qry->value(2).toString();
        Post = qry->value(3).toString();
        result.append(Name+' '+Surname+' '+Patronymic);
        QString stringResult = Name+' '+Surname+' '+Patronymic;
    }
    if (result.size() == 0)
    {
        itemsAreVisible(false);
    }

    return result;
}

void Widget::onSearchTextChanged()
{
   timer->start(1000);
}

void Widget::fillComboBox(){
    QStringList result1 = sqlData(generalRequest);
    _model1 = new StringListModel(result1);
    ui->dataBox->setModel(_model1);
}

void Widget::performSearch()
{
    QStringList filter;
    QString toCompare;
    QString search = ui->dataText->text();
    if(!search.isEmpty())
    {
        for(int i = 0; i < _filteredModel->rowCount(); ++i)
        {
            toCompare = _filteredModel->itemData(i);
            if(compareStrings(search, toCompare))
            {
                filter.append(toCompare);
            }
            else
                itemsAreVisible(false);
        }
        _model1->updateItem(filter);

        ui->dataText->setText(search);
        ui->dataText->setFocus();
        qDebug()<<ui->dataText->isActiveWindow();
        ui->dataBox->setFocusProxy(ui->dataText);
        qDebug()<<ui->dataBox->isActiveWindow();
        ui->dataBox->setHidden(false);
    }
    else
        fillComboBox();
}

int32_t Widget::idFinder(QString request)
{
    idQuery = new QSqlQuery(sdb);
    idQuery->prepare(request);
    idQuery->exec();
    int32_t ID;
    while(idQuery->next())
    {
        ID = idQuery->value(4).toInt();
    }
    return ID;
}

Widget::~Widget()
{
    delete ui;
}

void Widget::editMode(bool mode)
{
    ui->deleteButton->setVisible(mode);
    ui->deleteButton->setDisabled(!mode);
    ui->postData->setReadOnly(!mode);
    ui->personData->setReadOnly(!mode);
    ui->saveAddButton->setDisabled(mode);
    ui->addButton->setDisabled(mode);
    ui->addButton->setDisabled(!mode);
    //itemsAreVisible(mode);
    ui->saveEditButton->setDisabled(!mode);
    ui->saveEditButton->setVisible(mode);
    ui->addButton->setDisabled(mode);
}

void Widget::onComboboxItemActivated(const QModelIndex &index){
    editMode(false);
    addMode(false);
    QString item = _model1->data(index, Qt::DisplayRole).toString();
    ui->dataText->setText(item);

    eqRequest = QString("select title, description, serialNumber, comment, takeDate, returnDate from history where NSP = '%1'")
                    .arg(item);
    qDebug()<<eqRequest;

    equipmentListData(eqRequest);
    QStringList separator = item.split(" ");
    Name = separator[0];
    Surname = separator[1];
    Patronymic = separator [2];
    request = QString("select * from staff where name = '%1' and surname = '%2' and patronymic = '%3'")
                  .arg(Name).arg(Surname).arg(Patronymic);
    sqlData(request);

    ui->personData->setText(Name + ' ' + Surname + ' ' + Patronymic);
    ui->postData->setText(Post);
    itemsAreVisible(true);
    ui->personData->setReadOnly(true);
    ui->postData->setReadOnly(true);
    ui->dataBox->setHidden(true);
}

void Widget::on_saveEditButton_clicked()
{
    Person = ui->personData->text();
    Post = ui->postData->text();
    QStringList separator = Person.split(" ");
    if(separator.size() == 3 && !Post.isEmpty()){
    Name = separator[0];
    Surname = separator[1];
    Patronymic = separator [2];
    int32_t id = idFinder(request);
    request = QString("UPDATE staff set name = '%1', surname = '%2', patronymic = '%3', post = '%4' where uniqueID = '%5'")
               .arg(Name).arg(Surname).arg(Patronymic).arg(Post).arg(id);

    sqlData(request);
    fillComboBox();
    editMode(false);
    itemsAreVisible(true);
    }
    else{
    QMessageBox msg;
    msg.setText("Введите корректные данные!");
    msg.exec();
    }
}

void Widget::on_editButton_clicked()
{
    editMode(true);
}

void Widget::on_saveAddButton_clicked()
{
    ui->addButton->setDisabled(false);
    Person = ui->personData->text();
    Post = ui->postData->text();
    QStringList separator = Person.split(" ");
    if(separator.size() == 3 && !Post.isEmpty())
    {
        Name = separator[0];
        Surname = separator[1];
        Patronymic = separator [2];
        request = QString("insert into staff (name, surname, patronymic, post) values ('%1', '%2', '%3', '%4')")
                      .arg(Name).arg(Surname).arg(Patronymic).arg(Post);

        sqlData(request);
        fillComboBox();
        qDebug()<<result;
        addMode(false);

        ui->addButton->setDisabled(false);
        ui->postData->clear();
        ui->personData->clear();
    }
    else{
        QMessageBox msg;
        msg.setText("Введите корректные данные!");
        msg.exec();
    }
}

void Widget::on_addButton_clicked()
{
    addMode(true);
}
void Widget::addMode(bool mode)
{
    itemsAreVisible(mode);

    ui->deleteButton->setDisabled(!mode);
    ui->tableView->setVisible(!mode);
    ui->postData->setReadOnly(!mode);
    ui->personData->setReadOnly(!mode);
    ui->saveAddButton->setDisabled(!mode);
    ui->saveAddButton->setVisible(mode);
    ui->editButton->setDisabled(mode);
    ui->editButton->setVisible(!mode);
    ui->saveEditButton->setDisabled(mode);
    ui->addButton->setDisabled(!mode);
}

void Widget::equipmentListData(QString eqRequest)
{
    QSqlQueryModel *equipdModel = new QSqlQueryModel;
    qry = new QSqlQuery(edb);
    qry->prepare(eqRequest);
    if(qry->exec()){
    equipdModel->setQuery(*qry);
    ui->tableView->setModel(equipdModel);
    }
    else
    qDebug()<<"ШМОТОК НЕТ)))"<<endl<<qry->lastError();

}

void Widget::on_deleteButton_clicked()
{
    QStringList separator = Person.split(" ");
    qDebug()<<"pered id"<<request;
    int32_t id = idFinder(request);
    qDebug()<<"IDISHNIK"<<id;
    request = QString("delete from staff where UniqueID = '%1'")
                  .arg(id);
    qDebug()<<"POSLE"<<request;

    sqlData(request);
    fillComboBox();
    qDebug()<<result;
    editMode(false);

    ui->deleteButton->setDisabled(true);
    ui->postData->clear();
    ui->personData->clear();
}


void Widget::on_cancelButton_clicked()
{
   addMode(false);
//    editMode(false);
}

