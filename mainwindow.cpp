#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenuBar>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QDialog>
#include <QLineEdit>
#include <QFile>
#include <QMessageBox>
#include <QDir>
#include <QMediaPlayer>
#include <QTime>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /*设置窗口标题和图标*/
    this->setWindowIcon(QIcon(":/res/0.png"));
    this->setWindowTitle("时间都去哪了 v2.3");
    /*设置菜单栏*/
    this->setMenuBarStyle();
    /*将文件中的item写到ComboBox中*/
    this->addItemFromFile();
    /*软件启动初始化*/
    this->softwareInit();
    /*如何判断事件已经改变了*/
    void(QComboBox::*strActivated)(const QString &) = &QComboBox::activated;
    connect(ui->sumComboBox,strActivated,this,&MainWindow::on_changeEvent);
}
/*软件启动初始化*/
void MainWindow::softwareInit()
{
    /*创建一个日志文件，用来收集日志*/
    logFile =new QFile("./Sandro.log");
    logFile->remove();
    logFile->open(QIODevice::Append);
    COLLECT_LOG("start collect log................\r\n");
    /*默认定时器是关闭的*/
    startTime=0;
    /*默认久坐提醒是没有打开的*/
    openRemind=0;
    /*防止野指针，否则定时器未开启时，直接重置会导致奔溃*/
    sedentaryTimer=NULL;
    /*创建用来保存文件的文件夹*/
    QDir dir ;
    if(!dir.exists("./Sandro"))
    {
        dir.mkdir("./Sandro");
        COLLECT_LOG("Sandro dir is created!\r\n");
    }
    else
    {
        COLLECT_LOG("Sandro dir is existed!\r\n");
    }
    /*从文件中读取已累计的时间*/
    QString currentItem = ui->sumComboBox->currentText();
    if(currentItem == "")
    {
        COLLECT_LOG("currentItem is empty!\r\n");
        return;
    }
    QFile *saveTimeFile = new QFile("./Sandro/"+currentItem+".txt");
    if(!saveTimeFile->open(QIODevice::ReadOnly))
    {
        COLLECT_LOG("open"+currentItem.toUtf8()+" .txt file failed!\r\n");
        return;
    }
    totalTime=QString(saveTimeFile->readLine()).toInt();
    saveTimeFile->close();
    connect(this,initShowTime,this,&MainWindow::on_showTime);
    emit initShowTime();
}
MainWindow::~MainWindow()
{
    /*在没有暂停的时候关闭了软件也自动保存时间*/
    QString currentItem = ui->sumComboBox->currentText();
    QFile *saveTimeFile = new QFile("./Sandro/"+currentItem+".txt");
    if(!saveTimeFile->open(QIODevice::WriteOnly))
    {
        COLLECT_LOG("open"+currentItem.toUtf8()+" .txt file failed!");
    }
    saveTimeFile->write(QString::number(totalTime).toUtf8());
    saveTimeFile->close();
    delete ui;
    /*结束日志收集*/
    logFile->close();
}
/*设置点击新建事件的槽函数*/
void MainWindow::on_newEvent()
{
    inputNewEventDialog = new QDialog();
    inputNewEventLine = new QLineEdit(inputNewEventDialog);
    QLabel *tipInputLabel = new QLabel("输入事件名:",inputNewEventDialog);
    QPushButton *confirmAddPushButton  = new QPushButton("确定",inputNewEventDialog);
    inputNewEventDialog->setWindowTitle("输入事件");
    inputNewEventDialog->setWindowIcon(QIcon(":/res/0.png"));
    inputNewEventLine->setGeometry(120,30,150,30);
    tipInputLabel->setGeometry(20,30,100,30);
    confirmAddPushButton->setGeometry(100,90,100,30);
    inputNewEventLine->setStyleSheet("QLineEdit{font:\"楷体\";font-size:18px;}");
    tipInputLabel->setStyleSheet("QLabel{font:\"楷体\";font-size:18px;}");
    confirmAddPushButton->setStyleSheet("QPushButton{font:\"楷体\";font-size:18px;}");
    inputNewEventDialog->setFixedSize(300,150);
    inputNewEventDialog->show();
    connect(confirmAddPushButton,&QPushButton::clicked,this,&MainWindow::on_creatNewItem);
}
/*根据用户输入的事件名创建对应的Item*/
void MainWindow::on_creatNewItem()
{
    if(inputNewEventLine->text() == ""){
        QMessageBox::critical(this,"错误","事件名称不能为空，请输入事件名！");
        inputNewEventDialog->close();
        return ;
    }
    /*每次将用户新建的Item保存到文件中，下次软件启动时候从文件中获取*/
    QFile *itemFile = new QFile("./Sandro/item.txt");
    itemFile->open(QIODevice::ReadOnly);
    while(!itemFile->atEnd())
    {
        QString item = itemFile->readLine();
        int length=item.length();
        QString inemWithoutRN = item.mid(0,length-2);
        qDebug()<<inemWithoutRN;
        if(inputNewEventLine->text() == inemWithoutRN){
            QMessageBox::critical(this,"错误","该事件已存在，不能再新建！");
            inputNewEventDialog->close();
            itemFile->close();
            return;
        }
    }
    itemFile->close();
    itemFile = new QFile("./Sandro/item.txt");
    itemFile->open(QIODevice::Append | QIODevice::ReadWrite);
    itemFile->write(inputNewEventLine->text().toUtf8()+"\r\n");
    itemFile->close();
    /*立马在界面中生效*/
    ui->sumComboBox->addItem(inputNewEventLine->text());
    /*再新建一个文件用来保存这个事件的累计时间*/
    QFile *eventTimeFile = new QFile("./Sandro/"+inputNewEventLine->text()+".txt");
    eventTimeFile->open(QIODevice::WriteOnly);
    eventTimeFile->close();
    /*每次只输入一个确保不会因用户多次点击而添加多个*/
    inputNewEventDialog->close();
}


