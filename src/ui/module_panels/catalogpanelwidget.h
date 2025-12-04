#ifndef CATALOGPANELWIDGET_H
#define CATALOGPANELWIDGET_H

#include <QWidget>
#include <QVector>
#include <QListWidget>
#include "../../items/modulemetainfo.h"


namespace AppUtils::Text { // Framework::StringUtils
QVector<int> filterByWildcard(const QString& filter,
                              const QStringList& itemsToFilter,
                              Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);
bool matchesWildcard(const QString& item,
                     const QString& filter,
                     Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);
}
namespace Ui {
class CatalogPanelWidget;
}


class CatalogPanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CatalogPanelWidget(QWidget *parent = nullptr);
    ~CatalogPanelWidget();

    void populate(const QVector<ModuleDistillMetaInfo>& allModules);
    enum ViewMode { Detailed, Compact };

private slots:
    void onFilterTextChanged(const QString& text);
    void onToolBoxContextMenuRequested(const QPoint& pos);

signals:
    void requestOpenModule(QString uuid);
    void requestOpenSettings(); // <--- Добавить

private:
    void add_module_to_qlistwidget(QListWidget* list, ModuleDistillMetaInfo module_info, bool isShowActions, ViewMode view_mode);

    Ui::CatalogPanelWidget *ui;

    QVector<ModuleDistillMetaInfo> m_modules;

    ViewMode m_currentMode = Detailed;
};


#endif // CATALOGPANELWIDGET_H
