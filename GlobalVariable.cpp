#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include<fstream>
#include <locale>
#include <sstream>
#include <AutomaticStart.h>
#include <AutoStartPlan.h>
#include <Service.h>
#include <windows.h>
#include "GlobalVariable.h"
// 使用 nlohmann::json 的别名
using json = nlohmann::json;

int midCols = 4;    //中间表格列数
std::string filePath = "service.json";   //配置文件路径
extern std::string startName = "IsAutoStart";
AutomaticStart* win;
std::unique_ptr<std::unordered_map< std::string, std::unique_ptr<std::ostringstream> >>logs = std::make_unique<std::unordered_map< std::string, std::unique_ptr<std::ostringstream> >>();
std::vector<std::unique_ptr<AutoStartPlan>>planList = {};

void setPlanList() {
    // 读取现有文件内容
    nlohmann::json plansArray;
    std::ifstream fileIn(filePath);
    if ( fileIn.is_open() ) {
        fileIn >> plansArray;
        fileIn.close();
    }
    planList.clear();
    for ( auto& planJson : plansArray ) {
        if ( planJson.contains(startName) )continue;
        std::unique_ptr<AutoStartPlan> plan = std::make_unique<AutoStartPlan>();
        plan->setPlanName(planJson["planName"].get<std::string>());
        //记录日志，不存在则添加
        if ( logs->find(plan->getPlanName()) == logs->end() ) {
            std::unique_ptr<std::ostringstream>log = std::make_unique< std::ostringstream>();
            (*logs)[plan->getPlanName()] = std::move(log);
        }

        if ( planJson.contains("services") && planJson["services"].is_array() ) {
            for ( const auto& serviceJson : planJson["services"] ) {
                Service ser;
                ser.setName(serviceJson["serviceName"].get<std::string>());

                // 从 JSON 中获取路径
                std::string path = serviceJson["path"].get<std::string>();

                ser.setPath(path);
                ser.setOrder(serviceJson["order"].get<int>());
                std::string time = serviceJson["lastStartTime"].get<std::string>();
                if ( !( time.length() == 0 || time == "0" ) ) {
                    ser.setStartTime(time);
                }
                plan->addService(ser);
            }
        }
        planList.push_back(std::move(plan));
    }
}

void applicationStart() {
    setPlanList();
    for ( int i = 0; i < planList.size(); i++ ) {
        try {
            AutoStartPlan& temp = *( planList.at(i) );
            temp.start();

        }
        catch ( std::exception ex ) {

        }
    }
}

void setAppWindows(AutomaticStart* w) {
    win = w;
}

std::string UTF8_To_string(const std::string& str)
{
    int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t* pwBuf = new wchar_t[nwLen + 1];//加1用于截断字符串 
    memset(pwBuf, 0, nwLen * 2 + 2);

    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

    int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

    char* pBuf = new char[nLen + 1];
    memset(pBuf, 0, nLen + 1);

    WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string retStr = pBuf;

    delete []pBuf;
    delete []pwBuf;

    pBuf = NULL;
    pwBuf = NULL;

    return retStr;
}

std::string string_To_UTF8(const std::string& str)
{
    int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

    wchar_t* pwBuf = new wchar_t[nwLen + 1];//加1用于截断字符串 
    ZeroMemory(pwBuf, nwLen * 2 + 2);

    ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

    int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

    char* pBuf = new char[nLen + 1];
    ZeroMemory(pBuf, nLen + 1);

    ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string retStr(pBuf);

    delete []pwBuf;
    delete []pBuf;

    pwBuf = NULL;
    pBuf = NULL;

    return retStr;
}

std::string getCurrentTimeAsString() {
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 将时间点转换为 time_t（C 风格时间类型）
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    // 使用 localtime_s 安全地转换为 tm 结构体（本地时间）
    std::tm now_tm = {}; // 初始化 tm 结构体
    localtime_s(&now_tm, &now_time_t); // 使用 localtime_s 代替 localtime

    // 创建一个字符串流来格式化时间
    std::ostringstream oss;

    // 使用流操作符和 std::put_time 格式化时间
    oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");

    // 返回格式化后的字符串
    return oss.str();
}

void setSerciveJson(std::string key ,bool content) {
    // 读取现有文件内容
    json plansArray;
    std::ifstream fileIn(filePath);

    if ( fileIn.is_open() ) {
        fileIn >> plansArray;
        fileIn.close();
    }
    // 检查是否存在指定的键;键存在，修改其值;键不存在，新建键值对
    bool found = false;
    for ( auto& elem : plansArray ) {
        if ( elem.is_object() && elem.contains(startName) ) {
            elem[startName] = content;  // 修改现有值
            found = true;
            break;
        }
    }
    // 如果不存在，则添加新的
    if ( !found ) {
        json isAutoStartObj = {
            {startName, content}
        };
        plansArray.push_back(isAutoStartObj);
    }

    // 将更新后的内容写回文件
    std::ofstream fileOut(filePath);
    if ( fileOut.is_open() ) {
        fileOut << std::setw(4) << plansArray << std::endl;
        fileOut.close();
    }
}

bool getServiceJson(std::string name)
{    
    // 读取现有文件内容
    json plansArray;
    std::ifstream fileIn(filePath);

    if ( fileIn.is_open() ) {
        fileIn >> plansArray;
        fileIn.close();
    }
    for ( auto& elem : plansArray ) {
        if ( elem.is_object() && elem.contains(startName) ) {
            return elem[startName].get<bool>(); // 修改现有值
        }
    }
    return false;
}