/*软件启动时，从item文件中获取item*/
void MainWindow::addItemFromFile()
{
    QFile *file = new QFile("./Sandro/item.txt");
    file->open(QIODevice::ReadOnly);
    while(!file->atEnd()){
        QString item = file->readLine();
        int length=item.length();
        QString inemWithoutRN = item.mid(0,length-2);
        ui->sumComboBox->addItem(inemWithoutRN);
    }
    file->close();
}

/*设置删除事件的槽函数*/
void MainWindow::on_deleteEvent()
{
    QMessageBox::information(this,"通知","待完善!");

}
/*设置点击导出的槽函数*/
void MainWindow::on_exportTimeTable()
{
    QMessageBox::information(this,"通知","待完善!");
}
/*显示累计时间的槽函数*/
void MainWindow::on_showTime()
{
    int hour=0,min=0,sec=0;
    totalTime++;
    hour=totalTime/3600;
    min=(totalTime-hour*3600)/60;
    sec=totalTime-hour*3600-min*60;
    //qDebug()<<"hour"<<hour<<"min"<<min<<"sec"<<sec;
    ui->showTimeLabel->setText(QString::number(hour)+"小时"
                               +QString::number(min)+"分"
                               +QString::number(sec)+"秒");

}
/*设置菜单栏信息*/
void MainWindow::setMenuBarStyle()
{
    QMenu *eventMenuBar = ui->menuBar->addMenu("事件(&N)");
    QMenu *exportMenuBar = ui->menuBar->addMenu("导出(&O)");
    QMenu *ToolMenuBar = ui->menuBar->addMenu("工具(&T)");
    QMenu *helpMenuBar = ui->menuBar->addMenu("帮助(&H)");
    newEvent = eventMenuBar->addAction(QIcon(":/0.jpeg"),"新建事件",this,0,Qt::CTRL+Qt::Key_N);
    delEvent = eventMenuBar->addAction(QIcon(":/0.jpeg"),"删除事件",this,0,Qt::CTRL+Qt::Key_Delete);
    exportTimeTable = exportMenuBar->addAction(QIcon(":/1.jpeg"),"导出时间累计表",this,0,Qt::CTRL+Qt::Key_O);
    addTime = ToolMenuBar->addAction(QIcon(":/1.jpeg"),"添加时间",this,0,Qt::CTRL+Qt::Key_T);
    userGuide = helpMenuBar->addAction(QIcon(":/0.jpeg"),"使用指南",this,0,Qt::CTRL+Qt::Key_H);
    hadFunction = helpMenuBar->addAction(QIcon(":/0.jpeg"),"支持的功能",this,0,Qt::CTRL+Qt::Key_F);
    versIonInfo = helpMenuBar->addAction(QIcon(":/0.jpeg"),"版本更新记录",this,0,Qt::CTRL+Qt::Key_V);
    connect(newEvent,&QAction::triggered,this,&MainWindow::on_newEvent);
    connect(delEvent,&QAction::triggered,this,&MainWindow::on_exportTimeTable);
    connect(exportTimeTable,&QAction::triggered,this,&MainWindow::on_exportTimeTable);
    connect(addTime,&QAction::triggered,this,&MainWindow::on_addTimeTool);
    connect(userGuide,&QAction::triggered,this,  &MainWindow::on_userGuide);
    connect(hadFunction,&QAction::triggered,this,&MainWindow::on_hadFunction);
    connect(versIonInfo,&QAction::triggered,this,&MainWindow::on_versIonInfo);
}
/*开始计时的槽函数*/
void MainWindow::on_startSumPushButton_clicked()
{
    if(startTime == 1){
        QMessageBox::critical(this,"错误","已开始计时！");
        return;
    }
    if(ui->sumComboBox->currentText() == "")
    {
        QMessageBox::critical(this,"错误","还未新建事件，请先新建事件！");
        return;
    }
    startTime=1;
    timeKeeperTimer = new QTimer(this);
    connect(timeKeeperTimer,&QTimer::timeout,this,&MainWindow::on_showTime);
    timeKeeperTimer->start(1000);
    ui->sumComboBox->setDisabled(true);
}
/*结束计时的槽函数*/
void MainWindow::on_stopSumPushButton_clicked()
{
    if(ui->sumComboBox->currentText() == "")
    {
        QMessageBox::critical(this,"错误","还未新建事件，请先新建事件！");
        return;
    }
    if(startTime == 0){
        QMessageBox::critical(this,"错误","已停止计时！");
        return;
    }
    timeKeeperTimer->stop();
    startTime=0;
    /*将时间写到文件中*/
    QString currentItem = ui->sumComboBox->currentText();
    QFile *saveTimeFile = new QFile("./Sandro/"+currentItem+".txt");
    saveTimeFile->open(QIODevice::WriteOnly);
    saveTimeFile->write(QString::number(totalTime).toUtf8());
    saveTimeFile->close();
    ui->sumComboBox->setDisabled(false);
}

