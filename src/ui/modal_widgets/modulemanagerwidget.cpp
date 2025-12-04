#include "modulemanagerwidget.h"
#include "ui_modulemanagerwidget.h"
#include "../../core/core.h"
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QToolButton>
#include <QScrollArea>

ModuleManagerWidget::ModuleManagerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ModuleManagerWidget)
{
    ui->setupUi(this);

    // Блокировка
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->checkBox->setChecked(true);
    ui->checkBox->setEnabled(false);

    // --- ПРАВКА 1: Убираем отступ (Spacer) и Группу ---
    ui->groupBox->setVisible(false);

    // В XML структуре в конце verticalLayout лежит Spacer.
    // Поскольку через ui-> он недоступен по имени, мы просто добавляем
    // наш виджет в этот же layout, и он растянется, придавив спейсер.
    // Но лучше всего - добавить ScrollArea с Stretch-фактором.

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* scrollContent = new QWidget();
    scrollContent->setStyleSheet("background-color: white;");
    QVBoxLayout* listLayout = new QVBoxLayout(scrollContent);
    listLayout->setSpacing(0);
    listLayout->setContentsMargins(0,0,0,0);
    scrollArea->setWidget(scrollContent);

    // Добавляем в 'verticalLayout' (тот, что внутри), а не в корень
    ui->verticalLayout->addWidget(scrollArea);

    // ВАЖНО: Заставляем ScrollArea занимать всё место, игнорируя старый спейсер
    ui->verticalLayout->setStretchFactor(scrollArea, 1);

    // ... (Код заголовка headerFrame и заполнения списка оставляем как в прошлом ответе) ...
    // Скопируйте код создания headerFrame и цикла for из предыдущего ответа сюда
    // ...

    // --- ЗАГОЛОВОК ---
    QFrame* headerFrame = new QFrame();
    headerFrame->setStyleSheet("background-color: #f0f0f0; border-bottom: 1px solid #ccc;");
    headerFrame->setFixedHeight(30);
    QGridLayout* headerGrid = new QGridLayout(headerFrame);
    headerGrid->setContentsMargins(10, 0, 10, 0);
    headerGrid->setVerticalSpacing(0);

    auto addHead = [&](QString t, int c) {
        QLabel* l = new QLabel(t);
        QFont f = l->font(); f.setBold(true); l->setFont(f);
        headerGrid->addWidget(l, 0, c);
    };

    addHead("Название", 2); addHead("Версия", 3); addHead("Вкл?", 4);
    addHead("API?", 5); addHead("Пан?", 6); addHead("Окн?", 7);
    headerGrid->setColumnStretch(2, 1);

    listLayout->addWidget(headerFrame);

    // --- СПИСОК ---
    auto modules = Core::instance().modules();
    for (const auto& mod : modules) {
        auto info = mod->toMetaInfo();
        QFrame* rowFrame = new QFrame();
        rowFrame->setFrameShape(QFrame::NoFrame);
        rowFrame->setStyleSheet("border-bottom: 1px solid #eee;");
        QGridLayout* grid = new QGridLayout(rowFrame);
        grid->setContentsMargins(10, 8, 10, 8);

        QToolButton* btnHelp = new QToolButton();
        btnHelp->setIcon(QIcon::fromTheme("help-about")); btnHelp->setAutoRaise(true);
        grid->addWidget(btnHelp, 0, 0);

        QToolButton* btnSet = new QToolButton();
        btnSet->setIcon(QIcon(":/icon/settings_x16.png")); if(btnSet->icon().isNull()) btnSet->setText("...");
        btnSet->setAutoRaise(true);
        grid->addWidget(btnSet, 0, 1);

        QWidget* nameWidget = new QWidget();
        QVBoxLayout* nameLayout = new QVBoxLayout(nameWidget);
        nameLayout->setContentsMargins(0,0,0,0); nameLayout->setSpacing(2);

        QLabel* lblName = new QLabel(info.localizedName);
        QFont fName = lblName->font(); fName.setPointSize(10); fName.setBold(true); lblName->setFont(fName);

        QWidget* nameRow = new QWidget();
        QHBoxLayout* nameRowL = new QHBoxLayout(nameRow);
        nameRowL->setContentsMargins(0,0,0,0); nameRowL->setSpacing(5);
        if (!info.icon.isNull()) {
            QLabel* ico = new QLabel(); ico->setPixmap(info.icon.pixmap(16,16)); ico->setFixedSize(16,16);
            nameRowL->addWidget(ico);
        }
        nameRowL->addWidget(lblName); nameRowL->addStretch();

        QLabel* lblDesc = new QLabel(info.localizedDescription);
        lblDesc->setStyleSheet("color: #666;"); lblDesc->setWordWrap(true);
        nameLayout->addWidget(nameRow); nameLayout->addWidget(lblDesc);
        grid->addWidget(nameWidget, 0, 2);

        QLabel* lblVer = new QLabel(info.version); grid->addWidget(lblVer, 0, 3);

        auto addCheck = [&](bool checked, bool enabled, int col) {
            QCheckBox* cb = new QCheckBox(); cb->setChecked(checked); cb->setEnabled(enabled);
            grid->addWidget(cb, 0, col);
        };
        addCheck(info.isEnabled, true, 4); addCheck(false, false, 5);
        addCheck(info.supportsPanel, false, 6); addCheck(info.supportsWindow, false, 7);

        grid->setColumnStretch(2, 1);
        listLayout->addWidget(rowFrame);
        m_rows.append({info.localizedName, rowFrame});
    }
    listLayout->addStretch();

    connect(ui->catalogFilterLineEdit, &QLineEdit::textChanged, this, [this](const QString& text){
        for(const auto& row : m_rows) {
            bool match = text.isEmpty() || row.name.contains(text, Qt::CaseInsensitive);
            row.frame->setVisible(match);
        }
    });
}

ModuleManagerWidget::~ModuleManagerWidget() { delete ui; }
