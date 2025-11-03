#include "mainwindow.h"
#include "./main/ui_mainwindow.h"

#include <QTabBar>

#include "../mockup/mockdataprovider.h"
#include "../modal_widgets/filtereditdialog.h"
#include "../modal_widgets/filtereditcomplexmockup.h"
#include "../modal_widgets/appsettingsdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QTabWidget* tabWidget = ui->customPanelTabWidget;
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabText(i) == "Каталог") {
            tabWidget->tabBar()->setTabButton(i, QTabBar::RightSide, nullptr);
            break;
        }
    }

    ui->customPanelTabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->customPanelTabWidget, &QTabWidget::customContextMenuRequested,
            this, &MainWindow::onTabContextMenuRequested);

    // 1. Получаем список модулей от Ядра
    QVector<ModuleDistillMetaInfo> modules = MockDataProvider::createMockModuleList();//Core::instance()->moduleManager()->getAllModulesInfo();
    // 2. Передаем их в наш Каталог для отображения
    ui->catalogNonclosable->populate(modules); // 'catalogPanelWidget' - имя твоего виджета в MainWindow.ui

    connect(ui->openFilterEditorDebugAction, &QAction::triggered, [this]() {
        auto dialog = new FilterEditDialog(this);
        dialog->exec();
    });
    connect(ui->openFilterEditorComplexDebugAction, &QAction::triggered, [this]() {
        auto dialog = new FilterEditComplexMockup(this);
        dialog->exec();
    });   
    connect(ui->settingsAction, &QAction::triggered, [this]() {
        auto dialog = new AppSettingsDialog(this);
        dialog->exec();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTabContextMenuRequested(const QPoint &pos)
{
    QTabWidget* tabWidget = ui->customPanelTabWidget;

    QPoint globalPos = tabWidget->mapToGlobal(pos);
    int tabIndex = tabWidget->tabBar()->tabAt(tabWidget->tabBar()->mapFromGlobal(globalPos));
    if (tabIndex == -1) {
        return;
    }


    QMenu contextMenu(this);
    QAction *topAction = contextMenu.addAction("Расположить вверху (North)");
    QAction *bottomAction = contextMenu.addAction("Расположить внизу (South)");
    QAction *leftAction = contextMenu.addAction("Расположить слева (West)");
    QAction *rightAction = contextMenu.addAction("Расположить справа (East)");

    topAction->setCheckable(true);
    bottomAction->setCheckable(true);
    leftAction->setCheckable(true);
    rightAction->setCheckable(true);
    switch (ui->customPanelTabWidget->tabPosition())
    {
    case QTabWidget::North:
        topAction->setChecked(true);
        break;
    case QTabWidget::South:
        bottomAction->setChecked(true);
        break;
    case QTabWidget::West:
        leftAction->setChecked(true);
        break;
    case QTabWidget::East:
        rightAction->setChecked(true);
        break;
    }

    connect(topAction, &QAction::triggered, [this]() {
        ui->customPanelTabWidget->setTabPosition(QTabWidget::North);
    });
    connect(bottomAction, &QAction::triggered, [this]() {
        ui->customPanelTabWidget->setTabPosition(QTabWidget::South);
    });
    connect(leftAction, &QAction::triggered, [this]() {
        ui->customPanelTabWidget->setTabPosition(QTabWidget::West);
    });
    connect(rightAction, &QAction::triggered, [this]() {
        ui->customPanelTabWidget->setTabPosition(QTabWidget::East);
    });

    contextMenu.exec(tabWidget->mapToGlobal(pos));
}
