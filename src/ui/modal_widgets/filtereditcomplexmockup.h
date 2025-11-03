#ifndef FILTEREDITCOMPLEXMOCKUP_H
#define FILTEREDITCOMPLEXMOCKUP_H

#include <QDialog>

namespace Ui {
class FilterEditComplexMockup;
}

class FilterEditComplexMockup : public QDialog
{
    Q_OBJECT

public:
    explicit FilterEditComplexMockup(QWidget *parent = nullptr);
    ~FilterEditComplexMockup();

private:
    Ui::FilterEditComplexMockup *ui;
};

#endif // FILTEREDITCOMPLEXMOCKUP_H
