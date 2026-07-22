#include "themesettings.h"
#include <QApplication>
#include <QStyle>
#include <QPalette>
#include <QByteArray>

bool ThemeSettings::isSystemDarkMode() {
    QPalette pal = QApplication::palette();

    QColor window = pal.color(QPalette::Window);
    if (window.lightness() < 128) return true;

    QColor base = pal.color(QPalette::Base);
    if (base.lightness() < 128) return true;

    QColor text = pal.color(QPalette::WindowText);
    if (window.lightness() < text.lightness()) return true;

    QByteArray gtkTheme = qgetenv("GTK_THEME");
    if (!gtkTheme.isEmpty() && gtkTheme.toLower().contains("dark"))
        return true;

    QByteArray qtStyle = qgetenv("QT_STYLE_OVERRIDE");
    if (!qtStyle.isEmpty() && qtStyle.toLower().contains("dark"))
        return true;

    QByteArray desktop = qgetenv("XDG_CURRENT_DESKTOP");
    if (desktop.toLower().contains("gnome") || desktop.toLower().contains("pop")) {
        QByteArray colorScheme = qgetenv("COLOR_SCHEME");
        if (!colorScheme.isEmpty() && colorScheme.toLower().contains("dark"))
            return true;
    }

    return false;
}
