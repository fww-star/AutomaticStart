#include <AutomaticStart.h>
#include <QtWidgets/QApplication>
#include <QFile>
#include<QMessageBox>
#include <QComboBox>
#include <QScrollBar>
#include <Service.h>
#include<AutoStartPlan.h>
#include <GlobalVariable.h>
#include<SubmitForm.h>
#include <sstream>
#include <SwitchButton.h>
#include <ShadowMessageBox .h>
#include <qsettings.h>
#include <QtConcurrent/QtConcurrentRun>

AutomaticStart::AutomaticStart(QWidget *parent)
    : QWidget(parent)
{
    setUIStyle(QString(":/qss/flatgray.css"));
    ui.setupUi(this);
    //滑动按钮
    SwitchButton* switchButton = new SwitchButton(this);
    switchButton->setFixedWidth(80); 
    if ( getServiceJson(startName) )switchButton->mousePressEvent();
 
    ui.gridLayout->addWidget(switchButton, 3, 0, 1, 1);
    connect(switchButton, &SwitchButton::statusChanged, this, &AutomaticStart::handleSwitchStatusChanged);

    //初始化表格
    ui.leftTable->setColumnCount(1);
    // 设置列宽自适应（最后一列自动填充剩余空间）
    ui.leftTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui.leftTable->setHorizontalHeaderLabels({ "自启动计划名" });
    reSetLeftTable();

    ui.midTable->setColumnCount(midCols);
    ui.midTable->setHorizontalHeaderLabels({ "服务名","绝对路径","启动顺序","上次启动时间" });
    ui.deletePlan->setEnabled(false);
    ui.editPlan->setEnabled(false);
    //添加信号
    #pragma region 新增按钮信号
    connect(ui.addPlan, SIGNAL(clicked()), this, SLOT(AddButtonClicked()));
    connect(ui.deletePlan, SIGNAL(clicked()), this, SLOT(deleteButtonClicked()));
    connect(ui.editPlan, SIGNAL(clicked()), this, SLOT(editButtonClicked()));
    #pragma endregion

    #pragma region 表格行获取及没有行被选中
    connect(ui.leftTable, &QTableWidget::itemClicked, this, &AutomaticStart::onRowClicked);
    connect(ui.leftTable, &QTableWidget::itemSelectionChanged, this, &AutomaticStart::onRowFocusLost);
    #pragma endregion

    connect(ui.styleBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AutomaticStart::onComboBoxIndexChanged);

    threadStart();
}

AutomaticStart::~AutomaticStart()
{
    if ( sw != nullptr ) {
        delete sw;  // 释放 sw 指针
    }
}

void AutomaticStart::setColumnNotEditable(QTableWidget* table,int row_index, int column_index) {
    QTableWidgetItem* item = table->item(row_index, column_index);
    if ( item ) {
        // 移除编辑标志
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }
}

void AutomaticStart::updateLog()
{
    if ( currentPlan.length() > 0 ) {
        std::ostringstream& log = *( *logs )[currentPlan];
        ui.rightText->setText(QString::fromStdString(log.str()));
        // 滚动到最底部
        QScrollBar* scrollBar = ui.rightText->verticalScrollBar();
        if ( scrollBar ) {
            scrollBar->setValue(scrollBar->maximum());
        }
        QApplication::processEvents();
    }
}

void AutomaticStart::AddButtonClicked()
{
    if ( sw == nullptr ) {
        sw = new SubmitForm();
        sw->setWindowTitle("新增启动计划");
        sw->setAttribute(Qt::WA_DeleteOnClose);  // 设置关闭时自动销毁
        sw->show();
        // 连接 destroyed 信号到槽函数
        connect(sw, &SubmitForm::destroyed, this, &AutomaticStart::onFormDestroyed);
        // 连接信号和槽
        connect(sw, &SubmitForm::sendData, this, &AutomaticStart::receiveData);
    }
}

void AutomaticStart::editButtonClicked()
{
    if ( sw == nullptr ) {
        sw = new SubmitForm();
        sw->initialize(QString::fromStdString(currentPlan), ui.midTable);
        sw->setWindowTitle("修改启动计划");
        sw->setAttribute(Qt::WA_DeleteOnClose);  // 设置关闭时自动销毁
        sw->show();
        // 连接 destroyed 信号到槽函数
        connect(sw, &SubmitForm::destroyed, this, &AutomaticStart::onFormDestroyed);
        // 连接信号和槽
        connect(sw, &SubmitForm::sendData, this, &AutomaticStart::receiveData);
    }
}

void AutomaticStart::deleteButtonClicked()
{
    //根据当前指针删除对应计划
    for ( int i = 0; i < planList.size(); i++ ) {
        AutoStartPlan& temp = *( planList.at(i) );
        if ( currentPlan == temp.getPlanName() ) {
            temp.deletePlan();
        }
    }
    reSetLeftTable();
    showMessage(QString("删除成功!"));
    
}

