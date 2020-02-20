#ifndef AUTOROLL_H
#define AUTOROLL_H

#include <QObject>
#include <QThread>
#include <WinSock2.h>
#include <QMessageBox>

class AutoRoll : public QThread
{
    Q_OBJECT
public:
    AutoRoll();
    void setData(QVector<int> rollData);
    void setSocket(SOCKET s);
    void setConstraints(QVector<int> cs);
    void stopRolling();
    bool judge(int cmp, int num,int target);
    bool judgeAll();
    void run() override;

signals:
    void ended();

private:
    QVector<int> rollData = QVector<int>(5);
    SOCKET sock;
    QVector<int> constraints = QVector<int>(10);
    bool shouldStop = false;

};

#endif // AUTOROLL_H
