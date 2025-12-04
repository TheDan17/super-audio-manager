#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent *event) override;

public slots:
    void openModuleTab(QString uuid);

private slots:
    void onTabContextMenuRequested(const QPoint &pos);
    void onFilterTextChanged(const QString &text); // Слот для фильтра

private:
    void updateFilterComboBox();
    void updateLibraryTable(); // Вспомогательный метод

    void loadSettings();
    void saveSettings();

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
