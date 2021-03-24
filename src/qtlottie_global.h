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

#if defined(Q_OS_WIN) && !defined(Q_OS_WINDOWS)
#define Q_OS_WINDOWS
#endif

#ifndef Q_DISABLE_MOVE
#define Q_DISABLE_MOVE(Class) \
    Class(Class &&) = delete; \
    Class &operator=(Class &&) = delete;
#endif

#ifndef Q_DISABLE_COPY_MOVE
#define Q_DISABLE_COPY_MOVE(Class) \
    Q_DISABLE_COPY(Class) \
    Q_DISABLE_MOVE(Class)
#endif
