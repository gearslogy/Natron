/* ***** BEGIN LICENSE BLOCK *****
 * This file is part of Natron <http://www.natron.fr/>,
 * Copyright (C) 2016 INRIA and Alexandre Gauthier-Foichat
 *
 * Natron is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Natron is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Natron.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
 * ***** END LICENSE BLOCK ***** */

#ifndef NATRON_ENGINE_IMAGEPRIVATE_H
#define NATRON_ENGINE_IMAGEPRIVATE_H

// ***** BEGIN PYTHON BLOCK *****
// from <https://docs.python.org/3/c-api/intro.html#include-files>:
// "Since Python may define some pre-processor definitions which affect the standard headers on some systems, you must include Python.h before any standard headers are included."
#include <Python.h>
// ***** END PYTHON BLOCK *****

#include "Global/Macros.h"

#include <map>
#include <QMutex>
#include "Engine/AppManager.h"
#include "Engine/Cache.h"
#include "Engine/CacheEntryBase.h"
#include "Engine/CacheEntryKeyBase.h"
#include "Engine/EffectInstance.h"
#include "Engine/GPUContextPool.h"
#include "Engine/Image.h"
#include "Engine/ImageStorage.h"
#include "Engine/MultiThread.h"
#include "Engine/OSGLContext.h"
#include "Engine/OSGLFunctions.h"
#include "Engine/RectI.h"
#include "Engine/TimeValue.h"
#include "Engine/ViewIdx.h"

#include "Engine/EngineFwd.h"

// Define to print debug information about tiles caching
//#define DEBUG_TILES_ACCESS

NATRON_NAMESPACE_ENTER;


// Each tile with the coordinates of its lower left corner
// Each tile is aligned relative to (0,0):
// an image must at least contain a single tile with coordinates (0,0).
typedef std::map<TileCoord, Image::Tile, TileCoord_Compare> TileMap;


struct ImagePrivate
{
    Image* _publicInterface;

    // The rectangle where data are defined
    RectI originalBounds;

    // The actual rectangle of data. It might be slightly bigger than original bounds:
    // this is the original bounds rounded to the tile size.
    RectI boundsRoundedToTile;

    // Each individual tile storage
    TileMap tiles;

    // The size in pixels of a tile
    int tileSizeX, tileSizeY;

    // The layer represented by this image
    ImagePlaneDesc layer;

    // The proxy scale of the image
    RenderScale proxyScale;

    // The mipmap level of the image
    unsigned int mipMapLevel;

    // Controls the cache access for the image
    CacheAccessModeEnum cachePolicy;

    // The buffer format
    ImageBufferLayoutEnum bufferFormat;

    // This must be set if the cache policy is not none.
    // This will be used to prevent inserting in the cache part of images that had
    // their render aborted.
    EffectInstanceWPtr renderClone;

    // The channels enabled
    std::bitset<4> enabledChannels;

    // The bitdepth of the image
    ImageBitDepthEnum bitdepth;

    // The image storage type
    StorageModeEnum storage;

    // Protects tilesAllocated
    QMutex tilesAllocatedMutex;

    // If true, tiles are assumed to be allocated, otherwise only cached tiles hold memory
    bool tilesAllocated;

    // If true, throw a std::bad_alloc() as soon as an uncached tile is met
    bool failIfTileUncached;

    // The OpenGl context used is the image is stored in a texture
    OSGLContextPtr glContext;

    // The texture target if the image is stored in a texture
    U32 textureTarget;

    // The following values are passed to the ImageTileKey
    U64 nodeHash;
    bool isDraftImage;

    ImagePrivate(Image *publicInterface)
    : _publicInterface(publicInterface)
    , originalBounds()
    , boundsRoundedToTile()
    , tiles()
    , tileSizeX(0)
    , tileSizeY(0)
    , layer()
    , proxyScale(1.)
    , mipMapLevel(0)
    , cachePolicy(eCacheAccessModeNone)
    , bufferFormat(eImageBufferLayoutRGBAPackedFullRect)
    , renderClone()
    , enabledChannels()
    , bitdepth(eImageBitDepthNone)
    , storage(eStorageModeNone)
    , tilesAllocatedMutex()
    , tilesAllocated(false)
    , failIfTileUncached(false)
    , glContext()
    , textureTarget(0)
    , nodeHash(0)
    , isDraftImage(false)
    {

    }

    ActionRetCodeEnum init(const Image::InitStorageArgs& args);

    ActionRetCodeEnum initTiles();

    ActionRetCodeEnum initFromExternalBuffer(const Image::InitStorageArgs& args);

    void initTileChannelStorage(const CachePtr& cache, Image::Tile &tile, const std::vector<int>& channelIndices, std::size_t c);

    ActionRetCodeEnum initTileAndFetchFromCache(const TileCoord& coord, Image::Tile &tile);

    CacheEntryLocker::CacheEntryStatusEnum fetchBufferFromCacheInternal(const CachePtr& cache,
                                                                        const CacheImageTileStoragePtr& cacheBuffer,
                                                                        bool lookupDraft,
                                                                        unsigned int lookupMipMapLevel,
                                                                        U64 channelID,
                                                                        const std::string& pluginID,
                                                                        Image::Tile &tile,
                                                                        Image::MonoChannelTile& thisChannelTile);
    
    void fetchBufferFromCache(const CachePtr& cache,
                              const CacheImageTileStoragePtr& cacheBuffer,
                              U64 channelID,
                              int channelIndex,
                              const std::string& pluginID,
                              Image::Tile &tile,
                              Image::MonoChannelTile& thisChannelTile);

