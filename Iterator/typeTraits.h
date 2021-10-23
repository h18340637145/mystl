#pragma once
namespace MiniSTL{
    namespace{
        template<bool,class Ta,class Tb>
        struct IfThenElse;

        template <class Ta,class Tb>
        struct IfThenElse<true,Ta,Tb>{//特化
            using result = Ta;  //result就是Ta类型
        };
    }//namespace


    struct _true_type {};
    struct _false_type{};

    //默认自定义类型均为non_POD类型
    //非plain old data  旧数据类型  非基本类型
    template <class T>
    struct _type_traits
    {
        using has_trival_default_constructor = _false_type;//无冗余默认构造器
        using has_trival_copy_constructor = _false_type;//无冗余拷贝构造器
        using has_trival_assignment_operator = _false_type;//无冗余赋值
        using has_trival_destructor = _false_type;//无冗余析构
        using is_POD_type = _false_type;//non_POD类型
    };

//有类型的都要有冗余默认--类类型都要重新写
//上面那个需要指定类型的没有冗余默认构造，那么应该自己重写构造器
    template<>
    struct _type_traits<bool>
    {
        using has_trival_default_constructor = _true_type;//冗余默认构造
        using has_trival_copy_constrctor = _true_type;//
        using has_trival_assignment_operator = _true_type;//
        using has_trival_default_constructor = _true_type;//
        using is_POD_type = _true_type;//
    };

    template<>
    struct _type_traits<char>
    {
        using has_trival_default_constructor = _true_type;//冗余默认
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor = _true_type;
        using is_POD_type = _true_type;
    };
    
    template<>
    struct _type_traits<unsigned char>
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor  = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<signed char>
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor  = _true_type;
        using is_POD_type = _true_type;
    };


    template<>
    struct _type_traits<wchar_t>
    {
        using has_trival_default_constuctor = _true_type;
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    // short
    // unsigned short
    // int 
    // unsigned int
    // long
    // unsigned long
    // long long
    // unsigned long long
    // float
    // double
    // long double

    // 偏特化T*
    
    template<>
    struct _type_traits<short>
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor  = _true_type;
        using is_POD_type = _true_type;
    };


    template<>
    struct _type_traits<unsigned short>
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_default_constructor = _true_type;
        using is_POD_type = _true_type;
    };
    
    template<>
    struct _type_traits<long>
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor  = _true_type;
        using has_triavl_assignment_operator = _true_type;
        using has_trival_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<long long>
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator  = _true_type;
        using has_trival_destructor  = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<unsigned long>
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor = _true_type;
        using is_POD_type = _true_type;
    };
    template<>
    struct _type_traits<unsigned long long>
    {
        using has_trival_default_constructor  = _true_type;
        using has_trival_copy_constructor =  _true_type;
        using has_trival_assignment_operator  = _true_type;
        using has_trival_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<float>
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<double> 
    {
        using has_trival_default_constructor = _true_type;//double是true_type类型的对象3
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator  = _true_type;
        using has_trival_destructor = _true_type;
        using is_POD_type = _true_type;
    };
    template<>
    struct _type_traits<long double>
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator  = _true_type;
        using has_trival_destructor  = _true_type;
        using is_POD_type = _true_type;
    };
    

    //偏特化*
    template<class T>
    struct _type_traits<T*> 
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor  = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor  = _true_type;
        using is_POD_type = _true_type;
    };

    //偏特化const*
    template<class T>
    struct _type_traits<const T*>
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor  = _true_type;
        using is_POD_type = _true_type;
    };
        
    template<>
    struct _type_traits<char*>  
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor  = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<const char*>
    {
        using has_trival_default_constructor  = _true_type;
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<unsigned char*>
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<signed char *>
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<const unsigned char*>
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<const signed char*>
    {
        using has_trival_default_constructor = _true_type;
        using has_trival_copy_constructor = _true_type;
        using has_trival_assignment_operator = _true_type;
        using has_trival_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    //is_integer(默认自定义类型为false)
    //是整型类型？   整型类型就是POD类型
    template<class T>
    struct _is_integer
    {
        using _integral = _false_type;//默认对象的类类型不是传统的POD类型
    };

    //simluate C++14
    template<class T>
    using _is_integer_t = typename _is_integer<T>::_integral;

//其他都是POD integral类型 设置为_true_type
    template<>
    struct _is_integer<bool>
    {
        using _integral = _true_type;   
    };

    template<>
    struct _is_integer<char>
    {
        using _integral = _true_type;
    };

    template<>
    struct _is_integer<signed char>
    {
        using _integral = _true_type;
    };

    template<>
    struct _is_integer<unsigned char>
    {
        using _integral = _true_type;
    };

    template<>
    struct _is_integer<wchar_t>
    {
        using _integral = _true_type;
    };

    template<>
    struct _is_integer<short>
    {
        using _integral = _true_type;
    };

    template<>
    struct _is_integer<unsigned short>
    {
        using _integral = _true_type;
    };

    template<>
    struct _is_integer<int>
    {
        using _integral = _true_type;
    };

    template<>
    struct _is_integer<unsigned int>
    {
        using _integral = _true_type;
    };

    template<>
    struct _is_integer<long>
    {
        using _integral = _true_type;
    };

    template<>
    struct _is_integer<unsigned long>
    {
        using _integral = _true_type;
    };

    template<>
    struct _is_integer<long long>
    {
        using _integral  = _true_type;
    };

    template<>
    struct _is_integer<unsigned long long>
    {
        using _integral = _true_type;
    };
}//namespace MiniSTL