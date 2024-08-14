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
#include<QDialog>
#include<QDialogButtonBox>

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

    //QString title, description, serialNumber, comment, takeDate, returnDate;

    QString Name, Surname, Patronymic, Post, Person;
    QPixmap profilePic = QPixmap (":/tri.jpg");
    ui->label->setPixmap(profilePic);

    QPixmap searchPic = QPixmap (":/lupa.png");
    QSize PicSize (30, 30);
    searchPic = searchPic.scaled(PicSize, Qt::KeepAspectRatio);
    ui->searchLabel->setPixmap(searchPic);

    itemsAreVisible(false);
    ui->eqSaveButton->setVisible(false);
    ui->eqSaveButton->setDisabled(true);

    ui->tabWidget->setTabVisible(1, false);
    ui->tabWidget->setTabEnabled(1, false);

    ui->tabWidget->setTabVisible(2, false);
    ui->tabWidget->setTabEnabled(2, false);

    ui->addButton->setDisabled(false);
    ui->saveAddButton->setVisible(false);
    ui->saveEditButton->setVisible(false);
    ui->saveAddButton->setDisabled(true);
    ui->saveEditButton->setDisabled(true);
    ui->tableView->setVisible(false);

    ui->editButton->setVisible(false);
    ui->editButton->setEnabled(false);

    ui->nameData->setReadOnly(true);
    ui->surnameData->setReadOnly(true);
    ui->patronymicData->setReadOnly(true);

    timer = new QTimer(this);
    timer->setSingleShot(true);
    ui->dataText->activateWindow();
    ui->dataBox->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->dataText, SIGNAL(textEdited(QString)), SLOT(onSearchTextChanged()));
    connect (timer, SIGNAL(timeout()), SLOT(performSearch()));
    connect(ui->dataBox, SIGNAL(doubleClicked(QModelIndex)), SLOT(onComboboxItemActivated(QModelIndex)));
    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), SLOT(onTableViewItemActivated(QModelIndex)));
    connect(ui->eqHistoryTableView, SIGNAL(clicked(QModelIndex)), SLOT(historyItemActivated(QModelIndex)));

    connect(ui->nameData, &QLineEdit::textEdited, this, &Widget::newPersonValid);
    connect(ui->surnameData, &QLineEdit::textEdited, this, &Widget::newPersonValid);
    connect(ui->patronymicData, &QLineEdit::textEdited, this, &Widget::newPersonValid);
    connect(ui->postData, &QLineEdit::textEdited, this, &Widget::newPersonValid);

    connect(ui->serialNumberEdit, &QLineEdit::textEdited, this, &Widget::newEquipmentValid);
    connect(ui->titleEdit, &QLineEdit::textEdited, this, &Widget::newEquipmentValid);

    generalRequest = "select name, surname, patronymic from staff";
    eqGeneralRequest = "select * from current";

    ui->dataText->setFocus();
    QTimer::singleShot(0, ui->dataText, SLOT(setFocus()));
    sqlDataReserve(generalRequest);
    fillComboBox();
    //performSearch();

    ui->eqDeleteButton->setVisible(false);
    ui->eqDeleteButton->setEnabled(false);

    ui->returnButton->setVisible(false);
    ui->returnButton->setEnabled(false);

    QSqlQueryModel equipedModelReserve;

    ui->returnDateEdit->setEnabled(false);
    ui->returnDateEdit->setVisible(false);

    ui->saveReturnDateEditButton->setVisible(false);
    ui->saveReturnDateEditButton->setEnabled(false);

    ui->returnDateEditW->setVisible(false);
    ui->returnDateEditW->setEnabled(false);

    ui->returnDateLabel->setVisible(false);
    ui->dateLabel->setVisible(false);

    eqEditMode(false);
    ui->takeDateW->setDate(QDate::currentDate());
    ui->returnDateEditW->setReadOnly(false);
    ui->takeDateW->setCalendarPopup(true);

    ui->cancelButton->setVisible(false);
    ui->cancelButton->setEnabled(false);

    ui->deleteButton->setVisible(false);
    ui->deleteButton->setEnabled(false);
}



