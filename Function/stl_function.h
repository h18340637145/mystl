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


    //unary_compose  and  binary_compose(not in standard)
    // 都是用于参数合成
// f(x)=3*x，g(y)=y+2，那么compose1(f(x),g(y))=(y+2)*3----第二的操作的结果作为第一个操作的，返回值类型与第一个操作相同
    template <class Operation1, class Operation2>
    class unary_compose : public unary_function<typename Operation2::argument_type,
                                                    typename Operation::result_type>{
    protected:
        Operation1 op1;
        Operation2 op2;
    public:
        unary_compose(const Operation1 & x,const Operation2& y) : op1(x),op2(y){}
        // x y 都是函数类型，继承自unary_function
        typename Operation1::result_type operator()(
            const typename Operation2::argument_type & x) const{
            return op1(op2(x));//第二个函数操作后的结果作为第一个函数的输入
        }
    };

    template<class Operation1,class Operation2>
    inline unary_compose<Operation1,Operation2> compose1(const Operation1& op1,
                                                        const Operation2& op2){
        return unary_compose<Operation1,Operation2>(op1,op2);//
    }
    /*
        compose1(f(x),g(x));
        --->return  unary_compose<f(x)类型,g(x)类型)>(f(x),g(x))
        本身就只是返回这个函数类型的对象  如果想时候，还需要在后面加上调用()使用的参数
        后续使用的时候，是定义unary_compose对象，使用obj(x)去使用
    */
    
// g1(x)=3*x，g2(x)=2*x，h(x)=a+b，那么compose2(h(x),g1(x),g2(x))=3*x+2*x
// 函数是一个参数 一个返回值的unary  
//返回类型是hx的  g1 和 g2进行操作  外界输入的操作数是g1的参数 x的类型 的
    template<class Operation1,class Operation2,class Operation3>
    class binary_compose : public unary_function<typename Operation2::argument_type,
                                                typename Operation1::result_type>{
    protected:
        Operation1 op1;
        Operation2 op2;
        Operation3 op3;
    public :
        binary_compose(const Operation1& x,const Operation2 &y,
                        const Operation3 &z) : op1(x),op2(y),op3(z){}
        typename Operation1::result_type operator()(
            const typename Operation2::argument_type & x
        )const {
            return op1(op2(x),op3(x));
        }
    };

    //返回组合后的函数 binary_compose类型的  
    template<class Operation1 , class Operation2,class Operation3>
    inline binary_compose<Operation1,Operation2,Operation3> compose2(
        const Operation1 & op1,const Operation2& op2,const Operation3& op3
    ){
        return binary_compose<Operation1,Operation2,Operation3>(op1,op2,op3);
    }