/*事件改变的槽函数*/
void MainWindow::on_changeEvent()
{
    QString currentEvent = ui->sumComboBox->currentText();
    //qDebug()<<currentEvent;
    QFile *currentEventFile = new QFile("./Sandro/"+currentEvent+".txt");
    currentEventFile->open(QIODevice::ReadOnly);
    totalTime =QString(currentEventFile->readLine()).toInt();
    currentEventFile->close();
    /*用于切换时间后立马能够显示当前事件的时间*/
    int hour=0,min=0,sec=0;
    hour=totalTime/3600;
    min=(totalTime-hour*3600)/60;
    sec=totalTime-hour*3600-min*60;
    //qDebug()<<"hour"<<hour<<"min"<<min<<"sec"<<sec;
    ui->showTimeLabel->setText(QString::number(hour)+"小时"
                               +QString::number(min)+"分"
                               +QString::number(sec)+"秒");
}
/*对应工具栏中手动添加时间的功能*/
void MainWindow::on_addTimeTool()
{
    if(ui->sumComboBox->currentText() == "")
    {
        QMessageBox::critical(this,"错误","事件为空，请先添加事件！");
        return ;
    }
    QMessageBox::information(this,"告知","注意：默认为当前事件添加时间！");
    addTimeDialog = new QDialog();
    addTimeEdit = new QTimeEdit(addTimeDialog);
    QLabel *tipInputLabel = new QLabel("设置时间:",addTimeDialog);
    QPushButton *confirmAddPushButton  = new QPushButton("确定",addTimeDialog);
    addTimeEdit->setStyleSheet("QTimeEdit{font:\"楷体\";font-size:18px;}");
    tipInputLabel->setStyleSheet("QLabel{font:\"楷体\";font-size:18px;}");
    confirmAddPushButton->setStyleSheet("QPushButton{font:\"楷体\";font-size:18px;}");
    addTimeDialog->setWindowTitle("输入时间");
    addTimeDialog->setWindowIcon(QIcon(":/res/0.png"));
    addTimeEdit->setGeometry(120,30,150,30);
    addTimeEdit->setDisplayFormat("H:mm:ss");
    tipInputLabel->setGeometry(20,30,100,30);
    confirmAddPushButton->setGeometry(100,90,100,30);
    addTimeDialog->setFixedSize(300,150);
    addTimeDialog->show();
    connect(confirmAddPushButton,&QPushButton::clicked,this,&MainWindow::on_addTime);
}
/*最终添加时间的地方*/
void MainWindow::on_addTime()
{
    qDebug()<<123;
    addTimeDialog->close();
    QTime userInputTime = addTimeEdit->time();
    totalTime+=userInputTime.hour()*3600+userInputTime.minute()*60+userInputTime.second();
    /*用于切换时间后立马能够显示当前事件的时间*/
    int hour=0,min=0,sec=0;
    hour=totalTime/3600;
    min=(totalTime-hour*3600)/60;
    sec=totalTime-hour*3600-min*60;
    //qDebug()<<"hour"<<hour<<"min"<<min<<"sec"<<sec;
    ui->showTimeLabel->setText(QString::number(hour)+"小时"
                               +QString::number(min)+"分"
                               +QString::number(sec)+"秒");
}