QStringList Widget::sqlDataReserve(QString req)
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

    _cacheModel = new StringListModel(reserveResult);
    return reserveResult;
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
    ui->nameData->setVisible(mode);
    ui->surnameData->setVisible(mode);
    ui->patronymicData->setVisible(mode);
    ui->postData->setVisible(mode);
    ui->nameLabel->setVisible(mode);
    ui->surnameLabel->setVisible(mode);
    ui->patronymicLabel->setVisible(mode);
    ui->postLabel->setVisible(mode);
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
   timer->start(100);
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
        for(int i = 0; i < _cacheModel->rowCount(); ++i)
        {
            toCompare = _cacheModel->itemData(i);
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
    ui->postData->setReadOnly(!mode);
    ui->nameData->setReadOnly(!mode);
    ui->surnameData->setReadOnly(!mode);
    ui->patronymicData->setReadOnly(!mode);
    ui->addButton->setDisabled(mode);
    itemsAreVisible(mode);
    ui->saveEditButton->setDisabled(!mode);
    ui->saveEditButton->setVisible(mode);
}

void Widget::onComboboxItemActivated(const QModelIndex &index)
{
    ui->editButton->setVisible(true);
    ui->editButton->setEnabled(true);

    ui->deleteButton->setDisabled(false);
    ui->deleteButton->setVisible(true);

    ui->tabWidget->setTabVisible(1, true);
    ui->tabWidget->setTabEnabled(1, true);

    ui->tabWidget->setTabVisible(2, true);
    ui->tabWidget->setTabEnabled(2, true);

    ui->returnDateEditButton->setVisible(false);
    ui->returnDateEditButton->setEnabled(false);

    item = _model1->data(index, Qt::DisplayRole).toString();
    ui->dataText->setText(item);

    eqGeneralRequest = QString("select title, description, serialNumber, comment, takeDate from current where NSP = '%1'")
                    .arg(item);
    qDebug()<<eqGeneralRequest;
    eqHistoryRequest = QString("select title, description, serialNumber, comment, takeDate, returnDate from history where NSP = '%1'")
                           .arg(item);
    equipmentListData(eqGeneralRequest);
    equipmentHistoryListData(eqHistoryRequest);
    QStringList separator = item.split(" ");
    Name = separator[0];
    Surname = separator[1];
    Patronymic = separator [2];
    request = QString("select * from staff where name = '%1' and surname = '%2' and patronymic = '%3'")
                  .arg(Name).arg(Surname).arg(Patronymic);

    sqlData(request);
    ui->nameData->setText(Name);
    ui->surnameData->setText(Surname);
    ui->patronymicData->setText(Patronymic);
    ui->postData->setText(Post);
    itemsAreVisible(true);
    ui->nameData->setReadOnly(true);
    ui->surnameData->setReadOnly(true);
    ui->patronymicData->setReadOnly(true);
    ui->editButton->setEnabled(true);

    ui->postData->setReadOnly(true);
}

void Widget::on_saveEditButton_clicked()
{
    ui->cancelButton->setVisible(false);
    ui->cancelButton->setEnabled(false);

    Name = ui->nameData->text();
    Surname = ui->surnameData->text();
    Patronymic = ui->patronymicData->text();
    Post = ui->postData->text();
    if(!Name.isEmpty() && !Surname.isEmpty() && !Patronymic.isEmpty() && !Post.isEmpty())
    {
    int32_t id = idFinder(request);
    request = QString("UPDATE staff set name = '%1', surname = '%2', patronymic = '%3', post = '%4' where uniqueID = '%5'")
               .arg(Name).arg(Surname).arg(Patronymic).arg(Post).arg(id);

    sqlData(request);
    _model1->updateItem(sqlData(generalRequest));
    _cacheModel->updateItem(sqlData(generalRequest));
    editMode(false);
    itemsAreVisible(true);
    }
    else{
    QMessageBox msg;
    msg.setText("Введите корректные данные!");
    msg.exec();
    }

    ui->editButton->setDisabled(false);
    ui->editButton->setVisible(true);
}

void Widget::on_editButton_clicked()
{
    ui->cancelButton->setVisible(true);
    ui->cancelButton->setEnabled(true);

    editMode(true);
    ui->editButton->setDisabled(true);
    ui->editButton->setVisible(false);
}

