#include "SkottieWrapper.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkGraphics.h"
#include "include/core/SkPictureRecorder.h"
#include "include/core/SkStream.h"
#include "include/core/SkSurface.h"
#include  "include/core/SkImageInfo.h"
#include "src/utils/SkOSPath.h"
#include "include/encode/SkPngEncoder.h"
#include "modules/skottie/include/Skottie.h"
#include <windows.h>
struct Skottie_Animation_Private
{
    sk_sp<skresources::CachingResourceProvider> rp;
    sk_sp<skottie::Animation> ani;
    sk_sp<SkSurface> surface;

};


Skottie_Animation* skottie_animation_from_file(const char* path, const char* resource)
{
    Skottie_Animation_Private *ani = new Skottie_Animation_Private{};
    if(!ani)
        return nullptr;

    ani->rp = skresources::CachingResourceProvider::Make(
            skresources::DataURIResourceProviderProxy::Make(
                skresources::FileResourceProvider::Make(SkOSPath::Dirname(resource),
                    /*predecode=*/true), /*predecode=*/true));

    ani->ani = skottie::Animation::Builder()
            .setResourceProvider(ani->rp)
            .makeFromFile(path);

    SkSize aniSize = ani->ani->size();
    ani->surface = SkSurface::MakeRasterN32Premul(aniSize.width(), aniSize.height());

    return reinterpret_cast<Skottie_Animation*>(ani);
}

Skottie_Animation* skottie_animation_from_data(void *data, size_t data_size, const char *resource)
{
    Skottie_Animation_Private *ani = new Skottie_Animation_Private{};
    if(!ani)
        return nullptr;

    ani->rp = skresources::CachingResourceProvider::Make(
            skresources::DataURIResourceProviderProxy::Make(
                skresources::FileResourceProvider::Make(SkOSPath::Dirname(resource),
                    /*predecode=*/true), /*predecode=*/true));

    ani->ani = skottie::Animation::Builder()
            .setResourceProvider(ani->rp)
            .make(reinterpret_cast<const char*>(data), data_size);

    SkSize aniSize = ani->ani->size();
    ani->surface = SkSurface::MakeRasterN32Premul(aniSize.width(), aniSize.height());

    return reinterpret_cast<Skottie_Animation*>(ani);
}

void skottie_animation_get_size(const Skottie_Animation *animation, size_t *width, size_t *height)
{
    const Skottie_Animation_Private *pAni =
            reinterpret_cast<const Skottie_Animation_Private*>(animation);
    auto view_size = pAni->ani->size();
    *width = static_cast<size_t>(view_size.width());
    *height = static_cast<size_t>(view_size.height());
    return;
}

double skottie_animation_get_duration(const Skottie_Animation *animation)
{
    const Skottie_Animation_Private *pAni =
            reinterpret_cast<const Skottie_Animation_Private*>(animation);
    return pAni->ani->duration();
}

size_t skottie_animation_get_totalframe(const Skottie_Animation *animation)
{
    const Skottie_Animation_Private *pAni =
            reinterpret_cast<const Skottie_Animation_Private*>(animation);
    return static_cast<size_t>(pAni->ani->duration() * pAni->ani->fps());
}

double skottie_animation_get_framerate(const Skottie_Animation *animation)
{
    const Skottie_Animation_Private *pAni =
            reinterpret_cast<const Skottie_Animation_Private*>(animation);
    return pAni->ani->fps();
}

Skottie_Pixmap* skottie_new_pixmap()
{
    return  reinterpret_cast<Skottie_Pixmap*>(new SkPixmap{});
}

Skottie_Pixmap* skottie_new_pixmap_wh(size_t width, size_t height, void *buffer)
{
    auto imgInfo = SkImageInfo::Make(width, height, SkColorType::kBGRA_8888_SkColorType, SkAlphaType::kUnpremul_SkAlphaType);
    return  reinterpret_cast<Skottie_Pixmap*>(new SkPixmap(imgInfo, buffer, width * 4));
}

const void* skottie_get_pixmap_buffer(Skottie_Pixmap* pixmap)
{
    SkPixmap *skPixmap = reinterpret_cast<SkPixmap*>(pixmap);
    return reinterpret_cast<const void*>(skPixmap->addr());
}

void skottie_delete_pixmap(Skottie_Pixmap* pixmap)
{
    delete reinterpret_cast<SkPixmap*>(pixmap);
}

void skottie_animation_render(Skottie_Animation *animation,
                                         size_t frame_num,
                                 Skottie_Pixmap *pixmap)
{
    if(!animation)
        return;
    Skottie_Animation_Private *pAni =
            reinterpret_cast<Skottie_Animation_Private*>(animation);
    auto canvas = pAni->surface->getCanvas();
    if(!canvas)
        return;
    canvas->clear(SkColorSetARGB(0,0,0,0));

    pAni->ani->seekFrame(static_cast<double>(frame_num));
    pAni->ani->render(canvas);

    auto img = pAni->surface->makeImageSnapshot();
    img->peekPixels(reinterpret_cast<SkPixmap*>(pixmap));
    return;
}

void skottie_animation_render_scale(Skottie_Animation *animation,
                                            size_t frame_num,
                                    Skottie_Pixmap *pixmap)
{
    if(!animation)
        return;
    Skottie_Animation_Private *pAni =
            reinterpret_cast<Skottie_Animation_Private*>(animation);
    auto canvas = pAni->surface->getCanvas();

    if(!canvas)
        return;
    canvas->clear(SkColorSetARGB(0,0,0,0));

    pAni->ani->seekFrame(static_cast<double>(frame_num));
    pAni->ani->render(canvas);

    auto img = pAni->surface->makeImageSnapshot();
    SkSamplingOptions opt(SkFilterMode::kNearest, SkMipmapMode::kNone);
    img->scalePixels(*reinterpret_cast<SkPixmap*>(pixmap), opt);
    return;
}

void skottie_animation_destroy(Skottie_Animation *animation)
{
    delete reinterpret_cast<Skottie_Animation_Private*>(animation);
}
