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
    ui->saveAddButton->setVisible(false);
    ui->saveEditButton->setVisible(false);
    ui->saveAddButton->setDisabled(true);
    ui->saveEditButton->setDisabled(true);
    ui->tableView->setVisible(false);

    ui->personData->setReadOnly(true);
    ui->comboBox->setEditable(true);

    timer = new QTimer(this);
    timer->setSingleShot(true);

    connect(ui->comboBox->lineEdit(), SIGNAL(textEdited(QString)), SLOT(onSearchTextChanged()));
    connect (timer, SIGNAL(timeout()), SLOT(performSearch()));
    connect(ui->comboBox, SIGNAL(activated(int)), SLOT(onComboboxItemActivated(int)));

    request = "select name, surname, patronymic from staff";
    eqRequest = "select * from current";
    sqlDataReserve(request);
    fillComboBox();
    performSearch();
//    ui->comboBox->lineEdit()->setFocusPolicy(Qt::StrongFocus);
//    ui->comboBox->lineEdit()->setFocus();
}

QString Widget::sqlDataReserve(QString request)
{
    stringReserve = "";
    qry = new QSqlQuery(sdb);
    qry->exec(request);

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
    ui->label->setVisible(mode);
    ui->editButton->setVisible(mode);
    ui->personData->setVisible(mode);
    ui->postData->setVisible(mode);
    ui->nspLable->setVisible(mode);
    ui->postLabel->setVisible(mode);
    ui->equipmentList->setVisible(mode);
    ui->tableView->setVisible(mode);
}



QStringList Widget::sqlData(QString request)
{
    qry = new QSqlQuery(sdb);
    qry->exec(request);

    while (qry->next()){
        Name = qry->value(0).toString();
        Surname = qry->value(1).toString();
        Patronymic = qry->value(2).toString();
        Post = qry->value(3).toString();
        result.append(Name+' '+Surname+' '+Patronymic);
        QString stringResult = Name+' '+Surname+' '+Patronymic;
    }
    qDebug()<<"razmer"<<result.size();
    if (result.size() == 0)
    {
        itemsAreVisible(false);
    }
    return result;
}

//void Widget::addNewPerson(){
//    QMessageBox messageBox(this);
//    QAbstractButton *accept = messageBox.addButton(tr("Yes"), QMessageBox::ActionRole);
//    QAbstractButton *cancel = messageBox.addButton(tr("No"), QMessageBox::ActionRole);
//    QStringList separator = searchString.split(" ");

//    QString Name = separator[0];
//    QString Surname = separator[1];
//    QString Patronymic = separator [2];
//    messageBox.exec();
//    if(messageBox.clickedButton()==accept){
//        request = QString("insert into staff (name, surname, patronymic) values ('%1, %2, %3").arg(Name).arg(Surname).arg(Patronymic);
//        sqlData(request);
//    }
//}

void Widget::onSearchTextChanged()
{
   timer->start(1000);
}

void Widget::fillComboBox(){
    QStringList result = sqlData(request);

    _model1 = new StringListModel(result);

//    QCompleter* mycompletear = new QCompleter(this);
//    mycompletear->setCaseSensitivity(Qt::CaseInsensitive);
//    mycompletear->setModel(_model1);
//    mycompletear->setCompletionColumn(1);
//    mycompletear->setCompletionMode(QCompleter::UnfilteredPopupCompletion);

//    ui->comboBox->setCompleter(mycompletear);
    ui->comboBox->setEditable(true);
    ui->comboBox->setModel(_model1);
}

void Widget::performSearch()
{
    QStringList filter;
    QString toCompare;
    QString search = ui->comboBox->currentText();
    if(!search.isEmpty())
    {
        for(int i = 0; i < _filteredModel->rowCount(); ++i)
        {
            toCompare = _filteredModel->itemData(i);
            qDebug()<<"d vodeli nomer "<<i<<toCompare;
            if(compareStrings(search, toCompare))
            {
                filter.append(toCompare);
            }
            else
                itemsAreVisible(false);
        }
        _model1->updateItem(filter);

        ui->comboBox->showPopup();
        ui->comboBox->lineEdit()->setText(search);
        ui->comboBox->setFocusProxy(ui->comboBox->lineEdit());
    }
}




//void Widget::handleItemClick(const QModelIndex &index)
//{
//        QStringList results;
//        QStringListModel *model = new QStringListModel(results);
//        QString detail = model->data(index, Qt::DisplayRole).toString();
//        secondWindow->show();
//}

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
    qDebug()<<"nomerok"<<ID;
    return ID;
}



Widget::~Widget()
{
    delete ui;
}

//void Widget::on_pushButton_clicked()
//{
//    QString test = ui->comboBox->currentText();
//    qDebug()<<"proverka"<<test;

//}

//void Widget::on_deleteButton_clicked()
//{
//    QStringList separator;
//    QMessageBox msg2;
//    QSqlQuery query2(sdb);
//    QString deletePerson="wrqwe qwer";
//    if (!deletePerson.isEmpty())
//    {
//        separator = deletePerson.split(" ");
//        QString Name = separator[0];
//        QString Surname = separator[1];
//        QString Patronymic = separator [2];

