#include <windows.h>
#include <iostream>
#include <vector>
#include <Service.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include<AutoStartPlan.h>
#include <GlobalVariable.h>
#include<AutomaticStart.h>
#include <qprocess.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <QThread>
#include <QFileInfo>

// 使用 nlohmann::json 的别名
using json = nlohmann::json;

AutoStartPlan::AutoStartPlan() {
    this->services = new std::vector<Service>();
}

// 析构函数

AutoStartPlan::~AutoStartPlan() {}


// 判断服务是否运行中
bool AutoStartPlan::isRunning(Service service) {
    QString stPath = QString::fromStdString(service.getPath());
    // 使用 QFileInfo 获取文件名
    QFileInfo fileInfo(stPath);
    QString fileName = fileInfo.fileName();
    QProcess process;
    process.start("tasklist", QStringList() << "/FI" << ( "imagename eq " + fileName ));
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    return output.contains(fileName, Qt::CaseInsensitive);
}

// 服务启动
void AutoStartPlan::isExcute(Service& service) {
    std::ostringstream& log = *( *logs )[this->planName];
    std::string path = service.getPath();
    if ( isRunning(service) ) {
        log << "【" << getCurrentTimeAsString() << "】" << "服务" << service.getName() << "已启动" << std::endl;
        log << "**********************************" << std::endl;
        return;
    }
    updateMainThread();
    log << "【" << getCurrentTimeAsString() << "】" << "服务" << service.getName() << "启动中" << std::endl;
    bool success = QProcess::startDetached(QString::fromStdString(path));
    if ( !success ) {
        log << "【" << getCurrentTimeAsString() << "】" << "服务" << service.getName() << "启动失败 " << std::endl;
    }
    service.setStartTime(getCurrentTimeAsString());
    int times = 10;
    int i = 0;
    while ( true ) {
        Sleep(1000);
        if ( isRunning(service) ) {
            log << "【" << getCurrentTimeAsString() << "】" << "服务" << service.getName() << "启动成功" << std::endl;
            log << "**********************************" << std::endl;
            log << "程序等待10秒..." << std::endl;
            updateMainThread();
            //启动成功后等待10秒
            Sleep(10000);
            break;
        }
        i++;
        if ( i > times ) {
            log << "【" << getCurrentTimeAsString() << "】" << "服务" << service.getName() << "启动失败" << std::endl;
            break;
        }
    }
    log << "**********************************" << std::endl;
}

// 将服务按order排序
void AutoStartPlan::orderSort(std::vector<Service>* services) {
    std::sort(services->begin(), services->end(), [](const Service& a, const Service& b) {
        return a.getOrder() < b.getOrder();
        });
}

// 启动服务
void AutoStartPlan::start() {
    int count = 4;
    orderSort(this->services);
    //std::ostringstream& log = *( *logs )[this->planName];
    for (Service& serv : *this->services ) {
        isExcute(serv);
        updateMainThread();
    }
}

void AutoStartPlan::updateMainThread()
{
    QMetaObject::invokeMethod(win, [=]() {
        win->updateLog();
        // 这里是你需要在主线程中执行的代码
        });
}


bool AutoStartPlan::savePlan()
{
    // 读取现有文件内容
    nlohmann::json plansArray;
    std::ifstream fileIn(filePath);
    if ( fileIn.is_open() ) {
        fileIn >> plansArray;
        fileIn.close();
    }

    // 检查是否存在相同的 planName
    bool planExists = false;
    for ( auto& plan : plansArray ) {
        if ( plan.contains("planName") && plan["planName"] == this->planName ) {
            // 更新现有计划
            plan["planName"] = this->planName;
            nlohmann::json servicesJson = nlohmann::json::array();
            for ( int i = 0; i < this->services->size(); i++ ) {
                Service service = this->services->at(i);
                servicesJson.push_back({
                    {"serviceName", service.getName()},
                    {"path", service.getPath()},
                    {"order", service.getOrder()},
                    {"lastStartTime", ""}
                    });
            }
            plan["services"] = servicesJson;
            planExists = true;
            break;
        }
    }

    if ( !planExists ) {
        // 添加新的计划
        nlohmann::json newPlan = {
            {"planName", this->planName},
            {"services", nlohmann::json::array()}
        };
        for ( int i = 0; i < this->services->size(); i++ ) {
            Service service = this->services->at(i);
            newPlan["services"].push_back({
                    {"serviceName", service.getName()},
                    {"path", service.getPath()},
                    {"order", service.getOrder()},
                    {"lastStartTime", ""}
                });
        }
        plansArray.push_back(newPlan);
    }

    // 将 JSON 数组写回文件
    std::ofstream fileOut(filePath);
    //fileOut << plansArray.dump(4) << std::endl;  // 使用 dump(4) 格式化输出
    if ( fileOut.is_open() ) {
        fileOut << plansArray.dump(4); // 将 JSON 数据格式化（缩进为 4 个空格）
    }
    else {
        std::cerr << "无法打开文件！" << std::endl;
    }
    fileOut.close();
    return true;
}


void AutoStartPlan::addService(Service ser)
{
    this->services->push_back(ser);
}

void AutoStartPlan::deletePlan()
{
    // 读取现有文件内容
    json plansArray;
    std::ifstream fileIn(filePath);
    if ( fileIn.is_open() ) {
        fileIn >> plansArray;
        fileIn.close();
    }

    // 检查是否存在相同的 planName 并删除
    auto it = plansArray.begin();
    while ( it != plansArray.end() ) {
        if ( it->contains("planName") && it->at("planName") == this->planName ) {
            it = plansArray.erase(it);
        }
        else {
            ++it;
        }
    }

    // 将修改后的内容写回文件
    std::ofstream fileOut(filePath);
    if ( fileOut.is_open() ) {
        fileOut << plansArray.dump(4); // 4 表示缩进4个空格，使 JSON 更易读
        fileOut.close();
    }
}


std::string AutoStartPlan::getPlanName()
{
    return this->planName;
}

void AutoStartPlan::setPlanName(std::string name)
{
    this->planName = name;
}


std::vector<Service>* AutoStartPlan::getService()
{
    return this->services;
}