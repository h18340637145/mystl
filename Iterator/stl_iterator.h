#pragma once

#include <cstddef>// prtdiff_t 定义
#include <iostream>

namespace MiniSTL{

    //五种迭代器类型
    struct input_iterator_tag{};
    struct output_iterator_tag{};
    struct forward_iterator_tag : public input_iterator_tag{};
    struct bidirectional_iterator_tag : public forward_iterator_tag{};
    struct random_access_iterator_tag : public bidirectional_iterator_tag{};

    //为了避免遗忘记traits,此基类以供自定义iterator继承之
    //迭代器创建时需要提供两个参数即可，Category T  其他自动默认
    template<class Category, class T ,class Distance = ptrdiff_t,
            class Pointer = T*,class Reference = T&>
    struct iterator{
        using iterator_category = Category;//迭代器分类
        using value_type = T;//迭代器指向 的类型
        using difference_type = Distance;//距离，标准提供的类型，模板声明时进行更名
        using pointer = Pointer;//迭代器指向数据的指针类型
        using reference = Reference;//迭代器指向数据的引用类型
    };

// 创建一个迭代器
// template<Item>
// struct ListIter:
//     std::iterator<std::forward_iterator_tag,Item>{
//         ....仅仅提供两个参数即可
//     }


    //traits 展现Iterator所有特性
    //之所以不直接使用别名模板完全替代，是因为存在偏特化版本
    //似乎别名模板无法偏特化(别名模板是指一族类型的名字)
    template<class Iterator>
    struct iterator_traits{
        //typename 的作用：Iterator::iterator_category是类型而不是变量，需要使用typename来修饰
        using iterator_category = typename Iterator::iterator_category;//分类就是当前迭代器的分类
        using value_type = typename Iterator::value_type;
        using difference_type = typename Iterator::difference_type;
        using pointer = typename Iterator::pointer;
        using reference = typename Iterator::reference;
    };

    //针对raw pointer设计的偏特化版本(原始指针)
    template<class T>
    struct iterator_traits<T*>{
        using iterator_category = random_access_iterator_tag;//原始指针的迭代器分类，随机访问
        using value_type = T;//迭代器指向的类型  就T*中的T类型  直接设置
        using difference_type = ptrdiff_t;//距离---定义好的库
        using pointer = T*;
        using reference = T&;
    };

    //针对pointer to const 设计的偏特化版本
    template<class T>
    struct iterator_traits<const T*>{
        using iterator_category = random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T*;//因为本身是const ptr  萃取同样是const ptr
        using reference = const T&;
    };

    //以下为模仿C++14 type_traits_t 而设定的别名模板
    template<class Iterator>
    using iterator_category_t = //迭代器分类
        typename iterator_traits<Iterator>::iterator_category;

    template<class Iterator>
    using value_type_t = 
        typename iterator_traits<Iterator>::value_type;
    
    template<class Iterator>
    using difference_type_t =
        typename iterator_traits<Iterator>::difference_type;
    
    template<class Iterator>
    using pointer_t = 
        typename iterator_traits<Iterator>::pointer;
    
    template<class Iterator>
    using reference_t = 
        typename iterator_traits<Iterator>::reference;

    //以下为整租distance函数
    template <class InputIterator>//返回值是个ptrdiff_t的距离值
    inline difference_type_t<InputIterator> __distance(InputIterator first,
                                                        InputIterator last,
                                                        input_iterator_tag){//tag 设置为输入型
        //调用方式__distance(iterator,iterator),根据iterator类型自动填充模板
        difference_type_t<InputIterator> n = 0;//初始距离设置为0
        while (first != last)//两个迭代器指针之间的距离来记录距离
        {
            //为input_iterator_tag 设置 只能向前的指针，
            //因为input是四个输入型迭代器的根，只有随机迭代器求解方法不同，所以只实现两种方式
            //left != right   left++ cnt++
            ++first ,++n;
        }
        return n;
    }