//        query2.prepare("delete from staff where name = :name and surname = :surname and patronymic = :patronymic");
//        query2.bindValue(":name", Name);
//        query2.bindValue(":surname", Surname);
//        query2.bindValue(":patronymic", Patronymic);
//        qDebug()<<"del"<<Name<<Surname<<Patronymic;
//        query2.exec();
//        if(!query2.numRowsAffected())
//        {
//            qDebug()<<query2.numRowsAffected();
//            msg2.setText("There is no such person");
//            }
//        else {
//            msg2.setText("This person has been deleted!");
//            //model = new QSqlQueryModel();
//            qry = new QSqlQuery(sdb);
//            qry->prepare("SELECT * FROM staff");
//            qry->exec();

//           // model->setQuery(*qry);
//           // ui->tableView->setModel(model);
//            }
//    }

//    else
//    {
//        msg2.setText("Input field can not be empty");
//    }
//    msg2.exec();
//}


//void Widget::on_selectButton_clicked()
//{

//    QMessageBox msg3;
//    QSqlQuery query3(sdb);
//    QString selectPerson;
//    if (!selectPerson.isEmpty())
//    {
//        QStringList separator;
//        separator = selectPerson.split(" ");
//        QString Name = separator[0];
//        QString Surname = separator[1];
//        QString Patronymic = separator [2];
//        query3.prepare("select * from staff where name = :name and surname = :surname and patronymic = :patronymic");
//        query3.bindValue(":name", Name);
//        query3.bindValue(":surname", Surname);
//        query3.bindValue(":patronymic", Patronymic);
//        qDebug()<<"select"<<Name<<Surname<<Patronymic;
//        qDebug()<<query3.lastError();

//        if(!query3.exec() || !query3.next() )
//        {
//            qDebug()<<query3.lastError();
//            msg3.setText("There is no such person");
//        }
//        else
//        {
//            //model = new QSqlQueryModel();
//            qry = new QSqlQuery(sdb);
//            qry->prepare("select * from staff where name = :name and surname = :surname and patronymic = :patronymic");
//            qry->bindValue(":name", Name);
//            qry->bindValue(":surname", Surname);
//            qry->bindValue(":patronymic", Patronymic);
//            qry->exec();

//           // model->setQuery(*qry);
//           // ui->tableView->setModel(model);
//            //secondWindow->show();
//        }
//    }
//    else
//    {
//        msg3.setText("Input field can not be empty");
//    }
//    msg3.exec();
//}

void Widget::editMode(bool mode)
{
    ui->postData->setReadOnly(!mode);
    ui->personData->setReadOnly(!mode);
    ui->saveAddButton->setDisabled(mode);
    ui->addButton->setDisabled(mode);
    itemsAreVisible(mode);
    ui->saveEditButton->setDisabled(!mode);
    ui->saveEditButton->setVisible(mode);
    ui->addButton->setDisabled(mode);
}

void Widget::onComboboxItemActivated(int index){
    equipmentListData(eqRequest);
    editMode(false);
    addMode(false);
    QString item = _model1->itemData(index);
    qDebug()<<"vibral"<<item;
    ui->comboBox->setCurrentText(item);
    QStringList separator = item.split(" ");
    Name = separator[0];
    Surname = separator[1];
    Patronymic = separator [2];
    //Post = separator[3];
    request = QString("select * from staff where name = '%1' and surname = '%2' and patronymic = '%3'")
                  .arg(Name).arg(Surname).arg(Patronymic);
    sqlData(request);
    ui->personData->setText(Name + ' ' + Surname + ' ' + Patronymic);
    ui->postData->setText(Post);
    itemsAreVisible(true);
    ui->personData->setReadOnly(true);
    ui->postData->setReadOnly(true);
    qDebug()<<"nazhal"<<item<<index<<request;
}


void Widget::on_saveEditButton_clicked()
{
    Person = ui->personData->text();
    Post = ui->postData->text();
    QStringList separator = Person.split(" ");
    Name = separator[0];
    Surname = separator[1];
    Patronymic = separator [2];
    int32_t id = idFinder(request);
    request = QString("UPDATE staff set name = '%1', surname = '%2', patronymic = '%3', post = '%4' where uniqueID = '%5'")
               .arg(Name).arg(Surname).arg(Patronymic).arg(Post).arg(id);
    qDebug()<< request;
    sqlData(request);
    editMode(false);
    ui->postData->clear();
    ui->personData->clear();
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
    Name = separator[0];
    Surname = separator[1];
    Patronymic = separator [2];
    request = QString("insert into staff (name, surname, patronymic, post) values ('%1', '%2', '%3', '%4')")
                  .arg(Name).arg(Surname).arg(Patronymic).arg(Post);
    qDebug()<<request;
    sqlData(request);
    addMode(false);
    ui->addButton->setDisabled(false);
    ui->postData->clear();
    ui->personData->clear();
}

void Widget::on_addButton_clicked()
{
    addMode(true);
}
void Widget::addMode(bool mode)
{
    ui->tableView->setVisible(!mode);
    ui->postData->setReadOnly(!mode);
    ui->personData->setReadOnly(!mode);
    ui->saveAddButton->setDisabled(!mode);
    ui->saveAddButton->setVisible(mode);
    ui->editButton->setDisabled(mode);
    itemsAreVisible(mode);
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
    qDebug()<<"5К НЕТВОРСА СЮДААААА: ";
    }
    else
    qDebug()<<"ШМОТОК НЕТ)))"<<endl<<qry->lastError();

}
