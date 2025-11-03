#include "filtereditdialog.h"
#include "ui_filtereditdialog.h"

FilterEditDialog::FilterEditDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FilterEditDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
}

FilterEditDialog::~FilterEditDialog()
{
    delete ui;
}
