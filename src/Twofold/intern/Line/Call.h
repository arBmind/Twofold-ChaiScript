/* Twofold-Qt
 * (C) Copyright 2014 HicknHack Software GmbH
 *
 * The original code can be found at:
 *     https://github.com/hicknhack-software/Twofold-Qt
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

#include "Twofold/MessageHandler.h"

namespace Twofold {
namespace intern {

struct FileLine;
class PreparedJavascriptBuilder;

namespace Line {

/**
 * @brief line processing for calling a javascript function
 * whose output is handled indentation aware.
 */
class Call
{
public:
    Call(const MessageHandlerPtr &messageHandler, PreparedJavascriptBuilder &builder);

    void operator() (const FileLine& line) const;

private:
    const MessageHandlerPtr m_messageHandler;
    PreparedJavascriptBuilder &m_builder;
};

} // namespace Line
} // namespace intern
} // namespace Twofold

