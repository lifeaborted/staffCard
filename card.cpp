#include "card.h"
#include "ui_card.h"
#include <QtGui>
#include "widget.h"

#include <QMessageBox>

#include <QLabel>

#include <QSqlQuery>
#include <QDebug>
#include <QSqlRecord>
#include <QtSql>
#include <iostream>
card::card(QSqlDatabase sdb, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::card),
    sdb(sdb)
{
    ui->setupUi(this);
    nameTable();
    equipmentList();

}

card::~card()
{
    delete ui;
}

void card::equipmentList()
{
    QStringList results;
    QStringListModel *model = new QStringListModel(results);

    QSqlQuery query(sdb);
    query.exec("SELECT * FROM staff");
    qDebug()<<query.lastError();
    QSqlRecord rec = query.record();

    QStringList equipment;

    while (query.next()){

        equipment = query.value(rec.indexOf("equipment")).toStringList();
        QListView *resultListView = card::findChild<QListView *>("listView");
        results.append(equipment);
        model->setStringList(results);
        resultListView->setModel(model);
    }

}
void card::nameTable()
{
    QSqlQuery query(sdb);
    query.exec("SELECT name FROM staff where name =4");
    qDebug() << query.lastError();

    if (!query.isValid()){
        qDebug() << query.lastError().text();
    }
    else { qDebug() << "ok"; }
    QSqlRecord rec = query.record();

    QString name,post,equipment;

    while (query.next()){

        name = query.value(rec.indexOf("name")).toString();
        qDebug() << ". fullName is " << name;
    }
    ui->dataName->setText(name);
}

void card::on_pushButton_clicked()
{
    QMessageBox msg1;
    QSqlQuery query1(sdb);
    QString newEquipment = ui->lineEdit->text();
    if (!newEquipment.isEmpty())
    {
        query1.prepare("insert into staff (equipment) values (:equipment)");
        query1.bindValue(":equipment", newEquipment);

        if(!query1.exec())
        {
            msg1.setText("Something went wrong :/");
        }

        else
        {
            msg1.setText("New equipment has been added!");

            QStringList results;
            QStringListModel *model = new QStringListModel(results);

            QSqlQuery query(sdb);
            query.exec("SELECT * FROM staff");
            qDebug()<<query.lastError();
            QSqlRecord rec = query.record();

            QStringList equipment;

            while (query.next()){

                equipment = query.value(rec.indexOf("equipment")).toStringList();
                QListView *resultListView = card::findChild<QListView *>("listView");
                results.append(equipment);
                model->setStringList(results);
                resultListView->setModel(model);
            }
        }
    }

    else
    {
        msg1.setText("Input field can not be empty");
    }
    msg1.exec();
}

