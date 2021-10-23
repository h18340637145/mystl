#pragma once

namespace MiniSTL{
// STL中的仿函数大致可以分为三个类
// 1.算数类
// 2.逻辑运算类
// 3.相对关系类

// 所有的STL仿函数都需要继承binary_function<T, T, bool>(两个参数)
// 或者unary_function<T, bool>(一个参数),
// 只有继承自这两种父类,你声明的仿函数才可以融入STL


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
    


    //函数适配器  下面的 ======== 函数对象绑定器 也是一种函数对象
    //less<int>()就是二元谓词对象
    //bind2nd(less<int>(),40) 是一元谓词对象 ----将40 绑定到less函数的第二个参数上
    
    //取某个Adaptable predicate的逻辑负值
    template<class Predicate>
    class unary_negate   //一元适配器继承自一元仿函数  一元仿函数的参数
        : public unary_function<typename Predicate::argument_type,bool>{
    protected:
        Predicate pred;//一元谓词对象
    //Predicate 是一个unary_function 的派生类 是一个函数类类型
    public:
        explicit unary_negate(const Predicate &x) : pred(x){}
        bool operator()(const typename Predicate::argument_type & x) const{
            return !pred(x);//构造一个一元谓词对象  取反
            //本身是一个函数类对象  调用函数  obj(param)
        }
    };

    template<class Predicate>
    inline unary_negate<Predicate> not1(const Predicate &pred){
        //取反   not1是构造一个与谓词结果相反的一元函数对象。
        return unary_negate<Predicate>(pred);
        //A(param)    其中param是一个仿函数对象  比如 less<>
    }

    template<class Predicate>
    class binary_negate
        : public binary_function<typename Predicate::first_argument_type,
                                typename Predicate::second_argument_type,
                                bool>{
    protected:
        Predicate pred;//函数类对象
    public:
        explicit binary_negate(const Predicate & x):pred(x){}
        bool operator()(const typename Predicate::first_argument_type &x,
                    const typename Predicate::second_argument_type &y){
            return !pred(x,y);
        }
    };

    template<class _Predicate>
    inline binary_negate<_Predicate> not2(const _Predicate &pred){
        return binary_negate<_Predicate>(pred);//obj(param)  调用仿函数
    }

//传递进来的参数一定是二元仿函数和一个参数，其中二元仿函数的第一个参数提取出来，
    template<class Operation>
    class binder1st
        : public unary_function<typename Operation::second_argument_type,
                                typename Operation::result_type>{
    protected:
        Operation op;
        typename Operation::first_argument_type value;

    public:
        binder1st(const Operation &x,
                const typename Operation::first_argument_type &y)
            :op(x) , value(y){}
        
        typename Operation::result_type operator()(
            const typename Operation::second_argument_type &x
        )const {
            return op(value,x);//调用表达式，将value作为第一个参数
        }
    };
    //bind2nd(less<int>(),40) 是一元谓词对象 ----将40 绑定到less函数的第二个参数上
    //op ---less     value---40

    template<class Operation,class T>
    inline binder1st<Operation> bind1st(const Operation &op,const T& x){
        using Arg1_Type = typename Operation::first_argument_type;
        return binder1st<Operation>(op,static_cast<Arg1_Type>(x));
    }

    template<class Operation>
    class binder2nd:public unary_function<typename Operation::first_argument_type,
                                            typename Operation::result_type>{
    protected:
        Operation op;
        typename Operation::second_argument_type value;

    public:
        binder2nd(const Operation &x
                    const typename Operation::second_argument_type &y)
            :op(x) ,value(y){}
        typename Operation::result_type operator()(
            const typename Operation::first_argument_type &x
        )const{
            return op(x,value);
        }
    };

    template<class Operation,class T>
    inline binder2nd<Operation> bind2nd(const Operation &op,const T &x){
        using Arg2_type = typename Operation::second_argument_type;
        return binder2nd<Operation>(op,Arg2_type(x));
    }

    
    
    
}