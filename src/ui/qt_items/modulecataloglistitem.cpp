#include "modulecataloglistitem.h"
#include "ui_modulecataloglistitem.h"
#include <QMessageBox>

ModuleCatalogListItem::ModuleCatalogListItem(const ModuleDistillMetaInfo& info, bool showActionBox, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModuleCatalogListItem),
    m_moduleInfo(info)
{
    ui->setupUi(this);

    auto icon = ui->modulePinnedIconLabel;
    QSizePolicy policy = icon->sizePolicy();
    policy.setRetainSizeWhenHidden(true);
    icon->setSizePolicy(policy);

    // 1. Заполнение данными
    ui->moduleNameLabel->setText(m_moduleInfo.localizedName);
    ui->modulePinnedIconLabel->setVisible(m_moduleInfo.isPinned);
    ui->moduleActionBox->setVisible(showActionBox);

    ui->moduleAboutLabel->setWordWrap(true);
    updateDescriptionText();

    // --- ИСПРАВЛЕНИЕ (Пункт 3): Настройка доступности кнопок ---

    // Кнопка "Открыть панель"
    if (m_moduleInfo.supportsPanel) {
        ui->openPanelButton->setEnabled(true);
        ui->openPanelButton->setToolTip(tr("Открыть модуль в виде панели"));

        // --- ИСПРАВЛЕНИЕ (Пункт 2): Подключение сигнала ---
        connect(ui->openPanelButton, &QPushButton::clicked, this, [this](){
            emit open_panel_clicked(m_moduleInfo);
        });
    } else {
        ui->openPanelButton->setEnabled(false);
        ui->openPanelButton->setToolTip(tr("Этот модуль не поддерживает отображение в виде панели"));
    }

    // Кнопка "Открыть окно"
    if (m_moduleInfo.supportsWindow) {
        ui->openWindowButton->setEnabled(true);
        ui->openWindowButton->setToolTip(tr("Открыть модуль в отдельном окне"));

        connect(ui->openWindowButton, &QPushButton::clicked, this, [this](){
            emit open_window_clicked(m_moduleInfo);
        });
    } else {
        ui->openWindowButton->setEnabled(false);
        ui->openWindowButton->setToolTip(tr("Этот модуль не поддерживает отображение в отдельном окне"));
    }

    // Кнопка "Инфо"
    connect(ui->moduleInfoButton, &QPushButton::clicked, this, [this](){
        QMessageBox::information(this,
                                 tr("Информация о модуле"),
                                 QString("Название: %1\nВерсия: %2\nID: %3")
                                     .arg(m_moduleInfo.localizedName)
                                     .arg(m_moduleInfo.version)
                                     .arg(m_moduleInfo.stringId));
    });
}

ModuleCatalogListItem::~ModuleCatalogListItem()
{
    delete ui;
}

QString ModuleCatalogListItem::moduleName() const
{
    return m_moduleInfo.localizedName;
}

void ModuleCatalogListItem::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateDescriptionText();
}

void ModuleCatalogListItem::updateDescriptionText()
{
    // (Код обрезки текста оставляем тот же, он был корректен)
    QString originalText = m_moduleInfo.localizedDescription;
    bool hasDescription = !originalText.isEmpty();

    QPalette palette = ui->moduleAboutLabel->palette();
    if (hasDescription) {
        palette.setColor(QPalette::WindowText, QGuiApplication::palette().color(QPalette::Active, QPalette::WindowText));
    } else {
        originalText = tr("Описание отсутствует");
        QColor disabledColor = QGuiApplication::palette().color(QPalette::Disabled, QPalette::WindowText);
        ui->moduleAboutLabel->setStyleSheet(QString("color: %1;").arg(disabledColor.name()));
    }
    ui->moduleAboutLabel->setPalette(palette);

    QFontMetrics fm(ui->moduleAboutLabel->font());
    int availableWidth = ui->moduleAboutLabel->width();
    if (availableWidth <= 0) return;

    const int maxHeight = fm.height() * 2;
    QRect textRect = fm.boundingRect(QRect(0, 0, availableWidth, 0), Qt::TextWordWrap, originalText);
    QString finalText = originalText;

    if (textRect.height() > maxHeight) {
        QString tempText = originalText;
        while (true) {
            int lastSpace = tempText.lastIndexOf(' ');
            if (lastSpace == -1) {
                tempText.chop(4); tempText += "…";
            } else {
                tempText = tempText.left(lastSpace) + "…";
            }
            textRect = fm.boundingRect(QRect(0, 0, availableWidth, 0), Qt::TextWordWrap, tempText);
            if (textRect.height() <= maxHeight) {
                finalText = tempText;
                break;
            }
            tempText.chop(2);
            if (tempText.isEmpty()) break;
        }
    }
    ui->moduleAboutLabel->setText(finalText);
}
