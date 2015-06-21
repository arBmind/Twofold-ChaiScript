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

#include "Twofold/intern/SourceMapTextBuilder.h"

#include <QString>

#include <vector>

namespace Twofold {
namespace intern {

enum class OriginTextType {
    Script, ScriptExpression, Target, IndentPart, PushIndention, PopIndention, NewLine
};

template<OriginTextType>
struct OriginTextTemplate {
    OriginText text;
};

using OriginScript = OriginTextTemplate<OriginTextType::Script>;
using OriginScriptExpression = OriginTextTemplate<OriginTextType::ScriptExpression>;
using OriginTarget = OriginTextTemplate<OriginTextType::Target>;
using IndentTargetPart = OriginTextTemplate<OriginTextType::IndentPart>;
using PushTargetIndentation = OriginTextTemplate<OriginTextType::PushIndention>;
using PopTargetIndentation = OriginTextTemplate<OriginTextType::PopIndention>;
using TargetNewLine = OriginTextTemplate<OriginTextType::NewLine>;

/**
 * @brief data structure representing the prepared ChaiScript
 */
struct PreparedChaiScript
{
    const QString chaiscript;
    const SourceMapping sourceMap;
    const FileLineColumnPositionList originPositions;
};

/**
 * @brief Builds template rendering ChaiScript and source maps
 *
 * builder class
 */
class PreparedChaiScriptBuilder
{
public:
    PreparedChaiScript build() const;

    inline const QString& indentation() const { return m_sourceMapBuilder.indentation(); }
    inline void setIndentation(const QString &indent) { m_sourceMapBuilder.setIndentation(indent); }

    PreparedChaiScriptBuilder &operator <<(const OriginScript& script);
    PreparedChaiScriptBuilder &operator <<(const OriginScriptExpression& expr);
    PreparedChaiScriptBuilder &operator <<(const OriginTarget& target);

    PreparedChaiScriptBuilder &operator <<(const IndentTargetPart& indent);
    PreparedChaiScriptBuilder &operator <<(const PushTargetIndentation& indent);
    PreparedChaiScriptBuilder &operator <<(const PopTargetIndentation &indent);

    PreparedChaiScriptBuilder &operator <<(const TargetNewLine newLine);

    PreparedChaiScriptBuilder &operator <<(const NewLine);

private:
    size_t addOriginPosition(const FileLineColumnPosition &position);

private:
    SourceMapTextBuilder m_sourceMapBuilder;
    FileLineColumnPositionList m_originPositions;
};

} // namespace intern
} // namespace Twofold

