#include "scheme-loader-fetcher.h"

#include "scheme-loader.h"
#include "style-palette.h"

namespace Kiran
{
SchemeLoader* SchemeLoaderFetcher::getSchemeLoader()
{
    return StylePalette::instance()->getSchemeLoader();
}
}  // namespace Kiran
