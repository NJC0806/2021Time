#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QLineEdit>
#include <QTime>
#include <QFile>
#include <QDateTime>
#include <QTimeEdit>
/*为采集日志封装的宏*/
#define COLLECT_LOG(log) QDateTime currentTime = QDateTime::currentDateTime();\
                         QString strCurrentTime = currentTime.toString("yyyy-MM-dd hh:mm:ss ");\
                         logFile->write(strCurrentTime.toUtf8()+log);
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    
private slots:
    void on_newEvent();
    void on_deleteEvent();
    void on_exportTimeTable();
    void on_showTime(void);
    void on_creatNewItem(void);
    void on_userGuide();
    void on_hadFunction();
    void on_versIonInfo();
    void on_changeEvent(void);
    void on_sedentaryRemind();
    void on_addTimeTool();
    void on_addTime();
    void on_startSumPushButton_clicked();

    void on_stopSumPushButton_clicked();

    void on_startPushButton_clicked();

    void on_resetPushButton_clicked();

signals:
    void initShowTime(void);
private:
    Ui::MainWindow *ui;
    QAction *newEvent;
    QAction *exportTimeTable;
    QAction *delEvent;
    QAction *addTime;
    QAction *userGuide   ;
    QAction *hadFunction ;
    QAction *versIonInfo ;
    QTimer *timeKeeperTimer;
    QTimer *sedentaryTimer;
    QLineEdit *inputNewEventLine;
    QDialog *inputNewEventDialog;
    QDialog *addTimeDialog ;
    QTime userSetSedentaryTime;
    QFile *logFile;
    QTimeEdit *addTimeEdit;
    int totalTime;
    int sedentaryTimeToSecond;
    bool startTime;
    bool openRemind;
    void setMenuBarStyle(void);
    void addItemFromFile(void);
    void softwareInit(void);
};
#endif // MAINWINDOW_H
