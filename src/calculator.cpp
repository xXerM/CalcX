#include "calculator.h"
#include "themeeditor.h"
#include <QFont>
#include <QFontDatabase>
#include <cmath>
#include <QStyleFactory>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QPainter>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
    , leftOperand(0.0)
    , rightOperand(0.0)
    , currentOperator("")
    , waitingForOperand(false)
    , justCalculated(false)
{
    loadTheme();
    setupUI();
    applyTheme();

    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    display->installEventFilter(this);
    buttonContainer->installEventFilter(this);
    qApp->installEventFilter(this);
}

void Calculator::setupUI() {
    setWindowTitle("CalcX");
    setFixedSize(360, 580);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    displayContainer = new QWidget();
    displayContainer->setFixedHeight(170);
    displayContainer->setObjectName("displayContainer");

    auto *displayLayout = new QVBoxLayout(displayContainer);
    displayLayout->setContentsMargins(20, 10, 20, 10);
    displayLayout->setSpacing(5);

    auto *headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);

    themeBtn = new QPushButton("⚙");
    themeBtn->setFixedSize(24, 24);
    themeBtn->setCursor(Qt::PointingHandCursor);
    themeBtn->setObjectName("themeBtn");
    themeBtn->setFocusPolicy(Qt::NoFocus);
    connect(themeBtn, &QPushButton::clicked, this, &Calculator::showThemeEditor);

    aboutBtn = new QPushButton("ⓘ");
    aboutBtn->setFixedSize(24, 24);
    aboutBtn->setCursor(Qt::PointingHandCursor);
    aboutBtn->setObjectName("aboutBtn");
    aboutBtn->setFocusPolicy(Qt::NoFocus);
    connect(aboutBtn, &QPushButton::clicked, this, &Calculator::showAbout);

    headerLayout->addStretch();
    headerLayout->addWidget(themeBtn);
    headerLayout->addWidget(aboutBtn);

    expressionLabel = new QLabel();
    expressionLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    expressionLabel->setObjectName("expressionLabel");
    expressionLabel->setFixedHeight(30);

    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    display->setObjectName("display");
    display->setFixedHeight(60);

    QFont displayFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    if (displayFont.styleHint() == QFont::AnyStyle)
        displayFont = QFont("monospace", 36, QFont::Bold);
    displayFont.setPixelSize(36);
    displayFont.setBold(true);
    display->setFont(displayFont);
    display->setTextMargins(0, 0, 5, 0);

    QFont exprFont = displayFont;
    exprFont.setPixelSize(16);
    exprFont.setBold(false);
    expressionLabel->setFont(exprFont);

    displayLayout->addLayout(headerLayout);
    displayLayout->addWidget(expressionLabel);
    displayLayout->addWidget(display);
    mainLayout->addWidget(displayContainer);

    buttonContainer = new QWidget();
    buttonContainer->setObjectName("buttonContainer");
    auto *buttonLayout = new QGridLayout(buttonContainer);
    buttonLayout->setSpacing(2);
    buttonLayout->setContentsMargins(10, 5, 10, 15);

    struct ButtonInfo {
        QString text;
        int row, col, rowSpan, colSpan;
        QString type;
    };

    QVector<ButtonInfo> buttons = {
        {"C",    0, 0, 1, 1, "clear"},
        {"⌫",   0, 1, 1, 1, "backspace"},
        {"%",    0, 2, 1, 1, "percent"},
        {"÷",    0, 3, 1, 1, "operator"},
        {"7",    1, 0, 1, 1, "digit"},
        {"8",    1, 1, 1, 1, "digit"},
        {"9",    1, 2, 1, 1, "digit"},
        {"×",    1, 3, 1, 1, "operator"},
        {"4",    2, 0, 1, 1, "digit"},
        {"5",    2, 1, 1, 1, "digit"},
        {"6",    2, 2, 1, 1, "digit"},
        {"−",    2, 3, 1, 1, "operator"},
        {"1",    3, 0, 1, 1, "digit"},
        {"2",    3, 1, 1, 1, "digit"},
        {"3",    3, 2, 1, 1, "digit"},
        {"+",    3, 3, 1, 1, "operator"},
        {"±",    4, 0, 1, 1, "unary"},
        {"0",    4, 1, 1, 1, "digit"},
        {".",    4, 2, 1, 1, "decimal"},
        {"=",    4, 3, 1, 1, "equal"},
    };

    for (const auto &btn : buttons) {
        auto *button = new QPushButton(btn.text);
        button->setCursor(Qt::PointingHandCursor);
        button->setMinimumSize(70, 60);
        buttonLayout->addWidget(button, btn.row, btn.col, btn.rowSpan, btn.colSpan);
        m_buttonTypes.insert(button, btn.type);

        if (btn.type == "clear")
            connect(button, &QPushButton::clicked, this, &Calculator::clearPressed);
        else if (btn.type == "backspace")
            connect(button, &QPushButton::clicked, this, &Calculator::backspacePressed);
        else if (btn.type == "unary")
            connect(button, &QPushButton::clicked, this, &Calculator::unaryOperatorPressed);
        else if (btn.type == "percent")
            connect(button, &QPushButton::clicked, this, &Calculator::percentPressed);
        else if (btn.type == "operator")
            connect(button, &QPushButton::clicked, this, &Calculator::operatorPressed);
        else if (btn.type == "equal")
            connect(button, &QPushButton::clicked, this, &Calculator::equalPressed);
        else if (btn.type == "digit")
            connect(button, &QPushButton::clicked, this, &Calculator::digitPressed);
        else if (btn.type == "decimal")
            connect(button, &QPushButton::clicked, this, &Calculator::decimalPressed);
    }

    mainLayout->addWidget(buttonContainer);
    setLayout(mainLayout);
}

