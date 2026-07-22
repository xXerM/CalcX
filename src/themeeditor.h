#ifndef THEMEEDITOR_H
#define THEMEEDITOR_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QRadioButton>
#include <QLineEdit>
#include <QCheckBox>
#include "themesettings.h"

class ThemeEditor : public QDialog {
    Q_OBJECT

public:
    explicit ThemeEditor(const ThemeSettings &current, QWidget *parent = nullptr);
    ThemeSettings settings() const;

private slots:
    void onThemeModeChanged(int index);
    void pickColor(QColor &target, QPushButton *btn);
    void onBgTypeChanged();
    void browseImage();
    void onRadiusChanged(int val);
    void onApply();

private:
    QWidget* createColorRow(const QString &label, QColor &color, QPushButton *&btn);
    QWidget* createSection(const QString &title, QWidget *content);

    ThemeSettings m_settings;

    QComboBox *m_modeCombo;
    QWidget *m_customWidget;
    QRadioButton *m_solidRadio;
    QRadioButton *m_imageRadio;
    QPushButton *m_bgColorBtn;
    QLineEdit *m_imagePathEdit;
    QPushButton *m_browseBtn;
    QSlider *m_radiusSlider;
    QLabel *m_radiusLabel;
    QSlider *m_opacitySlider;
    QLabel *m_opacityLabel;
    QPushButton *m_digitBgBtn, *m_digitTextBtn;
    QPushButton *m_opBgBtn, *m_opTextBtn;
    QPushButton *m_eqBgBtn, *m_eqTextBtn;
    QPushButton *m_clrBgBtn, *m_clrTextBtn;
    QPushButton *m_fnBgBtn, *m_fnTextBtn;
    QPushButton *m_displayTextBtn;
    QPushButton *m_exprTextBtn;
};

#endif
