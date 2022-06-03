#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <iostream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardItemModel>
#include "UFScanner.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&p, qOverload<PREPROCESSING_ALL_RESULTS>(&Preprocessing::preprocessingDoneSignal), this, &MainWindow::preprocHandler);
    connect(&p, qOverload<PREPROCESSING_RESULTS>(&Preprocessing::preprocessingDoneSignal), this, &MainWindow::preprocHandlerBasic);
    connect(&e, qOverload<QVector<MINUTIA>>(&Extraction::extractionDoneSignal), this, &MainWindow::extractionHandler);
    connect(&m, &Matcher::verificationDoneSignal, this, &MainWindow::matchingHandler);
    this->p.setCPUOnly(true,8);
    this->p.setFeatures(false);
    this->e.setCPUOnly(true);

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("localhost");
        db.setPort(3306);
        db.setUserName("testuser2");
        db.setPassword("test123");
        db.setDatabaseName("users");

        if (db.open()) {
            //QMessageBox::information(this, "Connection", "Database Connected Successfully");
        }
        else {
            QMessageBox::information(this, "Not connected", "Database Not Connected");
        }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QImage MainWindow::scanner()
{
    HUFScanner scanner;
    UFS_STATUS status;
    status = UFS_Init();
    if (status == UFS_OK) {
        unsigned char *buff = new unsigned char[320*480];
        UFS_GetScannerHandle(0, &scanner);
        UFS_CaptureSingleImage(scanner);
        UFS_GetCaptureImageBuffer(scanner, buff);
        QImage img = QImage(buff, 320, 480, QImage::Format_Grayscale8);

        if (this->tabIndex == 0) {
            ui->img_scan_registration->setPixmap(QPixmap::fromImage(img));
            this->fingerListMap[this->dbRegistration.size()] = img;
        }
        else if (this->tabIndex == 1){
            ui->img_scan_login->setPixmap(QPixmap::fromImage(img));
        }
        else {
             ui->img_scan_exp->setPixmap(QPixmap::fromImage(img));
        }
        UFS_Uninit();
        return img;
    }
    else {
        return QImage();
    }
}

void MainWindow::preprocHandler(PREPROCESSING_ALL_RESULTS results)
{
    if (this->tabIndex == 2) {
        QImage img = Helper::Mat2QImage(results.imgSkeleton, QImage::Format_Grayscale8);
        ui->img_scan_exp_skeleton->setPixmap(QPixmap::fromImage(img));
    }
    this->e.loadInput(results.imgOriginal, results.imgSkeleton, results.orientationMap);
    this->e.start();
}

void MainWindow::preprocHandlerBasic(PREPROCESSING_RESULTS results)
{
    if (this->tabIndex == 2) {
        QImage img = Helper::Mat2QImage(results.imgSkeleton, QImage::Format_Grayscale8);
        ui->img_scan_exp_skeleton->setPixmap(QPixmap::fromImage(img));
    }
    this->e.loadInput(results.imgOriginal, results.imgSkeleton, results.orientationMap);
    this->e.start();
}

void MainWindow::extractionHandler(QVector<MINUTIA> results)
{
    this->img = this->img.convertToFormat(QImage::Format_RGB888);
    if (this->tabIndex == 0) {
        ui->fingerList->addItem("FingerScan "+QString::number(this->dbRegistration.size()+1));
        this->dbRegistration.push_back(results);
    }
    else if (this->tabIndex == 1) {
        this->dbLogin = results;
    }
    else {
        this->img = this->img.convertToFormat(QImage::Format_RGB888);
        QPainter painter(&this->img);
        int circleSize = 7;
        for(MINUTIA &m:results) {
           if (m.type == 0) {
               painter.setPen(QPen(QBrush(QColor(255,0,0)),1));
           }
           else {
               painter.setPen(QPen(QBrush(QColor(0,255,0)),1));
           }
           painter.drawEllipse(m.xy.x()-circleSize, m.xy.y()-circleSize, circleSize, circleSize);
        }
        ui->img_scan_exp_extraction->setPixmap(QPixmap::fromImage(this->img));
    }
}

void MainWindow::on_scanButton_registration_clicked()  //scan
{
    this->img = this->scanner();
    this->p.loadInput(this->img);
    this->p.start();
}

void MainWindow::on_rescanButton_registration_clicked()   //rescan
{
    this->dbRegistration.pop_back();
    ui->fingerList->takeItem(ui->fingerList->count()-1);
    this->img = this->scanner();
    this->p.loadInput(this->img);
    this->p.start();
}