void Calculator::applyTheme() {
    int radius = theme.buttonRadius;
    bool translucent = (theme.mode == ThemeSettings::Custom && theme.opacity < 1.0);

    auto btnStyle = [radius](const QColor &bg, const QColor &fg) -> QString {
        QColor hover = bg.lighter(130);
        QColor pressed = bg.darker(130);
        return QString(
            "QPushButton {"
            "  background-color: %1;"
            "  color: %2;"
            "  border: none;"
            "  border-radius: %3px;"
            "  font-size: 20px;"
            "  font-weight: bold;"
            "  padding: 10px;"
            "}"
            "QPushButton:hover { background-color: %4; }"
            "QPushButton:pressed { background-color: %5; }"
        ).arg(bg.name(), fg.name()).arg(radius).arg(hover.name(), pressed.name());
    };

    for (auto it = m_buttonTypes.constBegin(); it != m_buttonTypes.constEnd(); ++it) {
        QPushButton *btn = it.key();
        const QString &type = it.value();
        QColor bg, fg;

        if (type == "clear" || type == "backspace") {
            bg = theme.clearBg; fg = theme.clearText;
        } else if (type == "operator") {
            bg = theme.operatorBg; fg = theme.operatorText;
        } else if (type == "equal") {
            bg = theme.equalBg; fg = theme.equalText;
        } else if (type == "unary" || type == "percent") {
            bg = theme.functionBg; fg = theme.functionText;
        } else {
            bg = theme.digitBg; fg = theme.digitText;
        }

        btn->setStyleSheet(btnStyle(bg, fg));
    }

    display->setStyleSheet(QString(
        "QLineEdit {"
        "  background-color: transparent;"
        "  color: %1;"
        "  border: none;"
        "  font-size: 36px;"
        "  font-weight: bold;"
        "  selection-background-color: #2ecc71;"
        "}"
    ).arg(theme.displayText.name()));

    expressionLabel->setStyleSheet(QString(
        "color: %1; font-size: 16px;"
    ).arg(theme.expressionText.name()));

    aboutBtn->setStyleSheet(QString(
        "QPushButton { background-color: transparent; color: %1; border: none; "
        "border-radius: 12px; font-size: 14px; }"
        "QPushButton:hover { color: %2; background-color: #2c2c2e; }"
    ).arg(theme.expressionText.name(), theme.titleColor.name()));

    themeBtn->setStyleSheet(QString(
        "QPushButton { background-color: transparent; color: %1; border: none; "
        "border-radius: 12px; font-size: 14px; }"
        "QPushButton:hover { color: %2; background-color: #2c2c2e; }"
    ).arg(theme.expressionText.name(), theme.titleColor.name()));

    if (translucent) {
        setAttribute(Qt::WA_TranslucentBackground);
        setAutoFillBackground(false);
        setStyleSheet(R"(
            QWidget#displayContainer { background: transparent; border-bottom: 1px solid rgba(44,44,46,0.5); }
            QWidget#buttonContainer { background: transparent; }
        )");
        displayContainer->setStyleSheet("");
        buttonContainer->setStyleSheet("");
    } else if (theme.bgType == ThemeSettings::Image && hasBgImage) {
        setAttribute(Qt::WA_TranslucentBackground, false);
        setAutoFillBackground(false);
        setStyleSheet(R"(
            QWidget#displayContainer { background: transparent; border-bottom: 1px solid #2c2c2e; }
            QWidget#buttonContainer { background: transparent; }
        )");
        displayContainer->setStyleSheet("");
        buttonContainer->setStyleSheet("");
    } else {
        QColor bg = theme.bgColor;
        setAttribute(Qt::WA_TranslucentBackground, false);
        setAutoFillBackground(true);
        QPalette pal = palette();
        pal.setColor(QPalette::Window, bg);
        setPalette(pal);
        setStyleSheet(QString(
            "QWidget#displayContainer { background-color: %1; border-bottom: 1px solid #2c2c2e; }"
            "QWidget#buttonContainer { background-color: %2; }"
        ).arg(bg.name(), bg.name()));
        displayContainer->setStyleSheet("");
        buttonContainer->setStyleSheet("");
    }

    update();
}

