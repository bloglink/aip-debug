/*******************************************************************************
 * Copyright (c) 2016,青岛艾普智能仪器有限公司
 * All rights reserved.
 *
 * version:     1.0
 * author:      link
 * date:        2016.08.23
 * brief:       手主界面
*******************************************************************************/
#include "CWinDebug.h"
#include "ui_CWinDebug.h"
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      构造函数
******************************************************************************/
CWinDebug::CWinDebug(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWinDebug)
{
    ui->setupUi(this);

    WinInit();
    KeyInit();
    DatInit();
    timer1 = new QTimer(this);
    connect(timer1,SIGNAL(timeout()),this,SLOT(ComRead()));
    timer2 = new QTimer(this);
    connect(timer2,SIGNAL(timeout()),this,SLOT(TestThread()));
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      析构函数
******************************************************************************/
CWinDebug::~CWinDebug()
{
    delete ui;
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      界面初始化
******************************************************************************/
void CWinDebug::WinInit()
{
    QStringList com;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        com << info.portName();
    }
    ui->Box1->addItems(com);
    ui->Box2->addItems(com);
    ui->Box3->addItems(com);
    ui->Box4->addItems(com);

    QFile file(":/css/BlackGoogle.css");
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    this->setStyleSheet(qss);

    ui->TabVolt->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->TabVolt->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->TabVolt->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    ui->TabVolt->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);
    ui->TabVolt->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Stretch);
    ui->TabVolt->horizontalHeader()->setSectionResizeMode(5,QHeaderView::Stretch);
    ui->TabVolt->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      按键初始化
