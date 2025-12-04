#include "mainwindow.h"
#include "./main/ui_mainwindow.h"
#include "../core/core.h"
#include "../modal_widgets/sourcesmanagerdialog.h"
#include "../modal_widgets/filtereditdialog.h"
#include "../modal_widgets/appsettingsdialog.h"
#include "../modal_widgets/modulemanagerwidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTabBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // --- ПРАВКА 1: Чистим вкладки при запуске ---
    // Удаляем всё, кроме Каталога (индекс 0)
    while(ui->customPanelTabWidget->count() > 1) {
        ui->customPanelTabWidget->removeTab(1);
    }
    // Делаем Каталог незакрываемым (убираем кнопку закрытия на табе 0)
    ui->customPanelTabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);

    Core::instance().rescanLibrary();
    updateLibraryTable();
    updateFilterComboBox();

    // --- ПРАВКА 2: Каталог показывает реальные данные ---
    QVector<ModuleDistillMetaInfo> modules = Core::instance().getModulesMetaInfo();
    ui->catalogNonclosable->populate(modules);

    // Подключаем сигнал открытия модуля из каталога
    connect(ui->catalogNonclosable, &CatalogPanelWidget::requestOpenModule, this, &MainWindow::openModuleTab);

    // --- ПРАВКА 3: Обновление заголовка панели ---
    connect(ui->customPanelTabWidget, &QTabWidget::currentChanged, this, [this](int index){
        if (index == 0) {
            ui->customPanelBox->setTitle("<выберите панель>");
        } else {
            // Берем имя из текста вкладки и (можно было бы хранить версию в property)
            QString name = ui->customPanelTabWidget->tabText(index);
            // Для красоты добавим заглушку версии, если нет в property
            ui->customPanelBox->setTitle(name);
        }
    });

    // Обработка закрытия вкладок
    connect(ui->customPanelTabWidget, &QTabWidget::tabCloseRequested, this, [this](int index){
        if (index > 0) ui->customPanelTabWidget->removeTab(index);
    });

    // --- ПРАВКА 4: Очистка списка результатов фильтра ---
    ui->filterResultList->clear();
    ui->filterResultList->addItem("Точные совпадения (0)");

    // --- ПРАВКА 5: Фильтр "Все" ---
    connect(ui->filterComboBox, &QComboBox::currentIndexChanged, [this](int index){
        if (ui->filterComboBox->itemText(index) == "Все") {
            onFilterTextChanged(""); // Сбрасываем фильтр
        }
    });

    // Логика поиска
    connect(ui->filterComboBox, &QComboBox::editTextChanged, this, &MainWindow::onFilterTextChanged);


    // --- ПРАВКА 6: Источники ---
    connect(ui->sourcesManagerAction, &QAction::triggered, [this]() {
        SourcesManagerDialog dlg(this);
        dlg.exec();
        updateLibraryTable();
    });

    // --- ПРАВКА 7: Меню ---
    connect(ui->aboutProgramAction, &QAction::triggered, [this](){
        QMessageBox::about(this, "О программе",
                           "<h3>Продвинутый менеджер библиотеки музыки v0.1</h3>"
                           "<p>Курсовой проект по SRS.</p>"
                           "<p>Автор: Студент</p>");
    });

    connect(ui->shutdownProgramAction, &QAction::triggered, qApp, &QApplication::quit);

    // 7.1 Добавить панель
    ui->addPanelMenu->clear(); // Очищаем заглушки из дизайнера

    // Используем другое имя переменной (coreModules вместо modules)
    auto coreModules = Core::instance().modules();

    for (const auto& mod : coreModules) {
        if (mod->supportsGuiPanel()) {
            QAction* action = ui->addPanelMenu->addAction(mod->icon(), mod->localizedName());

            // Сохраняем uuid в локальную переменную для захвата
            QString modUuid = mod->uuid();

            // Привязываем открытие. Захватываем modUuid по значению.
            connect(action, &QAction::triggered, this, [this, modUuid](){
                openModuleTab(modUuid);
            });
        }
    }

    ui->addPanelMenu->addSeparator();
    QAction* disabledInfo = ui->addPanelMenu->addAction("Отключенные (0)");
    disabledInfo->setEnabled(false);


    // --- Остальные подключения (Плеер, Настройки) ---
    connect(ui->filterTable, &QTableWidget::cellDoubleClicked, [this](int row, int column) {
        QString filePath = ui->filterTable->item(row, 0)->data(Qt::UserRole).toString();

        // Ищем полный объект трека
        const auto& lib = Core::instance().getLibrary();
        AudioTrack t;
        for(const auto& lt : lib) {
            if (lt.filePath == filePath) { t = lt; break; }
        }

        if (!t.filePath.isEmpty()) {
            QVector<AudioTrack> newQueue;
            newQueue.append(t); // Только один трек!
            Core::instance().playQueue(newQueue, 0); // Играть с 0 позиции
            ui->statusbar->showMessage("Играет: " + t.fileName);
        }
    });

    connect(ui->modulesManagerAction, &QAction::triggered, [this]() {
        AppSettingsDialog dlg(this);
        dlg.exec();
    });

    connect(ui->settingsAction, &QAction::triggered, [this]() {
        AppSettingsDialog dlg(this); dlg.exec();
    });
    // Меню модулей тоже открывает настройки
    connect(ui->modulesManagerAction, &QAction::triggered, [this]() {
        AppSettingsDialog dlg(this); dlg.exec();
    });

    connect(ui->catalogNonclosable, &CatalogPanelWidget::requestOpenSettings, this, [this](){
        AppSettingsDialog dlg(this);
        dlg.exec();
    });

    updateFilterComboBox();
    loadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

