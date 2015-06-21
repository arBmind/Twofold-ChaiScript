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
#include "MethodDescription.h"

#include <QCoreApplication>
#include <QtDebug>

const QString included_twofold = R"TWOFOLD(
def methodArgs(args) {
  args.for_each_with_index(fun(arg, i){
    if (0!=i) {
        // use interpolation to preserve the whitespace
        \#{", "}
    }
    // use backslash to avoid linebreaks
        \#{arg}
  });
}
def showMethod(method) {
        |function #{method.name}(#{methodArgs(method.args)}) {
  // if body contains multiple lines they will all be indented
        |  #{method.body}
        |}
}
)TWOFOLD";

static const QString main_twofold = R"TWOFOLD(
#include "included.twofold"

class MyObject {
  attr memLambda
  def MyObject(cb) {
    this.memLambda = cb
  }
}
var x = MyObject(fun(){
  \hello
})
        var y = x.memLambda
        | invoke #{y()}

        |function #{name}Class(#{methodArgs(args)}) {
methods.for_each_with_index(fun(method, i){
  // every line outputted by showMethod is indented by two extra spaces
        =  showMethod(method)
});
        |
        |  return {
methods.for_each_with_index(fun(method, i){
        |    "#{method.name}": #{method.name}#{(i+1 < methods.size) ? "," : ""}
});
        |  };
        |}
)TWOFOLD";

class FakeTextLoader : public Twofold::TextLoader
{
public:
    Result load(const QString &name) const
    {
        if (name == "included.twofold") {
            return { Success, name, included_twofold };
        }
        if (name == "main.twofold") {
          return { Success, name, main_twofold };
        }
        return { NotFound, {}, {} };
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Q_UNUSED(app); // required to get QScriptEngine

    using namespace Twofold;

    Engine engine(std::make_shared<MessageHandler>(),
                  std::make_shared<FakeTextLoader>());
    PreparedTemplate prepared = engine.prepare("main.twofold");

    ::qDebug() << "generated script:";
    ::qDebug() << prepared.script;
    ::qDebug() << "-----";

    MethodDescription helloMethod("hello", {"greeted"});
    // Use QObject dynamic properties
    helloMethod.setProperty("body", "console.log('Hello' + greeted);");

    Engine::Context context; // these are the globals in the javascript execution
    context["name"] = chaiscript::const_var(std::string("TwofoldGenerated"));
    context["args"] = chaiscript::const_var(std::vector<chaiscript::Boxed_Value>());
    context["methods"] = chaiscript::const_var(std::vector<chaiscript::Boxed_Value>({
        chaiscript::const_var((QObject*)&helloMethod)
    }));

    Target target = engine.exec(prepared, context);

    ::qDebug() << "generated sourcecode:";
    ::qDebug() << target.text;
    ::qDebug() << "-----";
    ::qDebug() << "done";
}
