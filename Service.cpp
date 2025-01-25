#include <string>
#include <ctime>
#include <nlohmann/json.hpp>
#include <sstream>
#include <Service.h>
#include <Windows.h>

// ʹ�� nlohmann::json �ı���
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
    // �����Դ������룬�����Ҫ
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
        // �� expression ��ƥ���κ� case ʱִ�еĴ���
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
        // �� expression ��ƥ���κ� case ʱִ�еĴ���
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
    // ���� tm �ṹ��
    struct tm tm_info;
    localtime_s(&tm_info, &t);
    // ʹ�� std::put_time ��ʽ��ʱ��
    std::stringstream ss;
    ss << std::put_time(&tm_info, "%F %T"); // %F ��ʾ���ڸ�ʽ YYYY-MM-DD��%T ��ʾʱ���ʽ HH:MM:SS
    return ss.str();
}


time_t Service::stringToTime(std::string str) {
    char* cha = (char*) str.data();             // ��stringת����char*��
    tm tm_;                                    // ����tm�ṹ�塣
    int year, month, day, hour, minute, second;// ����ʱ��ĸ���int��ʱ������
    sscanf_s(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);// ��string�洢������ʱ�䣬ת��Ϊint��ʱ������
    tm_.tm_year = year - 1900;                 // �꣬����tm�ṹ��洢���Ǵ�1900�꿪ʼ��ʱ�䣬����tm_yearΪint��ʱ������ȥ1900��
    tm_.tm_mon = month - 1;                    // �£�����tm�ṹ����·ݴ洢��ΧΪ0-11������tm_monΪint��ʱ������ȥ1��
    tm_.tm_mday = day;                         // �ա�
    tm_.tm_hour = hour;                        // ʱ��
    tm_.tm_min = minute;                       // �֡�
    tm_.tm_sec = second;                       // �롣
    tm_.tm_isdst = 0;                          // ������ʱ��
    time_t t_ = mktime(&tm_);                  // ��tm�ṹ��ת����time_t��ʽ��
    return t_;
}