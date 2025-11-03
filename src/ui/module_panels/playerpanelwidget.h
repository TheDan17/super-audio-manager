#ifndef PLAYERPANELWIDGET_H
#define PLAYERPANELWIDGET_H

#include <QWidget>

namespace Ui {
class PlayerPanelWidget;
}

class PlayerPanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerPanelWidget(QWidget *parent = nullptr);
    ~PlayerPanelWidget();

private:
    Ui::PlayerPanelWidget *ui;
};

#endif // PLAYERPANELWIDGET_H