    /**
     * @brief Called in the destructor to insert tiles that were processed in the cache.
     **/
    void insertTilesInCache();

    /**
     * @brief Returns a rectangle of tiles coordinates that span the given rectangle of pixel coordinates
     **/
    RectI getTilesCoordinates(const RectI& pixelCoordinates) const;

    /**
     * @brief Helper to copy from a untiled image to a tiled image
     **/
    void copyUntiledImageToTiledImage(const Image& fromImage, const Image::CopyPixelsArgs& args);

    /**
     * @brief Helper to copy from a tiled image to a untiled image
     **/
    void copyTiledImageToUntiledImage(const Image& fromImage, const Image::CopyPixelsArgs& args);

    /**
     * @brief Helper to copy from an untiled image to another untiled image
     **/
    void copyUntiledImageToUntiledImage(const Image& fromImage, const Image::CopyPixelsArgs& args);

    /**
     * @brief The main entry point to copy image portions.
     * The storage may vary as well as the number of components and the bitdepth.
     **/
    static void copyRectangle(const Image::Tile& fromTile,
                              StorageModeEnum fromStorage,
                              ImageBufferLayoutEnum fromLayout,
                              const Image::Tile& toTile,
                              StorageModeEnum toStorage,
                              ImageBufferLayoutEnum toLayout,
                              const Image::CopyPixelsArgs& args,
                              const EffectInstancePtr& renderClone);

    /**
     * @brief If copying pixels from fromImage to toImage cannot be copied directly, this function
     * returns a temporary image that is suitable to copy then to the toImage.
     **/
    static ImagePtr checkIfCopyToTempImageIsNeeded(const Image& fromImage, const Image& toImage, const RectI& roi);

   

    /**
     * @brief This function can be used to convert CPU buffers with different
     * bitdepth or components.
     *
     * @param renderWindow The rectangle to convert
     *
     * @param srcColorSpace Input data will be taken to be in this color-space
     *
     * @param dstColorSpace Output data will be converted to this color-space.
     *
     *
     * @param conversionChannel A value in 0 <= index < 4 indicating the channel to use 
     * for the AlphaChannelHandlingEnum and MonoToPackedConversionEnum flags.
     *
     *
     * @param useAlpha0 If true, when creating an alpha channel the alpha will be filled with 0, otherwise filled with 1.
     *
     * @param requiresUnpremult If true, if a component conversion from RGBA to RGB happens
     * the RGB channels will be divided by the alpha channel when copied to the output image
     **/
    static void convertCPUImage(const RectI & renderWindow,
                                ViewerColorSpaceEnum srcColorSpace,
                                ViewerColorSpaceEnum dstColorSpace,
                                bool requiresUnpremult,
                                int conversionChannel,
                                Image::AlphaChannelHandlingEnum alphaHandling,
                                Image::MonoToPackedConversionEnum monoConversion,
                                const void* srcBufPtrs[4],
                                int srcNComps,
                                ImageBitDepthEnum srcBitDepth,
                                const RectI& srcBounds,
                                void* dstBufPtrs[4],
                                int dstNComps,
                                ImageBitDepthEnum dstBitDepth,
                                const RectI& dstBounds,
                                const EffectInstancePtr& renderClone);

    static void fillGL(const RectI & roi,
                       float r,
                       float g,
                       float b,
                       float a,
                       const GLImageStoragePtr& texture);

    static void fillCPU(void* ptrs[4],
                        float r,
                        float g,
                        float b,
                        float a,
                        int nComps,
                        ImageBitDepthEnum bitDepth,
                        const RectI& bounds,
                        const RectI& roi,
                        const EffectInstancePtr& renderClone);

    static void halveImage(const void* srcPtrs[4],
                           int nComps,
                           ImageBitDepthEnum bitdepth,
                           const RectI& srcBounds,
                           void* dstPtrs[4],
                           const RectI& dstBounds);

    static bool checkForNaNs(void* ptrs[4],
                             int nComps,
                             ImageBitDepthEnum bitdepth,
                             const RectI& bounds,
                             const RectI& roi);

    static void applyMaskMixGL(const GLImageStoragePtr& originalTexture,
                               const GLImageStoragePtr& maskTexture,
                               const GLImageStoragePtr& dstTexture,
                               double mix,
                               bool invertMask,
                               const RectI& roi);

    static void applyMaskMixCPU(const void* originalImgPtrs[4],
                                const RectI& originalImgBounds,
                                int originalImgNComps,
                                const void* maskImgPtrs[4],
                                const RectI& maskImgBounds,
                                void* dstImgPtrs[4],
                                ImageBitDepthEnum dstImgBitDepth,
                                int dstImgNComps,
                                double mix,
                                bool invertMask,
                                const RectI& dstBounds,
                                const RectI& roi,
                                const EffectInstancePtr& renderClone);

    static void copyUnprocessedChannelsGL(const GLImageStoragePtr& originalTexture,
                                          const GLImageStoragePtr& dstTexture,
                                          const std::bitset<4> processChannels,
                                          const RectI& roi);

    static void copyUnprocessedChannelsCPU(const void* originalImgPtrs[4],
                                           const RectI& originalImgBounds,
                                           int originalImgNComps,
                                           void* dstImgPtrs[4],
                                           ImageBitDepthEnum dstImgBitDepth,
                                           int dstImgNComps,
                                           const RectI& dstBounds,
                                           const std::bitset<4> processChannels,
                                           const RectI& roi,
                                           const EffectInstancePtr& renderClone);
    
    
};

NATRON_NAMESPACE_EXIT;

#endif // NATRON_ENGINE_IMAGEPRIVATE_H