/*提醒开启的槽函数*/
void MainWindow::on_startPushButton_clicked()
{
    /*为真表示选中久坐提醒功能*/
    if(!ui->sedentaryCheckBox->isChecked()){
        QMessageBox::critical(this,"错误提醒","未选中久坐功能功能！");
        return;
    }
    if(openRemind == 1)
    {
        QMessageBox::critical(this,"错误提醒","功能已开启！");
        return;
    }
    QMessageBox::information(this,"开启成功","久坐提醒功能已开启，请确保电脑处在非常静音状态下！");
    openRemind=1;
    if(ui->sedentaryCheckBox->isChecked())
    {
        userSetSedentaryTime = ui->sedentaryTimeEdit->time();
        /*将时间转化为秒*/
        sedentaryTimeToSecond=userSetSedentaryTime.hour()*3600+\
                      userSetSedentaryTime.minute()*60+\
                      userSetSedentaryTime.second();
        //qDebug()<<sedentaryTimeToSecond;
        sedentaryTimer = new QTimer(this);
        connect(sedentaryTimer,&QTimer::timeout,this,&MainWindow::on_sedentaryRemind);
        /*开启定时器，1秒进一次中断*/
        sedentaryTimer->start(1000);
    }
    /*设置为禁止修改的*/
    ui->sedentaryCheckBox->setEnabled(false);
    ui->sedentaryTimeEdit->setEnabled(false);
}
/*久坐提醒的槽函数*/
void MainWindow::on_sedentaryRemind()
{
    if(sedentaryTimeToSecond==1)
    {
        QMediaPlayer *musicPlayer =new QMediaPlayer(this);
        musicPlayer->setMedia(QUrl::fromLocalFile("./music/0.mp3"));
        musicPlayer->setVolume(50);
        musicPlayer->play();
        QMessageBox::information(this,"久坐提醒","坐了很久了，起来休息一下吧！");
        ui->sedentaryCheckBox->setEnabled(true);
        ui->sedentaryTimeEdit->setEnabled(true);
        sedentaryTimer->stop();
        openRemind=0;
        return ;
    }
    sedentaryTimeToSecond--;
    //qDebug()<<sedentaryTimeToSecond;
}
/*重置提醒时间*/
void MainWindow::on_resetPushButton_clicked()
{
    QMessageBox::information(this,"重置成功","已重置！");
    ui->sedentaryTimeEdit->setTime(QTime(1,0,0,0));
    ui->sedentaryTimeEdit->setEnabled(true);
    ui->sedentaryCheckBox->setEnabled(true);
    openRemind=0;
    if(sedentaryTimer!=NULL)
        sedentaryTimer->stop();
}
/*用户提示的槽函数*/
void MainWindow::on_userGuide()
{
    QMessageBox::information(this,"使用指南","一、时间管理使用方法如下：\
                                            \r\n1、点击事件-->新建事件,创建一个新的事件;\
                                            \r\n2、开始学习时点击开始计时即可;\
                                            \r\n3、结束学习点击结束计时,软件会自动保存累计时间。\
                                            \r\n4、下次继续学习时时间会在上次累计的时间上继续累加;\
                                            \r\n\r\n二、久坐提醒使用如下\
                                            \r\n1、设置时间间隔(默认为1小时);\
                                            \r\n2、启动提醒，系统就会在设置的时间间隔后发出提示;\
                                            \r\n3、重置设置可以提前结束此次提醒，并将时间间隔恢复到默认值;\
                                            "
                             );
}

void MainWindow::on_hadFunction()
{
    QMessageBox::information(this,"已有功能","支持功能如下：\
                                            \r\n1、理论上支持无限个事件;\
                                            \r\n2、重启软件后会自动更新到上一次累计的时间;\
                                            \r\n3、关闭软件后会自动保存当前累计的时间;\
                                            \r\n4、切换任意事件，自动更新当前事件的累计时间;\
                                            \r\n5、支持自定义时间的久坐提醒;\
                                            \r\n6、久坐提醒支持音乐;\
                                            "
                             );
}

void MainWindow::on_versIonInfo()
{
    QMessageBox::information(this,"开发记录","\r\n2021年2月12日;\
                                            \r\n1、首次开发软件的基本功能;\
                                            \r\n2021年2月13日;\
                                            \r\n1、添加久坐提醒功能;\
                                            \r\n2、添加切换事件立马更新累计时间功能;\
                                            \r\n3、修复开始计时后切换事件导致时间紊乱的bug;\
                                            \r\n4、修复未开启提醒，直接重置提醒导致的软件奔溃bug;\
                                            \r\n5、修复可以新建已存在事件的bug;\
                                            \r\n6、增加日志收集功能\
                                            \r\n7、久坐提醒支持音乐;\
                                            \r\n2021年2月15日;\
                                            \r\n1、添加手动为事件增加时间的功能;\
                                            "
            );
}
