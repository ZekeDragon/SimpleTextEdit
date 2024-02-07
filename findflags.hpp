/***********************************************************************************************************************
** The Simple Qt Text Editor Application
** findflags.hpp
** Copyright (C) 2024 Ezekiel Oruven
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
** rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
** Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
** WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
** COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
** OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***********************************************************************************************************************/
#pragma once

#include <bitset>

#include <QMetaType>

typedef std::bitset<5> FindFlags;
Q_DECLARE_METATYPE(FindFlags)

namespace FFlags
{
[[maybe_unused]] constexpr FindFlags None =                0;
[[maybe_unused]] constexpr FindFlags FindBackward =	       1 << 0;
[[maybe_unused]] constexpr FindFlags FindCaseSensitively = 1 << 1;
[[maybe_unused]] constexpr FindFlags FindWholeWords =      1 << 2;
[[maybe_unused]] constexpr FindFlags FindByRegex =         1 << 3;
[[maybe_unused]] constexpr FindFlags WrapAround =          1 << 4;
}
