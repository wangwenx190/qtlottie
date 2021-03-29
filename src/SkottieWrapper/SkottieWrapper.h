#ifndef SKOTTIEWRAPPER_H
#define SKOTTIEWRAPPER_H

#ifdef __cplusplus
#define SKOTTIE_EXTERN extern "C"
#else
#define SKOTTIE_EXTERN
#endif

#ifdef SKOTTIEWRAPPER_LIBRARY_BUILD
#define SKOTTIE_API SKOTTIE_EXTERN __declspec(dllexport)
#else
#define SKOTTIE_API SKOTTIE_EXTERN __declspec(dllimport)
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
 *  @ingroup Skottie_Animation
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
 *  @ingroup Skottie_Animation
 *  @internal
 */
SKOTTIE_API
Skottie_Animation* skottie_animation_from_data(void *data, size_t data_size, const char *resource);


/**
 *  @brief Returns default viewport size of the Lottie resource.
 *
 *  @param[in] animation Animation object.
 *  @param[out] w default width of the viewport.
 *  @param[out] h default height of the viewport.
 *
 *  @ingroup Skottie_Animation
 *  @internal
 */
SKOTTIE_API
void skottie_animation_get_size(const Skottie_Animation *animation, size_t *width, size_t *height);

/**
 *  @brief Returns total animation duration of Lottie resource in second.
 *         it uses totalFrame() and frameRate() to calculate the duration.
 *         duration = totalFrame() / frameRate().
 *
 *  @param[in] animation Animation object.
 *
 *  @return total animation duration in second.
 *          @c 0 if the Lottie resource has no animation.
 *
 *  @see skottie_animation_get_totalframe()
 *  @see skottie_animation_get_framerate()
 *
 *  @ingroup Skottie_Animation
 *  @internal
 */
SKOTTIE_API
double skottie_animation_get_duration(const Skottie_Animation *animation);

/**
 *  @brief Returns total number of frames present in the Lottie resource.
 *
 *  @param[in] animation Animation object.
 *
 *  @return frame count of the Lottie resource.*
 *
 *  @note frame number starts with 0.
 *
 *  @see skottie_animation_get_duration()
 *  @see skottie_animation_get_framerate()
 *
 *  @ingroup Skottie_Animation
 *  @internal
 */
SKOTTIE_API
size_t skottie_animation_get_totalframe(const Skottie_Animation *animation);

/**
 *  @brief Returns default framerate of the Lottie resource.
 *
 *  @param[in] animation Animation object.
 *
 *  @return framerate of the Lottie resource
 *
 *  @ingroup Skottie_Animation
 *  @internal
 *
 */
SKOTTIE_API
double skottie_animation_get_framerate(const Skottie_Animation *animation);

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
 *  @brief Constructs an pixmap object with width,height.
 *
 *  @param[in] width the width of pixmap.
 *  @param[in] height the height of pixmap.
 *  @param[in] buffer to store pixmap data
 *
 *  @return Pixmap object that store frame data
 *
 *  @see skottie_delete_pixmap()
 *
 *  @ingroup Skottie_Pixmap
 *  @internal
 */
SKOTTIE_API
Skottie_Pixmap* skottie_new_pixmap_wh(size_t width, size_t height, void *buffer);


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
 *  @ingroup Skottie_Animation
 *  @internal
 */
SKOTTIE_API void skottie_animation_render(Skottie_Animation *animation,
                                          size_t frame_num,
                                          Skottie_Pixmap *pixmap);

/**
 *  @brief Request to render scale the content of the frame @p frame_num to buffer @p buffer.
 *
 *
 *  @param[in] animation Animation object.
 *  @param[in] frame_num the frame number needs to be rendered.
 *  @param[in] pixmap surface buffer use for rendering.
 *
 *
 *  @ingroup Skottie_Animation
 *  @internal
 */
SKOTTIE_API void skottie_animation_render_scale(Skottie_Animation *animation,
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
 *  @ingroup Skottie_Animation
 *  @internal
 */
SKOTTIE_API
void skottie_animation_destroy(Skottie_Animation *animation);

#endif // SKOTTIEWRAPPER_H
