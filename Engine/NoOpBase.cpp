/* ***** BEGIN LICENSE BLOCK *****
 * This file is part of Natron <http://www.natron.fr/>,
 * Copyright (C) 2013-2017 INRIA and Alexandre Gauthier-Foichat
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

// ***** BEGIN PYTHON BLOCK *****
// from <https://docs.python.org/3/c-api/intro.html#include-files>:
// "Since Python may define some pre-processor definitions which affect the standard headers on some systems, you must include Python.h before any standard headers are included."
#include <Python.h>
// ***** END PYTHON BLOCK *****

#include "NoOpBase.h"

#include <cassert>
#include <stdexcept>

#include "Engine/Transform.h" // Matrix3x3
#include "Engine/ViewIdx.h"

NATRON_NAMESPACE_ENTER;

NoOpBase::NoOpBase(const NodePtr& n)
    : EffectInstance(n)
{
}

NoOpBase::NoOpBase(const EffectInstancePtr& mainInstance, const TreeRenderPtr& render)
: EffectInstance(mainInstance, render)
{

}

void
NoOpBase::addAcceptedComponents(int /*inputNb*/,
                                std::bitset<4>* supported)
{
    (*supported)[0] = (*supported)[1] = (*supported)[2] = (*supported)[3] = 1;
}

void
NoOpBase::addSupportedBitDepth(std::list<ImageBitDepthEnum>* depths) const
{
    depths->push_back(eImageBitDepthByte);
    depths->push_back(eImageBitDepthShort);
    depths->push_back(eImageBitDepthFloat);
}

ActionRetCodeEnum
NoOpBase::isIdentity(TimeValue time,
                     const RenderScale & /*scale*/,
                     const RectI & /*roi*/,
                     ViewIdx view,
                     TimeValue* inputTime,
                     ViewIdx* inputView,
                     int* inputNb)
{
    *inputTime = time;
    *inputNb = 0;
    *inputView = view;

    return eActionStatusOK;
}

bool
NoOpBase::isHostChannelSelectorSupported(bool* /*defaultR*/,
                                         bool* /*defaultG*/,
                                         bool* /*defaultB*/,
                                         bool* /*defaultA*/) const
{
    return false;
}

NATRON_NAMESPACE_EXIT;
