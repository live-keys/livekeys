#ifndef LVFUNCTION_H
#define LVFUNCTION_H


#include "live/elements/lvelementsglobal.h"
#include "live/elements/value.h"

#include "live/meta/functionargs.h"
#include "live/meta/indextuple.h"
#include "live/typename.h"

#include <functional>
#include <vector>
#include <string>
#include <tuple>
#include <QDebug>

namespace lv{

class Exception;

namespace el{

/**
 * @brief Base class for any mapped script function, method or event.
 */
class LV_ELEMENTS_EXPORT Function{

public:
    typedef void(*FunctionPointer)(const v8::FunctionCallbackInfo<v8::Value>& value);

    // CallInfo class
    // ----------------

    class LV_ELEMENTS_EXPORT CallInfo{

    public:
        CallInfo(v8::FunctionCallbackInfo<v8::Value> const* info) : m_info(info){}

        template<typename T> T* that() const{
            return reinterpret_cast<T*>(internalField());
        }

        template<int index, typename T> T getValue() const{
            return extractValue<T>(m_info, index);
        }

        template<typename T> static T extractValue(v8::FunctionCallbackInfo<v8::Value> const* info, int index){
            return static_cast<T>(extractValue<Element*>(info, index));
        }

        ScopedValue at(size_t index) const;
        size_t length() const;

        void* userData();

        bool clearedPendingException(Engine* engine) const;
        void throwError(Engine* engine, Exception* e) const;
        void throwError(Engine* engine, const std::string& message) const;

        void assignReturnValue(const v8::Local<v8::Value>& value) const;
        v8::Local<v8::Object> thatObject();

        Engine* engine() const;

    private:
        void* internalField() const;

        v8::FunctionCallbackInfo<v8::Value> const* m_info;
    };

    // Parameters class
    // ----------------

    class LV_ELEMENTS_EXPORT Parameters{

        friend class Callable;

    public:
        Parameters(int length);
        Parameters(const std::initializer_list<ScopedValue>& init);
        ~Parameters();

        void assign(int index, const ScopedValue& ref);
        void assign(int index, const v8::Local<v8::Value>& value);

        ScopedValue at(Engine* engine, int index) const;
        int length() const;

        template<int index, typename T> T getValue(Engine* engine) const{
            return extractValue<T>(engine, m_args, index);
        }

        template<typename T> static T extractValue(Engine* engine, v8::Local<v8::Value> const* args, int index);

    private:
        DISABLE_COPY(Parameters);

        int m_length;
        v8::Local<v8::Value>* m_args;
    };

public:
    static void assignReturnValue(const el::ScopedValue& val, const v8::FunctionCallbackInfo<v8::Value>& info);

    static void ptrImplementation(const v8::FunctionCallbackInfo<v8::Value>& info){
        Function::CallInfo pr(&info);
        Function* f = reinterpret_cast<Function*>(pr.userData());
        f->call(pr);
    }

    template<class T>
    static void methodPtrImplementation(const v8::FunctionCallbackInfo<v8::Value>& info){
        Function::CallInfo pr(&info);
        Function* f = reinterpret_cast<Function*>(pr.userData());
        f->call(pr);
    }

    template<typename RT>
    static void checkReturn(Engine* engine, const Function::CallInfo& params, const RT& rt){
        if ( params.clearedPendingException(engine) )
            return;

        ScopedValue lv(engine, rt);
        params.assignReturnValue(lv.data());
    }

    template<typename RT, typename ...Args, int... Indexes>
    static void callFunction(
            Engine* engine,
            const std::function<RT(Args ...args)>& f,
            meta::IndexTuple<Indexes...>,
            const Function::CallInfo& params)
    {
        checkReturn(
            engine,
            params,
            f(params.template getValue<
                Indexes,
                typename std::remove_const<typename std::remove_reference<Args>::type>::type>()...));
    }

    template<typename ...Args, int... Indexes>
    static void callVoidFunction(
            const std::function<void(Args ...args)>& f,
            meta::IndexTuple<Indexes...>,
            const Function::CallInfo& params)
    {
        f(params.template getValue<Indexes,
             typename std::remove_const<typename std::remove_reference<Args>::type>::type>()...);
    }

    template<typename C, typename RT, typename ...Args, int... Indexes>
    static void callMethod(
            Engine* engine,
            const std::function<RT(C*, Args ...args)>& f,
            meta::IndexTuple<Indexes...>,
            const Function::CallInfo& params)
    {
        checkReturn(
            engine,
            params,
            f(params.template that<C>(),
              params.template getValue<
                    Indexes,
                    typename std::remove_const<typename std::remove_reference<Args>::type>::type>()...));
    }

