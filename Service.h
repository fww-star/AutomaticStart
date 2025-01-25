#pragma once
#include <string>   // 用于 std::string 和 std::wstring
#include <ctime>    // 用于 time_t

class Service {
private:
    std::string name;    // 服务名
    std::string path;    // 服务路径
    int order;       // 启动顺序
    time_t startTime;    // 上次启动时间
    time_t endTime;      // 结束时间
public:
    Service();
    Service(std::string name, std::string path, int order);
    ~Service();

    // Setter 方法
    void setValue(std::string st,int pos);
    std::string getValue(int pos);
    void setName(std::string name) { this->name = name; }
    void setPath(std::string path) { this->path = path; }
    void setOrder(int order) { this->order = order; }
    void setStartTime(std::string startTime);
    void setEndTime(time_t endTime) { this->endTime = endTime; }
    // Getter 方法（可选，以便访问私有成员）
    std::string getName() const { return name; }
    std::string getPath() const { return path; }
    int getOrder() const { return order; }
    time_t getStartTime() const { return startTime; }
    time_t getEndTime() const { return endTime; }

    std::wstring String2Wstring(std::string wstr);
    std::string Wstring2String(std::wstring wstr);
    std::string timeToString(time_t t);
    time_t stringToTime(std::string str);
};
