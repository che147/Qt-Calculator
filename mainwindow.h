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
 * 状态模型（核心）：
 *  - m_firstOperand    第一个操作数（等待运算的左操作数）
 *  - m_pendingOperator 待执行的操作符（"" 表示当前没有待运算的操作）
 *  - m_currentInput    当前正在输入的操作数（第二个操作数或刚输入的数字）
 *  - m_justEvaluated   刚按下"="的标志，用于决定后续输入是"继续运算"还是"开始新运算"
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
    // —— 统一的处理流程（鼠标 / 键盘共用） ——
    void processKey(const QString &key);
    void inputDigit(const QString &digit);           // 数字 0~9
    void inputDecimalPoint();                        // 小数点
    void inputOperator(const QString &op);           // + − × ÷
    void calculate();                                // =
    void backspace();                                // 退格
    void clearAll();                                 // 清除（复位全部状态）
    void toggleSign();                               // 正负号 ±
    void applyPercent();                             // 百分号 %

    // —— 辅助 ——
    QString formatNumber(double value) const;        // 结果格式化（去掉多余的 0）
    void updateDisplay();                            // 根据状态刷新显示
    double currentValue() const;                     // 当前屏幕上代表的数值

    Ui::MainWindow *ui;

    double  m_firstOperand     = 0.0;
    QString m_pendingOperator;                       // "" / "+" / "-" / "×" / "÷"
    QString m_currentInput;                          // 正在输入的数字串（含小数点）
    bool    m_justEvaluated  = false;
};

#endif // MAINWINDOW_H
