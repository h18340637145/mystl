#pragma once
#include "stl_deque.h"

namespace MiniSTL
{
    
    template<class T, class Sequence = deque<T>>
    class stack;
    template<class T, class Sequence>
    bool operator==(const stack<T,Sequence>&, const stack<T,Sequence>&);

    template<class T,class Sequence>
    bool operator<(const stack<T, Sequence>& , const stack<T, Sequence> &);

    template<class T, class Sequence>
    class Stack{
        //friend declarations
        template<class _T, class _Sequence>
        friend bool operator==(const stack<_T, _Sequence> &, const stack<_T, _Sequence> &);
        template<class _T, class _Sequence>
        friend bool operator<(const stack<_T, _Sequence> &, const stack<_T, _Sequence> &);


    public :
        using value_type = typename Sequence::value_type;
        using size_type = typename Sequence::size_type;
        using reference = typename Sequence::reference;
        using const_reference = typename Sequence::const_reference;

    private:
    //only data member
        Sequence c;
    public:
        //ctor
        stack() = default;
        explicit stack(const Sequence& rhs) : c(rhs){}//将底层的deque赋值

    public:
        //copy operations 
        stack(const stack &rhs): c(rhs.c) {}
        stack& operator=(const stack& rhs) {
            c.operator=(rhs.c);
        }

    public : 
        //move
        stack(stack && rhs) noexcept c(std::move(rhs.c)){}
        stack & operator=(stack &&rhs ) noexcept {
            c.operator=(std::move(rhs.c));
        }
    
    public:
        //getter
        bool empty() const noexcept{
            return c.empty();//deque是否是空的
        }

        size_type size() const noexcept{
            return c.size();//deque的size
        }
        
        const_reference top() const noexcept{
            return c.back();//deque的最后一个元素  就是栈顶 
        }
    public:
        //setter
        reference top(){
            return c.back();
        }
    public:
        //push  &&pop
        void push(const value_type & value){
            return c.push_back(value);  //push就是在deque的末尾加元素
        }
        void pop(){
            c.pop_back();  //pop就是在deque的末尾清除元素
        }
    public://swap
        void swap(stack &rhs)noexcept{
            MiniSTL::swap(c, rhs.c);
        }
    };


    template<class T, class Sequence>
    inline bool operator==(const stack<T,Sequence> &lhs, const stack<T,Sequence> &rhs){
        return lhs.c == rhs.c;
    }

    template<class T, class Sequence>
    inline bool operator<(const stack<T, Sequence> &lhs, const stack<T,Sequence> &rhs){
        return lhs.c < rhs.c;
    }

    template<class T,class Sequence>
    inline bool operator!=(const stack<T,Sequence> &lhs, const stack<T, Sequence> &rhs){
        return !(lhs == rhs);
    }

    template<class T, class Sequence>
    inline bool operator>(const stack<T,Sequence> &lhs, const stack<T, Sequence> &rhs){
        return rhs < lhs;
    }

    template<class T, class Sequence>
    inline bool operator<=(const stack<T,Sequence> &lhs, const stack<T, Sequence> &rhs){
        return !(rhs < lhs);
    }

    template<class T, class Sequence>
    inline bool operator>=(const stack<T,Sequence> &lhs, const stack<T,Sequence> &rhs){
        return !(lhs < rhs);
    }

    template<class T, class Sequence>
    inline void swap(stack<T,Sequence> &lhs, stack<T,Sequence> &rhs) noexcept{
        lhs.swap(rhs);
    }
} // namespace MiniSTL