    template<class InputIterator>
    inline difference_type_t<InputIterator> __distacne(InputIterator first,
                                                        InputIterator last,
                                                        random_access_iterator_tag){//tag设置为随机
        //因为是随机存取的指针，
        //所以直接距离是差值，直接return
        return last - first;
    }



//此时的distance 可以接受任何类型的迭代器
    template<class InputIterator>
    inline difference_type_t<InputIterator> distance(InputIterator first,
                                                     InputIterator last){
        //调用distance()函数，输入first，base = sub;调用底层__distance
        return __distacne(first,last,iterator_category_t<InputIterator>());//传入一个对象
                                    // 只需要传入迭代器，就会自动萃取出对应类别
    }

        
    //以下为整组advance函数
    //功能： 将iterator迭代器移动n个位置(根据迭代器类别不同，移动可以不一样)
    //向前
    //向前，向后
    //随机
    template <class InputIterator,class Distance>
    inline void __advance(InputIterator &it,Distance n ,input_iterator_tag){
        while(n--){
            ++it;
        }
        //此函数为input_iterator_tag 只能向前移动
    }

    template <class InputIterator,class Distance>
    inline void __advance(InputIterator& it,Distance n,bidirectional_iterator_tag){
        //双向迭代器，n为+  向前进   n为- 向后  n为0  都可以  反正是while都是false
        if(n >= 0){
            while(n--){
                ++it;
            }
        }
        else{
            while(n++){
                --it;
            }
        }
    }

    template<class InputIterator,class Distance>
    inline void __advance(InputIterator& it,Distance n , random_access_iterator_tag){
        it+=n;//随机访问，直接+n即可
    }

    template<class InputIterator,class Distance>
    inline void advance(InputIterator &it ,Distance n){
        __advance(i,n,iterator_category_t<InputIterator>());
        //传入一个InputIterator对象(实体可能是其子类的对象) 根据traits进行萃取，获取tag
        //因为创建iterator的时候，需要两个参数，tag和value_type，所以tag会被自动获取出来
    }

    //以下为三种迭代适配器
    //insert  reverse  stream
//------------------------------------------------------------------------------//
    //insert:back_insert,front_insert, insert
/*
*class  :   back_insert_iterator
            继承了iterator<output_iterator_tag, void, void, void, void>。
            所以迭代器的5种型别它只具有一种。
*type   :   output iterator 持有一个容器的指针，用于调用push_back
            与普通迭代器不同的是：他不指向任何元素，他的唯一一个
            数据成员指向一个容器的指针，某种程度上可以看做永远指向end()
            唯一有意义的操作就是赋值*it = x
*/

// 例子：
// back_insert_iterator<vector<int> > bii(v);
// bii = 6;

    template<class Container>
    class back_insert_iterator{
    protected:
        Container *Container;//底层容器

    public:
        using iterator_category = output_iterator_tag;
        using value_type = void;
        using difference_type = void;
        using pointer = void;
        using reference = void;

    public:
        //构造器
        explicit back_insert_iterator(Container &value) : container(value){}
        // 唯一有意义的操作:赋值*it = x    使用operator=  数据类型是value_type
        back_insert_iterator& operator=(const typename Container::value_type &value){
            container->push_back(value);//本质上是调用了push_back
            return *this;//返回back_insert_iterator对象
        }
        //以下三个接口对back_insert_iterator无用，关闭功能(为什么不直接设置为private？)
        back_insert_iterator operator*(){return *this;}//无意义  直接返回this
        back_insert_iterator operator++(){return *this;}
        back_insert_iterator operator++(int) {return *this;}
    };

     //以下是一个辅助函数，便于快速使用back_insert_iterator
    template<class Container>
    inline back_insert_iterator<Container> back_inserter(Container& x){
        return back_insert_iterator<Container>(x);
    }
//------------------------------------------------------------------------------//
//------------------------------------------------------------------------------//
    // front_insert_iterator输出型迭代器 
    //front_insert_iterator 无法用于vector，因为后者不具备push_front
    template<class Container>
    class front_insert_iterator
    {
    protected:
        Container * container;//底层容器
    public:
        using iterator_category = output_iterator_tag;
        using value_type = void;
        using difference_type = void;
        using pointer = void;
        using reference = void;
    public:
        explicit front_insert_iterator(Container& value):container(value){}
        front_insert_iteator& operator=(const typename Container::value_type &value){
            container->push_front(value);
            return *this;
        }
        front_insert_iterator operator*(){return *this;}
        front_insert_iterator operator++(){return *this;}
        front_insert_iterator operator++(int){return *this;}
    };

