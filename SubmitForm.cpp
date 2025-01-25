#include "SubmitForm.h"
#include <QComboBox>
#include <GlobalVariable.h>
#include <QMessageBox>
#include <Service.h>
#include <AutoStartPlan.h>
#include <ShadowMessageBox .h>
#include <QFileDialog>
#include <QThread>
using namespace std;


SubmitForm::SubmitForm(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    edit = false;
	//初始化表格
	ui.serviceList->setColumnCount(midCols);
	ui.serviceList->setHorizontalHeaderLabels({ "服务名","绝对路径","启动顺序","上次启动时间" });
    ui.serviceList->setColumnWidth(1, 200);
#pragma region 按钮信号
	connect(ui.addService, SIGNAL(clicked()), this, SLOT(addServiceClicked()));
	connect(ui.deleteService, SIGNAL(clicked()), this, SLOT(deleteServiceClicked()));
    connect(ui.submit, SIGNAL(clicked()), this, SLOT(submitClicked()));
#pragma endregion
}



SubmitForm::~SubmitForm()
{}


void SubmitForm::addServiceClicked() {
    QString path = getServicePath();
    int row = ui.serviceList->rowCount();
    ui.serviceList->insertRow(row);
    for ( int column = 0; column < midCols; ++column ) {
        QTableWidgetItem* item = new QTableWidgetItem();
        if ( column == 1 ) {
            item->setText(path);
        }
        if ( column == midCols - 1 ) {
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
        ui.serviceList->setItem(row, column, item);
    }
}




void SubmitForm::deleteServiceClicked() {
    QList<QTableWidgetItem*> selected = ui.serviceList->selectedItems();

    QSet<int> selectedRows;
    for ( QTableWidgetItem* item : selected ) {
        selectedRows.insert(item->row());
    }
    // 将 QSet 转换为 QList 并按降序排序
    QList<int> sortedRows = selectedRows.values();
    sort(sortedRows.begin(), sortedRows.end(), greater<int>());

    // 删除选中的行
    for ( int row : sortedRows ) {
        ui.serviceList->removeRow(row);
    }
}


void SubmitForm::submitClicked() {
    QString mes = "";
    if ( ui.textEdit->toPlainText().isEmpty() ) {
        mes.append("计划名不能为空！");
    }
    if ( !this->edit ) {
        for ( int i = 0; i < planList.size(); i++ ) {
            AutoStartPlan& temp = *( planList.at(i) );
            if ( ui.textEdit->toPlainText().toStdString() == temp.getPlanName() ) {
                mes.append("计划名不能重复！");
            }
        }
    }
    if ( ui.serviceList->rowCount() == 0 ) {
        mes.append("服务列表不能为空！");
    }
    if ( mes.length() > 0 ) {
        showMessage(mes);
        return;
    }
    //新增或修改计划
    AutoStartPlan* plan = new AutoStartPlan();
    plan->setPlanName(ui.textEdit->toPlainText().toStdString());
    //遍历表格
    int row_count = ui.serviceList->rowCount();
    int column_count = ui.serviceList->columnCount();

    for ( int row = 0; row < row_count; ++row ) {
        Service* ser = new Service();
        for ( int column = 0; column < column_count; ++column ) {
            QTableWidgetItem* item = ui.serviceList->item(row, column);
            if ( !item->text().isEmpty()) {
                string text = item->text().toStdString();
                ser->setValue(text,column);
            }
            else if( column != midCols - 1){
                QString mes = QString("第%1行，第%2列，数据不能为空!").arg(row + 1).arg(column + 1);
                showMessage(mes);
                return;
            }
        }
        plan->addService(*ser);
    }
    plan->savePlan();
    emit sendData();
    this->close();
}

void SubmitForm::showMessage(QString mes) {
    // 创建自定义消息框
    ShadowMessageBox* msgBox = new ShadowMessageBox("提示", mes);
    // 显示消息框并等待用户响应
    //msgBox.exec();
    //不阻塞主线程
    msgBox->open();
    // 当对话框关闭时，自动删除 msgBox
    QObject::connect(msgBox, &QDialog::finished, msgBox, &QObject::deleteLater);
}

void SubmitForm::initialize(QString name, QTableWidget* serviceList)
{
    ui.textEdit->setText(name);
    // 复制行数和列数
    int rowCount = serviceList->rowCount();
    int columnCount = serviceList->columnCount();
    ui.serviceList->setRowCount(rowCount);
    ui.serviceList->setColumnCount(columnCount);
    this->edit = true;

    // 复制单元格内容并设置为不可编辑
    for ( int row = 0; row < rowCount; ++row ) {
        for ( int col = 0; col < columnCount; ++col ) {
            QTableWidgetItem* item = serviceList->item(row, col);
            QTableWidgetItem* newItem = item->clone(); // 复制内容
            if ( col != columnCount - 1 ) {
                newItem->setFlags(newItem->flags() | Qt::ItemIsEditable); // 设置为可编辑
            }
            ui.serviceList->setItem(row, col, newItem);
        }
    }
}

QString SubmitForm::getServicePath()
{
    // 打开文件选择器
    QString filePath;
    try {
        //疑似qt版本与VS版本不对应导致的访问冲突问题，不影响实际功能实现
        filePath = QFileDialog::getOpenFileName(this, "选择文件", "", "可执行文件 (*.exe)");
    }
    catch ( exception ex ) {

    }
    if ( !filePath.isEmpty() ) {
        return filePath;
    }
    return QString("获取路径失败!");
}