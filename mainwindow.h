#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QPushButton;

/**
 * @brief 带键盘事件的计算器
 *
 * 鼠标按键与键盘输入统一转换为按键"标记"（如 "7"、"."、"⌫"），
 * 全部走同一个 processKey()，保证两种输入方式行为完全一致。
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onButtonClicked();                          // 所有鼠标按钮的统一入口

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
