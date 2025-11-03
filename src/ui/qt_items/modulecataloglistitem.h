#ifndef MODULECATALOGLISTITEM_H
#define MODULECATALOGLISTITEM_H

#include <QWidget>

#include "../../items/modulemetainfo.h"


namespace Ui {
class ModuleCatalogListItem;
}


class ModuleCatalogListItem : public QWidget
{
    Q_OBJECT

public:
    explicit ModuleCatalogListItem(const ModuleDistillMetaInfo& info, bool showActionBox, QWidget *parent = nullptr);
    ~ModuleCatalogListItem();

    QString moduleName() const;

signals:
    void info_clicked(const ModuleDistillMetaInfo& obj);
    void open_panel_clicked(const ModuleDistillMetaInfo& obj);
    void open_window_clicked(const ModuleDistillMetaInfo& obj);
    void pin_status_changed(const ModuleDistillMetaInfo& obj, bool is_pinned);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateDescriptionText();

    Ui::ModuleCatalogListItem *ui;
    const ModuleDistillMetaInfo m_moduleInfo;

};


#endif // MODULECATALOGLISTITEM_H
