#include "appsettingsdialog.h"
#include "ui_appsettingsdialog.h"

AppSettingsDialog::AppSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AppSettingsDialog)
{
    ui->setupUi(this);
}

AppSettingsDialog::~AppSettingsDialog()
{
    delete ui;
}
