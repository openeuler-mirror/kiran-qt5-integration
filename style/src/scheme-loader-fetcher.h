//
// Created by liuxinhao on 2022/5/11.
//

#ifndef KIRAN_QT5_INTEGRATION_SCHEMELOADERFETCHER_H
#define KIRAN_QT5_INTEGRATION_SCHEMELOADERFETCHER_H

#include "scheme-loader.h"

namespace Kiran
{
class SchemeLoaderFetcher
{
public:
    static Kiran::SchemeLoader* getSchemeLoader();
};
}  // namespace Kiran

#endif  // KIRAN_QT5_INTEGRATION_SCHEMELOADERFETCHER_H
