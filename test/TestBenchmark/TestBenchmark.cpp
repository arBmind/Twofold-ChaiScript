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
#include "Twofold/Engine.h"

#include <QObject>
#include <QString>
#include <QtTest>

#include <utility>
#include <iostream>
#include <string>

class TestBenchmark : public QObject
{
    Q_OBJECT

public:
    TestBenchmark();

private Q_SLOTS:
    void prepare();
    void execute_data();
    void execute();
    void executeCPP_data();
    void executeCPP();

private:
    Twofold::Engine engine;
    Twofold::Engine::Context context;
    Twofold::PreparedTemplate prepared;
    Twofold::Target target;
};

#include "TestBenchmark.moc"

#define TEMPLATE_REPETITION 1000

class FakeTextLoader : public Twofold::TextLoader
{
public:
    Result load(const QString &name) const
    {
        static const QString templateText1 = QString::fromLatin1(R"EXAMPLE(
for(var i = 0; i <= %1; ++i) {
    | Das ist ein kurzer Text! #{i}
    # include "lib.twofold"
}
)EXAMPLE"
        ).arg(TEMPLATE_REPETITION);

        static const QString templateText2 = R"EXAMPLE(
|Line 1 included
|Line 2 incldued
)EXAMPLE";

        return (name != "lib.twofold")
                ? Result { Success, QString(), templateText1 }
                : Result { Success, QString(), templateText2 };
    }
};

TestBenchmark::TestBenchmark()
    : engine(std::make_shared<Twofold::MessageHandler>(),
             std::make_shared<FakeTextLoader>())
#if defined(_MSC_VER) && _MSC_VER < 1900
    , prepared({})
#endif
{
    QObject* pType = new QObject();
    pType->setProperty( "isArray", true );
    pType->setProperty( "name", "TestArray" );

    std::vector< chaiscript::Boxed_Value > baseNames;
    baseNames.push_back( chaiscript::const_var("base1") );
    baseNames.push_back( chaiscript::const_var("base2") );
    baseNames.push_back( chaiscript::const_var("base3") );

    context.insert(std::make_pair("type", chaiscript::const_var(pType)));
    context.insert(std::make_pair("baseNames", chaiscript::const_var(baseNames)));
    context.insert(std::make_pair("name", chaiscript::const_var("Main")));
}

void TestBenchmark::prepare()
{
    QBENCHMARK {
        engine.prepare("TextTemplate.twofold");
    }
}

void TestBenchmark::execute_data()
{
    prepared = engine.prepare("TextTemplate.twofold");
    // qDebug() << prepared.script;
}

void TestBenchmark::execute()
{
    QBENCHMARK {
        target = engine.exec(prepared, context);
    }
}

#include "Twofold/intern/ChaiScriptTargetBuilderApi.h"

void TestBenchmark::executeCPP_data()
{
    prepared = engine.prepare("TextTemplate.twofold");
}

void TestBenchmark::executeCPP()
{
    using namespace Twofold;
    using namespace Twofold::intern;
    QBENCHMARK {
        auto _template = ChaiScriptTargetBuilderApi{prepared.originPositions};
        for(auto i = 0; i <= TEMPLATE_REPETITION; ++i) {
            _template.indentPart(" ", 0);
            _template.append("Das ist ein kurzer Text! ", 1);
            _template.pushPartIndent(2);_template.append(std::to_string(i), 2);_template.popPartIndent();
            _template.newLine();
            _template.pushIndentation(" ", 3);

            _template.indentPart("", 4);
            _template.append("Line 1 included", 5);
            _template.newLine();
            _template.indentPart("", 6);
            _template.append("Line 2 incldued", 7);
            _template.newLine();
            _template.popIndentation();
        }
        const auto sourceMapText = _template.build();
        target = Target{ sourceMapText.sourceMap, sourceMapText.text };
    }
}

QTEST_GUILESS_MAIN(TestBenchmark)
