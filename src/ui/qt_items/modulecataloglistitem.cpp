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

    // --- 1. ЗАПОЛНЕНИЕ ДАННЫМИ ---
    ui->moduleNameLabel->setText(m_moduleInfo.localizedName);
    // Иконку модуля можно установить здесь, если бы она была в UI
    // ui->moduleIconLabel->setPixmap(m_moduleInfo.icon.pixmap(32, 32));

    // --- 2. УПРАВЛЕНИЕ ВИДИМОСТЬЮ ЭЛЕМЕНТОВ ---
    ui->modulePinnedIconLabel->setVisible(m_moduleInfo.isPinned);
    ui->moduleActionBox->setVisible(showActionBox);

    // Включаем wordWrap один раз. Дальнейшее управление будет через обрезание текста.
    ui->moduleAboutLabel->setWordWrap(true);
    updateDescriptionText(); // Первый вызов для установки и форматирования текста

    // --- 3. НАСТРОЙКА КНОПОК ДЕЙСТВИЙ ---
    // Настраиваем доступность и подсказки для кнопок в зависимости от "возможностей" модуля.
    if (m_moduleInfo.supportsPanel) {
        ui->openPanelButton->setEnabled(true);
        ui->openPanelButton->setToolTip(tr("Открыть модуль в виде панели"));
    } else {
        ui->openPanelButton->setEnabled(false);
        ui->openPanelButton->setToolTip(tr("Этот модуль не поддерживает отображение в виде панели"));
    }

    if (m_moduleInfo.supportsWindow) {
        ui->openWindowButton->setEnabled(true);
        ui->openWindowButton->setToolTip(tr("Открыть модуль в отдельном окне"));
    } else {
        ui->openWindowButton->setEnabled(false);
        ui->openWindowButton->setToolTip(tr("Этот модуль не поддерживает отображение в отдельном окне"));
    }

    // --- 4. ПОДКЛЮЧЕНИЕ СИГНАЛОВ ---
    // Кнопка "Инфо" показывает простое сообщение. В реальном приложении она бы
    // испускала сигнал, который обрабатывал бы Модуль Настроек.
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
    updateDescriptionText(); // Обновляем текст при каждом изменении размера.
}

void ModuleCatalogListItem::updateDescriptionText()
{
    QString originalText = m_moduleInfo.localizedDescription;
    bool hasDescription = !originalText.isEmpty();

    // Устанавливаем текст-заглушку и меняем цвет, если описания нет.
    QPalette palette = ui->moduleAboutLabel->palette();
    if (hasDescription) {
        palette.setColor(QPalette::WindowText, QGuiApplication::palette().color(QPalette::Active, QPalette::WindowText));
    } else {
        originalText = tr("Описание отсутствует");
        QColor disabledColor = QGuiApplication::palette().color(QPalette::Disabled, QPalette::WindowText);
        ui->moduleAboutLabel->setStyleSheet(QString("color: %1;").arg(disabledColor.name()));
        //palette.setColor(QPalette::WindowText, QGuiApplication::palette().color(QPalette::Disabled, QPalette::WindowText));
    }
    ui->moduleAboutLabel->setPalette(palette);

    QFontMetrics fm(ui->moduleAboutLabel->font());
    int availableWidth = ui->moduleAboutLabel->width();

    if (availableWidth <= 0) return;


    const int maxHeight = fm.height() * 2;
    QRect textRect = fm.boundingRect(QRect(0, 0, availableWidth, 0), Qt::TextWordWrap, originalText);

    QString finalText = originalText;

    if (textRect.height() > maxHeight)
    {
        // --- Текст не влезает, начинаем обрезать ---
        QString tempText = originalText;
        while (true)
        {
            // Находим последний пробел
            int lastSpace = tempText.lastIndexOf(' ');
            if (lastSpace == -1) {
                // В тексте нет пробелов (одно длинное слово), просто обрезаем по символам.
                // Это крайний случай.
                tempText.chop(4); // Убираем 3 символа + 1 для '…'
                tempText += "…";
            } else {
                // Обрезаем до последнего пробела и добавляем троеточие
                tempText = tempText.left(lastSpace) + "…";
            }

            // Проверяем, влез ли укороченный текст
            textRect = fm.boundingRect(QRect(0, 0, availableWidth, 0), Qt::TextWordWrap, tempText);
            if (textRect.height() <= maxHeight) {
                // Влез! Это наш финальный вариант.
                finalText = tempText;
                break;
            }

            // Если все еще не влез, нужно укоротить еще.
            // Убираем троеточие и последний символ перед пробелом, чтобы цикл продолжился.
            tempText.chop(2);
            if (tempText.isEmpty()) break; // Защита от бесконечного цикла
        }
    }
    else if (textRect.height() <= fm.height())
    {
        // --- Текст влезает в одну строку, добавляем \n для выравнивания ---
        finalText += "\n";
    }


    ui->moduleAboutLabel->setText(finalText);
}
