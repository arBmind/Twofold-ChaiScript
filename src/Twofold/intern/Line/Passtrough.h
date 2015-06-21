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

namespace Twofold {
namespace intern {

class PreparedChaiScriptBuilder;
struct FileLine;

namespace Line {

/**
 * @brief This line processing could be used as default behavior.
 * It passthrough the input line to the output.
 */
class Passtrough
{
public:
    explicit Passtrough(PreparedChaiScriptBuilder &builder);

    void operator() (const FileLine& line) const;

private:
    PreparedChaiScriptBuilder &m_builder;
};

} // namespace Line
} // namespace intern
} // namespace Twofold

