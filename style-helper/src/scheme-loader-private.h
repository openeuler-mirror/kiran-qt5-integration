//
// Created by lxh on 2021/12/10.
//

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