void AutomaticStart::onRowClicked(QTableWidgetItem* item)
{
    ui.midTable->setRowCount(0);
    ui.deletePlan->setEnabled(true);
    ui.editPlan->setEnabled(true);
    for ( int i = 0; i < planList.size(); i++ ) {
        AutoStartPlan& temp = *( planList.at(i) );
        if ( item->text().toStdString() == temp.getPlanName() ) {
            currentPlan.clear();
            currentPlan = temp.getPlanName();
            // 复制单元格内容并设置为不可编辑
            for ( int row = 0; row < temp.getService()->size(); ++row ) {
                ui.midTable->insertRow(row);
                Service Service = temp.getService()->at(row);
                for ( int col = 0; col < midCols; ++col ) {
                    QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(Service.getValue(col)));
                    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                    ui.midTable->setItem(row, col, item);
                }
            }
            ui.midTable->resizeColumnsToContents();
            std::ostringstream& log = *( *logs )[temp.getPlanName()];
            ui.rightText->setText(QString::fromStdString(log.str()));
            // 滚动到最底部
            QScrollBar* scrollBar = ui.rightText->verticalScrollBar();
            if ( scrollBar ) {
                scrollBar->setValue(scrollBar->maximum());
            }
        }
    }
}

void AutomaticStart::onRowFocusLost()
{
    if ( ui.leftTable->selectedItems().isEmpty() ) {
        ui.deletePlan->setEnabled(false);
        ui.editPlan->setEnabled(false);
        ui.midTable->setRowCount(0);
        ui.rightText->clear();
        currentPlan.clear();
    }

}

void AutomaticStart::onFormDestroyed()
{
    sw = nullptr;  // 将 sw 指针设为空
}


void AutomaticStart::receiveData()
{
    // 清空 ui.midTable 的内容
    ui.midTable->clearContents();
    ui.midTable->setRowCount(0);
    reSetLeftTable();
    ui.rightText->clear();
}

void AutomaticStart::reSetLeftTable()
{
    //重新加载数据
    setPlanList();
    ui.leftTable->setRowCount(0);
    for ( int i = 0; i < planList.size(); i++ ) {
        AutoStartPlan& temp = *( planList.at(i) );
        int row = ui.leftTable->rowCount();
        ui.leftTable->insertRow(row);
        QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(temp.getPlanName()));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui.leftTable->setItem(row, 0, item);
    }
    
}

void AutomaticStart::showMessage(QString mes) {
    // 创建自定义消息框
    ShadowMessageBox* msgBox = new ShadowMessageBox("提示", mes);
    // 显示消息框并等待用户响应
    //msgBox.exec();
    //不阻塞主线程
    msgBox->open();
    // 当对话框关闭时，自动删除 msgBox
    QObject::connect(msgBox, &QDialog::finished, msgBox, &QObject::deleteLater);
}


void AutomaticStart::handleSwitchStatusChanged(bool checked)
{
    //记录在配置中
    setSerciveJson(startName, checked);
    //applicationStart();
    // 获取当前程序的路径
    QString appPath = QCoreApplication::applicationFilePath();
    QString appName = QCoreApplication::applicationName();
    appPath.replace("/", "\\");
    QString path = " /directory \"" + appPath + "\"";
    if ( checked ) {
        // 设置开机自启
        setAutoStart(appName, path);
    }
    else {
        // 取消开机自启
        removeAutoStart(appName);
    }
}

void AutomaticStart::onComboBoxIndexChanged(int index)
{
    if ( index == 0 ) {
        setUIStyle(QString(":/qss/flatgray.css"));
    }
    else if(index == 1)
    {
        setUIStyle(QString(":/qss/lightblue.css"));     
    }
    else
    {
        setUIStyle(QString(":/qss/blacksoft.css"));
    }
}

void AutomaticStart::setUIStyle(QString path)
{
    //加载样式表
    QString qss;
    QFile file(path);
    //QFile file(":/qss/flatgray.css");    //白色
    //QFile file(":/qss/blacksoft.css");    //黑色
    //QFile file(":/qss/lightblue.css");    //蓝色

    if ( file.open(QFile::ReadOnly) )
    {
        //用readAll读取默认支持的是ANSI格式,如果不小心用creator打开编辑过了很可能打不开
        qss = QLatin1String(file.readAll());

        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}

// 设置开机自启
void AutomaticStart::setAutoStart(const QString& appName, const QString& appPath)
{
    // 使用 QSettings 访问注册表
    QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    reg.setValue(appName, appPath);
    showMessage(QString("程序已设置为开机自动启动"));
}

// 取消开机自启
void AutomaticStart::removeAutoStart(const QString& appName)
{
    // 使用 QSettings 访问注册表
    QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    reg.remove(appName);
    showMessage(QString("程序已取消开机自动启动"));
}

void AutomaticStart::threadStart() {
    QApplication::processEvents();
    // 异步执行 applicationStart，避免阻塞主线程
    QTimer::singleShot(1000, this, [this]() {
        // 使用 QtConcurrent::run 异步执行 applicationStart
        QtConcurrent::run(applicationStart);
        });
}