void Widget::on_saveAddButton_clicked()
{
    ui->cancelButton->setVisible(false);
    ui->cancelButton->setEnabled(false);

    Name = ui->nameData->text();
    Surname = ui->surnameData->text();
    Patronymic = ui->patronymicData->text();
    Post = ui->postData->text();

        request = QString("insert into staff (name, surname, patronymic, post) values ('%1', '%2', '%3', '%4')")
                      .arg(Name).arg(Surname).arg(Patronymic).arg(Post);

        sqlData(request);
        _model1->updateItem(sqlData(generalRequest));
        _cacheModel->updateItem(sqlData(generalRequest));
        addMode(false);

        ui->addButton->setDisabled(true);
        ui->postData->clear();
        ui->nameData->clear();
        ui->surnameData->clear();
        ui->patronymicData->clear();

        ui->dataText->clear();
    ui->addButton->setDisabled(false);
    ui->addButton->setVisible(true);
    ui->tabWidget->setTabVisible(1, false);
    ui->tabWidget->setTabEnabled(1, false);
}

void Widget::on_addButton_clicked()
{
    ui->saveAddButton->setVisible(true);

    ui->editButton->setVisible(false);
    ui->editButton->setEnabled(false);

    ui->deleteButton->setVisible(false);
    ui->deleteButton->setEnabled(false);

    ui->cancelButton->setVisible(true);
    ui->cancelButton->setEnabled(true);

    ui->postData->clear();
    ui->nameData->clear();
    ui->surnameData->clear();
    ui->patronymicData->clear();


    addMode(true);
    ui->addButton->setDisabled(true);
    ui->addButton->setVisible(false);
}
void Widget::addMode(bool mode)
{
    itemsAreVisible(mode);
    ui->postData->setReadOnly(!mode);
    ui->nameData->setReadOnly(!mode);
    ui->surnameData->setReadOnly(!mode);
    ui->patronymicData->setReadOnly(!mode);

    ui->saveEditButton->setDisabled(mode);
    ui->addButton->setDisabled(!mode);
}

void Widget::equipmentListData(QString eqRequest)
{
    QSqlQueryModel *equipdModel = new QSqlQueryModel;
    qry = new QSqlQuery(edb);
    qry->prepare(eqRequest);
    if(qry->exec())
    {
        equipdModel->setQuery(*qry);
        equipedModelReserve.setQuery(*qry);
        ui->tableView->setModel(equipdModel);
    }
    else
        qDebug()<<"equipmentListData"<<qry->lastError()<<endl;
}

void Widget::equipmentHistoryListData(QString req)
{
    QSqlQueryModel *historyModel = new QSqlQueryModel;
    qry = new QSqlQuery(edb);
    qry->prepare(req);
    if(qry->exec())
    {
        historyModel->setQuery(*qry);
        historyEquipedModelReserve.setQuery(*qry);
        ui->eqHistoryTableView->setModel(historyModel);
    }
    else
        qDebug()<<"equipmentHistoryListData"<<qry->lastError();
}

void Widget::on_deleteButton_clicked()
{
    ui->editButton->setVisible(false);
    ui->editButton->setEnabled(false);
    QStringList separator = Person.split(" ");
    qDebug()<<"pered id"<<request;
    int32_t id = idFinder(request);
    qDebug()<<"IDISHNIK"<<id;
    request = QString("delete from staff where UniqueID = '%1'")
                  .arg(id);
    qDebug()<<"POSLE"<<request;

    sqlData(request);
    _model1->updateItem(sqlData(generalRequest));
    _cacheModel->updateItem(sqlData(generalRequest));

    qDebug()<<result;
    editMode(false);

    ui->deleteButton->setDisabled(true);
    ui->deleteButton->setVisible(false);
    ui->addButton->setDisabled(false);
    ui->editButton->setDisabled(false);
    ui->postData->clear();
    ui->nameData->clear();
    ui->surnameData->clear();
    ui->patronymicData->clear();
    ui->tabWidget->setTabVisible(1, false);
    ui->tabWidget->setTabEnabled(1, false);
}


void Widget::on_cancelButton_clicked()
{
    ui->cancelButton->setVisible(false);
    ui->cancelButton->setEnabled(false);

   addMode(false);
   editMode(false);
   ui->addButton->setVisible(true);
   ui->addButton->setEnabled(true);

   if (!ui->nameData->text().isEmpty() || !ui->surnameData->text().isEmpty() || !ui->patronymicData->text().isEmpty() || !ui->postData->text().isEmpty())
   {
        itemsAreVisible(true);
        ui->editButton->setVisible(true);
        ui->editButton->setEnabled(true);
   }
}

