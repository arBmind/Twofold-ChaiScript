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

#include <QString>
#include <QMap>

#include <functional>

namespace Twofold {
namespace intern {

struct FileLine;

struct LineCommand
{
    const FileLine& line;
    QString::const_iterator begin, end;
};

namespace Line {

/**
 * @brief line processing for commands.
 */
class Command
{
public:
    using Function = std::function<void (const LineCommand&)>;
    using Map = std::map<QString, Function>;

public:
    Command(Map&& map, Function&& fallback);

    void operator() (const FileLine& line) const;

private:
    const Map m_map;
    const Function m_fallback;
};

} // namespace Line
} // namespace intern
} // namespace Twofold

