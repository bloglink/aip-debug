#ifndef CWINDEBUG_H
#define CWINDEBUG_H

#include <QTimer>
#include <QDebug>
#include <QString>
#include <QWidget>
#include <QSettings>
#include <QStringList>
#include <QCloseEvent>
#include <QDataStream>
#include <QButtonGroup>
#include <QElapsedTimer>
#include <QTableWidgetItem>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#define CHECK 0x00
#define TOOL0 0x01
#define TOOL1 0x02
#define STOP0 0x03
#define STOP1 0x04
#define START 0x05
#define CONFG 0x06
#define RSLT0 0x10
#define RSLT1 0x11
#define RSLT2 0x12
#define LOAD 0x20
#define SAVE 0x21
#define OVER 0x80

#define Name 0
#define Volt 1
#define Parm 2
#define Real 3
#define KK   4
#define BB   5

namespace Ui {
class CWinDebug;
}

class CWinDebug : public QWidget
{
    Q_OBJECT

public:
    explicit CWinDebug(QWidget *parent = 0);
    ~CWinDebug();

private:
    Ui::CWinDebug *ui;

private slots:
    void WinInit(void);
    void KeyInit(void);
    void KeyJudge(int id);
    void DatInit(void);
    void DatSave(void);
    void ComInit(void);
    void ComQuit(void);
    void ComRead(void);
    void Display(QByteArray msg);

    void closeEvent(QCloseEvent *e);

    void TestThread(void);
    bool TestCmdCheck(void);
    void TestCmdEquip();
    void TestCmdBreak(void);
    void TestCmdConfig(void);
    void TestCmdStart(void);
    void TestCmdParam(void);
    void TestGetParam(void);
    void TestCalcParam(void);
    void TestCmdLoad(void);
    void TestCmdSave(void);
    void TestTreadInit(void);
    void TestThreadQuit(void);
    void TestClearDisplay(void);
    void TestDelay(int ms);
private:
    QTimer *timer1;
    QTimer *timer2;
    QSerialPort *com1;
    QSerialPort *com2;
    QSerialPort *com3;
    QSerialPort *com4;

    quint8 CurrentRow;
    quint8 CurrentSetp;
    QString CurrentItem;
    QStringList param;
};

#endif // CWINDEBUG_H