    template<typename C, typename RT, typename ...Args, int... Indexes>
    static void callVoidMethod(
            const std::function<RT(C*, Args ...args)>& f,
            meta::IndexTuple<Indexes...>,
            const Function::CallInfo& params)
    {
        f(params.template that<C>(),
          params.template getValue<Indexes,
              typename std::remove_const<typename std::remove_reference<Args>::type>::type>()...);
    }

public:
    size_t totalArguments() const{ return m_totalArguments; }
    Function* less() const { return m_less; }

    void call(const Function::CallInfo& params){
        if ( params.length() >= totalArguments() )
            m_unwrapFunction(params);
        else if ( params.length() < totalArguments() && less() )
            less()->call(params);
        else {
            params.throwError(params.engine(), "Invalid number of arguments sent to function.");
        }
    }


    FunctionPointer ptr(){ return m_ptr; }
    void assignLess(Function* f){ m_less = f;}

    const std::string& getDeclaration(){ return m_declaration; }

protected:
    std::function<void(const Function::CallInfo& params)> m_unwrapFunction;

    size_t          m_totalArguments;
    std::string     m_declaration;
    FunctionPointer m_ptr;
    Function*       m_less;

};

template<> LV_ELEMENTS_EXPORT bool Function::CallInfo::extractValue(v8::FunctionCallbackInfo<v8::Value> const* info, int index);
template<> LV_ELEMENTS_EXPORT Value::Int32 Function::CallInfo::extractValue(v8::FunctionCallbackInfo<v8::Value> const* info, int index);
template<> LV_ELEMENTS_EXPORT Value::Int64 Function::CallInfo::extractValue(v8::FunctionCallbackInfo<v8::Value> const* info, int index);
template<> LV_ELEMENTS_EXPORT Value::Number Function::CallInfo::extractValue(v8::FunctionCallbackInfo<v8::Value> const* info, int index);
template<> LV_ELEMENTS_EXPORT std::string Function::CallInfo::extractValue(v8::FunctionCallbackInfo<v8::Value> const* info, int index);
template<> LV_ELEMENTS_EXPORT Callable Function::CallInfo::extractValue(v8::FunctionCallbackInfo<v8::Value> const* info, int index);
template<> LV_ELEMENTS_EXPORT Object Function::CallInfo::extractValue(v8::FunctionCallbackInfo<v8::Value> const* info, int index);
template<> LV_ELEMENTS_EXPORT Buffer Function::CallInfo::extractValue(v8::FunctionCallbackInfo<v8::Value> const* info, int index);
template<> LV_ELEMENTS_EXPORT Value Function::CallInfo::extractValue(v8::FunctionCallbackInfo<v8::Value> const* info, int index);
template<> LV_ELEMENTS_EXPORT ScopedValue Function::CallInfo::extractValue(v8::FunctionCallbackInfo<v8::Value> const* info, int index);
template<> LV_ELEMENTS_EXPORT Element* Function::CallInfo::extractValue(v8::FunctionCallbackInfo<v8::Value> const* info, int index);


template<typename T> T Function::Parameters::extractValue(Engine* engine, v8::Local<v8::Value> const* args, int index){
    return static_cast<T>(Function::Parameters::extractValue<Element*>(engine, args, index));
}

template<> LV_ELEMENTS_EXPORT bool Function::Parameters::extractValue(Engine* engine, v8::Local<v8::Value> const* args, int index);
template<> LV_ELEMENTS_EXPORT Value::Int32 Function::Parameters::extractValue(Engine* engine, v8::Local<v8::Value> const* args, int index);
template<> LV_ELEMENTS_EXPORT Value::Int64 Function::Parameters::extractValue(Engine* engine, v8::Local<v8::Value> const* args, int index);
template<> LV_ELEMENTS_EXPORT Value::Number Function::Parameters::extractValue(Engine* engine, v8::Local<v8::Value> const* args, int index);
template<> LV_ELEMENTS_EXPORT std::string Function::Parameters::extractValue(Engine* engine, v8::Local<v8::Value> const* args, int index);
template<> LV_ELEMENTS_EXPORT Callable Function::Parameters::extractValue(Engine* engine, v8::Local<v8::Value> const* args, int index);
template<> LV_ELEMENTS_EXPORT Object Function::Parameters::extractValue(Engine* engine, v8::Local<v8::Value> const* args, int index);
template<> LV_ELEMENTS_EXPORT Buffer Function::Parameters::extractValue(Engine* engine, v8::Local<v8::Value> const* args, int index);
template<> LV_ELEMENTS_EXPORT Value Function::Parameters::extractValue(Engine* engine, v8::Local<v8::Value> const* args, int index);
template<> LV_ELEMENTS_EXPORT ScopedValue Function::Parameters::extractValue(Engine* engine, v8::Local<v8::Value> const* args, int index);
template<> LV_ELEMENTS_EXPORT Element* Function::Parameters::extractValue(Engine* engine, v8::Local<v8::Value> const* args, int index);


template<typename RT, typename ...Args>
class PointerFunction : public Function{

public:
    typedef RT(*FunctionType)(Args...);
    using ReturnType = typename std::remove_const<typename std::remove_reference<RT>::type>::type;

