#include "filtereditdialog.h"
#include "ui_filtereditdialog.h"
#include "../../core/core.h"

FilterEditDialog::FilterEditDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FilterEditDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->pushButton, &QPushButton::clicked, this, [this](){
        // Сохранение фильтра
        UserFilter filter;
        filter.name = ui->lineEdit->text(); // Поле названия из XML

        // В реальном app мы бы парсили правила.
        // Для лабы: считаем, что этот фильтр ищет файлы, содержащие имя фильтра
        filter.searchPattern = filter.name;

        Core::instance().addFilter(filter);

        accept(); // Закрываем окно
    });

    connect(ui->pushButton_2, &QPushButton::clicked, this, &QDialog::reject);
}

FilterEditDialog::~FilterEditDialog()
{
    delete ui;
}