void MainWindow::on_enrollButton_clicked() //enroll
{
    QSqlQuery queryUsers, queryFinger;
    queryUsers.exec("INSERT INTO person (name) VALUES ('"+ui->username_input_registration->text()+"')");
    int lastID = queryUsers.lastInsertId().toInt();
    QString level2string;
    for (int i=0; i<dbRegistration.size(); i++) {
        for (int j=0; j<dbRegistration.at(i).size(); j++) {
            level2string.append(QString::number(dbRegistration.at(i).at(j).xy.x()));
            level2string.append(" ");
            level2string.append(QString::number(dbRegistration.at(i).at(j).xy.y()));
            level2string.append(" ");
            level2string.append(QString::number(dbRegistration.at(i).at(j).type));
            level2string.append(" ");
            level2string.append(QString::number(dbRegistration.at(i).at(j).quality));
            level2string.append(" ");
            level2string.append(QString::number(dbRegistration.at(i).at(j).imgWH.x()));
            level2string.append(" ");
            level2string.append(QString::number(dbRegistration.at(i).at(j).imgWH.y()));
            level2string.append(" ");
            level2string.append(QString::number(dbRegistration.at(i).at(j).angle));
            level2string.append(";");
        }
        queryFinger.exec("INSERT INTO fingerprint (person_id, level_2_znaky) VALUES ("+QString::number(lastID)+", '"+level2string+"')");
        level2string.clear();
    }
    QMessageBox msgBoxRegistration;
    msgBoxRegistration.setText("You've been registered successfully");
    msgBoxRegistration.exec();
    dbRegistration.clear();
    ui->username_input_registration->clear();
    ui->fingerList->clear();
    ui->img_scan_registration->clear();
    ui->img_preview->clear();
}

void MainWindow::on_fingerList_itemClicked(QListWidgetItem *item)
{
    int row = ui->fingerList->row(item);
    for (i = this->fingerListMap.begin(); i != this->fingerListMap.end(); ++i) {
        if (i.key() == row) {
            QImage img = this->fingerListMap[row];
            ui->img_preview->setPixmap(QPixmap::fromImage(img));
       }
    }
}

void MainWindow::on_scanButton_login_clicked()
{
    this->img = this->scanner();
    this->p.loadInput(this->img);
    this->p.start();
}

void MainWindow::on_loginButton_clicked()
{
    QSqlQuery queryUserID, queryFinger;
    queryUserID.exec("SELECT id FROM person WHERE name = \"user6\"");
    queryUserID.next();
    int userID = queryUserID.value(0).toInt();
    queryFinger.exec("SELECT level_2_znaky FROM fingerprint WHERE person_id = "+QString::number(userID)+"");
    queryFinger.next();
    QVector<MINUTIA> fingerprint;
    while (queryFinger.next()) {
            QString level2string = queryFinger.value(0).toString();
            QStringList substringsList = level2string.split(';');
            for (int i=0; i<substringsList.size()-1; i++) {
                MINUTIA level2;
                QStringList level2StringsList = substringsList.at(i).split(" ");
                int xCoordinate = level2StringsList.at(0).toInt();
                level2.xy.setX(xCoordinate);
                int yCoordinate = level2StringsList.at(1).toInt();
                level2.xy.setY(yCoordinate);
                int type = level2StringsList.at(2).toInt();
                level2.type = type;
                int quality = level2StringsList.at(3).toInt();
                level2.quality = quality;
                int xWidth = level2StringsList.at(4).toInt();
                level2.imgWH.setX(xWidth);
                int yWidth = level2StringsList.at(5).toInt();
                level2.imgWH.setY(yWidth);
                float angle = level2StringsList.at(6).toFloat();
                level2.angle = angle;
                fingerprint.push_back(level2);
            }
            //jeden spracovany odtlacok
            this->fingerprints.push_back(fingerprint);
            fingerprint.clear();
    }
    this->m.setMatcher(MATCHER::bozorth3);
    m.verify(this->dbLogin, this->fingerprints);
}

void MainWindow::matchingHandler(bool success)
{
    if (success == 1) {
        QMessageBox msgBoxLogin;
        msgBoxLogin.setText("You've been logged in successfully");
        msgBoxLogin.exec();
    }
    else {
        QMessageBox msgBoxLogin;
        msgBoxLogin.setText("Invalid login, try again or register");
        msgBoxLogin.exec();
    }
    dbLogin.clear();
    ui->username_input_login->clear();
    ui->img_scan_registration->clear();
    this->fingerprints.clear();
}


void MainWindow::on_tabWidget_currentChanged(int index)
{
   this->tabIndex = index;
}


void MainWindow::on_scanButton_experiment_clicked()
{
    this->img = this->scanner();
    this->p.loadInput(this->img);
    this->p.start();
}