void Calculator::paintEvent(QPaintEvent *event) {
    QPainter p(this);

    if (theme.bgType == ThemeSettings::Image && hasBgImage && !bgPixmap.isNull()) {
        p.drawPixmap(rect(), bgPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    } else if (theme.mode == ThemeSettings::Custom && theme.opacity < 1.0) {
        QColor bg = theme.bgColor;
        bg.setAlphaF(theme.opacity);
        p.fillRect(rect(), bg);
    }

    QWidget::paintEvent(event);
}

void Calculator::saveTheme() {
    QSettings s("CalcX", "CalcX");
    s.setValue("mode", (int)theme.mode);
    s.setValue("bgType", (int)theme.bgType);
    s.setValue("bgColor", theme.bgColor.name());
    s.setValue("bgImagePath", theme.bgImagePath);
    s.setValue("buttonRadius", theme.buttonRadius);
    s.setValue("digitBg", theme.digitBg.name());
    s.setValue("digitText", theme.digitText.name());
    s.setValue("operatorBg", theme.operatorBg.name());
    s.setValue("operatorText", theme.operatorText.name());
    s.setValue("equalBg", theme.equalBg.name());
    s.setValue("equalText", theme.equalText.name());
    s.setValue("clearBg", theme.clearBg.name());
    s.setValue("clearText", theme.clearText.name());
    s.setValue("functionBg", theme.functionBg.name());
    s.setValue("functionText", theme.functionText.name());
    s.setValue("displayText", theme.displayText.name());
    s.setValue("expressionText", theme.expressionText.name());
    s.setValue("titleColor", theme.titleColor.name());
    s.setValue("opacity", theme.opacity);
}

void Calculator::loadTheme() {
    QSettings s("CalcX", "CalcX");
    if (!s.contains("mode")) {
        theme = ThemeSettings::darkTheme();
        return;
    }

    ThemeSettings::ThemeMode mode = (ThemeSettings::ThemeMode)s.value("mode", 0).toInt();

    if (mode == ThemeSettings::System) {
        theme = ThemeSettings::systemTheme();
        return;
    }

    theme.mode = mode;
    theme.bgType = (ThemeSettings::BgType)s.value("bgType", 0).toInt();
    theme.bgColor = QColor(s.value("bgColor", "#1c1c1e").toString());
    theme.bgImagePath = s.value("bgImagePath", "").toString();
    theme.buttonRadius = s.value("buttonRadius", 10).toInt();
    theme.digitBg = QColor(s.value("digitBg", "#3a3a3c").toString());
    theme.digitText = QColor(s.value("digitText", "#ffffff").toString());
    theme.operatorBg = QColor(s.value("operatorBg", "#f39c12").toString());
    theme.operatorText = QColor(s.value("operatorText", "#ffffff").toString());
    theme.equalBg = QColor(s.value("equalBg", "#2ecc71").toString());
    theme.equalText = QColor(s.value("equalText", "#ffffff").toString());
    theme.clearBg = QColor(s.value("clearBg", "#e74c3c").toString());
    theme.clearText = QColor(s.value("clearText", "#ffffff").toString());
    theme.functionBg = QColor(s.value("functionBg", "#7f8c8d").toString());
    theme.functionText = QColor(s.value("functionText", "#ffffff").toString());
    theme.displayText = QColor(s.value("displayText", "#ffffff").toString());
    theme.expressionText = QColor(s.value("expressionText", "#8e8e93").toString());
    theme.titleColor = QColor(s.value("titleColor", "#2ecc71").toString());
    theme.opacity = s.value("opacity", 1.0).toDouble();

    if (theme.bgType == ThemeSettings::Image && !theme.bgImagePath.isEmpty()) {
        hasBgImage = bgPixmap.load(theme.bgImagePath);
    }
}

void Calculator::showThemeEditor() {
    ThemeEditor editor(theme, this);
    if (editor.exec() == QDialog::Accepted) {
        ThemeSettings newTheme = editor.settings();
        if (newTheme.mode == ThemeSettings::Dark)
            theme = ThemeSettings::darkTheme();
        else if (newTheme.mode == ThemeSettings::White)
            theme = ThemeSettings::whiteTheme();
        else if (newTheme.mode == ThemeSettings::System)
            theme = ThemeSettings::systemTheme();
        else
            theme = newTheme;

        if (theme.bgType == ThemeSettings::Image && !theme.bgImagePath.isEmpty())
            hasBgImage = bgPixmap.load(theme.bgImagePath);
        else
            hasBgImage = false;

        applyTheme();
        saveTheme();
    }
}

bool Calculator::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        keyPressEvent(static_cast<QKeyEvent*>(event));
        return true;
    }
    if (event->type() == QEvent::ApplicationPaletteChange ||
        event->type() == QEvent::ThemeChange) {
        if (theme.mode == ThemeSettings::System) {
            theme = ThemeSettings::systemTheme();
            applyTheme();
        }
    }
    return QWidget::eventFilter(obj, event);
}

