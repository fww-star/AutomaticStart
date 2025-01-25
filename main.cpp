#include "AutomaticStart.h"
#include <QtWidgets/QApplication>
#include <nlohmann/json.hpp>
#include <GlobalVariable.h>
#include <Windows.h>
#include <QTimer>
using namespace std;


int main(int argc, char *argv[])
{
    //// 设置控制台输出编码为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    QApplication a(argc, argv);
    AutomaticStart* windows = new AutomaticStart();
    setAppWindows(windows);
    windows->setWindowTitle("服务自启动程序");
    windows->show();
    //applicationStart();
    return a.exec();
}
