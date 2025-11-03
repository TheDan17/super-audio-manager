#ifndef MOCKDATAPROVIDER_H
#define MOCKDATAPROVIDER_H

#include <QVector>

#include "../items/modulemetainfo.h"


class MockDataProvider
{
public:
    MockDataProvider() = delete;

    static QVector<ModuleDistillMetaInfo> createMockModuleList();
};


#endif // MOCKDATAPROVIDER_H
