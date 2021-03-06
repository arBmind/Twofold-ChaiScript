/* Twofold-ChaiScript
 * (C) Copyright 2015 HicknHack Software GmbH
 *
 * The original code can be found at:
 *     https://github.com/arBmind/Twofold-ChaiScript
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "Twofold/SourceMap.h"
#include "Twofold/intern/TextBuilder.h"
#include "Twofold/intern/LineProcessor.h"

namespace Twofold {
namespace intern {

/**
 * @brief data for a originated text span
 */
struct OriginText
{
    inline OriginText(FileLineColumnPosition origin,
                      TextSpan span,
                      Interpolation interpolation = Interpolation::OneToOne)
        : origin(origin), span(span), interpolation(interpolation) {}

    /// construct from a FileLine of a LineProcessor
    inline OriginText(const FileLine &line,
                      TextSpan span,
                      Interpolation interpolation = Interpolation::OneToOne)
        : origin({ line.position.name, {line.position.line, 1 + static_cast<int>(span.begin - line.begin)}})
        , span(span), interpolation(interpolation)
    {}

    FileLineColumnPosition origin; // position this entry is from
    TextSpan span; // text added to output
    Interpolation interpolation; // text generated or from source
};

struct SourceMapText
{
    const SourceMapping sourceMap;
    const QString text;
};

/**
 * @brief Builds Text and SourceMap
 *
 * makes sure both are consistent
 */
class SourceMapTextBuilder
{
public:
    SourceMapText build() const;

    inline bool isBlankLine() const { return m_textBuilder.isBlankLine(); }
    inline const QString& indentation() const { return m_textBuilder.indentation(); }
    inline void setIndentation(const QString &indent) { m_textBuilder.setIndentation(indent); }

    void pushCaller(const FileLineColumnPosition& originPosition);
    void popCaller();

    SourceMapTextBuilder& operator << (const OriginText& originText);
    SourceMapTextBuilder& operator << (const NewLine);

private:
    TextBuilder m_textBuilder;
    SourceData m_sourceData;
    std::vector< CallerIndex > m_callerIndexStack;
};

} // namespace intern
} // namespace Twofold

