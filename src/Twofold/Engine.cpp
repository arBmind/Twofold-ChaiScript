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
#include "Engine.h"

#include "Twofold/intern/ChaiScriptTargetBuilderApi.h"

#include "Twofold/intern/QStringHelper.h"
#include "Twofold/intern/find_last.h"

#include <chaiscript/chaiscript.hpp>
#include <chaiscript/utility/utility.hpp>

#include <QVector>

#include <vector>

namespace Twofold {
namespace intern {
chaiscript::ModulePtr chaiscript_stdlib();
} // namespace intern

using namespace intern;

namespace {

//FileLineColumnPositionList generateExceptionCallerStack(const PreparedTemplate &preparedTemplate, const QStringList &backtrace)
//{
//    FileLineColumnPositionList callerStack;
//    for (const auto& traceLine : backtrace) {
//        // traceline format: "<function>() at <line>"
//        auto begin = traceLine.begin();
//        const auto end = traceLine.end();

//        begin = find_last(begin, end, QChar(' '));
//        QString lineString = toQString(begin, end);

//        bool convertSuccesful = false;
//        const int line = lineString.toInt(&convertSuccesful);
//        const int column = 1;
//        if (convertSuccesful) {
//            const auto position = SourceMap::getOriginalPositionFromGenerated(preparedTemplate.sourceMap, {line, column});
//            callerStack.push_back(position);
//        }
//    }
//    return callerStack;
//}

} // namespace

class Engine::Private
{
public:
    Private(MessageHandlerPtr messageHandler, TextLoaderPtr textLoader)
        : m_messageHandler(messageHandler)
        , m_textLoader(textLoader)
        , m_chai(chaiscript_stdlib())
    {
        defineQVariantCasts();
    }

    Target execPrepared(const PreparedTemplate &preparedTemplate, const Engine::Context &inputs)
    {
        ChaiScriptTargetBuilderApi scriptTargetBuilder(preparedTemplate.originPositions);
        auto old_state = m_chai.get_state();

        defineInputs(inputs);
        defineTemplateApi(scriptTargetBuilder);

        try {
            auto resultValue = m_chai.eval(preparedTemplate.script.toStdString(),
                                           chaiscript::exception_specification<const std::exception &>());
        } catch (const chaiscript::exception::eval_error &error) {
            const int line = error.start_position.line;
            const int column = error.start_position.column;
            FileLineColumnPositionList position {{ SourceMap::getOriginalPositionFromGenerated(preparedTemplate.sourceMap, {line, column}) }};
            const QString text = "Eval Error: " + QString::fromLatin1(error.what());
            m_messageHandler->scriptMessage(MessageType::Error, position, text);
        } catch (const chaiscript::exception::bad_boxed_cast &e) {
            const QString text = "Bad Boxed Cast: " + QString::fromLatin1(e.what());
            m_messageHandler->message(MessageType::Error, text);
        } catch (const std::exception &e) {
            const QString text = "Exception: " + QString::fromLatin1(e.what());
            m_messageHandler->message(MessageType::Error, text);
        }

        m_chai.set_state(old_state);

        const auto sourceMapText = scriptTargetBuilder.build();
        return { sourceMapText.sourceMap, sourceMapText.text };
    }

    //    void showSyntaxError(QScriptSyntaxCheckResult checkResult, const PreparedTemplate &preparedTemplate)
    //    {
    //        const int line = checkResult.errorLineNumber();
    //        const int column = checkResult.errorColumnNumber();
    //        FileLineColumnPositionList position {{ SourceMap::getOriginalPositionFromGenerated(preparedTemplate.sourceMap, {line, column}) }};
    //        const QString text = "Syntax Error: " + checkResult.errorMessage();
    //        m_messageHandler->scriptMessage(MessageType::Error, position, text);
    //    }

    PreparedTemplateBuilder createPreparedBuilder() {
        return { m_messageHandler, m_textLoader };
    }

private:
    //    void showException(QScriptValue resultValue, const PreparedTemplate &preparedTemplate)
    //    {
    //        const QStringList backtrace = m_chai.uncaughtExceptionBacktrace();
    //        auto positionStack = generateExceptionCallerStack(preparedTemplate, backtrace);
    //        const int line = m_chai.uncaughtExceptionLineNumber();
    //        const int column = 1; // TODO: use agent and stack!
    //        positionStack.insert(positionStack.begin(), SourceMap::getOriginalPositionFromGenerated(preparedTemplate.sourceMap, {line, column}));
    //        const QString text = "Uncaught Exception: " + resultValue.toString();
    //        m_messageHandler->scriptMessage(MessageType::Error, positionStack, text);
    //    }

