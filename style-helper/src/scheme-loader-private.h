/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-qt5-integration is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */
#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_LIB_SRC_SCHEME_LOADER_PRIVATE_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_LIB_SRC_SCHEME_LOADER_PRIVATE_H_

#include <private/qcssparser_p.h>
#include <QReadWriteLock>

#include "scheme-loader.h"

namespace Kiran
{
namespace Style
{
class SchemeLoaderPrivate
{
    Q_DECLARE_PUBLIC(SchemeLoader)
public:
    SchemeLoaderPrivate(SchemeLoader* ptr)
        : q_ptr(ptr)
          ,m_styleScheme(new QCss::StyleSheet){};
    ~SchemeLoaderPrivate()
    {
        delete m_styleScheme;
    };

private:
    SchemeLoader* q_ptr;
    SchemeLoader::SchemeType m_schemeType;
    bool m_isValid = false;
    QCss::StyleSheet* m_styleScheme;
    QReadWriteLock m_rwLock;
    QMap<QString, QVariant> m_styleSchemeCache;
};
}  // namespace Style
}  // namespace Kiran

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_LIB_SRC_SCHEME_LOADER_PRIVATE_H_
