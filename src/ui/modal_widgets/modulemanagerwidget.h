#ifndef MODULEMANAGERWIDGET_H
#define MODULEMANAGERWIDGET_H

#include <QWidget>

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
};

#endif // MODULEMANAGERWIDGET_H