void Calculator::refocus() {
    setFocus();
}

void Calculator::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_0: case Qt::Key_1: case Qt::Key_2: case Qt::Key_3: case Qt::Key_4:
    case Qt::Key_5: case Qt::Key_6: case Qt::Key_7: case Qt::Key_8: case Qt::Key_9:
        if (waitingForOperand) {
            display->setText(event->text());
            waitingForOperand = false;
        } else {
            if (display->text() == "0" && event->text() != ".")
                display->setText(event->text());
            else
                display->setText(display->text() + event->text());
        }
        justCalculated = false;
        break;

    case Qt::Key_Plus:
        handleOperator("+");
        break;
    case Qt::Key_Minus:
        handleOperator("−");
        break;
    case Qt::Key_Asterisk:
        handleOperator("×");
        break;
    case Qt::Key_Slash:
        handleOperator("÷");
        break;

    case Qt::Key_Return: case Qt::Key_Enter:
        equalPressed();
        break;

    case Qt::Key_Backspace:
        backspacePressed();
        break;

    case Qt::Key_Escape: case Qt::Key_Delete:
        clearPressed();
        break;

    case Qt::Key_Period: case Qt::Key_Comma:
        decimalPressed();
        break;

    case Qt::Key_Percent:
        percentPressed();
        break;

    default:
        QWidget::keyPressEvent(event);
        return;
    }
}