// 指向函数的指针对象  obj->ptr(arg)
// obj(arg)  相当于obj->ptr(x)
    template<class Arg,class Result>
    class pointer_to_unary_function : public unary_function<Arg,Result>{
    protected:
        Result (*ptr)(Arg);//指向参数为Arg的函数指针

    public:
        pointer_to_unary_function(){}
        explicit pointer_to_unary_function(Result (*x)(Arg)) : ptr(x){}
        Result operator()(Arg x) const {
            return ptr(x);
        }
    };

    template<class Arg,class Result>
    inline pointer_to_unary_function<Arg,Result> ptr_fun(Result (*x)(Arg)){
        // 返回这个对象pointer_to_unary_function类型的
        return pointer_to_unary_function<Arg,Result>(x);
    }


    template<class Arg1,class Arg2,class Result>
    class pointer_to_binary_function : public binary_function<Arg1,Arg2,Result>{
    protected:
        Result (*ptr)(Arg1,Arg2);

    public:
        pointer_to_binary_function(){}
        explicit pointer_to_binary_function(Result (*x)(Arg1,Arg2)) : ptr(x){}
        Result operator()(Arg1,Arg2) const{
            return ptr(x,y);
            //仿函数的作用是调用ptr
        }
    };

    template<class Arg1,class Arg2 , class Result>
    inline pointer_to_binary_function<Arg1,Arg2,Result> ptr_fun(
        Result (*x)(Arg1,Arg2)
    ){
        return pointer_to_binary_function<Arg1,Arg2,Result>(x);
        //构造对象
    }
    /*
    *指向友元的无参成员函数
    */
    template<class S,class T>
    //obj(p)  //返回 指向  友元对象p中的函数  的函数指针
    class mem_fun_t : public unary_function<T*,S>{
    public:
        explicit mem_fun_t(S (T::*pf)()) : f(pf){}
        S operator()(T *p) const{
            return (p->*f)();
        }
    private:
        S (T::*f)();//指向友元类中 函数指针
    };

    template<class S, class T>
    //obj(friendObj)
    class const_mem_fun_t : public unary_function<const T*,S>{
    public:
        explicit const_mem_fun_t(S (T::*pf)() const) : f(pf){}
        S operator()(const T* p) const {
            return (p->*f)();
        }
    private:
        S (T::*f)() const;
    };

    template<class S,class T>
    class mem_fun_ref_t : public unary_function< T,S>{
    public:
        explicit mem_fun_ref_t(S (T::*pf)()) : f(pf){}// S 类型的  指向友元类T的成员函数的函数指针
        S operator()(T& r) const{
            return (r.*f)();//友元对象r  中的函数指针*f   r.*f)()
        }
    private:
        S (T::*pf)();
    };

    template<class S,class T>
    class const_mem_fun_t : public unary_function<const T,S>{
    public :
        explicit const_mem_fun_t(S (T::*pf)() const) : f(pf){}
        S operator()(const T& r) const{
            return (r.*f)();
        }
    private:
        S (T::*f)() const;
    };

    /*
    *指向友元的一参成员函数
    */

    template<class S,class T,class Arg>
    class mem_fun1_t : binary_function<T*,Arg,S>{//S是返回类型  
    public:
        explicit mem_fun1_t(S (T::*pf)(Arg) const) : f(pf){}
        S operator()(T* p,Arg x) const { //指定友元类，和参数
            return (p->*f)(x);
        }
    private:
        S (T::*f)(Arg ) const;
    };
    //const
    template<class S,class T,class Arg>
    class const_mem_fun1_t :binary_function<const T*,Arg,S>{
    public:
        explicit const_mem_fun_t(S (T::*pf)(Arg x) const) : f(pf){}
        S operator()(const T* p, Arg x) const{
            return (p->*f)(x) ;
        }
    private: 
        S (T::*f)(Arg) const;
    };

    template<class S, class T,class Arg>
    class mem_fun1_ref_t : binary_function<T, Arg,S>{
    public: 
        explicit mem_fun_ref_t(S (T::*pf)(Arg x)) : f(pf){}
        S operator()(T & r, Arg x) const{
            return (r.*f)(x);
        }
    private:
        S (T::*f)(Arg) ;
    };

    template<class S,class T,class Arg>
    class const_mem_fun1_ref_t : binary_function<T,Arg,S>{
    public :
        explicit const_mem_fun1_ref_t(S (T::*pf)(Arg x) : f(pf){}
        S operator(const T& r,Arg x) const{
            return (r.*f)(x);
        }

    private:
        S (T::*f) const;
    };

    //找到友元类的函数  但不返回  operator()不需要return 直接调用()即可
    template<class T>//只需要知道友元类即可  返回类型不需要了，
    class mem_fun_t<void,T> : public unary_function<T*,void>{
    private:
        void (T::*f)();//无返回类型
    public:
        explicit mem_fun_t(void (T::*pf)()) : f(pf){}
        void operator()(T * p) const {
            (p->*f)();
        }
    };

    template<class T>
    class const_mem_fun_t<void,T> : unary_function<const T*,void>{
    private:
        void (T::*f)() const;
    public :
        explicit const_mem_fun_t(void (T::*pf)() ) : f(pf){}
        void operator(const T * p) const
        {
            (p->*f)();
        }
    };

    template<class T>
    class mem_fun_ref_t<void , T> : public unary_function<T,void>{
    private:
        void (T::*f)();
    public:
        explicit mem_fun_ref_t(void (T::pf)()) : f(pf){}
        void operator()(const T& r)const{
            (r.*f)();
        }
    };

    template<class T>
    //<void,T>
    class const_mem_fun_ref_t : public unary_function< T,void>{
    private:
        void (T::*f)() const;
    public:
        explicit const_mem_fun_ref_t(void (T::*pf)() const) : f(pf){}
        void operator()(const T& r) const{
            (r.*f)();
        }
    };

    //一个参数，无返回类型的友元函数指针 mem_fun1_t
    template<class T,class Arg>
    class mem_fun1_t : public binary_function<T*,Arg,void>{
    private:
        void (T::*f)(Arg) ; 
    public:
        explicit mem_fun1_t(void (T::*pf)(Arg )) : f(pf){}
        void operator(T * p , Arg x) const{
            (p->*f)(x);
        }
    };

    template<class T,class Arg>
    class const_mem_fun1_t : public binary_function<const T*,Arg,void>{
    private:
        void (T::*f)(Arg);
    public:
        explicit const_mem_fun1_t(void (T::*pf)(Arg)) : f(pf){}
        void operator()(const T* p, Arg x) const{
            (p->*f)(x);
        }
    };

    template<class T,class Arg>
    class mem_fun1_ref_t : public binary_function<T ,Arg,void>{
    private:
        void (T::*f)(Arg);
    public:
        explicit mem_fun1_ref_t(void (T::*f)(Arg)) : f(pf){}
        void operator(T& r,Arg x)const{
            (r.*f)(x);
        }
    };

    template<class T,class Arg>
    class const_mem_fun1_ref_t : public binary_function<const T,Arg,void>{
    private:
        void (T::*f)(Arg);
    public:
        explicit const_mem_fun1_ref_t(void (T::*pf)(Arg)) : f(pf){}
        void operator(const T & r, Arg x) const {
            (r.*f)(x);
        }
    };
    


}