    static_assert(
        std::is_same<ReturnType, bool>::value ||
        std::is_same<ReturnType, Value::Int32>::value ||
        std::is_same<ReturnType, Value::Int64>::value ||
        std::is_same<ReturnType, Value::Number>::value ||
        std::is_same<ReturnType, std::string>::value ||
        std::is_same<ReturnType, Callable>::value ||
        std::is_same<ReturnType, Buffer>::value ||
        std::is_same<ReturnType, Object>::value ||
        std::is_same<ReturnType, ScopedValue>::value ||
        std::is_same<ReturnType, Value>::value ||
        std::is_pointer<ReturnType>::value,
        "Return type is not convertible to script value."
    );

public:
    PointerFunction(const std::function<RT(Args...)>& f)
        : m_function(f)
    {
        m_less = nullptr;
        m_ptr = &Function::ptrImplementation;
        m_totalArguments  = sizeof...(Args);
        m_declaration =
            TypeName<RT>::capture(TypeNameOptions::None) +
            "(" + TypeName<Args...>::capture(TypeNameOptions::None) + ")";
        m_unwrapFunction = [this](const Function::CallInfo& params){
            Function::callFunction(
                params.engine(), m_function, typename meta::make_indexes<Args...>::type(), params
            );
        };
    }


    RT callArgs(Args... args){
        return m_function(args...);
    }

private:
    std::function<RT(Args ...args)> m_function;
};

template<typename ...Args>
class PointerFunction<void, Args...> : public Function{

public:
    PointerFunction(const std::function<void(Args...)>& f)
        : m_function(f)
    {
        m_less           = nullptr;
        m_ptr            = &Function::ptrImplementation;
        m_totalArguments = sizeof...(Args);
        m_declaration = "void(" + TypeName<Args...>::capture(TypeNameOptions::None) + ")";
        m_unwrapFunction = [this](const Function::CallInfo& params){
            Function::callVoidFunction(
                m_function, typename meta::make_indexes<Args...>::type(), params
            );
        };
    }


    void callArgs(Args... args){
        m_function(args...);
    }

private:
    std::function<void(Args ...args)> m_function;
};

template<typename RT>
class PointerFunction<RT, const Function::CallInfo&> : public Function{

public:
    typedef RT(*FunctionType)(const Function::CallInfo&);
    using ReturnType = typename std::remove_const<typename std::remove_reference<RT>::type>::type;

    static_assert(
        std::is_same<ReturnType, Value::Int32>::value ||
        std::is_same<ReturnType, Value::Int64>::value ||
        std::is_same<ReturnType, Value::Number>::value ||
        std::is_same<ReturnType, std::string>::value ||
        std::is_same<ReturnType, Callable>::value ||
        std::is_same<ReturnType, Buffer>::value ||
        std::is_same<ReturnType, Object>::value ||
        std::is_same<ReturnType, ScopedValue>::value ||
        std::is_same<ReturnType, Value>::value ||
        std::is_pointer<ReturnType>::value,
        "Return type is not convertible to script value."
    );

public:
    PointerFunction(const std::function<RT(const Function::CallInfo&)>& f)
        : m_function(f)
    {
        m_less = nullptr;
        m_ptr = &Function::ptrImplementation;
        m_totalArguments  = 0;
        m_declaration = TypeName<RT>::capture(TypeNameOptions::None) + "(Optional)";
        m_unwrapFunction = [this](const Function::CallInfo& params){
            Function::checkReturn(params.engine(), params, m_function(params));
        };
    }


    RT callArgs(const Function::CallInfo& p){ return m_function(p); }

private:
    std::function<RT(const Function::CallInfo&)> m_function;
};

template<>
class PointerFunction<void, const Function::CallInfo&> : public Function{

public:
    PointerFunction(const std::function<void(const Function::CallInfo&)>& f)
        : m_function(f)
    {
        m_less           = nullptr;
        m_ptr            = &Function::ptrImplementation;
        m_totalArguments = 0;
        m_declaration = "void(Optional)";
        m_unwrapFunction = [this](const Function::CallInfo& params){
            m_function(params);
        };
    }


    void callArgs(const Function::CallInfo& p){ m_function(p);}

private:
    std::function<void(const Function::CallInfo&)> m_function;
};


}}// namespace lv, el

#endif // LVFUNCTION_H
