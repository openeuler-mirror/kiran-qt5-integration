#include "scheme-loader-fetcher.h"

#include "kiran-palette.h"
#include "scheme-loader.h"

Kiran::Style::SchemeLoader* SchemeLoaderFetcher::getSchemeLoader()
{
    return  KiranPalette::instance()->getSchemeLoader();
}
