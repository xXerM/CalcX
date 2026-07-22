#ifndef THEMESETTINGS_H
#define THEMESETTINGS_H

#include <QColor>
#include <QString>

struct ThemeSettings {
    enum ThemeMode { Dark, White, System, Custom };
    ThemeMode mode = Dark;

    enum BgType { SolidColor, Image };
    BgType bgType = SolidColor;
    QColor bgColor{"#1c1c1e"};
    QString bgImagePath;

    int buttonRadius = 10;
    double opacity = 1.0;

    QColor digitBg{"#3a3a3c"};
    QColor digitText{"#ffffff"};
    QColor operatorBg{"#f39c12"};
    QColor operatorText{"#ffffff"};
    QColor equalBg{"#2ecc71"};
    QColor equalText{"#ffffff"};
    QColor clearBg{"#e74c3c"};
    QColor clearText{"#ffffff"};
    QColor functionBg{"#7f8c8d"};
    QColor functionText{"#ffffff"};

    QColor displayText{"#ffffff"};
    QColor expressionText{"#8e8e93"};
    QColor titleColor{"#2ecc71"};

    static ThemeSettings darkTheme() {
        ThemeSettings t;
        t.mode = Dark;
        t.bgType = SolidColor;
        t.bgColor = QColor("#1c1c1e");
        t.buttonRadius = 10;
        t.digitBg = QColor("#3a3a3c"); t.digitText = QColor("#ffffff");
        t.operatorBg = QColor("#f39c12"); t.operatorText = QColor("#ffffff");
        t.equalBg = QColor("#2ecc71"); t.equalText = QColor("#ffffff");
        t.clearBg = QColor("#e74c3c"); t.clearText = QColor("#ffffff");
        t.functionBg = QColor("#7f8c8d"); t.functionText = QColor("#ffffff");
        t.displayText = QColor("#ffffff");
        t.expressionText = QColor("#8e8e93");
        t.titleColor = QColor("#2ecc71");
        return t;
    }

    static ThemeSettings whiteTheme() {
        ThemeSettings t;
        t.mode = White;
        t.bgType = SolidColor;
        t.bgColor = QColor("#f5f5f5");
        t.buttonRadius = 10;
        t.digitBg = QColor("#e0e0e0"); t.digitText = QColor("#1c1c1e");
        t.operatorBg = QColor("#d4a017"); t.operatorText = QColor("#ffffff");
        t.equalBg = QColor("#27ae60"); t.equalText = QColor("#ffffff");
        t.clearBg = QColor("#c0392b"); t.clearText = QColor("#ffffff");
        t.functionBg = QColor("#bdc3c7"); t.functionText = QColor("#1c1c1e");
        t.displayText = QColor("#1c1c1e");
        t.expressionText = QColor("#7f8c8d");
        t.titleColor = QColor("#27ae60");
        return t;
    }

    static bool isSystemDarkMode();

    static ThemeSettings systemTheme() {
        ThemeSettings t = isSystemDarkMode() ? darkTheme() : whiteTheme();
        t.mode = System;
        return t;
    }
};

#endif
