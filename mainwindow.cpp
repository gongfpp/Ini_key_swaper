#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "QInputDialog"
#include "QFileDialog"
#include "QDir"
#include "QSettings"
#include "QDebug"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);

//    ui->lineEdit_2->setText("PCB");
//    ui->lineEdit_3->setText("DSN");

    ui->statusBar->showMessage("本工具可交换ini文件内每组的keyA与keyB");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString dirStr = QFileDialog::getExistingDirectory(this,"选择目录");
    ui->lineEdit->setText(dirStr);
    ui->pushButton_2->setEnabled(true);
}

void MainWindow::on_pushButton_2_clicked()
{
    QString dirStr = ui->lineEdit->text();
    QDir dir(dirStr);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);  // 获取文件列表
    for (int i = 0; i < file_list.size(); i++)
    {
        ui->plainTextEdit->appendPlainText(file_list.at(i).filePath());
    }
    ui->pushButton_3->setEnabled(true);
    ui->plainTextEdit->appendPlainText(QString("该文件夹共有%1个文件").arg(file_list.size()));

}

void MainWindow::on_pushButton_3_clicked()
{
    QString key_a_replace = ui->lineEdit_2->text();
    QString key_b_replace = ui->lineEdit_3->text();
    QString dirStr = ui->lineEdit->text();

    QDir dir(dirStr);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);  // 获取文件列表

    if(QMessageBox::question(this,"确认",QString("确认交换%1个文件中每组的%2和%3吗").
                             arg(file_list.size()).arg(key_a_replace).arg(key_b_replace)) == QMessageBox::No){
        return;
    }
    ui->plainTextEdit_2->clear();
//    遍历每个ini文件
    for (int i = 0; i < file_list.size(); i++)
    {
        QSettings setting(file_list.at(i).filePath(),QSettings::Format::IniFormat,this);
        qDebug() << file_list.at(i).filePath();
        qDebug() << "setting :" << setting.fileName();
        QStringList groups = setting.childGroups();
        qDebug() << groups;

//        遍历每个组
        foreach (QString group, groups) {
            setting.beginGroup(group);

            QString value_a = setting.value(key_a_replace).toString();
            QString value_b = setting.value(key_b_replace).toString();

            if(!value_a.isEmpty()){
                setting.setValue(key_b_replace,value_a);
                ui->plainTextEdit_2->appendPlainText(
                            QString("%1.ini文件 %2组 %3转为%4").arg(file_list.at(i).baseName()).arg(group)
                            .arg(key_a_replace,key_b_replace));
            }else{
                setting.remove(key_b_replace);
            }

            if(!value_b.isEmpty()){
                setting.setValue(key_a_replace,value_b);
                ui->plainTextEdit_2->appendPlainText(
                            QString("%1.ini文件 %2组 %3转为%4").arg(file_list.at(i).baseName()).arg(group)
                            .arg(key_b_replace,key_a_replace));
            }else {
                setting.remove(key_a_replace);
            }

            setting.endGroup();
        }

    }
}