void Widget::on_eqAddButton_clicked()
{
   eqEditMode(true);
   ui->eqDeleteButton->setVisible(false);
   ui->eqDeleteButton->setEnabled(false);
   ui->titleEdit->clear();
   ui->descriptionEdit->clear();
   ui->serialNumberEdit->clear();
   ui->commentEdit->clear();
   //ui->takeDateEdit->clear();
   ui->eqSaveEditButton->setVisible(false);
   ui->eqSaveEditButton->setEnabled(false);
   ui->takeDateW->setEnabled(false);
   ui->takeDateW->setVisible(false);
   ui->returnDateEdit->setVisible(false);
}


void Widget::on_eqSaveButton_clicked()
{
   eqEditMode(false);

   ui->eqAddButton->setVisible(true);
   ui->eqAddButton->setEnabled(true);

    title = ui->titleEdit->text();
    description = ui->descriptionEdit->text();
    serialNumber = ui->serialNumberEdit->text();
    comment = ui->commentEdit->text();
   if (!title.isEmpty())
   {

   eqRequest = QString ("insert into current (NSP, title, description, serialNumber, comment, takeDate) values ('%1', '%2', '%3', '%4', '%5', date())")
                   .arg(item).arg(title).arg(description).arg(serialNumber).arg(comment);
           qDebug()<<"NEW PERSON"<<endl<<"{"<<eqRequest<<endl<<eqGeneralRequest<<endl<<"}";

   equipmentListData(eqRequest);
   equipmentListData(eqGeneralRequest);

   ui->titleEdit->clear();
   ui->descriptionEdit->clear();
   ui->serialNumberEdit->clear();
   ui->commentEdit->clear();
   }
   else
   {
        QMessageBox msg;
        msg.setText("Введите обязательные данные!");
        msg.exec();
   }
}

void Widget::eqEditMode(bool mode)
{
   ui->eqSaveEditButton->setVisible(mode);
   ui->eqSaveEditButton->setEnabled(mode);

   ui->eqSaveButton->setVisible(mode);

   ui->saveAddButton->setVisible(mode);
   ui->saveAddButton->setEnabled(mode);

    ui->eqDeleteButton->setVisible(mode);
    ui->eqDeleteButton->setEnabled(mode);

    ui->eqCancelButton->setVisible(mode);
    ui->eqCancelButton->setEnabled(mode);

    ui->titleEdit->setVisible(mode);
    ui->serialNumberEdit->setVisible(mode);
    ui->commentEdit->setVisible(mode);
    ui->takeDateW->setVisible(mode);
    ui->descriptionEdit->setVisible(mode);

    ui->titleEdit->setEnabled(mode);
    ui->serialNumberEdit->setEnabled(mode);
    ui->commentEdit->setEnabled(mode);
    ui->takeDateW->setEnabled(mode);
    ui->descriptionEdit->setEnabled(mode);

    ui->titleNumberLabel->setVisible(mode);
    ui->descComLabel->setVisible(mode);
}

void Widget::onTableViewItemActivated(const QModelIndex &index)
{
   ui->returnButton->setVisible(true);
   ui->returnButton->setEnabled(true);

   eqEditMode(true);

   ui->eqSaveButton->setVisible(false);

   ui->eqSaveButton->setEnabled(false);

   QModelIndexList indexes = ui->tableView->selectionModel()->selectedIndexes();
   QModelIndex ind = ui->tableView->selectionModel()->currentIndex();
   int row = ind.row();

    QVariant data = equipedModelReserve.data(index);

    title = equipedModelReserve.data(equipedModelReserve.index(row, 0)).toString();
    description = equipedModelReserve.data(equipedModelReserve.index(row, 1)).toString();
    serialNumber = equipedModelReserve.data(equipedModelReserve.index(row, 2)).toString();
    comment = equipedModelReserve.data(equipedModelReserve.index(row, 3)).toString();
    takeDate = equipedModelReserve.data(equipedModelReserve.index(row, 4)).toString();

    reqRowId = QString("select rowid from current where title = '%1' and description = '%2' and serialNumber = '%3' and comment = '%4' and takeDate = '%5' and NSP = '%6'")
                   .arg(title).arg(description).arg(serialNumber).arg(comment).arg(takeDate).arg(item);
    qDebug()<<"ZAPROS ID"<<reqRowId;
    eqRowId(reqRowId);

    qDebug()<<"onTableViewItemActivated"<<title<<description<<serialNumber<<comment<<takeDate;

    ui->returnButton->setVisible(!isReturned());
    ui->returnButton->setEnabled(!isReturned());

    ui->titleEdit->setText(title);
    ui->descriptionEdit->setText(description);
    ui->serialNumberEdit->setText(serialNumber);
    ui->commentEdit->setText(comment);
}

