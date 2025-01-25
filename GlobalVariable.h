#pragma once
#include <string>

//ǰ������
class AutoStartPlan;
class AutomaticStart;

extern  int midCols ;    //�м�������
extern  std::string filePath;   //�����ļ�·��
extern std::string startName;	//�Զ���������

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