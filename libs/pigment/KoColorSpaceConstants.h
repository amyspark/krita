/*
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.bet
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KO_COLOR_SPACE_CONSTANTS_H_
#define _KO_COLOR_SPACE_CONSTANTS_H_

#include <limits>
#include <QtGlobal>

// TODO: find a better place or way to define those stuff
static constexpr quint8 OPACITY_TRANSPARENT_U8 = std::numeric_limits<quint8>::min();
static constexpr quint8 OPACITY_OPAQUE_U8 = std::numeric_limits<quint8>::max();
static constexpr qreal OPACITY_TRANSPARENT_F = 0.0;
static constexpr qreal OPACITY_OPAQUE_F = 1.0;

#endif
