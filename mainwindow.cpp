#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("计算器"));

    // 统一连接所有按钮：把按钮文字当作"按键标记"交给 processKey 处理，
    // 这样鼠标点击与键盘输入最终走的是同一套逻辑。
    const auto buttons = findChildren<QPushButton *>();
    for (QPushButton *btn : buttons) {
        btn->setFocusPolicy(Qt::NoFocus);   // 防止焦点停留在按钮上干扰键盘事件
        connect(btn, &QPushButton::clicked,
                this, &MainWindow::onButtonClicked);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onButtonClicked()
{
    auto *btn = qobject_cast<QPushButton *>(sender());
    if (!btn)
        return;
    // TODO: 下一步实现统一处理逻辑 processKey()
    ui->lineEditDisplay->setText(btn->text());
}
