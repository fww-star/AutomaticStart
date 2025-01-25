#include <string>
#include <ctime>
#include <nlohmann/json.hpp>
#include <sstream>
#include <Service.h>
#include <Windows.h>

// 使用 nlohmann::json 的别名
using json = nlohmann::json;

Service::Service()
{
    this->startTime = 0;
    this->endTime = 0;
    this->order = -1;
}


Service::Service(std::string name, std::string path, int order)
    : name(name), path(path), order(order), startTime(0), endTime(0) {
}


Service::~Service() {
    // 添加资源清理代码，如果需要
}


void Service::setValue(std::string st, int pos)
{
    switch ( pos ) {
    case 0:
        this->name = st;
        break;
    case 1:
        this->path = st;
        break;
    case 2:
        this->order = std::stoi(st);
        break;
    default:
        // 当 expression 不匹配任何 case 时执行的代码
        break;
    }
}



std::string Service::getValue(int pos)
{
    switch ( pos ) {
    case 0:
        return this->name;
        break;
    case 1:
        return this->path;
        break;
    case 2:
        return std::to_string(this->order);
        break;
    case 3:
        if ( this->startTime != 0 ) {
            return timeToString(this->startTime);
        }
        return "";
    default:
        // 当 expression 不匹配任何 case 时执行的代码
        break;
    }
}


void Service::setStartTime(std::string startTime)
{
    this->startTime = stringToTime(startTime);
}


std::wstring Service::String2Wstring(std::string wstr)
{
    std::wstring res;
    int len = MultiByteToWideChar(CP_ACP, 0, wstr.c_str(), wstr.size(), nullptr, 0);
    if ( len < 0 ) {
        return res;
    }
    wchar_t* buffer = new wchar_t[len + 1];
    if ( buffer == nullptr ) {
        return res;
    }
    MultiByteToWideChar(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len);
    buffer[len] = '\0';
    res.append(buffer);
    delete [] buffer;
    return res;
}


std::string Service::Wstring2String(std::wstring wstr)
{
    // support chinese
    std::string res;
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), nullptr, 0, nullptr, nullptr);
    if ( len <= 0 ) {
        return res;
    }
    char* buffer = new char[len + 1];
    if ( buffer == nullptr ) {
        return res;
    }
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, nullptr, nullptr);
    buffer[len] = '\0';
    res.append(buffer);
    delete [] buffer;
    return res;
}


std::string Service::timeToString(time_t t) {
    // 定义 tm 结构体
    struct tm tm_info;
    localtime_s(&tm_info, &t);
    // 使用 std::put_time 格式化时间
    std::stringstream ss;
    ss << std::put_time(&tm_info, "%F %T"); // %F 表示日期格式 YYYY-MM-DD，%T 表示时间格式 HH:MM:SS
    return ss.str();
}


time_t Service::stringToTime(std::string str) {
    char* cha = (char*) str.data();             // 将string转换成char*。
    tm tm_;                                    // 定义tm结构体。
    int year, month, day, hour, minute, second;// 定义时间的各个int临时变量。
    sscanf_s(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);// 将string存储的日期时间，转换为int临时变量。
    tm_.tm_year = year - 1900;                 // 年，由于tm结构体存储的是从1900年开始的时间，所以tm_year为int临时变量减去1900。
    tm_.tm_mon = month - 1;                    // 月，由于tm结构体的月份存储范围为0-11，所以tm_mon为int临时变量减去1。
    tm_.tm_mday = day;                         // 日。
    tm_.tm_hour = hour;                        // 时。
    tm_.tm_min = minute;                       // 分。
    tm_.tm_sec = second;                       // 秒。
    tm_.tm_isdst = 0;                          // 非夏令时。
    time_t t_ = mktime(&tm_);                  // 将tm结构体转换成time_t格式。
    return t_;
}