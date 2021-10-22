#pragma once

namespace MiniSTL{
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
    
    //����������  �����
    //less<int>()���Ƕ�Ԫν�ʶ���
    //bind2nd(less<int>(),40) ��һԪν�ʶ��� 
    template<class Predicate>
    class unary_negate
        : public unary_function<typename Predicate::argument_type,bool>{
    protected:
        Predicate pred;//һԪν�ʶ���

    public:
        explicit unary_negate(const Predicate &x) : pred(x){}
        bool operator()(const typename Predicate::argument_type & x) const{
            return !pred(x);
        }
    };





    
    
}