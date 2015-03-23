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
#include "ChaiScriptTargetBuilderApi.h"

namespace Twofold {
namespace intern {

namespace {

template< typename T, typename S>
std::pair<T,S> make_pair(const T& t, const S& s) {
    return std::pair<T,S>(t, s);
}

} // namespace

ChaiScriptTargetBuilderApi::ChaiScriptTargetBuilderApi(const FileLineColumnPositionList &originPositions)
    : m_originPositions(originPositions)
{}

void ChaiScriptTargetBuilderApi::append(const QString &text, int originIndex)
{
    if (!text.isEmpty()) {
        m_sourceMapBuilder << OriginText { m_originPositions[originIndex], text, Interpolation::None };
    }
}

void ChaiScriptTargetBuilderApi::newLine()
{
    m_sourceMapBuilder << NewLine();
}

void ChaiScriptTargetBuilderApi::pushIndentation(const QString &indent, int originIndex)
{
    QString fullIndent = indent;
    if (!m_indentationStack.empty()) fullIndent.prepend(m_indentationStack.back().second);
    m_indentationStack.push_back(make_pair(indent, fullIndent));
    m_sourceMapBuilder.pushCaller(m_originPositions[originIndex]);
    m_sourceMapBuilder.setIndentation(fullIndent);
}

void ChaiScriptTargetBuilderApi::popIndentation()
{
    m_indentationStack.pop_back();
    m_sourceMapBuilder.popCaller();

    QString newIndent = m_indentationStack.empty() ? QString() : m_indentationStack.back().second;
    m_sourceMapBuilder.setIndentation(newIndent);
}

void ChaiScriptTargetBuilderApi::indentPart(const QString &indent, int originIndex)
{
    if (m_sourceMapBuilder.isBlankLine()) {
        m_partIndent = indent;
    }
    m_sourceMapBuilder << OriginText { m_originPositions[originIndex], indent, Interpolation::None };
}

void ChaiScriptTargetBuilderApi::pushPartIndent(int originIndex)
{
    pushIndentation(m_partIndent, originIndex);
}

void ChaiScriptTargetBuilderApi::popPartIndent()
{
    m_partIndent = m_indentationStack.back().first;
    popIndentation();
}

} // namespace intern
} // namespace Twofold
