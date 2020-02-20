#include "autoroll.h"

AutoRoll::AutoRoll()
{

}

void AutoRoll::setData(QVector<int> rollData)
{
    for(int i = 0;i<5;i++)
        this->rollData[i] = rollData[i];
}

void AutoRoll::setSocket(SOCKET s)
{
    sock = s;
}

void AutoRoll::setConstraints(QVector<int> cs)
{
    for(int i = 0;i<10;i++)
        this->constraints[i] = cs[i];
}

void AutoRoll::stopRolling()
{
    shouldStop = true;
}

bool AutoRoll::judge(int cmp, int num, int target)
{
    if(cmp == 0)
        return true;
    else if(cmp == 1)
        return (num>=target);
    else
        return (num<=target);
}

bool AutoRoll::judgeAll()
{
    for(int i =0;i<5;i++)
    {
        if(!judge(constraints[2*i],rollData[i],constraints[2*i+1]))
            return false;
    }
    return true;
}

void AutoRoll::run()
{
    shouldStop = false;
    while(!shouldStop)
    {
        const unsigned char request[]=  {0x10,0x6b,0xae,0xad,0xba,0xd6,0xac};
        send(sock,(const char*)request,7,0);
        msleep(200);
        if(judgeAll())
        {
            shouldStop = true;
            emit ended();
        }
    }
}
