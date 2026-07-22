#include "themeeditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QScrollArea>
#include <QFontDatabase>
#include <QDialogButtonBox>
#include <QFrame>

ThemeEditor::ThemeEditor(const ThemeSettings &current, QWidget *parent)
    : QDialog(parent), m_settings(current)
{
    setWindowTitle("CalcX - Tema Ayarları");
    setFixedSize(520, 620);
    setStyleSheet(R"(
        QDialog { background-color: #1c1c1e; color: #ffffff; }
        QLabel { color: #ffffff; background: transparent; }
        QGroupBox { 
            color: #2ecc71; font-weight: bold; border: 1px solid #2c2c2e; 
            border-radius: 8px; margin-top: 16px; padding-top: 16px;
        }
        QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; }
        QComboBox {
            background-color: #2c2c2e; color: #ffffff; border: 1px solid #3a3a3c;
            border-radius: 6px; padding: 6px 12px; font-size: 13px;
        }
        QComboBox::drop-down { border: none; }
        QComboBox::down-arrow { image: none; border-left: 4px solid transparent; 
            border-right: 4px solid transparent; border-top: 6px solid #ffffff; 
            margin-right: 8px; }
        QComboBox QAbstractItemView {
            background-color: #2c2c2e; color: #ffffff; selection-background-color: #3a3a3c;
            border: 1px solid #3a3a3c; outline: none;
        }
        QSlider::groove:horizontal {
            background: #3a3a3c; height: 6px; border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: #2ecc71; width: 18px; height: 18px; margin: -6px 0;
            border-radius: 9px;
        }
        QRadioButton { color: #ffffff; spacing: 6px; }
        QRadioButton::indicator { width: 14px; height: 14px; border-radius: 7px;
            border: 2px solid #5a5a5c; background: transparent; }
        QRadioButton::indicator:checked { background: #2ecc71; border-color: #2ecc71; }
        QPushButton { font-size: 12px; }
        QLineEdit {
            background-color: #2c2c2e; color: #ffffff; border: 1px solid #3a3a3c;
            border-radius: 6px; padding: 4px 8px;
        }
    )");

    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    auto *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }"
                              "QScrollBar:vertical { width: 6px; background: #1c1c1e; }"
                              "QScrollBar::handle:vertical { background: #3a3a3c; border-radius: 3px; }"
                              "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }");

    auto *container = new QWidget();
    container->setStyleSheet("background: transparent;");
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(10);

    auto *themeSelector = new QWidget();
    auto *tsLayout = new QHBoxLayout(themeSelector);
    tsLayout->setContentsMargins(0, 0, 0, 0);
    auto *tsLabel = new QLabel("Tema:");
    tsLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    m_modeCombo = new QComboBox();
    m_modeCombo->addItem("CalcX - Dark", ThemeSettings::Dark);
    m_modeCombo->addItem("CalcX - White", ThemeSettings::White);
    m_modeCombo->addItem("CalcX - System", ThemeSettings::System);
    m_modeCombo->addItem("CUSTOM", ThemeSettings::Custom);
    m_modeCombo->setCurrentIndex(m_settings.mode);
    tsLayout->addWidget(tsLabel);
    tsLayout->addWidget(m_modeCombo, 1);
    layout->addWidget(themeSelector);

    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ThemeEditor::onThemeModeChanged);

    m_customWidget = new QWidget();
    m_customWidget->setStyleSheet("background: transparent;");
    auto *customLayout = new QVBoxLayout(m_customWidget);
    customLayout->setContentsMargins(0, 0, 0, 0);
    customLayout->setSpacing(8);

    auto *bgGroup = new QGroupBox("Arkaplan");
    auto *bgLayout = new QVBoxLayout(bgGroup);

    auto *bgTypeRow = new QWidget();
    auto *bgtLayout = new QHBoxLayout(bgTypeRow);
    bgtLayout->setContentsMargins(0, 0, 0, 0);
    m_solidRadio = new QRadioButton("Düz Renk");
    m_imageRadio = new QRadioButton("Görsel");
    if (m_settings.bgType == ThemeSettings::SolidColor)
        m_solidRadio->setChecked(true);
    else
        m_imageRadio->setChecked(true);
    bgtLayout->addWidget(m_solidRadio);
    bgtLayout->addWidget(m_imageRadio);
    bgtLayout->addStretch();
    bgLayout->addWidget(bgTypeRow);
    connect(m_solidRadio, &QRadioButton::toggled, this, &ThemeEditor::onBgTypeChanged);
    connect(m_imageRadio, &QRadioButton::toggled, this, &ThemeEditor::onBgTypeChanged);

    auto *bgColorRow = createColorRow("Arkaplan Rengi:", m_settings.bgColor, m_bgColorBtn);
    bgLayout->addWidget(bgColorRow);

    auto *imgRow = new QWidget();
    auto *imgLayout = new QHBoxLayout(imgRow);
    imgLayout->setContentsMargins(0, 0, 0, 0);
    auto *imgLabel = new QLabel("Görsel Yolu:");
    m_imagePathEdit = new QLineEdit();
    m_imagePathEdit->setPlaceholderText("Görsel dosyası seçin...");
    m_imagePathEdit->setText(m_settings.bgImagePath);
    m_browseBtn = new QPushButton("Gözat");
    m_browseBtn->setCursor(Qt::PointingHandCursor);
    m_browseBtn->setStyleSheet(R"(
        QPushButton { background-color: #3a3a3c; color: #fff; border: none;
            border-radius: 6px; padding: 6px 14px; font-weight: bold; }
        QPushButton:hover { background-color: #4a4a4c; }
    )");
    imgLayout->addWidget(imgLabel);
    imgLayout->addWidget(m_imagePathEdit, 1);
    imgLayout->addWidget(m_browseBtn);
    bgLayout->addWidget(imgRow);
    connect(m_browseBtn, &QPushButton::clicked, this, &ThemeEditor::browseImage);

    customLayout->addWidget(bgGroup);

    auto *shapeGroup = new QGroupBox("Buton Şekli");
    auto *shapeLayout = new QHBoxLayout(shapeGroup);
    auto *radiusLabel = new QLabel("Köşe Yuvarlaklığı:");
    m_radiusSlider = new QSlider(Qt::Horizontal);
    m_radiusSlider->setRange(0, 20);
    m_radiusSlider->setValue(m_settings.buttonRadius);
    m_radiusLabel = new QLabel(QString::number(m_settings.buttonRadius) + "px");
    m_radiusLabel->setMinimumWidth(36);
    shapeLayout->addWidget(radiusLabel);
    shapeLayout->addWidget(m_radiusSlider, 1);
    shapeLayout->addWidget(m_radiusLabel);
    customLayout->addWidget(shapeGroup);
    connect(m_radiusSlider, &QSlider::valueChanged, this, &ThemeEditor::onRadiusChanged);

    auto *opacityGroup = new QGroupBox("Pencere Opaklığı");
    auto *opacityLayout = new QHBoxLayout(opacityGroup);
    auto *opacityLabel = new QLabel("Opacity:");
    m_opacitySlider = new QSlider(Qt::Horizontal);
    m_opacitySlider->setRange(20, 100);
    m_opacitySlider->setValue(qRound(m_settings.opacity * 100.0));
    m_opacityLabel = new QLabel(QString::number(m_opacitySlider->value()) + "%");
    m_opacityLabel->setMinimumWidth(40);
    opacityLayout->addWidget(opacityLabel);
    opacityLayout->addWidget(m_opacitySlider, 1);
    opacityLayout->addWidget(m_opacityLabel);
    customLayout->addWidget(opacityGroup);
    connect(m_opacitySlider, &QSlider::valueChanged, this, [this](int val) {
        m_settings.opacity = val / 100.0;
        m_opacityLabel->setText(QString::number(val) + "%");
    });

    auto *btnGroup = new QGroupBox("Buton Renkleri");
    auto *btnGrid = new QGridLayout(btnGroup);
    btnGrid->setSpacing(6);

    btnGrid->addWidget(new QLabel("Buton"), 0, 0);
    btnGrid->addWidget(new QLabel("Arkaplan"), 0, 1);
    btnGrid->addWidget(new QLabel("Yazı"), 0, 2);

    int row = 1;
    auto addRow = [&](const QString &name, QColor &bg, QPushButton *&bgBtn,
                      QColor &text, QPushButton *&textBtn) {
        auto *nameLbl = new QLabel(name);
        auto *rowWidget = createColorRow("", bg, bgBtn);
        auto *textWidget = createColorRow("", text, textBtn);
        auto *rowLayout = new QWidget();
        auto *hl = new QHBoxLayout(rowLayout);
        hl->setContentsMargins(0, 0, 0, 0);
        hl->addWidget(rowWidget);
        hl->addWidget(textWidget);
        btnGrid->addWidget(nameLbl, row, 0);
        btnGrid->addWidget(rowWidget, row, 1);
        btnGrid->addWidget(textWidget, row, 2);
        row++;
    };

    addRow("Rakam", m_settings.digitBg, m_digitBgBtn, m_settings.digitText, m_digitTextBtn);
    addRow("İşleç", m_settings.operatorBg, m_opBgBtn, m_settings.operatorText, m_opTextBtn);
    addRow("Eşittir", m_settings.equalBg, m_eqBgBtn, m_settings.equalText, m_eqTextBtn);
    addRow("Temizle", m_settings.clearBg, m_clrBgBtn, m_settings.clearText, m_clrTextBtn);
    addRow("Fonksiyon", m_settings.functionBg, m_fnBgBtn, m_settings.functionText, m_fnTextBtn);

    customLayout->addWidget(btnGroup);

    auto *textGroup = new QGroupBox("Yazı Renkleri");
    auto *textLayout = new QVBoxLayout(textGroup);
    textLayout->addWidget(createColorRow("Ekran Yazısı:", m_settings.displayText, m_displayTextBtn));
    textLayout->addWidget(createColorRow("İfade Yazısı:", m_settings.expressionText, m_exprTextBtn));
    customLayout->addWidget(textGroup);

    m_customWidget->setVisible(m_settings.mode == ThemeSettings::Custom);
    layout->addWidget(m_customWidget);

    auto *bottomWidget = new QWidget();
    auto *bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(0, 8, 0, 0);

    auto *applyBtn = new QPushButton("Uygula");
    applyBtn->setCursor(Qt::PointingHandCursor);
    applyBtn->setStyleSheet(R"(
        QPushButton { background-color: #2ecc71; color: #fff; border: none;
            border-radius: 8px; padding: 10px 28px; font-size: 14px; font-weight: bold; }
        QPushButton:hover { background-color: #27ae60; }
    )");
    auto *cancelBtn = new QPushButton("İptal");
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet(R"(
        QPushButton { background-color: #3a3a3c; color: #fff; border: none;
            border-radius: 8px; padding: 10px 28px; font-size: 14px; }
        QPushButton:hover { background-color: #4a4a4c; }
    )");

    bottomLayout->addStretch();
    bottomLayout->addWidget(cancelBtn);
    bottomLayout->addWidget(applyBtn);

    connect(applyBtn, &QPushButton::clicked, this, &ThemeEditor::onApply);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    container->layout()->addWidget(bottomWidget);
    static_cast<QVBoxLayout*>(container->layout())->addStretch(1);

    scrollArea->setWidget(container);
    outerLayout->addWidget(scrollArea);
}

QWidget* ThemeEditor::createColorRow(const QString &label, QColor &color, QPushButton *&btn) {
    auto *widget = new QWidget();
    widget->setStyleSheet("background: transparent;");
    auto *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    if (!label.isEmpty()) {
        auto *lbl = new QLabel(label);
        layout->addWidget(lbl);
    }

    btn = new QPushButton();
    btn->setFixedSize(28, 28);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(QString(
        "QPushButton { background-color: %1; border: 2px solid #5a5a5c; "
        "border-radius: 6px; } QPushButton:hover { border-color: #2ecc71; }"
    ).arg(color.name()));

    connect(btn, &QPushButton::clicked, this, [this, &color, btn]() {
        pickColor(color, btn);
    });

    layout->addWidget(btn);
    layout->addStretch();
    return widget;
}

void ThemeEditor::pickColor(QColor &target, QPushButton *btn) {
    QColor chosen = QColorDialog::getColor(target, this, "Renk Seç");
    if (chosen.isValid()) {
        target = chosen;
        btn->setStyleSheet(QString(
            "QPushButton { background-color: %1; border: 2px solid #5a5a5c; "
            "border-radius: 6px; } QPushButton:hover { border-color: #2ecc71; }"
        ).arg(target.name()));
    }
}

void ThemeEditor::onThemeModeChanged(int index) {
    ThemeSettings::ThemeMode mode = static_cast<ThemeSettings::ThemeMode>(
        m_modeCombo->itemData(index).toInt());

    if (mode == ThemeSettings::Dark) {
        m_settings = ThemeSettings::darkTheme();
        m_customWidget->setVisible(false);
    } else if (mode == ThemeSettings::White) {
        m_settings = ThemeSettings::whiteTheme();
        m_customWidget->setVisible(false);
    } else if (mode == ThemeSettings::System) {
        m_settings = ThemeSettings::systemTheme();
        m_customWidget->setVisible(false);
    } else {
        m_settings.mode = ThemeSettings::Custom;
        m_customWidget->setVisible(true);
        return;
    }

    m_solidRadio->setChecked(true);
    m_bgColorBtn->setStyleSheet(QString(
        "QPushButton { background-color: %1; border: 2px solid #5a5a5c; "
        "border-radius: 6px; } QPushButton:hover { border-color: #2ecc71; }"
    ).arg(m_settings.bgColor.name()));
    m_radiusSlider->setValue(m_settings.buttonRadius);

    auto refreshBtn = [this](QPushButton *btn, const QColor &c) {
        if (!btn) return;
        btn->setStyleSheet(QString(
            "QPushButton { background-color: %1; border: 2px solid #5a5a5c; "
            "border-radius: 6px; } QPushButton:hover { border-color: #2ecc71; }"
        ).arg(c.name()));
    };
    refreshBtn(m_digitBgBtn, m_settings.digitBg);
    refreshBtn(m_digitTextBtn, m_settings.digitText);
    refreshBtn(m_opBgBtn, m_settings.operatorBg);
    refreshBtn(m_opTextBtn, m_settings.operatorText);
    refreshBtn(m_eqBgBtn, m_settings.equalBg);
    refreshBtn(m_eqTextBtn, m_settings.equalText);
    refreshBtn(m_clrBgBtn, m_settings.clearBg);
    refreshBtn(m_clrTextBtn, m_settings.clearText);
    refreshBtn(m_fnBgBtn, m_settings.functionBg);
    refreshBtn(m_fnTextBtn, m_settings.functionText);
    refreshBtn(m_displayTextBtn, m_settings.displayText);
    refreshBtn(m_exprTextBtn, m_settings.expressionText);
}

void ThemeEditor::onBgTypeChanged() {
    m_settings.bgType = m_solidRadio->isChecked()
        ? ThemeSettings::SolidColor : ThemeSettings::Image;
}

void ThemeEditor::browseImage() {
    QString path = QFileDialog::getOpenFileName(this, "Arkaplan Görseli Seç",
        QString(), "Resim Dosyaları (*.png *.jpg *.jpeg *.bmp *.svg)");
    if (!path.isEmpty()) {
        m_imagePathEdit->setText(path);
        m_settings.bgImagePath = path;
    }
}

void ThemeEditor::onRadiusChanged(int val) {
    m_settings.buttonRadius = val;
    m_radiusLabel->setText(QString::number(val) + "px");
}

void ThemeEditor::onApply() {
    if (m_imageRadio->isChecked() && !m_imagePathEdit->text().isEmpty())
        m_settings.bgImagePath = m_imagePathEdit->text();
    accept();
}

ThemeSettings ThemeEditor::settings() const {
    return m_settings;
}
