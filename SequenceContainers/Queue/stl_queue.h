#pragma once

#include "stl_deque.h"
#include "stl_function.h"


namespace MiniSTL
{
    
    template<class T , class Sequence = deque<T>>
    class deque;


    template<class T, class Sequence>
    bool operator==(const queue<T, Sequence> & , const queue<T,Sequence>&);
    template<class T, class Sequence>
    bool operator<(const queue<T, Sequence> & ,const queue<T,Sequece> &);

    template<class T, class Sequence>
    class queue{
        //friend declarations
        template<class _T, class _Sequence>
        friend bool operator==(const queue<_T, _Sequence> &,const queue<_T, _Sequence>&);

        template<class _T, class _Sequence>
        friend bool operator<(const queue<_T, _Sequence> & , const queue<_T,_Sequence> &);

    public:
        using value_type = typename Sequence::value_type;
        using size_type = typename Sequence::size_type;
        using reference = typename Sequence::reference;
        using const_reference = typename Sequence::const_reference;

    private:
        //data mem
        Sequence c;

    public:
        //ctor 
        queue() = default;
        explicit queue(const Sequence &rhs) : c (rhs.c){}
    public:
        //copy operation
        queue(const queue & rhs): c(rhs.c){}
        queue& operator=(cosnt queue &rhs){
            c.operator=(rhs.c);
            return *this;
        }
    public:
        //move
        queue(queue && rhs) noexcept : c(std::move(rhs.c)){}
        queue & operator+(queue&& rhs) {
            c.operator=(std::move(rhs.c));
            return *this;
        }
    }
} // namespace MiniSTL