    template<class Container>
    inline front_insert_iterator<Container> front_insert(Container & x){
        return front_insert_iterator<Container>(x);
    }

//------------------------------------------------------------------------------//
//------------------------------------------------------------------------------//
    // insert_iterator 可以在容易的某个迭代器指定的位置插入元素
    // 输出型迭代器  
    template<class Container>
    class insert_iterator{
    protected:
        Container * Container;//底层容器
        //当前容器的迭代器类型
        typename Container::iterator iter;//标记插入元素位置的迭代器指针

    public:
        using iterator_category = output_iterator_tag;
        using value_type = void;
        using difference_type = void;
        using pointer = void;
        using reference = void;

    public:
        insert_iterator(Container& value) : Container(value){}
        insert_iterator& operator=(const typename Container::value_type){
            container.insert(iter,value);//本质是在iter的后插入，
            //为了让迭代器永远与目标贴合
        //比如  1 it  2   插入3   1  it 3 2  此时it++变成  1  3  it  2 
        // 再插入4     1 3 4  it 2  所以it指针要不断的++
            ++iter;
            return *this;
        }
  
        //以下三个接口对insert_iterator无用，关闭功能
        insert_iterator operator*(){return *this;}
        insert_iterator operator++(){return *this;}
        insert_iterator operator++(int){return *this;}
    };
    template<class Container,class Iterator>
    inline insert_iterator<Container> inserter(Container&x,Iterator it){
        return insert_iterator<Container>(x,i);
    }

    //__reverse_iterator 
    template<class Iterator>
    class __reverse_iterator  {
        template<class It>
        friend bool operator==(const __reverse_iterator  <It> & ,
                                const __reverse_iterator  <It> &);
        
        template<class It>
        friend bool operator!=(const __reverse_iterator <It> &,
                                const __reverse_iterator <It> &);
        
    protected:
        Iterator current;//与之对应的正向迭代器

    public:
        using iterator_category = iterator_category_t<Iterator>;
        using value_type = value_type_t<Iterator>;
        using difference_type = difference_type_t<Iterator>;
        using pointer = pointer_t<Iterator>;
        using reference = reference_t<Iterator>;

        using iterator_type = Iterator;//正向迭代器
        using self = __reverse_iterator ;//反向迭代器---就是自己

    public:
        //构造器
        __reverse_iterator (){}
        //单参数构造器，对与之对应的正向迭代器赋值
        explicit __reverse_iterator (iterator_type value) : current(value){}
        //拷贝构造器  使用一个自己类型的对象，对当前对象赋值。实现成员变量赋值
        __reverse_iterator (const self& value) : current(value.current){}

        //获取底层的正向迭代器（iterator_type类型）
        iterator_type base() const{return current;}
        reference operator*() const{
            Iterator temp = current;//对反向迭代器取值，相当于对与之对应的正向迭代器
            return *--temp;//向前移动一次之后取值的结果
        }

        pointer operator->() const{//pointer是指针 是地址  比引用多了个&（）
            return &(operator*());
        }

        self& operator++(){//反向迭代器++  相当于与之对应的迭代器--
            --current;
            return *this;//返回当前对象，本质已经变过，返回引用没问题，因为不是局部引用，而是整体对象的引用
        }

        self operator++(int){//a++  先返回a   再将self+1
            self temp = *this;//返回的是一个新声明的，临时对象，不能使用临时引用，只能使用值返回
            --current;
            return temp;
        }

        self& operator--(){
            ++current;
            return *this;
        }

        self operator--(int){
            self temp = this;
            ++current;
            return temp;
        }
        self operator+(difference_type n)const{     //指针移动n个单位
            return self(current - n);
        }

        self& operator+=(difference_type_t n) const{
            current -= n;
            return *this;
        }

        self operator-(difference_type n) const{
            return self(current+n);
        }
        self& operator-=(difference_type n) const {
            current += n;
            return *this;
        }

        // 第n个元素
        //首地址+n
        reference operator[](difference_type n) const {
            return *(*this + n);
        }

        //为什么一个比较current  一个比较this
        bool operator==(const self &rhs) const {
            return current == rhs.current;
        }
        bool operator!=(const self &rhs) const{
            return !((*this) == rhs);
        }
    };

