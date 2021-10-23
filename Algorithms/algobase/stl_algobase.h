#pragma once

namespace MiniSTL{
// STL�еķº������¿��Է�Ϊ������
// 1.������
// 2.�߼�������
// 3.��Թ�ϵ��

// ���е�STL�º�������Ҫ�̳�binary_function<T, T, bool>(��������)
// ����unary_function<T, bool>(һ������),
// ֻ�м̳��������ָ���,�������ķº����ſ�������STL


    //һԪ�����
    //��������  1  ����ֵ
    template<class Arg,class Result>
    struct unary_function
    {
        using arguement_type = Arg;
        using result_type = Result;
    };

    //��Ԫ������ṹ����
    //��������  1  2  ����ֵ
    template<class Arg1,class Arg2,class Result>
    struct binary_function
    {
        using first_argument_type = Arg1;
        using second_argument_type = Arg2
        using result_type = Result;
    };

    //����������������º���
    //plus  minus  multipiples  divides modulus negate
    template<class T>
    struct plus:public binary_function<T,T,T>
    {
        //�º���������дoperator()(param)
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
    //�˷��������� 1 2 ����ֵ
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
        
    //֤ͬԪ�أ����Ǳ�׼STL��Ҫ��
    template<class T>
    inline T identity_element(plus<T>){
        return T(0);
    }

    template<class T>
    inline T identity_element(multiplies<T>){
        return T(1);
    }

    //�����ϵ�º���
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


    //�߼��º���
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
    

    //֤ͬ��ѡ��Ͷ��
    //ֻ������ԭ�ⲻ���ķ��أ�֮��������������Ϊ�����Ӽ����
    //��ʱ����
    template<class T>
    struct identity : public unary_function<T,T>
    {
        const T& operator()(const T& x) const{
            return x;
        }
    };


    //  ѡ���һ��  ������map����  ����һ��pair  ����first ���� pair����.first
    template<class Pair>
    struct select1st : public unary_function<Pair,typename Pair::first_type{
        //����������first_type����  ����pair����
        const typename Pair::first_type & operator()(const Pair& x){
            return x.first;
        } 
    };
    //ѡ��ڶ���
    template<class Pair>
    struct select2nd : public unary_function<Pair::typename Pair::second_type>
    {
        //����������pair��second_type���� 
        const typename Pair::second_type& operator()(const Pair& x){
            return x.second;
        }
    };

//��ɶ�ģ�-----------------
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
    


    //����������  ����� ======== ����������� Ҳ��һ�ֺ�������
    //less<int>()���Ƕ�Ԫν�ʶ���
    //bind2nd(less<int>(),40) ��һԪν�ʶ��� ----��40 �󶨵�less�����ĵڶ���������
    
    //ȡĳ��Adaptable predicate���߼���ֵ
    template<class Predicate>
    class unary_negate   //һԪ�������̳���һԪ�º���  һԪ�º����Ĳ���
        : public unary_function<typename Predicate::argument_type,bool>{
    protected:
        Predicate pred;//һԪν�ʶ���
    //Predicate ��һ��unary_function �������� ��һ������������
    public:
        explicit unary_negate(const Predicate &x) : pred(x){}
        bool operator()(const typename Predicate::argument_type & x) const{
            return !pred(x);//����һ��һԪν�ʶ���  ȡ��
            //������һ�����������  ���ú���  obj(param)
        }
    };

    template<class Predicate>
    inline unary_negate<Predicate> not1(const Predicate &pred){
        //ȡ��   not1�ǹ���һ����ν�ʽ���෴��һԪ��������
        return unary_negate<Predicate>(pred);
        //A(param)    ����param��һ���º�������  ���� less<>
    }

    template<class Predicate>
    class binary_negate
        : public binary_function<typename Predicate::first_argument_type,
                                typename Predicate::second_argument_type,
                                bool>{
    protected:
        Predicate pred;//���������
    public:
        explicit binary_negate(const Predicate & x):pred(x){}
        bool operator()(const typename Predicate::first_argument_type &x,
                    const typename Predicate::second_argument_type &y){
            return !pred(x,y);
        }
    };

    template<class _Predicate>
    inline binary_negate<_Predicate> not2(const _Predicate &pred){
        return binary_negate<_Predicate>(pred);//obj(param)  ���÷º���
    }

//���ݽ����Ĳ���һ���Ƕ�Ԫ�º�����һ�����������ж�Ԫ�º����ĵ�һ��������ȡ������
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
            return op(value,x);//���ñ��ʽ����value��Ϊ��һ������
        }
    };
    //bind2nd(less<int>(),40) ��һԪν�ʶ��� ----��40 �󶨵�less�����ĵڶ���������
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