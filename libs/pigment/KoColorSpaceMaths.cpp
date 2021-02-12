/*
 * SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 * SPDX-FileCopyrightText: 2021 L. E. Segovia <amy@amyspark.me>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <KoColorSpaceMaths.h>

#include <QtGlobal>

#ifdef HAVE_OPENEXR
const half KoColorSpaceMathsTraits<half>::zeroValue = 0.0;
const half KoColorSpaceMathsTraits<half>::unitValue = 1.0;
const half KoColorSpaceMathsTraits<half>::halfValue = 0.5;
const half KoColorSpaceMathsTraits<half>::max = HALF_MAX;
const half KoColorSpaceMathsTraits<half>::min = -HALF_MAX;
const half KoColorSpaceMathsTraits<half>::epsilon = HALF_EPSILON;
#endif

const Ko::FullLut< KoIntegerToFloat<quint16>, float, quint16> KoLuts::Uint16ToFloat;
const Ko::FullLut< KoIntegerToFloat<quint8>, float, quint8> KoLuts::Uint8ToFloat;