void Widget::on_eqSaveEditButton_clicked()
{
        eqEditMode(false);

        ui->eqDeleteButton->setVisible(false);
        ui->eqDeleteButton->setEnabled(false);

        QString title1 = ui->titleEdit->text();
        QString description1 = ui->descriptionEdit->text();
        QString serialNumber1 = ui->serialNumberEdit->text();
        QString comment1 = ui->commentEdit->text();
        QString takeDate1 = ui->takeDateW->text();
        //QString returnDate1 = ui->takeDateEdit->text();

        eqRequest = QString ("update current set title = '%1', description = '%2', serialNumber = '%3', comment = '%4', takeDate = '%5' where title = '%6' and description = '%7' and serialNumber = '%8' and comment = '%9' and takeDate = '%10' and NSP = '%11'")
                        .arg(title1).arg(description1).arg(serialNumber1).arg(comment1).arg(takeDate1).arg(title).arg(description).arg(serialNumber).arg(comment).arg(takeDate).arg(item);

        qDebug()<<"SOHRANENIE"<<endl<<"{"<<eqRequest<<endl<<eqGeneralRequest<<endl<<"}";

        equipmentListData(eqRequest);

        equipmentListData(eqGeneralRequest);

        ui->titleEdit->clear();
        ui->descriptionEdit->clear();
        ui->serialNumberEdit->clear();
        ui->commentEdit->clear();
        //ui->takeDateEdit->clear();
        ui->returnDateEdit->clear();
}


void Widget::on_eqDeleteButton_clicked()
{
    eqEditMode(false);

    eqRequest = QString ("delete from current where NSP = '%1' and  title = '%2' and description = '%3' and serialNumber = '%4' and comment = '%5' and takeDate = '%6'")
                    .arg(item).arg(title).arg(description).arg(serialNumber).arg(comment).arg(takeDate);
    qDebug()<<"UDALENIE"<<endl<<"{"<<eqRequest<<endl<<eqGeneralRequest<<endl<<"}";

    equipmentListData(eqRequest);
    equipmentListData(eqGeneralRequest);

    ui->eqSaveButton->setVisible(true);
    ui->eqAddButton->setVisible(true);
    ui->eqSaveButton->setEnabled(true);
    ui->eqAddButton->setEnabled(true);

    ui->titleEdit->clear();
    ui->descriptionEdit->clear();
    ui->serialNumberEdit->clear();
    ui->commentEdit->clear();
}





void Widget::on_eqCancelButton_clicked()
{
    ui->returnButton->setVisible(false);
    ui->returnButton->setEnabled(false);
    ui->eqAddButton->setVisible(true);
    ui->eqAddButton->setEnabled(true);
    ui->titleEdit->clear();
    ui->descriptionEdit->clear();
    ui->serialNumberEdit->clear();
    ui->commentEdit->clear();
    eqEditMode(false);
}


