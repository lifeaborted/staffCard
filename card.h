#ifndef CARD_H
#define CARD_H

#include <QWidget>
#include<QVBoxLayout>
#include <QtGui>
#include <QLabel>

#include <QSqlQuery>
#include <QDebug>
#include <QSqlRecord>
#include <QtSql>

namespace Ui {
class card;
}

class card : public QWidget
{
    Q_OBJECT

public:
    explicit card(QSqlDatabase sdb, QWidget *parent = nullptr);

    ~card();

private slots:
    void on_pushButton_clicked();

private:
    Ui::card *ui;
    void nameTable();
    void equipmentList();
    void addEquipment();
    QSqlDatabase sdb;
};

#endif // CARD_H
