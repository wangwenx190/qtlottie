#ifndef SKOTTIEWRAPPER_H
#define SKOTTIEWRAPPER_H

#ifdef SKOTTIEWRAPPER_LIBRARY_BUILD
#define SKOTTIE_API __declspec(dllexport)
#else
#define SKOTTIE_API __declspec(dllimport)
#endif

typedef void Skottie_Animation;

typedef void Skottie_Pixmap;

/**
 *  @brief Constructs an animation object from file path.
 *
 *  @param[in] path Lottie file path
 *  @param[in] resource Lottie resource path
 *
 *  @return Animation object that can build the contents of the
 *          Lottie resource represented by resource path.
 *
 *  @see skottie_animation_destroy()
 *
 *  @ingroup Lottie_Animation
 *  @internal
 */
SKOTTIE_API
Skottie_Animation* skottie_animation_from_file(const char* path, const char* resource);


/**
 *  @brief Constructs an animation object from JSON string data.
 *
 *  @param[in] data The JSON string data.
 *  @param[in] data_size the size of the JSON string data.
 *  @param[in] resource the path that will be used to load external resource needed by the JSON data.
 *
 *  @return Animation object that can build the contents of the
 *          Lottie resource represented by JSON string data.
 *
 *  @ingroup Lottie_Animation
 *  @internal
 */
SKOTTIE_API
Skottie_Animation* skottie_animation_fromdata(void *data, size_t data_size, const char *resource);

/**
 *  @brief Constructs an pixmap object.
 *
 *  @return Pixmap object that store frame data
 *
 *  @see skottie_delete_pixmap()
 *
 *  @ingroup Skottie_Pixmap
 *  @internal
 */
SKOTTIE_API
Skottie_Pixmap* skottie_new_pixmap();


/**
 *  @brief Get pixmap buffer from a Skottie_Pixmap object.
 *         the buffer is store with argb format, Don't free
 *         the buffer manaualy
 *
 *  @return Pixmap object that store frame data
 *
 *  @ingroup Skottie_Pixmap
 *  @internal
 */
SKOTTIE_API
const void* skottie_get_pixmap_buffer(Skottie_Pixmap* pixmap);


/**
 *  @brief Free an pixmap object.
 *
 *  @see skottie_new_pixmap()
 *
 *  @ingroup Skottie_Pixmap
 *  @internal
 */
SKOTTIE_API
void skottie_delete_pixmap(Skottie_Pixmap* pixmap);

/**
 *  @brief Request to render the content of the frame @p frame_num to buffer @p buffer.
 *
 *  @param[in] animation Animation object.
 *  @param[in] frame_num the frame number needs to be rendered.
 *  @param[in] pixmap surface buffer use for rendering.
 *
 *
 *  @ingroup Lottie_Animation
 *  @internal
 */
RLOTTIE_API void skottie_animation_render(Skottie_Animation *animation,
                                          size_t frame_num,
                                          Skottie_Pixmap *pixmap);

/**
 *  @brief Free given Animation object resource.
 *
 *  @param[in] animation Animation object to free.
 *
 *  @see lottie_animation_from_file()
 *  @see lottie_animation_from_data()
 *
 *  @ingroup Lottie_Animation
 *  @internal
 */
SKOTTIE_API
void skottie_animation_destroy(Skottie_Animation *animation);

#endif // SKOTTIEWRAPPER_H
