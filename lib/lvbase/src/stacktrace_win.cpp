#include "stacktrace.h"

#ifdef USE_STACK_TRACE
#include <Windows.h>
#include <process.h>
#include "dbghelp.h"
#endif

namespace lcv{

namespace{
    static const int TRACE_MAX_FUNCTION_NAME_LENGTH = 1024;
    static const int TRACE_MAX_OBJECT_PATH_LENGTH = 1024;
    static const int SLICE_FRAMES_START = 2;
}

StackTrace::Ptr StackTrace::capture(int maxFrames){
#ifdef USE_STACK_TRACE
    StackTrace::Ptr dest(new StackTrace);

    void **stack = new void*[maxFrames];
    HANDLE process = GetCurrentProcess();

    SymInitialize(process, NULL, TRUE);
    WORD numberOfFrames  = CaptureStackBackTrace(0, maxFrames, stack, NULL);
    SYMBOL_INFO *symbol  = (SYMBOL_INFO *)malloc(sizeof(SYMBOL_INFO) + (TRACE_MAX_FUNCTION_NAME_LENGTH - 1) * sizeof(TCHAR));
    symbol->MaxNameLen   = TRACE_MAX_FUNCTION_NAME_LENGTH;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    DWORD displacement;
    IMAGEHLP_LINE64 *line = (IMAGEHLP_LINE64 *)malloc(sizeof(IMAGEHLP_LINE64));
    line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    char* objectName = (char*)malloc(TRACE_MAX_OBJECT_PATH_LENGTH);

    for (int i = SLICE_FRAMES_START; i < numberOfFrames; i++){

        DWORD64 address = (DWORD64)(stack[i]);
        SymFromAddr(process, address, NULL, symbol);

        HMODULE module;
        if ( GetModuleHandleEx(
                 GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                 reinterpret_cast<LPCTSTR>(address),
                 &module) == TRUE )
        {
            DWORD objectPathLength = TRACE_MAX_OBJECT_PATH_LENGTH;
            GetModuleFileNameA(module, objectName, objectPathLength);
        } else {
            objectName[0] = '\0';
        }

        if (SymGetLineFromAddr64(process, address, &displacement, line)){
            dest->m_frames->push_back(StackFrame(
                symbol->Name,
                symbol->Address,
                objectName,
                line->FileName,
                line->LineNumber)
            );
        } else {
            dest->m_frames->push_back(StackFrame(symbol->Name, symbol->Address, objectName));
        }
    }

    free(objectName);
    free(line);
    free(symbol);
    delete[] stack;
    return dest;
#else
    return StackTrace::Ptr(0);
#endif
}


}// namespace
