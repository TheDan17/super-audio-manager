#include "appsettingsdialog.h"
#include "ui_appsettingsdialog.h"
#include "modulemanagerwidget.h"

AppSettingsDialog::AppSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AppSettingsDialog)
{
    ui->setupUi(this);

    // 1. Настраиваем список разделов
    connect(ui->listWidget, &QListWidget::currentRowChanged, [this](int row){
        // В вашем XML справа лежит ModuleManagerWidget (row 0).
        // Для других разделов (row 1, 2...) можно сделать QStackedWidget,
        // но для лабы достаточно показать, что "Модули" работают.

        if (row == 0) {
            ui->widget->setVisible(true); // Показываем менеджер модулей
        } else {
            ui->widget->setVisible(false); // Скрываем (или показываем заглушку)
        }
    });

    // Выбираем первый пункт по умолчанию
    ui->listWidget->setCurrentRow(0);
}

AppSettingsDialog::~AppSettingsDialog()
{
    delete ui;
}
