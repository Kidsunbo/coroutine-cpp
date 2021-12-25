#ifndef KIEDIS_CONTEXT_H
#define KIEDIS_CONTEXT_H


/**
 * This file define a class wrapper for epoll to handle the context and file descriptor for the whole library
 **/

namespace kiedis
{
    class IOContext {
        

        public:

        IOContext() = default;
        ~IOContext() = default;
    };
} // namespace kiedis


#endif