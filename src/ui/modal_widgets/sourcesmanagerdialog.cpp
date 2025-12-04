#include "sourcesmanagerdialog.h"
#include "../../core/core.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QGroupBox>

SourcesManagerDialog::SourcesManagerDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Управление источниками");
    resize(500, 350);

    auto mainLayout = new QVBoxLayout(this);

    auto infoLabel = new QLabel("Здесь перечислены папки, которые сканируются для создания библиотеки.", this);
    infoLabel->setWordWrap(true);
    mainLayout->addWidget(infoLabel);

    auto group = new QGroupBox("Активные источники", this);
    auto groupLayout = new QVBoxLayout(group);

    m_list = new QListWidget(this);
    m_list->setAlternatingRowColors(true);
    groupLayout->addWidget(m_list);

    mainLayout->addWidget(group);

    auto btnLayout = new QHBoxLayout();
    auto btnAdd = new QPushButton(QIcon::fromTheme("list-add"), "Добавить...", this);
    auto btnRemove = new QPushButton(QIcon::fromTheme("list-remove"), "Удалить", this);
    auto btnClose = new QPushButton("Закрыть", this);

    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnRemove);
    btnLayout->addStretch();
    btnLayout->addWidget(btnClose);

    mainLayout->addLayout(btnLayout);

    // Логика
    connect(btnAdd, &QPushButton::clicked, this, [this](){
        QString dir = QFileDialog::getExistingDirectory(this, "Выберите папку с музыкой");
        if (!dir.isEmpty()) {
            Core::instance().addSource(dir);
            refreshList();
        }
    });

    connect(btnRemove, &QPushButton::clicked, this, [this](){
        QListWidgetItem* item = m_list->currentItem();
        if (item) {
            Core::instance().removeSource(item->text());
            refreshList();
        }
    });

    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);

    refreshList();
}

void SourcesManagerDialog::refreshList() {
    m_list->clear();
    QStringList sources = Core::instance().getSources();
    for(const auto& src : sources) {
        QListWidgetItem* item = new QListWidgetItem(QIcon::fromTheme("folder"), src);
        m_list->addItem(item);
    }
}
