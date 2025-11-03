#include "modulemanagerwidget.h"
#include "ui_modulemanagerwidget.h"

ModuleManagerWidget::ModuleManagerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ModuleManagerWidget)
{
    ui->setupUi(this);
}

ModuleManagerWidget::~ModuleManagerWidget()
{
    delete ui;
}
