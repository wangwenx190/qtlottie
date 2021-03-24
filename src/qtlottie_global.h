#pragma once

#include <QtCore/qglobal.h>

#ifndef QTLOTTIE_API
#ifdef QTLOTTIE_STATIC
#define QTLOTTIE_API
#else
#ifdef QTLOTTIE_BUILD_LIBRARY
#define QTLOTTIE_API Q_DECL_EXPORT
#else
#define QTLOTTIE_API Q_DECL_IMPORT
#endif
#endif
#endif
