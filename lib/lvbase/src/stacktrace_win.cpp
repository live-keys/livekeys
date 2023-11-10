/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "stacktrace.h"

#ifdef USE_STACK_TRACE
#include <Windows.h>
#include <process.h>
#include "dbghelp.h"
#endif

namespace lv{

namespace{
    static const int TRACE_MAX_FUNCTION_NAME_LENGTH = 1024;
    static const int TRACE_MAX_OBJECT_PATH_LENGTH = 1024;
    static const int SLICE_FRAMES_START = 1;
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

    for (int i = 0; i < numberOfFrames; i++){

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
            if ( i >= SLICE_FRAMES_START ){
                dest->m_frames->push_back(StackFrame(
                    symbol->Name,
                    symbol->Address,
                    objectName,
                    line->FileName,
                    line->LineNumber)
                );
            }
        } else {
            if ( i >= SLICE_FRAMES_START ){
                dest->m_frames->push_back(StackFrame(symbol->Name, symbol->Address, objectName));
            }
        }
    }

    free(objectName);
    free(line);
    free(symbol);
    delete[] stack;
    return dest;
#else
    (void)maxFrames;
    return StackTrace::Ptr(0);
#endif
}


}// namespace
