#include "widget.h"
#include"CustomModel.h"
#include <QSqlQuery>
#include <QApplication>
#include <QDebug>
#include <QSqlRecord>
#include <QtSql>
#include <iostream>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();


}
