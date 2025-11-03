#include "catalogpanelwidget.h"
#include "ui_catalogpanelwidget.h"

#include <QListWidgetItem>
#include <QMenu>

#include "../qt_items/modulecataloglistitem.h"


CatalogPanelWidget::CatalogPanelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CatalogPanelWidget)
{
    ui->setupUi(this);


    connect(ui->catalogFilterLineEdit, &QLineEdit::textChanged, this, &CatalogPanelWidget::onFilterTextChanged);

    ui->catalogToolBox->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->catalogToolBox, &QWidget::customContextMenuRequested, this, &CatalogPanelWidget::onToolBoxContextMenuRequested);
}

CatalogPanelWidget::~CatalogPanelWidget()
{
    delete ui;
}

void add_module_to_qlistwidget(QListWidget* list, ModuleDistillMetaInfo module_info,
                               bool isShowActions, CatalogPanelWidget::ViewMode view_mode){
    QWidget* itemWidget = nullptr;
    if (view_mode == CatalogPanelWidget::ViewMode::Detailed) {
        itemWidget = new ModuleCatalogListItem(module_info, isShowActions);
    } else {
        itemWidget = new QLabel(module_info.localizedName); // TODO отдельный виджет
    }

    auto* listItem = new QListWidgetItem(list);
    listItem->setSizeHint(itemWidget->sizeHint());

    list->addItem(listItem);
    listItem->setData(Qt::UserRole, module_info.uuid); // Сохраняем ID модуля в данных элемента списка для легкого доступа
    list->setItemWidget(listItem, itemWidget);
}

void CatalogPanelWidget::populate(const QVector<ModuleDistillMetaInfo>& allModules)
{
    m_modules = allModules;

    ui->panelUiList->clear();
    ui->panelDisabledList->clear();
    ui->panelNonUiList->clear();

    int uiCount = 0, disabledCount = 0, nonUiCount = 0;
    for (const ModuleDistillMetaInfo& info : m_modules)
    {
        if (!info.isEnabled) {
            disabledCount++;
            add_module_to_qlistwidget(ui->panelDisabledList, info, false, this->m_currentMode);
        }
        else if (!info.supportsPanel && !info.supportsWindow) {
            nonUiCount++;
            add_module_to_qlistwidget(ui->panelNonUiList, info, false, this->m_currentMode);
        }
        else {
            uiCount++;
            add_module_to_qlistwidget(ui->panelUiList, info, true, this->m_currentMode);
        }
    }

    ui->catalogToolBox->setItemText(0, QString("Доступные модули с интерфейсом (%1)").arg(uiCount));
    ui->catalogToolBox->setItemText(1, QString("Отключенные модули (%1)").arg(disabledCount));
    ui->catalogToolBox->setItemText(2, QString("Доступные модули без интерфейса (%1)").arg(nonUiCount));
}

void CatalogPanelWidget::onFilterTextChanged(const QString& text)
{
    for (int i = 0; i < ui->panelUiList->count(); ++i) {
        auto* listItem = ui->panelUiList->item(i);
        auto* itemWidget = qobject_cast<ModuleCatalogListItem*>(ui->panelUiList->itemWidget(listItem));

        if (itemWidget) {
            // Предполагаем, что AppUtils::Text::matchesWildcard существует
            bool match = AppUtils::Text::matchesWildcard(itemWidget->moduleName(), text);
            listItem->setHidden(!match);
        }
    }
}

void CatalogPanelWidget::onToolBoxContextMenuRequested(const QPoint& pos)
{
    QMenu contextMenu(this);
    QAction* detailedAction = contextMenu.addAction(tr("Детальный вид"));
    QAction* compactAction = contextMenu.addAction(tr("Компактный вид"));
    detailedAction->setCheckable(true);
    compactAction->setCheckable(true);

    if (m_currentMode == Detailed) {
        detailedAction->setChecked(true);
    } else {
        compactAction->setChecked(true);
    }

    connect(detailedAction, &QAction::triggered, this, [this](){
        m_currentMode = Detailed;
        populate(m_modules); // Перерисовываем список в новом виде
    });
    connect(compactAction, &QAction::triggered, this, [this](){
        m_currentMode = Compact;
        populate(m_modules); // Перерисовываем список в новом виде
    });

    contextMenu.exec(ui->catalogToolBox->mapToGlobal(pos));
}


#include <QRegularExpression>

namespace AppUtils::Text {

QVector<int> filterByWildcard(const QString& filter,
                              const QStringList& itemsToFilter,
                              Qt::CaseSensitivity caseSensitivity)
{
    QVector<int> matchingIndexes;

    // 1. Если фильтр пустой, возвращаем индексы всех элементов.
    if (filter.isEmpty()) {
        matchingIndexes.resize(itemsToFilter.size());
        // Заполняем вектор числами от 0 до N-1
        std::iota(matchingIndexes.begin(), matchingIndexes.end(), 0);
        return matchingIndexes;
    }

    // 2. Преобразуем wildcard-фильтр в регулярное выражение.
    // Это самая главная часть.
    QRegularExpression regex = QRegularExpression::fromWildcard(filter, caseSensitivity);

    // Если регулярка невалидна (например, `[` без `]`), ничего не найдем.
    if (!regex.isValid()) {
        return matchingIndexes; // Возвращаем пустой вектор
    }

    // 3. Проходимся по всем строкам и проверяем их на соответствие.
    for (int i = 0; i < itemsToFilter.size(); ++i) {
        const QString& item = itemsToFilter.at(i);

        QRegularExpressionMatch match = regex.match(item);

        // 4. Если нашли совпадение, добавляем индекс в результирующий вектор.
        if (match.hasMatch()) {
            matchingIndexes.append(i);
        }
    }

    return matchingIndexes;
}
bool matchesWildcard(const QString& item,
                     const QString& filter,
                     Qt::CaseSensitivity caseSensitivity)
{
    // 1. Если фильтр пустой, считаем, что все подходит.
    if (filter.isEmpty()) {
        return true;
    }

    // 2. Создаем переменную для нашего регулярного выражения.
    QRegularExpression regex;

    // 3. "Умная" логика: проверяем, есть ли в фильтре спецсимволы.
    if (filter.contains('*') || filter.contains('?')) {
        // --- Сценарий A: Пользователь ввел wildcard'ы ---
        // Преобразуем его ввод напрямую в регулярное выражение.
        // Qt::CaseInsensitive уже учтено.
        regex.setPattern(QRegularExpression::wildcardToRegularExpression(filter));
    } else {
        // --- Сценарий Б: Пользователь ввел обычный текст ---
        // Ищем его как подстроку.
        // Сначала экранируем текст, чтобы символы вроде '.' или '+' не
        // воспринимались как спецсимволы регулярных выражений.
        QString escapedFilter = QRegularExpression::escape(filter);
        // "Заворачиваем" в `.*` с обеих сторон, что эквивалентно `*<ввод>*`
        regex.setPattern(escapedFilter);
    }

    // 4. Устанавливаем чувствительность к регистру.
    if (caseSensitivity == Qt::CaseInsensitive) {
        regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    }

    // Если регулярка по какой-то причине невалидна, считаем, что совпадений нет.
    if (!regex.isValid()) {
        return false;
    }

    // 5. Выполняем проверку и возвращаем результат.
    return regex.match(item).hasMatch();
}

} // namespace AppUtils::Text