#include <QCloseEvent>
void MainWindow::closeEvent(QCloseEvent *event) {
    saveSettings(); // Сохраняем перед выходом
    event->accept();
}

// Реализация открытия модуля
void MainWindow::openModuleTab(QString uuid) {
    auto modules = Core::instance().modules();
    std::shared_ptr<IModule> targetModule = nullptr;
    for(const auto& m : modules) {
        if (m->uuid() == uuid) { targetModule = m; break; }
    }

    if (!targetModule) return;

    QWidget* widget = targetModule->createPanelWidget(this);
    if (!widget) {
        QMessageBox::warning(this, "Ошибка", "Модуль не поддерживает панель.");
        return;
    }

    // --- ПРАВКА 3 (Часть 2): Подключаем кнопку "Из менеджера" ---
    if (auto* playerWidget = qobject_cast<PlayerPanelWidget*>(widget)) {
        connect(playerWidget, &PlayerPanelWidget::requestAddFromManager, this, [this](){
            // Получаем выделенные строки
            QList<QTableWidgetItem*> selectedItems = ui->filterTable->selectedItems();
            QSet<int> uniqueRows;

            // Собираем уникальные индексы строк
            for(auto* item : selectedItems) {
                if (!ui->filterTable->isRowHidden(item->row())) {
                    uniqueRows.insert(item->row());
                }
            }

            if (uniqueRows.isEmpty()) {
                QMessageBox::information(this, "Инфо", "Сначала выделите треки в таблице!");
                return;
            }

            QVector<AudioTrack> newQueue;
            const auto& lib = Core::instance().getLibrary();

            // Сортируем индексы, чтобы порядок был как в таблице
            QList<int> sortedRows = uniqueRows.values();
            std::sort(sortedRows.begin(), sortedRows.end());

            for(int row : sortedRows) {
                QString path = ui->filterTable->item(row, 0)->data(Qt::UserRole).toString();
                // Ищем трек в либе
                for(const auto& t : lib) {
                    if (t.filePath == path) {
                        newQueue.append(t);
                        break;
                    }
                }
            }

            // ЗАМЕНЯЕМ очередь и начинаем играть первый
            Core::instance().playQueue(newQueue, 0);
            ui->statusbar->showMessage(QString("Очередь заменена (%1 треков)").arg(newQueue.size()));
        });
    }

    int idx = ui->customPanelTabWidget->addTab(widget, targetModule->icon(), targetModule->localizedName());
    ui->customPanelTabWidget->setCurrentIndex(idx);
    ui->customPanelBox->setTitle(targetModule->localizedName() + " (v" + targetModule->version() + ")");
}

void MainWindow::updateLibraryTable() {
    const auto& library = Core::instance().getLibrary();
    ui->filterTable->setRowCount(0);
    ui->filterTable->setRowCount(library.size());
    ui->filterTable->setSortingEnabled(false);

    for (int i = 0; i < library.size(); ++i) {
        const auto& track = library[i];

        auto* itemLink = new QTableWidgetItem(QString::number(i + 1));
        itemLink->setData(Qt::UserRole, track.filePath);
        ui->filterTable->setItem(i, 0, itemLink);

        ui->filterTable->setItem(i, 1, new QTableWidgetItem(QString::number(i + 1)));
        ui->filterTable->setItem(i, 2, new QTableWidgetItem(track.title));
        ui->filterTable->setItem(i, 3, new QTableWidgetItem("-:--"));
        ui->filterTable->setItem(i, 4, new QTableWidgetItem(track.artist));
        ui->filterTable->setItem(i, 5, new QTableWidgetItem(track.album));
        ui->filterTable->setItem(i, 6, new QTableWidgetItem(track.extension.toUpper()));

        // --- ПРАВКА 2: Дату выхода (Год) ---
        ui->filterTable->setItem(i, 7, new QTableWidgetItem(track.year));
    }
    ui->filterTable->setSortingEnabled(true);
    ui->filterResultList->clear();
    ui->filterResultList->addItem(QString("Точные совпадения (%1)").arg(library.size()));
}

void MainWindow::onFilterTextChanged(const QString &text) {
    QString pattern = text;
    if (pattern == "Все") pattern = "";
    int rows = ui->filterTable->rowCount();
    int matchCount = 0;
    for (int i = 0; i < rows; ++i) {
        QString name = ui->filterTable->item(i, 2)->text();
        QString artist = ui->filterTable->item(i, 4)->text();
        bool match = pattern.isEmpty() || name.contains(pattern, Qt::CaseInsensitive) || artist.contains(pattern, Qt::CaseInsensitive);
        ui->filterTable->setRowHidden(i, !match);
        if (match) matchCount++;
    }
    ui->filterResultList->clear();
    ui->filterResultList->addItem(QString("Точные совпадения (%1)").arg(matchCount));
}

void MainWindow::updateFilterComboBox() {
    ui->filterComboBox->clear();
    ui->filterComboBox->addItem("Все");
    auto filters = Core::instance().getFilters();
    for(const auto& f : filters) ui->filterComboBox->addItem(f.name);
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

void MainWindow::loadSettings() {
    QSettings settings("AudioLab", "MusicManager");

    // Геометрия окна
    QByteArray geometry = settings.value("ui/windowGeometry").toByteArray();
    if (!geometry.isEmpty()) restoreGeometry(geometry);

    // Состояние сплиттера
    QByteArray splitterState = settings.value("ui/splitterState").toByteArray();
    if (!splitterState.isEmpty()) ui->splitter->restoreState(splitterState);
}

void MainWindow::saveSettings() {
    QSettings settings("AudioLab", "MusicManager");
    settings.setValue("ui/windowGeometry", saveGeometry());
    settings.setValue("ui/splitterState", ui->splitter->saveState());
}
