#pragma once
#include <QtWidgets/QWidget>
#include "ui_AutomaticStart.h"
class SubmitForm;
class AutomaticStart : public QWidget
{
    Q_OBJECT

public:
    AutomaticStart(QWidget *parent = nullptr);
    ~AutomaticStart();
    void updateLog();

private:
    Ui::AutomaticStartClass ui;
    SubmitForm *sw; //表单
    std::string currentPlan;
    void setColumnNotEditable(QTableWidget* table, int row_index, int column_index);   //设置某行某列不可编辑
    void setAutoStart(const QString& appName, const QString& appPath);
    void removeAutoStart(const QString& appName);

    void threadStart();

private slots:
    void AddButtonClicked();
    void editButtonClicked();
    void deleteButtonClicked();
    void onRowClicked(QTableWidgetItem* item);
    void onRowFocusLost();
    void onFormDestroyed();
    void receiveData();  //接收表单提交的数据
    void reSetLeftTable();
    void showMessage(QString mes);
    void handleSwitchStatusChanged(bool checked);
    void onComboBoxIndexChanged(int index);
    void setUIStyle(QString file);
};
