#pragma once
#include <QDialog>
#include <QListWidget>

class SourcesManagerDialog : public QDialog {
    Q_OBJECT
public:
    explicit SourcesManagerDialog(QWidget *parent = nullptr);
private:
    QListWidget* m_list;
    void refreshList();
};
