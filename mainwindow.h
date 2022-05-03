#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QtGlobal>

#include <preprocessing.h>
#include <extraction.h>
#include "matcher.h"
#include <algorithm>
#include <iostream>
#include <QtSql>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QListWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QImage scanner();
    void preprocHandler(PREPROCESSING_ALL_RESULTS);
    void preprocHandlerBasic(PREPROCESSING_RESULTS results);
    void extractionHandler(QVector<MINUTIA> results);
    void matchingHandler(bool success);

private slots:
    void on_scanButton_registration_clicked();

    void on_enrollButton_clicked();

    void on_rescanButton_registration_clicked();

    void on_fingerList_itemClicked(QListWidgetItem *item);

    void on_scanButton_login_clicked();

    void on_loginButton_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_scanButton_experiment_clicked();

private:
    Ui::MainWindow *ui;
    Preprocessing p;
    Extraction e;
    Matcher m;
    QImage img;
    int index;
    int tabIndex = 0;
    QVector<QVector<MINUTIA>> dbRegistration;
    QVector<MINUTIA> dbLogin;
    QVector<QVector<MINUTIA>> fingerprints;
    QMap<int, QImage> fingerListMap;
    QMap<int, QImage>::iterator i;
};
