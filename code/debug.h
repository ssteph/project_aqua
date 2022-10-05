#pragma once

#if AQUA_DEBUG
#define MY_ASSERT(expression)   __pragma(warning(push))          \
                                __pragma(warning(disable:4127))  \
                                do { if (!(expression)) {        \
                                    __pragma(warning(pop))       \
                                    __debugbreak();              \
                                } } while(0)                     
#else
#define MY_ASSERT(expression) do { (void)sizeof(expression); } while(0)
#endif