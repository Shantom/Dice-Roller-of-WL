#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <windows.h>
#include <QDebug>
#include <QFileSystemWatcher>
#include <QFile>
#include <QStandardPaths>
#include <stdio.h>
#include <QThread>
#include <QDir>
#pragma execution_character_set("utf-8")

#include "inject.h"
class AutoRoll;
#include "autoroll.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_roll_clicked();

    void on_pushButton_autoroll_clicked();

    void on_pushButton_stop_clicked();

    void endRolling();

private:
    Ui::MainWindow *ui;
    Inject inject;
    QFileSystemWatcher watcher;
    DWORD dwPid = 0;
    wchar_t addr_MyRecv[MAX_PATH];
    QString socket;
    QString dataFile;
    SOCKET fd = 0;
    QVector<int> rollData = QVector<int>(5);
    QVector<int> constraints = QVector<int>(10);
    bool clickedingame = false;

    void attempt_inject(LPCTSTR processName);

    void openSocket();

    void dataUpdate();

    void roll();

    AutoRoll roller;
};
#endif // MAINWINDOW_H
