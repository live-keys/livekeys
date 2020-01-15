#ifndef LVTYPENAME_H
#define LVTYPENAME_H

#include <string>
#include <type_traits>
#include <typeinfo>
#include <memory>

#ifndef _MSC_VER
#   include <cxxabi.h>
#endif

namespace lv{

enum TypeNameOptions{
    None = 0,
    Qualifiers = 1,
    SimplifyFunctions = 2
};

template<typename... T> class TypeName {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){ return options > 0 ? "":""; }
};


//template<> class TypeName<std::string> {
//public:
//    static std::string capture(int options = TypeNameOptions::Qualifiers){
//        return "std::string";
//    }
//};

template<typename T1, typename T2, typename... T> class TypeName<T1, T2, T...> {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        return TypeName<T1>::capture(options)+ "," + TypeName<T2>::capture(options)
            + (sizeof...(T) ? ("," + TypeName<T...>::capture(options)) : "");
    }
};

template<typename T> class TypeName<T>{

public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        typedef typename std::remove_reference<T>::type TR;
#ifndef _MSC_VER
        std::unique_ptr<char, void(*)(void*)> own(
            abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr),
            std::free
        );
        std::string r = own.get();
#else
        std::string r = typeid(TR).name();
#endif

        if ( options & TypeNameOptions::Qualifiers){
            if (std::is_const<T>::value)
                r += " const";
            if (std::is_volatile<T>::value)
                r += " volatile";
            if (std::is_lvalue_reference<T>::value)
                r += "&";
            else if (std::is_rvalue_reference<T>::value)
                r += "&&";
        }

        return r;
    }

};


template<typename T> class TypeName<T* const> {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        if ( options & TypeNameOptions::Qualifiers)
            return TypeName<T>::capture(options)+ "* const";
        return TypeName<T>::capture(options) + "*";
    }
};

template<typename T> class TypeName<T*> {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        return TypeName<T>::capture(options)+ "*";
    }
};

template <typename T, std::size_t N> class TypeName<T[N]> {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        return TypeName<T>::capture(options)+ "[" + std::to_string(N) + "]";
    }
};

template <typename T> class TypeName<T[]> {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        return TypeName<T>::capture(options)+ "[]";
    }
};

template <typename T, std::size_t N> class TypeName<const T[N]> {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        return "const " + TypeName<T>::capture(options)+ "[" + std::to_string(N) + "]";
    }
};

template <typename T> class TypeName<const T[]> {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        return "const " + TypeName<T>::capture(options) + "[]";
    }
};

/** \brief function type
*/
template <typename T, typename... Args> class TypeName<T(Args...)> {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        return TypeName<T>::capture(options)+ "(" + TypeName<Args...>::capture(options)+ ")";
    }
};

/** \brief function pointer type
*/
template <typename T, typename... Args> class TypeName<T(*)(Args...)> {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        if ( options & TypeNameOptions::SimplifyFunctions )
            return TypeName<T>::capture(options)+ "(" + TypeName<Args...>::capture(options)+ ")";
        return TypeName<T>::capture(options)+ " (*)(" + TypeName<Args...>::capture(options)+ ")";
    }
};

/** \brief member pointer
*/
template <typename T, class U> class TypeName<T U::*> {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        return TypeName<T>::capture(options)+ " " + TypeName<U>::capture(options)+ "::*";
    }
};

/** \brief member function pointer
*/
template <typename T, class U, typename... Args> class TypeName<T (U::*)(Args...)> {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        if ( options & TypeNameOptions::SimplifyFunctions )
            return TypeName<T>::capture(options)+ "(" + TypeName<Args...>::capture(options)+ ")";
        return TypeName<T>::capture(options)+ " (" + TypeName<U>::capture(options)+ "::*)(" + TypeName<Args...>::capture(options)+ ")";
    }
};

/** \brief const-qualified member function pointer
*/
template <typename T, class U, typename... Args> class TypeName<T (U::*)(Args...) const> {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        if ( options & TypeNameOptions::SimplifyFunctions )
            return TypeName<T>::capture(options)+ "(" + TypeName<Args...>::capture(options)+ ") const";
        return TypeName<T>::capture(options)+ " (" + TypeName<U>::capture(options)+ "::*)(" + TypeName<Args...>::capture(options)+ ") const";
    }
};

/** \brief lvalue-ref-qualified member function pointer
*/
template <typename T, class U, typename... Args> class TypeName<T (U::*)(Args...) &> {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        if ( options & TypeNameOptions::SimplifyFunctions )
            return TypeName<T>::capture(options)+ "(" + TypeName<Args...>::capture(options)+ ")";
        return TypeName<T>::capture(options)+ " (" + TypeName<U>::capture(options)+ "::*)(" + TypeName<Args...>::capture(options)+ ") &";
    }
};

/** \brief rvalue-ref-qualified member function pointer
*/
template <typename T, class U, typename... Args> class TypeName<T (U::*)(Args...) &&> {
public:
    static std::string capture(int options = TypeNameOptions::Qualifiers){
        if ( options & TypeNameOptions::SimplifyFunctions )
            return TypeName<T>::capture(options)+ "(" + TypeName<Args...>::capture(options)+ ")";
        return TypeName<T>::capture(options)+ " (" + TypeName<U>::capture(options)+ "::*)(" + TypeName<Args...>::capture(options)+ ") &&";
    }
};

//template<template <class...> class T, class... U> class TypeName<T<U...>> {
//public:
//    static std::string capture(int options = TypeNameOptions::Qualifiers){
//        return std::string{typeid(T<U...>).name()} + "<" + TypeName<U...>::capture(options)+ ">";
//    }
//};

}// namespace

#endif // LVTYPENAME_H
