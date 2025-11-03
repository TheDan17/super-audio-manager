#include "playerpanelwidget.h"
#include "ui_playerpanelwidget.h"

PlayerPanelWidget::PlayerPanelWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerPanelWidget)
{
    ui->setupUi(this);
}

PlayerPanelWidget::~PlayerPanelWidget()
{
    delete ui;
}
