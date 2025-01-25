#pragma once
#include <QPainterPath>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QResource>
class ShadowMessageBox : public QDialog {
    Q_OBJECT
public:
    explicit ShadowMessageBox(const QString& title, const QString& message, QWidget* parent = nullptr)
        : QDialog(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint) {
        // 设置无边框和透明背景
        setAttribute(Qt::WA_TranslucentBackground);
        // 从资源文件中加载图标
        QResource::registerResource("AutomaticStart.qrc");
        QIcon icon(":/提示.ico");
        // 设置窗口图标
        setWindowIcon(icon); // 替换为你的图标路径

        // 创建内容布局
        QGridLayout* layout = new QGridLayout(this);
        // 减小布局的垂直间距
        layout->setVerticalSpacing(0); // 将垂直间距设置为 5px
        // 添加标题
        QLabel* titleLabel = new QLabel(title);
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #333333; font-family: '楷体', 'SimKai', serif;");
        layout->addWidget(titleLabel, 0, 0, 1, 2, Qt::AlignTop | Qt::AlignHCenter);

        // 添加消息内容
        QLabel* messageLabel = new QLabel(message);
        messageLabel->setStyleSheet("font-size: 16px; color: #555555; font-family: '楷体', 'SimKai', serif;");
        messageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop); // 设置内容置顶对齐
        messageLabel->setWordWrap(true);
        layout->addWidget(messageLabel, 1, 0, 1, 2);

        // 添加按钮
        QPushButton* okButton = new QPushButton("确定");
        okButton->setStyleSheet(R"(
            QPushButton {
                background-color: #444444;
                color: #f0e8d8;
                border: 1px solid #333333;
                border-radius: 5px;
                padding: 4px 20px;
                font-size: 14px;
                font-family: '楷体', 'SimKai', serif;
            }
            QPushButton:hover {
                background-color: #555555;
            }
            QPushButton:pressed {
                background-color: #222222;
            }
        )");
        connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
        layout->addWidget(okButton, 2, 1, Qt::AlignRight);

        // 设置阴影效果
        QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
        shadow->setBlurRadius(15);       // 阴影模糊半径
        shadow->setColor(QColor(0, 0, 0, 60)); // 阴影颜色
        shadow->setOffset(0, 3);         // 阴影偏移
        setGraphicsEffect(shadow);

        // 设置布局
        setLayout(layout);
        setFixedSize(300, 150); // 固定窗口大小
    }

protected:
    // 绘制圆角背景
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        path.addRoundedRect(rect(), 10, 10); // 圆角半径
        painter.setPen(Qt::NoPen);
        painter.fillPath(path, QColor("#F0F8FF")); // 背景颜色（浅米色，模拟宣纸）
    }

    // 支持鼠标拖动窗口
    void mousePressEvent(QMouseEvent* event) override {
        if ( event->button() == Qt::LeftButton ) {
            m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if ( event->buttons() & Qt::LeftButton ) {
            move(event->globalPosition().toPoint() - m_dragPosition);
            event->accept();
        }
    }

private:
    QPoint m_dragPosition; // 用于窗口拖动的变量
};