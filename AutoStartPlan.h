#pragma once
#include<qstring.h>
#include<windows.h>
class Service;
class AutoStartPlan {
private:
    std::string planName;    //启动计划名
    std::vector<Service> *services;  // 需要启动的服务

public:
    AutoStartPlan();
    ~AutoStartPlan();
    bool isRunning(Service service);  // 判断服务是否运行中
    void isExcute(Service service);   // 服务是否启动成功
    bool savePlan();    //保存服务启动计划
    void addService(Service ser);       //增加服务
    void deletePlan();      //删除计划
    std::string getPlanName();
    void setPlanName(std::string name);
    std::vector<Service>* getService();
    void orderSort(std::vector<Service>* services);  // 将服务按order排序
    void start();       // 根据服务顺序启动服务
    void updateMainThread(); //主线程中更新界面
};