void Calculator::digitPressed() {
    auto *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    QString digit = btn->text();

    if (waitingForOperand) {
        display->setText(digit);
        waitingForOperand = false;
    } else {
        if (display->text() == "0" && digit != ".")
            display->setText(digit);
        else
            display->setText(display->text() + digit);
    }
    justCalculated = false;
    refocus();
}

void Calculator::handleOperator(const QString &op) {
    QString displayText = display->text();

    if (!waitingForOperand || justCalculated) {
        if (currentOperator.isEmpty()) {
            leftOperand = displayText.toDouble();
        } else {
            rightOperand = displayText.toDouble();
            calculate();
            leftOperand = display->text().toDouble();
        }

        if (op == "+") currentOperator = "+";
        else if (op == "−") currentOperator = "-";
        else if (op == "×") currentOperator = "*";
        else if (op == "÷") currentOperator = "/";

        expressionLabel->setText(QString::number(leftOperand, 'g', 10) + " " + op);
        waitingForOperand = true;
    }

    refocus();
}

void Calculator::operatorPressed() {
    auto *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    handleOperator(btn->text());
}

void Calculator::unaryOperatorPressed() {
    toggleSign();
    refocus();
}

void Calculator::equalPressed() {
    if (currentOperator.isEmpty()) return;

    double result = display->text().toDouble();
    rightOperand = result;

    QString expr = expressionLabel->text() + " " + QString::number(rightOperand, 'g', 10);
    calculate();

    expressionLabel->setText(expr + " =");
    currentOperator.clear();
    waitingForOperand = true;
    justCalculated = true;
    refocus();
}

void Calculator::calculate() {
    double result = 0.0;
    double left = leftOperand;
    double right = rightOperand;

    if (currentOperator == "/" && right == 0.0) {
        display->setText("Error");
        expressionLabel->setText("Division by zero");
        currentOperator.clear();
        waitingForOperand = true;
        return;
    }

    if (currentOperator == "+") result = left + right;
    else if (currentOperator == "-") result = left - right;
    else if (currentOperator == "*") result = left * right;
    else if (currentOperator == "/") result = left / right;

    if (std::isinf(result) || std::isnan(result)) {
        display->setText("Error");
        expressionLabel->setText("Invalid operation");
        currentOperator.clear();
        waitingForOperand = true;
        return;
    }

    QString resultStr = QString::number(result, 'g', 12);
    if (resultStr.contains('.') && resultStr.length() > 14) {
        resultStr = QString::number(result, 'f', 8);
        while (resultStr.endsWith('0')) resultStr.chop(1);
        if (resultStr.endsWith('.')) resultStr.chop(1);
    }

    display->setText(resultStr);
}

void Calculator::clearPressed() {
    display->setText("0");
    expressionLabel->clear();
    leftOperand = 0.0;
    rightOperand = 0.0;
    currentOperator.clear();
    waitingForOperand = false;
    justCalculated = false;
    refocus();
}

void Calculator::backspacePressed() {
    QString text = display->text();
    if (text == "0" || text == "Error") {
        refocus();
        return;
    }

    if (text.length() > 1)
        display->setText(text.chopped(1));
    else
        display->setText("0");

    waitingForOperand = false;
    justCalculated = false;
    refocus();
}

void Calculator::decimalPressed() {
    if (waitingForOperand) {
        display->setText("0.");
        waitingForOperand = false;
        refocus();
        return;
    }

    if (!display->text().contains('.'))
        display->setText(display->text() + ".");
    refocus();
}

