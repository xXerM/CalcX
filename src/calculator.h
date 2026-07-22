#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QMap>
#include <QDialog>
#include <QDialogButtonBox>
#include <QSettings>
#include <QPixmap>
#include <QPainter>
#include "themesettings.h"

class Calculator : public QWidget {
    Q_OBJECT

public:
    explicit Calculator(QWidget *parent = nullptr);
    ~Calculator() = default;

private slots:
    void digitPressed();
    void operatorPressed();
    void unaryOperatorPressed();
    void equalPressed();
    void clearPressed();
    void backspacePressed();
    void decimalPressed();
    void percentPressed();
    void showAbout();
    void showThemeEditor();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void refocus();
    void setupUI();
    void applyTheme();
    void saveTheme();
    void loadTheme();
    void calculate();
    void toggleSign();
    void handleOperator(const QString &op);

    QLineEdit *display;
    QLabel *expressionLabel;
    QWidget *displayContainer;
    QWidget *buttonContainer;
    QPushButton *aboutBtn;
    QPushButton *themeBtn;
    QPixmap bgPixmap;
    bool hasBgImage = false;
    QMap<QPushButton*, QString> m_buttonTypes;

    ThemeSettings theme;

    double leftOperand;
    double rightOperand;
    QString currentOperator;
    bool waitingForOperand;
    bool justCalculated;
};

#endif
