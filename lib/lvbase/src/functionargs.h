#ifndef LVFUNCTIONARGS_H
#define LVFUNCTIONARGS_H

namespace lv{ namespace meta{

// Returns the number of arguments for a function
// ----------------------------------------------

template <typename R, typename ... Types> constexpr size_t getArgumentCount( R(*)(Types ...)){
   return sizeof...(Types);
}


}} // namespace lv, meta

#endif // LVFUNCTIONARGS_H
