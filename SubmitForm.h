#pragma once
#include <QtWidgets/QWidget>
#include "ui_SubmitForm.h"


class SubmitForm : public QWidget
{
	Q_OBJECT

public:
	SubmitForm(QWidget *parent = nullptr);
	~SubmitForm();
	void showMessage(QString mes);
	void initialize(QString name, QTableWidget* serviceList);
private:
	Ui::SubmitFormClass ui;
	bool edit;
	QString getServicePath();

signals:
	void sendData(); // 定义一个信号

private slots:
	void addServiceClicked();
	void deleteServiceClicked();
public slots:
	void submitClicked();
};
