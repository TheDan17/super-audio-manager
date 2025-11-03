#ifndef FILTEREDITDIALOG_H
#define FILTEREDITDIALOG_H

#include <QDialog>

namespace Ui {
class FilterEditDialog;
}

class FilterEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterEditDialog(QWidget *parent = nullptr);
    ~FilterEditDialog();

private:
    Ui::FilterEditDialog *ui;
};

#endif // FILTEREDITDIALOG_H
