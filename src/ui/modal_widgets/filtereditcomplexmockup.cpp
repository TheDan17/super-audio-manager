#include "filtereditcomplexmockup.h"
#include "ui_filtereditcomplexmockup.h"

FilterEditComplexMockup::FilterEditComplexMockup(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FilterEditComplexMockup)
{
    ui->setupUi(this);
}

FilterEditComplexMockup::~FilterEditComplexMockup()
{
    delete ui;
}
