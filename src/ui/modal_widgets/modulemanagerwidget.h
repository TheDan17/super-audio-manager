#ifndef MODULEMANAGERWIDGET_H
#define MODULEMANAGERWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QVector>

namespace Ui {
class ModuleManagerWidget;
}

class ModuleManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModuleManagerWidget(QWidget *parent = nullptr);
    ~ModuleManagerWidget();

private:
    Ui::ModuleManagerWidget *ui;

    struct ModuleRow {
        QString name;
        QFrame* frame;
    };
    QVector<ModuleRow> m_rows;
};

#endif // MODULEMANAGERWIDGET_H
