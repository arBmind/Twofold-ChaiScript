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
#include "Twofold/intern/LineProcessor.h"
#include "Twofold/intern/QStringHelper.h"

#include <QtTest>


#include "chaiscript/chaiscript.hpp"

namespace Twofold {
namespace intern {
chaiscript::ModulePtr chaiscript_stdlib();
} // namespace intern
} // namespace Twofold

class TestLines : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testLines_data();
    void testLines();

    void testChai() {
        using namespace chaiscript;
//        ChaiScript chai(Twofold::intern::chaiscript_stdlib());
        Boxed_Value b = var(5);
        QVERIFY(5 == boxed_cast<int>(b));

        Boxed_Value c = var("Hello World");
        QVERIFY(0 == strcmp(boxed_cast<const char*>(c), "Hello World"));
    }
};

void TestLines::testLines_data()
{
    QTest::addColumn< QString >("templateText");
    QTest::addColumn< QStringList >("expectedSpaceStrings");
    QTest::addColumn< QStringList >("expectedNonSpaceStrings");

    QTest::newRow("empty") << "" << QStringList {} << QStringList {};
    QTest::newRow("newline") << " \t\n" << QStringList { " \t" } << QStringList { "" };
    QTest::newRow("line") << " \tFirst line \t" << QStringList { " \t" } << QStringList { "First line \t" };
    QTest::newRow("multiline") << " \tFirst line\n \tSecond line \t" << QStringList { " \t", " \t" } << QStringList { "First line", "Second line \t" };
}

void TestLines::testLines()
{
    QFETCH(QString, templateText);
    QFETCH(QStringList, expectedSpaceStrings);
    QFETCH(QStringList, expectedNonSpaceStrings);

    auto lineFunc = [&](const Twofold::intern::FileLine &line){
        QString spaceString = Twofold::intern::toQString(line.begin, line.firstNonSpace);
        QString nonSpaceString = Twofold::intern::toQString(line.firstNonSpace, line.end);

        Q_ASSERT( ! expectedSpaceStrings.isEmpty());
        Q_ASSERT( ! expectedNonSpaceStrings.isEmpty());

        QString exspectedSpaceString = expectedSpaceStrings.first();
        QString exspectedNonSpaceString = expectedNonSpaceStrings.first();

        expectedSpaceStrings.pop_front();
        expectedNonSpaceStrings.pop_front();

        QCOMPARE(spaceString, exspectedSpaceString);
        QCOMPARE(nonSpaceString, exspectedNonSpaceString);
    };

    Twofold::intern::LineProcessor processor { {}, lineFunc};
    processor("testLines", templateText);

    QVERIFY(expectedSpaceStrings.isEmpty());
    QVERIFY(expectedNonSpaceStrings.isEmpty());
}

QTEST_APPLESS_MAIN(TestLines)

#include "TestLines.moc"