void Calculator::percentPressed() {
    double value = display->text().toDouble();
    value /= 100.0;
    display->setText(QString::number(value, 'g', 12));
    waitingForOperand = true;
    refocus();
}

void Calculator::toggleSign() {
    QString text = display->text();
    if (text == "0") {
        refocus();
        return;
    }

    if (text.startsWith('-'))
        display->setText(text.mid(1));
    else
        display->setText("-" + text);
    refocus();
}

void Calculator::showAbout() {
    QDialog about(this);
    about.setWindowTitle("About CalcX");
    about.setFixedSize(340, 320);

    QString accent = theme.titleColor.name();
    QString bgColor = theme.bgColor.name();
    QString fgColor = theme.displayText.name();
    QString mutedColor = theme.expressionText.name();

    about.setStyleSheet(QString(R"(
        QDialog {
            background-color: %1;
            color: %2;
        }
        QLabel {
            color: %2;
            background: transparent;
        }
    )").arg(bgColor, fgColor));

    auto *layout = new QVBoxLayout(&about);
    layout->setContentsMargins(32, 28, 32, 24);
    layout->setSpacing(6);

    auto *title = new QLabel("CalcX");
    QFont tf = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    tf.setPixelSize(30);
    tf.setBold(true);
    title->setFont(tf);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QString("color: %1;").arg(accent));

    auto *version = new QLabel("v1.0.0");
    QFont vf = tf;
    vf.setPixelSize(13);
    vf.setBold(false);
    version->setFont(vf);
    version->setAlignment(Qt::AlignCenter);
    version->setStyleSheet(QString("color: %1;").arg(mutedColor));

    auto *desc = new QLabel("A modern, fully customizable\ncalculator for Linux");
    desc->setAlignment(Qt::AlignCenter);
    desc->setStyleSheet(QString("color: %1; font-size: 13px;").arg(fgColor));

    auto *sep = new QLabel("<hr style='border: 0; border-top: 1px solid " + mutedColor + "33;'>");
    sep->setTextFormat(Qt::RichText);

    auto *dev = new QLabel("Developer: xxerm");
    dev->setAlignment(Qt::AlignCenter);
    dev->setStyleSheet(QString("color: %1; font-size: 12px;").arg(mutedColor));

    auto *contact = new QLabel(QString("<a href='mailto:contact@xerm.site' style='color: %1; text-decoration: none;'>contact@xerm.site</a>").arg(accent));
    contact->setOpenExternalLinks(true);
    contact->setAlignment(Qt::AlignCenter);
    contact->setStyleSheet("font-size: 12px;");

    auto *website = new QLabel(QString("<a href='https://github.com/xXerM/CalcX' style='color: %1; text-decoration: none;'>github.com/xXerM/CalcX</a>").arg(accent));
    website->setOpenExternalLinks(true);
    website->setAlignment(Qt::AlignCenter);
    website->setStyleSheet("font-size: 12px;");

    auto *okBtn = new QPushButton("Close");
    okBtn->setFixedHeight(38);
    okBtn->setCursor(Qt::PointingHandCursor);
    okBtn->setStyleSheet(QString(R"(
        QPushButton {
            background-color: %1;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: bold;
            padding: 0 24px;
        }
        QPushButton:hover {
            background-color: %2;
        }
    )").arg(accent, QColor(accent).lighter(110).name()));
    connect(okBtn, &QPushButton::clicked, &about, &QDialog::accept);

    layout->addStretch();
    layout->addWidget(title);
    layout->addWidget(version);
    layout->addSpacing(8);
    layout->addWidget(desc);
    layout->addWidget(sep);
    layout->addSpacing(4);
    layout->addWidget(dev);
    layout->addWidget(contact);
    layout->addWidget(website);
    layout->addSpacing(12);
    layout->addWidget(okBtn, 0, Qt::AlignCenter);
    layout->addStretch();

    about.exec();
}
