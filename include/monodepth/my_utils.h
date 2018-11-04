/*
This file is part of monodepth-cpp

MIT License

Copyright (c) 2018 Shing Yan Loo (lsyan@ualberta.ca)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

NOTE:
This is an implementation of std::make_unique, which is not available to my compiler
std::make_unique -> util::make_unique

From: http://www.cplusplus.com/forum/beginner/154004/
*/

#ifndef MAKE_UNIQUE_H_INCLUDED
#define MAKE_UNIQUE_H_INCLUDED

#include <memory>
#include <utility>

namespace util
{
    #if __cplusplus == 201402L // C++14

        using std::make_unique ;

    #else // C++11

        template < typename T, typename... CONSTRUCTOR_ARGS >
        std::unique_ptr<T> make_unique( CONSTRUCTOR_ARGS&&... constructor_args )
        { return std::unique_ptr<T>( new T( std::forward<CONSTRUCTOR_ARGS>(constructor_args)... ) ); }

    #endif // __cplusplus == 201402L
}


#endif // MAKE_UNIQUE_H_INCLUDED
