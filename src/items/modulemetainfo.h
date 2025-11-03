#ifndef MODULEMETAINFO_H
#define MODULEMETAINFO_H

#include <QString>
#include <QIcon>


struct ModuleDistillMetaInfo {
    QString uuid;
    QString stringId;
    QString version;

    QString localizedName;
    QString localizedDescription;
    QIcon icon;

    bool isEnabled;
    bool isPinned;

    bool supportsPanel;
    bool supportsWindow;
};


#endif // MODULEMETAINFO_H
