#pragma once
#include <string>

//前向声明
class AutoStartPlan;
class AutomaticStart;

extern  int midCols ;    //中间表格列数
extern  std::string filePath;   //配置文件路径
extern std::string startName;	//自动启动名称

extern std::vector<std::unique_ptr<AutoStartPlan>>planList;
extern std::unique_ptr<std::unordered_map< std::string, std::unique_ptr<std::ostringstream>>>logs;
extern void setPlanList();

extern void applicationStart();
extern std::string UTF8_To_string(const std::string& str);
extern std::string string_To_UTF8(const std::string& str);
extern std::string getCurrentTimeAsString();
extern void setSerciveJson(std::string name, bool content);
extern bool getServiceJson(std::string name);
extern AutomaticStart* win;
extern void setAppWindows(AutomaticStart* w);