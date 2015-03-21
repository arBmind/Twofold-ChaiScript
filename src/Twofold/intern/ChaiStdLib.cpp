#include "ChaiStdLib.h"
#include <chaiscript/chaiscript_stdlib.hpp>

#include <vector>
#include <QVariant>

namespace Twofold {
namespace intern {

// MSVC doesn't like that we are using C++ return types from our C declared module
// but this is the best way to do it for cross platform compatibility
#ifdef CHAISCRIPT_MSVC
#pragma warning(push)
#pragma warning(disable : 4190)
#endif

#ifdef __llvm__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#endif


chaiscript::ModulePtr chaiscript_stdlib()
{
  auto m = chaiscript::Std_Lib::library();

  m->add(chaiscript::user_type<std::vector<QVariant>>(), "Vector_QVariant");
  m->add(chaiscript::fun<size_t (const std::vector<QVariant> *)>([](const std::vector<QVariant> *a) { return a->size(); } ), "size");
  chaiscript::bootstrap::standard_library::random_access_container_type<std::vector<QVariant>>("Vector_QVariant", m);

  return m;
}


#ifdef __llvm__
#pragma clang diagnostic pop
#endif

#ifdef CHAISCRIPT_MSVC
#pragma warning(pop)
#endif


} // namespace intern
} // namespace Twofold