void Widget::on_returnButton_clicked()
{

    QDialog dialog(this);
        QVBoxLayout *l = new QVBoxLayout(&dialog);

    dialog.setWindowTitle("Внимание!");
    QDialogButtonBox *newButton = new QDialogButtonBox(&dialog);
    QLabel *accepting = new QLabel;
    accepting->setAlignment(Qt::AlignCenter);
    l->addWidget(newButton);
    accepting->setText("Вы действительно хотите подтвердить возврат?");
    l->addWidget(accepting);
    dialog.setLayout(l);
    newButton->setCenterButtons(true);
    newButton->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(newButton, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(newButton, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if(dialog.exec() == QDialog::Accepted)
    {
        eqRequest = QString ("update history set returnDate = date() where title = '%1' and description = '%2' and serialNumber = '%3' and comment = '%4' and takeDate = '%5' and NSP = '%6'")
                        .arg(title).arg(description).arg(serialNumber).arg(comment).arg(takeDate).arg(item);
        qDebug()<<"vozvrat"<<eqRequest;
        equipmentListData(eqRequest);

        equipmentListData(eqGeneralRequest);
        eqEditMode(false);
    }
    ui->returnButton->setVisible(false);
    ui->returnButton->setEnabled(false);
}

bool Widget::isReturned(){
    QString req = QString ("select returnDate from history where NSP = '%1' and  title = '%2' and description = '%3' and serialNumber = '%4' and comment = '%5' and takeDate = '%6'")
                            .arg(item).arg(title).arg(description).arg(serialNumber).arg(comment).arg(takeDate);
    qDebug()<<"test"<<req;

    qry = new QSqlQuery(edb);
    qry->exec(req);
    qry->next();
    return qry->value(0).isNull();
}


void Widget::on_returnDateEditButton_clicked()
{
    ui->returnDateEditW->setDate(QDate::currentDate());
    ui->returnDateEditW->setCalendarPopup(true);
    ui->returnDateEditW->setReadOnly(false);

    ui->returnDateEditW->setVisible(true);
    ui->returnDateEditW->setEnabled(true);

    ui->returnDateEditButton->setVisible(false);
    ui->returnDateEditButton->setEnabled(false);

    ui->saveReturnDateEditButton->setVisible(true);
    ui->saveReturnDateEditButton->setEnabled(true);

    ui->returnDateLabel->setVisible(true);
}


void Widget::on_saveReturnDateEditButton_clicked()
{
    ui->returnDateEditW->setCalendarPopup(true);
    ui->returnDateEditW->setReadOnly(false);

    ui->returnDateEditW->setVisible(false);
    ui->returnDateEditW->setEnabled(false);

    ui->saveReturnDateEditButton->setVisible(false);
    ui->saveReturnDateEditButton->setEnabled(false);

    ui->returnDateEditButton->setVisible(true);
    ui->returnDateEditButton->setEnabled(true);

    ui->returnDateLabel->setVisible(false);

    QString date = ui->returnDateEditW->text();
    QString req = QString ("update history set returnDate = '%7' where title = '%1' and description = '%2' and serialNumber = '%3' and comment = '%4' and takeDate = '%5' and NSP = '%6'")
                    .arg(title).arg(description).arg(serialNumber).arg(comment).arg(takeDate).arg(item).arg(date);
    qDebug()<<date;
    equipmentHistoryListData(req);
    equipmentHistoryListData(eqHistoryRequest);
}

void Widget::historyItemActivated(const QModelIndex &index)
{
    QModelIndexList indexes = ui->eqHistoryTableView->selectionModel()->selectedIndexes();
    QModelIndex ind = ui->eqHistoryTableView->selectionModel()->currentIndex();
    int row = ind.row();

    QVariant data = historyEquipedModelReserve.data(index);

    title = historyEquipedModelReserve.data(historyEquipedModelReserve.index(row, 0)).toString();
    description = historyEquipedModelReserve.data(historyEquipedModelReserve.index(row, 1)).toString();
    serialNumber = historyEquipedModelReserve.data(historyEquipedModelReserve.index(row, 2)).toString();
    comment = historyEquipedModelReserve.data(historyEquipedModelReserve.index(row, 3)).toString();
    takeDate = historyEquipedModelReserve.data(historyEquipedModelReserve.index(row, 4)).toString();

    ui->returnDateEditButton->setVisible(!isReturned());
    ui->returnDateEditButton->setEnabled(!isReturned());
}


void Widget::newPersonValid()
{
    bool status = !ui->nameData->text().isEmpty() && !ui->patronymicData->text().isEmpty() && !ui->surnameData->text().isEmpty() && !ui->postData->text().isEmpty();

    ui->saveAddButton->setEnabled(status);

}

void Widget::newEquipmentValid()
{
    bool status = !ui->titleEdit->text().isEmpty() && !ui->serialNumberEdit->text().isEmpty();
    ui->eqSaveButton->setEnabled(status);
}

int32_t Widget::eqRowId(QString req)
{
    qry = new QSqlQuery(edb);
    if(qry->exec(req))
    {
    qry->next();
        qDebug()<<"NOMER OF EQ"<<qry->value(0);
    return qry->value(0).toInt();
    }
    else
    qDebug()<<qry->lastError();

}
