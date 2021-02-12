/*
 * SPDX-FileCopyrightText: 2006-2007 Cyrille Berger <cberger@cberger.net>
 * SPDX-FileCopyrightText: 2021 L. E. Segovia <amy@amyspark.me>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_GRAY_COLORSPACE_TRAITS_H_
#define _KO_GRAY_COLORSPACE_TRAITS_H_

/** 
 * Base class for graya traits, it provides some convenient functions to
 * access gray channels through an explicit API.
 */
#include "KoColorSpaceTraits.h"
template<typename _channels_type_>
struct KoGrayTraits : public KoColorSpaceTrait<_channels_type_, 2, 1> {
    
    using channels_type = _channels_type_;
    using parent = KoColorSpaceTrait<_channels_type_, 2, 1>;

    static constexpr qint32 gray_pos = 0;

    /**
     * An grayscale pixel
     */
    struct Pixel {
        channels_type gray;
        channels_type alpha;
    };

    /// @return the gray component
    inline static channels_type gray(quint8* data) {
        channels_type* d = parent::nativeArray(data);
        return d[gray_pos];
    }
    
    /// Set the gray component
    inline static void setGray(quint8* data, channels_type nv) {
        channels_type* d = parent::nativeArray(data);
        d[gray_pos] = nv;
    }
};


struct KoGrayU8Traits : public KoGrayTraits<quint8> {
};

struct KoGrayU16Traits : public KoGrayTraits<quint16> {
};

struct KoGrayU32Traits : public KoGrayTraits<quint32> {
};


#include <KoConfig.h>
#ifdef HAVE_OPENEXR
#include <half.h>

struct KoGrayF16Traits : public KoGrayTraits<half> {
};

#endif

struct KoGrayF32Traits : public KoGrayTraits<float> {
};

struct KoGrayF64Traits : public KoGrayTraits<double> {
};

#endif