******************************************************************************/
void CWinDebug::KeyInit()
{
    QButtonGroup *btnGroup = new QButtonGroup;
    btnGroup->addButton(ui->KeyOpen,  Qt::Key_A);
    btnGroup->addButton(ui->KeyStart, Qt::Key_B);
    btnGroup->addButton(ui->KeyCalc,  Qt::Key_C);
    btnGroup->addButton(ui->KeyLoad,  Qt::Key_D);
    btnGroup->addButton(ui->KeyClear, Qt::Key_E);
    connect(btnGroup,SIGNAL(buttonClicked(int)),this,SLOT(KeyJudge(int)));
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      按键功能
******************************************************************************/
void CWinDebug::KeyJudge(int id)
{
    switch (id) {
    case Qt::Key_A:
        if (ui->KeyOpen->text() == "打开串口")
            ComInit();
        else
            ComQuit();
        break;
    case Qt::Key_B:
        if (ui->KeyStart->text() == "开始测试")
            TestTreadInit();
        else
            TestThreadQuit();
        break;
    case Qt::Key_C:
        TestCalcParam();
        break;
    case Qt::Key_D:
        TestCmdLoad();
        TestDelay(100);
        TestCmdSave();
        break;
    case Qt::Key_E:
        TestClearDisplay();
        TestCmdLoad();
        TestDelay(100);
        TestCmdSave();
        break;
    default:
        break;
    }
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      数据显示
******************************************************************************/
void CWinDebug::DatInit()
{
    com1 = NULL;
    com2 = NULL;
    com3 = NULL;
    com4 = NULL;

    QSettings *set = new QSettings("./default.ini",QSettings::IniFormat);
    set->setIniCodec("GB18030");

    QString temp;
    ui->Box1->setCurrentIndex(set->value("/Default/COM1").toInt());
    ui->Box2->setCurrentIndex(set->value("/Default/COM2").toInt());
    ui->Box3->setCurrentIndex(set->value("/Default/COM3").toInt());
    ui->Box4->setCurrentIndex(set->value("/Default/COM4").toInt());
    ui->Box5->setCurrentIndex(set->value("/Default/BOX5").toInt());
    temp = set->value("/Default/EditACW").toString();
    if (temp.isEmpty())
        ui->EditACW->setText("33.33");
    else
        ui->EditACW->setText(temp);
    temp = set->value("/Default/EditDCW").toString();
    if (temp.isEmpty())
        ui->EditDCW->setText("33.33");
    else
        ui->EditDCW->setText(temp);
    temp = set->value("/Default/EditIMP").toString();
    if (temp.isEmpty())
        ui->EditIMP->setText("33.33");
    else
        ui->EditIMP->setText(temp);
    temp = set->value("/Default/EDIT2").toString();
    if (temp.isEmpty())
        ui->Edit2->setText("0.92");
    else
        ui->Edit2->setText(temp);

    param = (set->value("/TEST/TESTA").toString()).split(" @ ");
    if (param.isEmpty()) {
        param.append("耐压电压1(V) 500 500 0");
        param.append("耐压电压2(V) 2500 2500 1");
        param.append("耐压电压3(V) 4000 4000 1");
        param.append("绝缘电压1(V) 500 500 0");
        param.append("绝缘电压2(V) 1000 1000 1");
        param.append("匝间电压1(V) 500 500 0");
        param.append("匝间电压2(V) 2500 2500 1");
        param.append("耐压电流1(mA) 500 0.12 0");
        param.append("耐压电流2(mA) 500 1.7 1");
        param.append("耐压电流3(mA) 500 1.71 0");
        param.append("耐压电流4(mA) 300 16.6 1");
        param.append("耐压电流5(mA) 300 16.6 0");
        param.append("耐压电流6(mA) 300 23 1");
        param.append("绝缘电阻1(MΩ) 1000 1 0");
        param.append("绝缘电阻2(MΩ) 1000 10 1");
        param.append("绝缘电阻3(MΩ) 1000 10 0");
        param.append("绝缘电阻4(MΩ) 1000 100 1");
        param.append("绝缘电阻5(MΩ) 1000 100 0");
        param.append("绝缘电阻6(MΩ) 1000 1000 1");
        param.append("绝缘电阻7(MΩ) 500 500 0");
        param.append("绝缘电阻8(MΩ) 500 2500 1");
    }
    ui->TabVolt->setRowCount(param.size());

    for (int row=0; row<ui->TabVolt->rowCount(); row++) {
        for (int column=0; column<ui->TabVolt->columnCount(); column++){
            ui->TabVolt->setItem(row,column,new QTableWidgetItem);
            ui->TabVolt->item(row,column)->setTextAlignment(Qt::AlignCenter);
        }

    }
    for (int row=0; row<ui->TabVolt->rowCount(); row++) {
        QStringList temp = (param.at(row)).split(" ");
        if (temp.size()<3)
            continue;
        for (int column=0; column<3; column++){
            ui->TabVolt->item(row,column)->setText(temp.at(column));
        }
        if (!temp.at(0).contains("耐压电压3") && temp.at(3) == "1") {
            ui->TabVolt->setSpan(row-1,KK,2,1);
            ui->TabVolt->setSpan(row-1,BB,2,1);
        }
        if (temp.at(3) == "0" || temp.at(0).contains("耐压电压3")) {
            ui->TabVolt->item(row,KK)->setText("1024");
            ui->TabVolt->item(row,BB)->setText("1024");
        }
    }
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      数据保存
******************************************************************************/
void CWinDebug::DatSave()
{
    QSettings *set = new QSettings("./default.ini",QSettings::IniFormat);
    set->setValue("/Default/COM1",ui->Box1->currentIndex());
    set->setValue("/Default/COM2",ui->Box2->currentIndex());
    set->setValue("/Default/COM3",ui->Box3->currentIndex());
    set->setValue("/Default/COM4",ui->Box4->currentIndex());
    set->setValue("/Default/BOX5",ui->Box5->currentIndex());
    set->setValue("/Default/EditACW",ui->EditACW->text());
    set->setValue("/Default/EditDCW",ui->EditDCW->text());
    set->setValue("/Default/EditIMP",ui->EditIMP->text());
    set->setValue("/Default/EDIT2",ui->Edit2->text());
    for (int row=0; row<ui->TabVolt->rowCount(); row++) {
        QStringList temp = (param.at(row)).split(" ");
        temp[1] = ui->TabVolt->item(row,Volt)->text();
        temp[2] = ui->TabVolt->item(row,Parm)->text();
        param[row] = temp.join(" ");
    }
    set->setValue("/TEST/TESTA",param.join(" @ "));
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      串口初始化
******************************************************************************/
void CWinDebug::ComInit()
{
    com1 = new QSerialPort(ui->Box1->currentText(),this);
    if (com1->open(QIODevice::ReadWrite)) {
        com1->setBaudRate(9600);    //波特率
        com1->setParity(QSerialPort::NoParity);
        com1->setDataBits(QSerialPort::Data8);
        com1->setStopBits(QSerialPort::OneStop);
        com1->setFlowControl(QSerialPort::NoFlowControl);
    } else {
        Display(QString("串口%1打开失败\n").arg(ui->Box1->currentText()).toUtf8());
        ui->KeyOpen->setText("打开串口");
        return;
    }
    com2 = new QSerialPort(ui->Box2->currentText(),this);
    if (com2->open(QIODevice::ReadWrite)) {
        com2->setBaudRate(19200);    //波特率
        com2->setParity(QSerialPort::NoParity);
        com2->setDataBits(QSerialPort::Data8);
        com2->setStopBits(QSerialPort::OneStop);
        com2->setFlowControl(QSerialPort::NoFlowControl);
    } else {
        Display(QString("串口%1打开失败\n").arg(ui->Box2->currentText()).toUtf8());
        ui->KeyOpen->setText("打开串口");
        return;
    }
    com3 = new QSerialPort(ui->Box3->currentText(),this);
    if (com3->open(QIODevice::ReadWrite)) {
        com3->setBaudRate(19200);    //波特率
        com3->setParity(QSerialPort::OddParity);
        com3->setDataBits(QSerialPort::Data7);
        com3->setStopBits(QSerialPort::OneStop);
        com3->setFlowControl(QSerialPort::NoFlowControl);
        com3->setDataTerminalReady(true);
        com3->setRequestToSend(false);
    } else {
        Display(QString("串口%1打开失败\n").arg(ui->Box3->currentText()).toUtf8());
        ui->KeyOpen->setText("打开串口");
        return;
    }
    com4 = new QSerialPort(ui->Box4->currentText(),this);
    if (com4->open(QIODevice::ReadWrite)) {
        com4->setBaudRate(19200);    //波特率
        com4->setParity(QSerialPort::OddParity);
        com4->setDataBits(QSerialPort::Data7);
        com4->setStopBits(QSerialPort::OneStop);
        com4->setFlowControl(QSerialPort::NoFlowControl);
        com4->setDataTerminalReady(true);
        com4->setRequestToSend(false);
    } else {
        Display(QString("串口%1打开失败\n").arg(ui->Box4->currentText()).toUtf8());
        ui->KeyOpen->setText("打开串口");
        return;
    }
    ui->KeyOpen->setText("关闭串口");
    timer1->start(100);
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      串口退出
******************************************************************************/
void CWinDebug::ComQuit()
{
    TestThreadQuit();
    timer1->stop();
    if (com1 != NULL)
        com1->close();
    if (com2 != NULL)
        com2->close();
    if (com3 != NULL)
        com4->close();
    if (com4 != NULL)
        com4->close();
    ui->KeyOpen->setText("打开串口");
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      读取串口数据
******************************************************************************/
void CWinDebug::ComRead()
{
    if (com3 != NULL && com3->bytesAvailable()>=14) {
        QByteArray msg = com3->readAll();
        QString v = msg.mid(1,5);
        if (msg.at(6) == ';') {
            v.insert(msg.at(0)-0x30+1,".");
            if (msg.at(7) == '4')
                v.insert(0,"-");
            ui->NumVolt->display(v.toDouble());
            if (msg.at(10) == ':') {
                ui->LabUnit1->setText("DC");
            }
            if (msg.at(10) == '6') {
                ui->LabUnit1->setText("AC");
            }
        }
    }
    if (com4 != NULL && com4->bytesAvailable()>=14) {
        QByteArray msg = com4->readAll();
        QString v = msg.mid(1,5);
        if (msg.at(6) == '=') {
            ui->LabGear2->setText("uA");
            v.insert(msg.at(0)-0x30,".");
        }
        if (msg.at(6) == '?') {
            ui->LabGear2->setText("mA");
            v.insert(msg.at(0)-0x30+2,".");
        }
        if (msg.at(6) == '0') {
            ui->LabGear2->setText("A");
            v.insert(msg.at(0)-0x30+2,".");
        }
        if (msg.at(7) == '4')
            v.insert(0,"-");
        ui->NumElec->display(v.toDouble());

        if (msg.at(10) == '6') {
            ui->LabUnit2->setText("AC");
        }
        if (msg.at(10) == ':') {
            ui->LabUnit2->setText("DC");
        }
    }
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      测试信息
******************************************************************************/
void CWinDebug::Display(QByteArray msg)
{
    ui->textBrowser->clear();
    ui->textBrowser->insertPlainText(msg);
    ui->textBrowser->moveCursor(QTextCursor::End);
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      退出保存
******************************************************************************/
void CWinDebug::closeEvent(QCloseEvent *e)
{
    DatSave();
    ComQuit();
    e->accept();
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      测试流程
******************************************************************************/
void CWinDebug::TestThread()
{
    switch (CurrentSetp) {
    case CHECK:
        if (TestCmdCheck())
            CurrentSetp = TOOL0;
        break;
    case TOOL0:
        TestCmdEquip();
        CurrentSetp = STOP0;
        break;
    case STOP0:
        TestCmdBreak();
        CurrentSetp = CONFG;
        break;
    case CONFG:
        TestCmdConfig();
        CurrentSetp = START;
        break;
    case START:
        TestCmdStart();
        CurrentSetp = TOOL1;
        timer2->start(1500);
        break;
    case TOOL1:
        TestCmdEquip();
        CurrentSetp = RSLT0;
        timer2->start(3000);
        break;
    case RSLT0:
        if (CurrentItem.contains("耐压电压")) {
            ui->TabVolt->item(CurrentRow,Real)->setText(QString::number(ui->NumVolt->value()*ui->EditACW->text().toDouble()));
            CurrentSetp = STOP1;
            timer2->start(100);
        } else if (CurrentItem.contains("绝缘电压")) {
            ui->TabVolt->item(CurrentRow,Real)->setText(QString::number(ui->NumVolt->value()*ui->EditDCW->text().toDouble()));
            CurrentSetp = STOP1;
            timer2->start(100);
        } else if (CurrentItem.contains("匝间电压")) {
            ui->TabVolt->item(CurrentRow,Real)->setText(QString::number(ui->NumVolt->value()*ui->EditIMP->text().toDouble()));
            CurrentSetp = STOP1;
            timer2->start(100);
        } else if (CurrentItem.contains("耐压电流")) {
            ui->TabVolt->item(CurrentRow,Real)->setText(QString::number(ui->NumElec->value()));
            CurrentSetp = RSLT1;
            timer2->start(2000);
        } else if (CurrentItem.contains("绝缘电阻")) {
            CurrentSetp = RSLT1;
            timer2->start(2000);
        }
        break;
    case RSLT1:
        TestCmdParam();
        CurrentSetp = RSLT2;
        timer2->start(100);
        break;
    case RSLT2:
        TestGetParam();
        CurrentSetp = LOAD;
        break;
    case STOP1:
        TestCmdBreak();
        CurrentSetp = LOAD;
        break;
    case LOAD:
        if (ui->Box5->currentText() == "单步测试") {
            TestThreadQuit();
            break;
        }
        TestCalcParam();
        TestCmdLoad();
        CurrentSetp = SAVE;
        break;
    case SAVE:
        TestCmdSave();
        CurrentSetp = OVER;
        break;
    case OVER:
        CurrentSetp = 0;
        CurrentRow++;
        if (CurrentRow == ui->TabVolt->rowCount())
            TestThreadQuit();
        break;
    default:
        break;
    }
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      等待切换万用表
******************************************************************************/
bool CWinDebug::TestCmdCheck()
{
    qDebug()<<"TestWait";
    ui->textBrowser->clear();
    if (ui->Box5->currentText() == "单步测试")
        CurrentRow = ui->TabVolt->currentRow();

    CurrentItem = ui->TabVolt->item(CurrentRow,Name)->text();
    if (CurrentItem.contains("耐压电压") && ui->LabUnit1->text() != "AC") {
        Display("请将电压表打到AC档...\n");
        return false;
    } else if ((CurrentItem.contains("绝缘电压") || CurrentItem.contains("匝间电压")) && ui->LabUnit1->text() != "DC") {
        Display("请将电压表打到DC档...\n");
        return false;
    } else if (CurrentItem.contains("耐压电流") && ui->LabUnit2->text() != "AC") {
        Display("请将电流表打到AC档...\n");
        return false;
    } else if ((CurrentItem.contains("耐压电流1") || CurrentItem.contains("耐压电流2") ||CurrentItem.contains("耐压电流3")) && ui->LabGear2->text() != "uA") {
        Display("请将电流表打到uA档...\n");
        return false;
    } else if ((CurrentItem.contains("耐压电流4") || CurrentItem.contains("耐压电流5") ||CurrentItem.contains("耐压电流6")) && ui->LabGear2->text() != "mA") {
        Display("请将电流表打到mA档...\n");
        return false;
    }
    return true;
}
/******************************************************************************
 * version:    1.0
 * author:     link
 * date:       2016.08.23
 * brief:      工装切换
******************************************************************************/
void CWinDebug::TestCmdEquip()
{
    if (CurrentItem.contains("耐压电压") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000200000000027D"));
    else if (CurrentItem.contains("绝缘电压") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000200000000027D"));
    else if (CurrentItem.contains("匝间电压") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000600000000067D"));
    else if (CurrentItem.contains("耐压电流1") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000000020000027D"));
    else if (CurrentItem.contains("耐压电流2") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000002000000027D"));
    else if (CurrentItem.contains("耐压电流3") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000002200000227D"));
    else if (CurrentItem.contains("耐压电流3") && CurrentSetp==TOOL1)
        com2->write(QByteArray::fromHex("7B0C0010000002000000027D"));
    else if (CurrentItem.contains("耐压电流4") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000000010000017D"));
    else if (CurrentItem.contains("耐压电流5") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000020010000217D"));
    else if (CurrentItem.contains("耐压电流5") && CurrentSetp==TOOL1)
        com2->write(QByteArray::fromHex("7B0C0010000000010000017D"));
    else if (CurrentItem.contains("耐压电流6") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000020010000217D"));
    else if (CurrentItem.contains("绝缘电阻1") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000000100000107D"));
    else if (CurrentItem.contains("绝缘电阻2") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000000100000107D"));
    else if (CurrentItem.contains("绝缘电阻2") && CurrentSetp==TOOL1)
        com2->write(QByteArray::fromHex("7B0C0010000001000000017D"));
    else if (CurrentItem.contains("绝缘电阻3") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000001000000017D"));
    else if (CurrentItem.contains("绝缘电阻4") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000001000000017D"));
    else if (CurrentItem.contains("绝缘电阻4") && CurrentSetp==TOOL1)
        com2->write(QByteArray::fromHex("7B0C0010000000080000087D"));
    else if (CurrentItem.contains("绝缘电阻5") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000000080000087D"));
    else if (CurrentItem.contains("绝缘电阻6") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000000080000087D"));
    else if (CurrentItem.contains("绝缘电阻6") && CurrentSetp==TOOL1)
        com2->write(QByteArray::fromHex("7B0C0010000000800000807D"));
    else if (CurrentItem.contains("绝缘电阻7") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000080000000807D"));
    else if (CurrentItem.contains("绝缘电阻7") && CurrentSetp==TOOL1)
        com2->write(QByteArray::fromHex("7B0C0010000008000000087D"));
    else if (CurrentItem.contains("绝缘电阻8") && CurrentSetp==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000080000000807D"));
    else
        return ;
    Display("工装切换中...\n");
    qDebug()<<"TestTool";
}
/******************************************************************************
 * version:    1.0
 * author:     link
 * date:       2016.08.23
 * brief:      开始测试
******************************************************************************/
void CWinDebug::TestCmdStart()
{
    if (CurrentItem.contains("耐压"))
        com1->write(QByteArray::fromHex("7B070001030B7D"));
    else if (CurrentItem.contains("绝缘"))
        com1->write(QByteArray::fromHex("7B070001070F7D"));
    else if (CurrentItem.contains("匝间"))
        com1->write(QByteArray::fromHex("7B08001318083B7D"));
    Display("测试启动，等待...\n");
    qDebug()<<"TestStart";
}
/******************************************************************************
 * version:    1.0
 * author:     link
 * date:       2016.08.23
 * brief:      停止测试
******************************************************************************/
void CWinDebug::TestCmdBreak()
{
    QString temp = ui->TabVolt->item(CurrentRow,Name)->text();
    if (temp.contains("匝间"))
        com1->write(QByteArray::fromHex("7B08001338085B7D"));
    else
        com1->write(QByteArray::fromHex("7B060002087D"));
    Display("测试停止...\n");
    qDebug()<<"TestStop";
}
/******************************************************************************
 * version:    1.0
 * author:     link
 * date:       2016.08.23
 * brief:      参数配置
******************************************************************************/
void CWinDebug::TestCmdConfig()
{
    quint8 crc;
    QByteArray cmd;
    QDataStream out(&cmd,QIODevice::ReadWrite);
    quint16 v = ui->TabVolt->item(CurrentRow,1)->text().toInt();
    if (CurrentItem.contains("耐压"))
        cmd = QByteArray::fromHex("7B1B0010000003D000F401C409000032000000000000000000F27D");
    else if (CurrentItem.contains("绝缘"))
        cmd = QByteArray::fromHex("7B1B0010000007D200F40100000000320000000000000000002B7D");
    else if (CurrentItem.contains("匝间"))
        cmd = QByteArray::fromHex("7B1B00100000081500F40102000A000A000A000A0000000000677D");
    out.device()->seek(9);
    out<<quint8(v%256)<<quint8(v/256);
    out.device()->seek(cmd.size()-2);
    crc = 0;
    for (int i=1; i<cmd.size()-2; i++)
        crc += cmd.at(i);
    out<<quint8(crc);
    com1->write(cmd);
    Display("配置参数...\n");
    qDebug()<<"TestConfig:"<<cmd.toHex().toUpper();
}
/******************************************************************************
 * version:    1.0
 * author:     link
 * date:       2016.08.23
 * brief:      获取测试结果
******************************************************************************/
void CWinDebug::TestCmdParam()
{
    com1->clear();
    if (CurrentItem.contains("耐压"))
        com1->write(QByteArray::fromHex("7B070004030E7D"));
    if (CurrentItem.contains("绝缘"))
        com1->write(QByteArray::fromHex("7B07000407127D"));
    Display("获取测试结果...\n");
    qDebug()<<"TestGetResult";
}

void CWinDebug::TestGetParam()
{
    QByteArray bytes = com1->readAll();
    double Result = quint8(bytes.at(13))+quint8(bytes.at(14))*256+quint8(bytes.at(15))*256*256;
    if (CurrentItem.contains("耐压电流"))
        ui->TabVolt->item(CurrentRow,Parm)->setText(QString::number(Result/1000));
    if (CurrentItem.contains("绝缘电阻"))
        ui->TabVolt->item(CurrentRow,Real)->setText(QString::number(Result/100));
}
/******************************************************************************
 * version:    1.0
 * author:     link
 * date:       2016.08.23
 * brief:      计算调试参数
******************************************************************************/
void CWinDebug::TestCalcParam()
{
    for (int row=0; row<param.size(); row++) {
        QString isCalc = (param.at(row)).split(" ").at(3);

        if (isCalc != "1")
            continue;
        if (ui->TabVolt->item(row,Real)->text().isEmpty())
            continue;
        if (ui->TabVolt->item(row-1,Real)->text().isEmpty())
            continue;
        double s0 = ui->TabVolt->item(row-1,Parm)->text().toDouble();
        double r0 = ui->TabVolt->item(row-1,Real)->text().toDouble();
        double s1 = ui->TabVolt->item(row-0,Parm)->text().toDouble();
        double r1 = ui->TabVolt->item(row-0,Real)->text().toDouble();
        int k = 0;
        int b = 0;
        QString temp = ui->TabVolt->item(row,Name)->text();

        if (temp.contains("耐压电压") || temp.contains("绝缘电压")) {
            k = ((r1-r0)*1024/(s1-s0));
            b = ((r1-r0)*s0/(s1-s0)+1024-r0);
        } else if (temp.contains("匝间电压")) {
            r0 *= ui->Edit2->text().toDouble();
            r1 *= ui->Edit2->text().toDouble();
            k = ((s1-s0)*1024/(r1-r0));
            b = ((s1-s0)*r0/(r1-r0)+1024-s0);
        } else if (temp.contains("耐压电流2")) {
            r0 *= 1000;
            r1 *= 1000;
            s0 *= 1000;
            s1 *= 1000;
            k = ((r1-r0)*1024/(s1-s0));
            b = ((r1-r0)*s0/(s1-s0)+1024-r0);
        } else if (temp.contains("耐压电流4")) {
            r0 *= 100;
            r1 *= 100;
            s0 *= 100;
            s1 *= 100;
            k = ((r1-r0)*1024/(s1-s0));
            b = ((r1-r0)*s0/(s1-s0)+1024-r0);
        } else if (temp.contains("耐压电流6")) {
            r0 *= 100;
            r1 *= 100;
            s0 *= 100;
            s1 *= 100;
            k = ((r1-r0)*1024/(s1-s0));
            b = ((r1-r0)*s0/(s1-s0)+1024-r0);
        } else if (temp.contains("绝缘电阻2")) {
            r0 *= 100;
            r1 *= 100;
            s0 *= 100;
            s1 *= 100;
            k = ((r1-r0)*s0*s1/(s1-s0))*1024/(r0*r1);
            b = k*1000000/s0/1024+1024-1000000/r0;
        } else if (temp.contains("绝缘电阻4")) {
            r0 *= 10;
            r1 *= 10;
            s0 *= 10;
            s1 *= 10;
            k = ((r1-r0)*s0*s1/(s1-s0))*1024/(r0*r1);
            b = k*1000000/s0/1024+1024-1000000/r0;
        } else if (temp.contains("绝缘电阻6")) {
            r0 *= 1;
            r1 *= 1;
            s0 *= 1;
            s1 *= 1;
            k = ((r1-r0)*s0*s1/(s1-s0))*1024/(r0*r1);
            b = k*1000000/s0/1024+1024-1000000/r0;
        } else if (temp.contains("绝缘电阻8")) {
            r0 *= 0.1;
            r1 *= 0.1;
            s0 *= 0.1;
            s1 *= 0.1;
            k = ((r1-r0)*s0*s1/(s1-s0))*1024/(r0*r1);
            b = k*500000/s0/1024+1024-500000/r0;
        }
        if (abs(s0-r0)/s0 > 0.1)
            ui->TabVolt->item(row-1,Real)->setTextColor(QColor(Qt::red));
        else
            ui->TabVolt->item(row-1,Real)->setTextColor(QColor(Qt::white));
        if (abs(s1-r1)/s1 > 0.1)
            ui->TabVolt->item(row-0,Real)->setTextColor(QColor(Qt::red));
        else
            ui->TabVolt->item(row-0,Real)->setTextColor(QColor(Qt::white));

        if (temp.contains("耐压电压3")) {
            ui->TabVolt->item(row,KK)->setText(QString::number(k));
            ui->TabVolt->item(row,BB)->setText(QString::number(b));
        } else {
            ui->TabVolt->item(row-1,KK)->setText(QString::number(k));
            ui->TabVolt->item(row-1,BB)->setText(QString::number(b));
        }
        qDebug()<<r0<<r1<<s0<<s1<<isCalc<<temp;
    }

    Display("计算调试参数...\n");
    qDebug()<<"TestCalc";
}
/******************************************************************************
 * version:    1.0
 * author:     link
 * date:       2016.08.23
 * brief:      下发调试参数
******************************************************************************/
void CWinDebug::TestCmdLoad()
{
    if (ui->KeyOpen->text() == "打开串口") {
        Display("请打开串口\n");
        return;
    }
    quint8 crc;
    QByteArray cmd;
    QDataStream out(&cmd,QIODevice::ReadWrite);

    cmd = QByteArray::fromHex("7b6600A1000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400000000000000000000000000005C13a67d");
    out.device()->seek(1);
    out<<quint8(cmd.size());
    for (int i=0; i<ui->TabVolt->rowCount()/2; i++) {
        QString temp = ui->TabVolt->item(i*2,Name)->text();
        int k = ui->TabVolt->item(i*2,KK)->text().toInt();
        int b = ui->TabVolt->item(i*2,BB)->text().toInt();
        if (temp.contains("耐压电压1")) {
            out.device()->seek(4);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(8);
            out<<quint8(b%256)<<quint8(b/256);
            out.device()->seek(6);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(10);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("耐压电压3")) {
            out.device()->seek(6);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(10);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("绝缘电压1")) {
            out.device()->seek(12);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(18);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("耐压电流1")) {
            out.device()->seek(28);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(34);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("耐压电流3")) {
            out.device()->seek(26);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(32);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("耐压电流5")) {
            out.device()->seek(24);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(30);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("绝缘电阻1")) {
            out.device()->seek(48);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(56);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("绝缘电阻3")) {
            out.device()->seek(50);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(58);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("绝缘电阻5")) {
            out.device()->seek(52);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(60);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("绝缘电阻7")) {
            out.device()->seek(54);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(62);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("匝间电压1")) {
            out.device()->seek(80);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(82);
            out<<quint8(b%256)<<quint8(b/256);
        }
    }
    out.device()->seek(cmd.size()-2);
    crc = 0;
    for (int i=1; i<cmd.size()-2; i++)
        crc += cmd.at(i);
    out<<quint8(crc);
    com1->write(cmd);
    Display("下发调试参数...\n");
    qDebug()<<"TestLoad:"<<cmd.toHex().toUpper();
}
/******************************************************************************
 * version:    1.0
 * author:     link
 * date:       2016.08.23
 * brief:      保存调试参数
******************************************************************************/
void CWinDebug::TestCmdSave()
{
    if (ui->KeyOpen->text() == "打开串口")
        return;
    com1->write(QByteArray::fromHex("7B0700A300AA7D"));
    Display("保存调试参数...\n");
    qDebug()<<"TestSave";
}
/******************************************************************************
 * version:    1.0
 * author:     link
 * date:       2016.08.23
 * brief:      测试开始
******************************************************************************/
void CWinDebug::TestTreadInit()
{
    if (ui->KeyOpen->text() != "关闭串口") {
        Display("请打开串口");
        return;
    }
    if (ui->Box5->currentText().contains("连续测试")) {
        for (int i=0; i<ui->TabVolt->rowCount(); i++)
            ui->TabVolt->item(i,Real)->setText("");
    }
    CurrentRow = 0;
    CurrentSetp = 0;
    timer2->start(100);
    Display("测试开始\n");
    ui->KeyStart->setText("中断测试");
}
/******************************************************************************
 * version:    1.0
 * author:     link
 * date:       2016.08.23
 * brief:      测试退出
******************************************************************************/
void CWinDebug::TestThreadQuit()
{
    CurrentRow = 0;
    CurrentSetp = 0;
    timer2->stop();
    Display("测试结束\n");
    ui->KeyStart->setText("开始测试");
}
/******************************************************************************
 * version:    1.0
 * author:     link
 * date:       2016.08.23
 * brief:      参数清零
******************************************************************************/
void CWinDebug::TestClearDisplay()
{
    for (int row=0; row<ui->TabVolt->rowCount(); row++) {
        QStringList temp = (param.at(row)).split(" ");
        for (int column=0; column<3; column++){
            ui->TabVolt->item(row,column)->setText(temp.at(column));
        }
        if (!temp.at(0).contains("耐压电压3") && temp.at(3) == "1") {
            ui->TabVolt->setSpan(row-1,KK,2,1);
            ui->TabVolt->setSpan(row-1,BB,2,1);
        }
        if (temp.at(3) == "0" || temp.at(0).contains("耐压电压3")) {
            ui->TabVolt->item(row,KK)->setText("1024");
            ui->TabVolt->item(row,BB)->setText("1024");
        }
    }
}
/******************************************************************************
 * version:    1.0
 * author:     link
 * date:       2016.08.23
 * brief:      延时
******************************************************************************/
void CWinDebug::TestDelay(int ms)
{
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<ms)
        QCoreApplication::processEvents();
}

