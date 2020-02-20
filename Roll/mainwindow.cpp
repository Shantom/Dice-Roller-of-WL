#include "mainwindow.h"
#include "ui_mainwindow.h"
#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    QString myRecv = QDir::currentPath()+QString("/MyRecv.dll");
    myRecv.replace(QChar('/'),QChar('\\'));
    int len = myRecv.toWCharArray(addr_MyRecv);
    addr_MyRecv[len]=0;

    QString filesPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+QString("\\wlRoll");
    QDir qdir;
    qdir.mkpath(filesPath);


    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    // 禁止最大化按钮
    setFixedSize(this->width(),this->height());
    setWindowTitle("飘流手游摇点器");

    socket = filesPath+QString("\\socket");
    dataFile = filesPath+QString("\\res");
    //创建res文件
    QFile res(dataFile);
    res.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream out(&res);
    out<<0<<0<<0<<0<<0<<endl;
    watcher.addPath(dataFile);
    connect(&watcher,&QFileSystemWatcher::fileChanged,this,&MainWindow::dataUpdate);
    dataUpdate();

    ui->pushButton_stop->setEnabled(false);
    ui->pushButton_roll->setEnabled(false);
    ui->pushButton_autoroll->setEnabled(false);

    connect(&roller,&AutoRoll::ended,this,&MainWindow::endRolling);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    attempt_inject(_T("wl.exe"));
    clickedingame = false;
}

void MainWindow::dataUpdate()
{
    if(!clickedingame)
    {
        ui->pushButton_roll->setEnabled(true);
        clickedingame = true;
    }
    int s,c,i,w,a;
    QFile file(dataFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
           return;
    QTextStream in(&file);

    in>>s>>c>>i>>w>>a;
    file.close();

    rollData[0]=s;
    rollData[1]=c;
    rollData[2]=i;
    rollData[3]=w;
    rollData[4]=a;

    roller.setData(rollData);

    ui->label_str->setText(QString::number(s));
    ui->label_con->setText(QString::number(c));
    ui->label_int->setText(QString::number(i));
    ui->label_wis->setText(QString::number(w));
    ui->label_agi->setText(QString::number(a));

}

void MainWindow::attempt_inject(LPCTSTR processName)
{
    inject.FindProcessPid(processName, dwPid);
    if(dwPid==0)
    {
        QMessageBox::warning(NULL, "提示", "未找到程序");
        return;
    }

    qDebug()<<dwPid;
    inject.InjectDll(dwPid,addr_MyRecv);
    Sleep(300);
    if(inject.CheckDllInProcess(dwPid,addr_MyRecv))
    {
        QMessageBox::information(NULL, "提示", "注入成功,请在游戏中点击一次乱数分配");
        //ui->pushButton->setEnabled(false);
    }
    else
        QMessageBox::critical(NULL, "提示", "注入失败");
}

void MainWindow::openSocket()
{
    FILE *sock = fopen(socket.toStdString().c_str(),"rb");
    WSAPROTOCOL_INFO* ProtocolInfo = new WSAPROTOCOL_INFO;
    fread(ProtocolInfo, sizeof(WSAPROTOCOL_INFO), 1, sock);
    WORD wVersionRequested = MAKEWORD( 2, 1 );
    WSADATA wsaData;
    int err = WSAStartup( wVersionRequested, &wsaData );
    fd = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, ProtocolInfo, 0, 1);

    roller.setSocket(fd);
}

void MainWindow::on_pushButton_roll_clicked()
{
    roll();
}

void MainWindow::roll()
{
    if(fd==0)
    {
        openSocket();
    }

    const unsigned char request[]=  {0x10,0x6b,0xae,0xad,0xba,0xd6,0xac};
    send(fd,(const char*)request,7,0);

    ui->pushButton_autoroll->setEnabled(true);
    ui->pushButton_stop->setEnabled(true);
}

void MainWindow::on_pushButton_autoroll_clicked()
{
    constraints[0] = ui->comboBox_s->currentIndex();
    constraints[1] = ui->spinBox_s->value();
    constraints[2] = ui->comboBox_c->currentIndex();
    constraints[3] = ui->spinBox_c->value();
    constraints[4] = ui->comboBox_i->currentIndex();
    constraints[5] = ui->spinBox_i->value();
    constraints[6] = ui->comboBox_w->currentIndex();
    constraints[7] = ui->spinBox_w->value();
    constraints[8] = ui->comboBox_a->currentIndex();
    constraints[9] = ui->spinBox_a->value();

    roller.setConstraints(constraints);
    roller.start();


    ui->pushButton_roll->setEnabled(false);
    ui->pushButton_autoroll->setEnabled(false);
}

void MainWindow::on_pushButton_stop_clicked()
{
    roller.stopRolling();
    ui->pushButton_autoroll->setEnabled(true);
    ui->pushButton_roll->setEnabled(true);
}

void MainWindow::endRolling()
{
    ui->pushButton_autoroll->setEnabled(true);
    ui->pushButton_roll->setEnabled(true);
    QMessageBox::information(this,"提示","摇点成功");
}
