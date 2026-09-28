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

    updateDisplay();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ==================== 鼠标入口 ====================

void MainWindow::onButtonClicked()
{
    auto *btn = qobject_cast<QPushButton *>(sender());
    if (!btn)
        return;
    processKey(btn->text());
}

// ==================== 统一处理逻辑 ====================

void MainWindow::processKey(const QString &key)
{
    // 统一减号：UI 按钮用 "−"(U+2212)，键盘用 "-"(ASCII)，合并成同一种标记
    const QString normalized = (key == QStringLiteral("−")) ? QStringLiteral("-") : key;

    if (normalized == "C")  { clearAll(); return; }

    if      (normalized == "+") inputOperator("+");
    else if (normalized == "-") inputOperator("-");
    else if (normalized == "×") inputOperator("×");
    else if (normalized == "÷") inputOperator("÷");
    else if (normalized == "=") calculate();
    else if (normalized == ".") inputDecimalPoint();
    else if (normalized == "⌫") backspace();
    else if (normalized == "±") toggleSign();
    else if (normalized == "%") applyPercent();
    else                        inputDigit(normalized);   // 其余按数字处理
}

void MainWindow::inputDigit(const QString &digit)
{
    // 刚按过"="后再输入数字：丢弃旧结果，开始一次全新的计算
    if (m_justEvaluated) {
        m_currentInput.clear();
        m_firstOperand = 0.0;
        m_pendingOperator.clear();
        m_justEvaluated = false;
    }

    // 避免出现 "007" 这类前导零：当前是 "0" 时，新数字直接替换
    if (m_currentInput == "0")
        m_currentInput.clear();

    // 限制长度，防止数字过大溢出显示
    if (m_currentInput.size() >= 15)
        return;

    m_currentInput += digit;
    updateDisplay();
}

void MainWindow::inputDecimalPoint()
{
    // 刚按过"="后输入小数点：同样开始新一轮输入
    if (m_justEvaluated) {
        m_currentInput.clear();
        m_firstOperand = 0.0;
        m_pendingOperator.clear();
        m_justEvaluated = false;
    }

    // 关键防错：同一个操作数内不允许出现第二个小数点
    if (m_currentInput.contains('.'))
        return;

    if (m_currentInput.isEmpty())
        m_currentInput = "0";       // 直接按"."时补全为 "0."

    m_currentInput += '.';
    updateDisplay();
}

void MainWindow::inputOperator(const QString &op)
{
    // 情况 1：连续按操作符（如 5 + × ），用新操作符替换旧操作符
    if (!m_pendingOperator.isEmpty() && m_currentInput.isEmpty()) {
        m_pendingOperator = op;
        updateDisplay();
        return;
    }

    // 情况 2：已有待运算的操作符且第二个操作数已输入，先算出中间结果（连续运算）
    if (!m_pendingOperator.isEmpty() && !m_currentInput.isEmpty()) {
        double second = currentValue();
        double a = m_firstOperand;
        if      (m_pendingOperator == "+") m_firstOperand = a + second;
        else if (m_pendingOperator == "-") m_firstOperand = a - second;
        else if (m_pendingOperator == "×") m_firstOperand = a * second;
        else if (m_pendingOperator == "÷") m_firstOperand = a / second;
    } else {
        // 情况 3：首次输入操作符，把当前显示值作为第一个操作数
        m_firstOperand = currentValue();
    }

    m_pendingOperator = op;
    m_currentInput.clear();
    m_justEvaluated = false;
    updateDisplay();
}

void MainWindow::calculate()
{
    // 没有完整的 "左操作数 + 操作符 + 右操作数" 时，"="不做事
    if (m_pendingOperator.isEmpty() || m_currentInput.isEmpty())
        return;

    double second = currentValue();
    double a = m_firstOperand;
    double result = 0.0;
    if      (m_pendingOperator == "+") result = a + second;
    else if (m_pendingOperator == "-") result = a - second;
    else if (m_pendingOperator == "×") result = a * second;
    else if (m_pendingOperator == "÷") result = a / second;

    // 顶部小字显示完整算式，主屏显示结果
    ui->labelExpression->setText(formatNumber(a) + " " + m_pendingOperator
                                 + " " + formatNumber(second) + " =");

    m_firstOperand    = 0.0;            // 结果保存到 m_currentInput，状态清空
    m_pendingOperator.clear();
    m_currentInput    = formatNumber(result);
    m_justEvaluated   = true;
    updateDisplay();
}

void MainWindow::backspace()
{
    // 刚出结果后退格无效（避免改动一个"已经算完"的数）
    if (m_justEvaluated || m_currentInput.isEmpty())
        return;

    m_currentInput.chop(1);
    if (m_currentInput == "-" || m_currentInput == ".")   // 只剩符号/点时归零
        m_currentInput.clear();
    updateDisplay();
}

void MainWindow::clearAll()
{
    m_firstOperand    = 0.0;
    m_pendingOperator.clear();
    m_currentInput.clear();
    m_justEvaluated   = false;
    ui->labelExpression->clear();
    updateDisplay();
}

void MainWindow::toggleSign()
{
    if (m_currentInput.isEmpty() || m_currentInput == "0")
        return;

    if (m_currentInput.startsWith('-'))
        m_currentInput.remove(0, 1);
    else
        m_currentInput.prepend('-');
    updateDisplay();
}

void MainWindow::applyPercent()
{
    if (m_currentInput.isEmpty())
        return;

    bool ok = false;
    double v = m_currentInput.toDouble(&ok);
    if (!ok)
        return;
    m_currentInput = formatNumber(v / 100.0);
    updateDisplay();
}

// ==================== 辅助函数 ====================

QString MainWindow::formatNumber(double value) const
{
    // 'g' 格式自动在普通/科学计数法之间切换，最多保留 12 位有效数字
    return QString::number(value, 'g', 12);
}

double MainWindow::currentValue() const
{
    if (!m_currentInput.isEmpty())
        return m_currentInput.toDouble();
    return m_firstOperand;
}

void MainWindow::updateDisplay()
{
    // 显示优先级：正在输入的数字 > 第一个操作数 > 0
    QString text;
    if (!m_currentInput.isEmpty())
        text = m_currentInput;
    else if (m_justEvaluated)
        text = m_currentInput;                 // "="后结果保存在 m_currentInput
    else if (m_pendingOperator.isEmpty())
        text = formatNumber(m_firstOperand);
    else
        text = formatNumber(m_firstOperand);   // 输入操作符后仍显示左操作数

    if (text.isEmpty())
        text = "0";
    ui->lineEditDisplay->setText(text);

    // 顶部小字：显示"左操作数 + 操作符"的等待状态
    if (!m_pendingOperator.isEmpty())
        ui->labelExpression->setText(formatNumber(m_firstOperand) + " " + m_pendingOperator);
}
