#pragma once

namespace MiniSTL{
    //一元运算符
    //两个参数  1  返回值
    template<class Arg,class Result>
    struct unary_function
    {
        using arguement_type = Arg;
        using result_type = Result;
    };

    //二元运算符结构定义
    //三个参数  1  2  返回值
    template<class Arg1,class Arg2,class Result>
    struct binary_function
    {
        using first_argument_type = Arg1;
        using second_argument_type = Arg2
        using result_type = Result;
    };

    //以下是六个算数类仿函数
    //plus  minus  multipiples  divides modulus negate
    template<class T>
    struct plus:public binary_function<T,T,T>
    {
        //仿函数就是重写operator()(param)
        T operator()(const T&x,const T&y) const{
            return x+y;
        }
    };

    template<class T>
    struct minus : public binary_function<T,T,T>
    {
        T operator()(const T& x, const T& y)const{
            return x-y;
        }
    };

    template<class T>
    //乘法三个参数 1 2 返回值
    struct multiplies : public binary_function<T,T,T>
    {
        T operator()(const T& x, const T& y) const{
            return x * y;
        }
    };
    

    template<class T>
    struct divide : public binary_function<T,T,T>
    {
        T operator()(const T& x, const T& y) const{
            return x / y;
        }
    };
    
    template<class T>
    struct modulus : public binary_function<T,T,T>
    {
        T operator()(const T& x, const T& y) const{
            return x % y;
        }
    };

    template<class T>
    struct negate : public unary_function<T,T>
    {
        T operator()(const T& x)const{
            return -x
        }
    };
        
    //证同元素（并非标准STL所要求）
    template<class T>
    inline T identity_element(plus<T>){
        return T(0);
    }

    template<class T>
    inline T identity_element(multiplies<T>){
        return T(1);
    }

    //运算关系仿函数
    //equal_to  not_equal_to   greater,  less  greater_equal  less_equal
    template<class T>
    struct equal_to : public binary_function<T,T,bool>
    {
        // ==
        bool operator()(const T& x,const T& y) const{
            return x == y;
        }
    };

    template<class T>
    struct not_equal_to : public binary_function<T,T,bool>
    {
        bool operator()(const T& x , const T& y) const{
            return x!= y;
        }
    };

    template<class T>
    struct greater : public binary_function<T,T,bool>
    {
        bool operator()(const T& x,const T& y) const{
            return  x > y;
        }
    };

    template<class T>
    struct less : public binary_function<T,T,bool>
    {
        bool operator()(const T& x , const T& y)const{
            return x < y;
        }
    };
    
    template<class T>
    struct greater_equal : public binary_fuction<T,T,bool>
    {
        bool operator()(const T& x,const T& y) const{
            return x >= y;
        }
    };

    template<class T>
    struct less_equal : public binary_function<T,T,bool>
    {
        bool operator()(const T& x , const T&y) const{
            return x <= y;
        }
    };


    //逻辑仿函数
    //logical_and  logical_or  logical_not
    template<class T>
    struct logical_and : public binary_function<T,T,bool>
    {
        bool operator()(const T& x, const T& y) const {
            return x && y;
        }
    };
    
    template<class T>
    struct logical_or : public binary_function<T,T,bool>
    {
        bool operator()(const T& x, const T& y) const{
            return x || y;
        }
    };

    template<class T>
    struct logical_not : public unary_function<T,bool>
    {
        bool operator()(const T& x) const{
            return !x;
        }
    };
    

    //证同，选择，投射
    //只将参数原封不动的返回，之所以有这般设计是为了增加间接性
    //暂时不懂
    template<class T>
    struct identity : public unary_function<T,T>
    {
        const T& operator()(const T& x) const{
            return x;
        }
    };


    //  选择第一个  类似于map那种  输入一个pair  返回first 就是 pair对象.first
    template<class Pair>
    struct select1st : public unary_function<Pair,typename Pair::first_type{
        //返回类型是first_type类型  输入pair类型
        const typename Pair::first_type & operator()(const Pair& x){
            return x.first;
        } 
    };
    //选择第二个
    template<class Pair>
    struct select2nd : public unary_function<Pair::typename Pair::second_type>
    {
        //返回类型是pair的second_type类型 
        const typename Pair::second_type& operator()(const Pair& x){
            return x.second;
        }
    };

//干啥的？-----------------
    template<class Arg1,class Arg2>
    struct project1st : public binary_function<Arg1,Arg2,Arg1>
    {
        Arg1 operator()(const Arg1& x,const Arg2& )const {
            return x;
        }
    };

    template<class Arg1,class Arg2>
    struct project2nd : public binary_function<Arg1,Arg2,Arg1>
    {
        Arg2 operator()(const Arg1& , const Arg2& y) const{
            return y;
        }
    };
    
    //函数适配器  下面的
    //less<int>()就是二元谓词对象
    //bind2nd(less<int>(),40) 是一元谓词对象 
    template<class Predicate>
    class unary_negate
        : public unary_function<typename Predicate::argument_type,bool>{
    protected:
        Predicate pred;//一元谓词对象

    public:
        explicit unary_negate(const Predicate &x) : pred(x){}
        bool operator()(const typename Predicate::argument_type & x) const{
            return !pred(x);
        }
    };





    
    
}