    //比较两个迭代器，使用其中一个对象调用operator==运算符
    template<class Iterator>
    inline bool operator==(const __reverse_iterator <Iterator> &lhs,
                            const __reverse_iterator <Iterator> &rhs){
        return lhs.operator==(rhs);
    }

    template<class Iterator>
    inline bool operator!=(const __reverse_iterator <Iterator> &lhs,
                            const __reverse_iterator <Iterator> &rhs){
        return !(lhs == rhs);//调用了operator==
    }

    template<class Iterator>
    inline bool operator<(const __reverse_iterator <Iterator> &lhs,
                        const __reverse_iterator <Iterator> & rhs){
        return rhs.base() < lhs.base();
        //base  
        //new 大 r    l 小    if l < r---->  r.base() < l.base()
    }

    template<class Iterator>
    inline bool operator>(const __reverse_iterator <Iterator> &lhs,
                        const __reverse_iterator <Iterator> &rhs){
        return rhs < lhs;
        //base
        //new   l      r   ----> l > r--->  r.base > l.base
    }
    
    template<class Iterator> 
    inline bool operator<=(const __reverse_iterator <Iterator> &lhs,
                        const __reverse_iterator <Iterator> &rhs){
        return !(rhs > lhs)
        //base                  l.base() >= r.base() 
        //new       r       l    l <= r ---- > !(r < l)
    }

    template<class Iterator>
    inline bool operator>=(const __reverse_iterator <Iterator> &lhs,
                            const __reverse_iterator <Iterator> &rhs){
        return  !(lhs < rhs);
        //new  l------r    !(l<r)
    }

    template<class Iterator>
    inline typename __reverse_iterator <Iterator>::difference_type operator-(
        const __reverse_iterator <Iterator> &lhs,
        const __reverse_iterator <Iterator> &rhs){
        return rhs.base() - lhs.base();
    }

    template<class Iterator>
    inline __reverse_iterator<Iterator> operator+(
        //base       1 2 3 4(x)  5 6
        //new        1 2 3 4     5 6(x)   + 2  相当于base-2
        typename __reverse_iterator<Iterator>::difference_type n,
        const __reverse_iterator<Iterator> &x){
        return __reverse_iterator<Iterator>(x.base()-n);
    }

    //stream:   input_stream  ,  output_stream
    template <class T,class Distance = ptrdiff_t>
    class istream_iterator{
    protected:
        std::istream *stream;
        T value;
        bool end_maker;
        void read(){
            //stream 有值  就输入   没值了就不输入
            end_maker = (*stream) ? true : false;   
            if (end_marker == true) 
            {
                * stream >> value;
            }
            //完成输入后，stream状态可能发生了改变，再次判定
            end_maker = (*stream) ? true : false;
        }
    public:
        using iterator_category = input_iterator_tag;
        using value_type = T;
        using difference_type = Distance;
        using pointer = const T*;//由于身为input_iterator  采用const较为保险
        using reference = const T&;
                            
                            //继承一个标准输入
        istream_iterator() : stream(&std::cin),end_marker(false){}
        istream_iterator(std::istream &s) : stream(&s){
            read();//使用一个输入流（true）构造
        }

        reference operator*() const { return value; }
        pointer operator->() const { return &(operator*()); }

        istream_iterator & operator++(){
            read();
            return *this;
        }

        istream_iterator operator++(int)
        {
            istream_iterator temp = *this;
            read();
            return temp;
        }
    };

    template<class T>
    class ostream_iterator{
    protected:
        std::ostream* stream;
        const char * interval;//输出间隔符  默认没有

    public:
        //  一般输出只有cate  其他都是void
        using iterator_category = output_iterator_tag;
        using value_type = void;
        using difference_type = void;
        using pointer = void;
        using reference = void;

        ostream_iterator() : stream(&std::cout),interval(nullptr){}
        ostream_iterator(std::ostream & s,const char *c) : stream(s),interval(c){}

        ostream_iterator & operator=(const T &value){
            *stream << value;
            if (interval)
            {
                *stream << interval;
            }
            return *this;
        }
        ostream_iterator & operator*(){
            return *this;
        }

        ostream_iterator & operator++(){
            return *this;
        }

        ostream_iterator operator++(int)
        {
            return *this;
        }
    };
}//end  namespace MiniSTL

