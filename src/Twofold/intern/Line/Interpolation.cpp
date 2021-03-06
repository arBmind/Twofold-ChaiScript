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
#include "Interpolation.h"

#include "Twofold/intern/PreparedChaiScriptBuilder.h"
#include "Twofold/intern/LineProcessor.h"

#include "Twofold/intern/QStringHelper.h"
#include "Twofold/intern/QCharHelper.h"

#include "Twofold/intern/Script/BraceCounter.h"

#include <algorithm>

namespace Twofold {
namespace intern {
namespace Line {

#define HASH '#'
#define CURLY_OPEN '{'

namespace {

using BraceCounter = Twofold::intern::Script::BraceCounter;
inline bool isHash(QChar chr) { return chr == HASH; }

void reportError(const MessageHandlerPtr& messageHandler, const FileLine &line, const QString &message)
{
    messageHandler->templateMessage(MessageType::Error, line.position, message);
}

} // namespace

Interpolation::Interpolation(const MessageHandlerPtr &messageHandler, PreparedChaiScriptBuilder &builder)
    : m_messageHandler(messageHandler)
    , m_builder(builder)
{
}

void Interpolation::operator()(const FileLine &line) const
{
    const auto beginIndent = line.firstNonSpace+1;
    auto begin = std::find_if_not(beginIndent, line.end, QCharHelper::isSpace);
    m_builder << IndentTargetPart {{line, TextSpan {beginIndent, begin}}};

    auto end = begin;
    while (begin != line.end) {
        end = std::find_if(end, line.end, isHash);
        if (end == line.end) break; // reached line end
        auto expressionBegin = end + 1;
        if (expressionBegin == line.end) break; // reached line end
        switch (expressionBegin->unicode())
        {
        case HASH:
            m_builder << OriginTarget {{line, TextSpan{begin, expressionBegin}}};
            begin = end = expressionBegin + 1;
            continue; // double ExprInitiator - one is skipped

        case CURLY_OPEN:
            m_builder << OriginTarget {{line, TextSpan{begin, end}}};
            expressionBegin++;
            auto expressionEnd = BraceCounter::findExpressionEnd(expressionBegin, line.end);
            if (expressionEnd == line.end) {
                reportError(m_messageHandler, line, "Missing close bracket!");
                begin = line.end;
                break; // terminate as invalid chaiscript
            }
            m_builder << OriginScriptExpression {{line, TextSpan{expressionBegin, expressionEnd}}};
            begin = end = expressionEnd + 1; // skip closing brackets
            continue; // expression evaluated
        }
        end = expressionBegin + 1;
    }
    m_builder << OriginTarget {{line, TextSpan{begin, line.end}}};
}

} // namespace Line
} // namespace intern
} // namespace Twofold