    void defineQVariantCasts()
    {
        m_chai.eval(R"CHAISCRIPT(
def for_each_with_index(container, func) {
  for (var i = 0; i < container.size(); ++i) {
    func(container[i], i);
  }
}
def to_string(void) {
  ""
}
                       )CHAISCRIPT"
                , chaiscript::Exception_Handler(), "twofold prelude");

        m_chai.add(chaiscript::user_type<QVariant>(), "QVariant");
        m_chai.add(chaiscript::type_conversion<QVariant, int>([](const QVariant& v){return v.toInt();}));
        m_chai.add(chaiscript::type_conversion<QVariant, float>([](const QVariant& v){return v.toFloat();}));
        m_chai.add(chaiscript::type_conversion<QVariant, bool>([](const QVariant& v){return v.toBool();}));

        m_chai.add(chaiscript::type_conversion<QVariant, std::vector<QVariant> >([](const QVariant& v){
            std::vector<QVariant> r;
            const auto seq = v.value<QSequentialIterable>();
            r.reserve(1 + seq.size());
            for (const auto &e : seq) r.push_back(e);
            return r;
        }));

        m_chai.add(chaiscript::fun([](const QVariant* v){
            return v->toString().toStdString();
        }), "to_string");
//        m_chai.add(chaiscript::fun< std::string (void) >([](void){
//            return "";
//        }), "to_string");

        m_chai.add(chaiscript::fun([](const QVariant* v, bool x){
            return v->toBool() == x;
        }), "==");
        m_chai.add(chaiscript::fun([](const QVariant* v, const std::string &x){
            return v->toString().toStdString() == x;
        }), "==");

        m_chai.add(chaiscript::fun([](const QVariant* v, const std::string& k){
            if (v->canConvert(QMetaType::QObjectStar)) {
                const auto* o = v->value<QObject*>();
                return o->property( k.c_str() );
            }
            else if (v->canConvert(QMetaType::QVariantHash)) {
                const auto& a = v->value<QAssociativeIterable>();
                return a.value(QString::fromStdString(k));
            }
            throw std::range_error("unknown type");
        }), "method_missing");

        m_chai.add(chaiscript::fun([](const QVariant* v, int index)->QVariant{
            if (v->canConvert(QMetaType::QVariantList)) {
                const auto& s = v->value<QSequentialIterable>();
                if (index >= s.size())
                    throw std::range_error("index out of range");

                return s.at(index);
            }
            throw std::range_error("unknown type");
        }), "[]");

        m_chai.add(chaiscript::user_type<QObject>(), "QObject");
        m_chai.add(chaiscript::fun([](const QObject* o, const std::string& k){
            return o->property( k.c_str() );
        }), "method_missing");

        m_chai.add(chaiscript::user_type<QString>(), "QString");
        m_chai.add(chaiscript::type_conversion<std::string, QString>([](const std::string& v){return QString::fromStdString(v);}));

    }

    void defineTemplateApi(ChaiScriptTargetBuilderApi &templateApi)
    {
        auto module = std::make_shared<chaiscript::Module>();
        ChaiScriptTargetBuilderApi::add_class(*module);
        m_chai.add(module);
        m_chai.add_global(chaiscript::var(&templateApi), "_template");
    }

    void defineInputs(const Engine::Context &inputs)
    {
        for (const auto &v : inputs) {
            m_chai.add_global(v.second, v.first);
        }
    }

    MessageHandlerPtr m_messageHandler;
    TextLoaderPtr m_textLoader;
    chaiscript::ChaiScript m_chai;
};

Engine::Engine(MessageHandlerPtr messageHandler, TextLoaderPtr textLoader)
    : m_private(new Private(messageHandler, textLoader))
{
}

Engine::Engine(TextLoaderPtr textLoader, MessageHandlerPtr messageHandler)
    : Engine(messageHandler, textLoader)
{

}

void Engine::showTemplateSyntaxErrors(const PreparedTemplate &) const
{
    //    auto checkResult = QScriptEngine::checkSyntax(preparedTemplate.script);
    //    if (checkResult.state() == QScriptSyntaxCheckResult::Error)
    //        m_private->showSyntaxError(checkResult, preparedTemplate);
}

Target Engine::exec(const PreparedTemplate &preparedTemplate, const Context &inputs)
{
    return m_private->execPrepared(preparedTemplate, inputs);
}

PreparedTemplate Engine::prepare(const QString &templateName) const
{
    auto prepared = m_private->createPreparedBuilder().build(templateName);
    this->showTemplateSyntaxErrors(prepared);
    return prepared;
}

Target Engine::execTemplateName(const QString &templateName, const Context &inputs)
{
    auto prepared = this->prepare(templateName);
    return this->exec(prepared, inputs);
}

void Engine::PrivateDeleter::operator()(Engine::Private *p) const
{
    delete p;
}

} // namespace Twofold
