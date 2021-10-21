#pragma once

#include <string.h> //memcpy
#include <stdlib.h>//malloc  free
#include <new> //bad_alloc

namespace MiniSTL
{
    
    //һ��������
    //ֱ�ӵ���malloc free    ����ռ����128bytes
    //һ����������allocate()   realloc()  �����ڵ���malloc()��realloc()���ɹ���
    //�ĵ���oom_malloc()  oom_realloc(),�������ѭ���ڲ��ϵĵ����ڴ治�㴦������
    //������ĳ�ε���֮�����㹻��Բ��������
    //������ڴ洦�����̲�δ���ͻ����趨��oom_malloc()  oom_realloc() �㲻�����ĵ���bad_alloc()
    class __malloc_alloc{
        //alias declaration  ��������
        using malloc_handler = void(*)();//ָ�򷵻�����Ϊvoid�ĺ��� ��ָ��ԭ�ͣ���
        // ��������������ָ��  

    private:
        //���º���ָ�����Դ����ڴ治������
        static void *oom_malloc(size_t);
        static void *oom_realloc(void* ,size_t);
        //void(*)() __malloc_alloc_oom_handler   ��������Ϊvoid�ĺ���  ��malloc_handlerȥָ�򲢵���
        static malloc_handler __malloc_alloc_oom_handler;//���Ǹ�����ָ��

    public:
        static void *allocate(size_t n)
        {
            void *result = malloc(n); // ֱ�ӵ���malloc
            if (result == nullptr)
            {
                //����ʧ�ܣ������ڴ治������,ʹ��oom_malloc�����ٴ�����
                oom_malloc(n);
            }
            return result;
        }
        static void deallocate(void * p , size_t ){
            free(p);//�ͷ�p���ռ�
        }

        static void* reallocate(void*p,size_t,size_t new_size){
            void * result = realloc(p,new_size); //ֱ�����·���ռ�
            if (result == nullptr){
                //����ڴ�ռ䲻��
                //ʹ��oom������������
                oom_realloc(p,new_size);//
            }
        }
    };

    //����handler---�ڴ治��ʱ�Ĵ���
    typename __malloc_alloc::malloc_handler   //ԭ��
        __malloc_alloc::__malloc_alloc_oom_handler = nullptr;//������ָ��һ���պ���

    //�ڴ治��ʱ�Ĵ���---malloc
    void * __malloc_alloc::oom_malloc(size_t n){
        malloc_handler new_alloc_handler;//ָ�򷵻�����Ϊvoid�ĺ�����ָ��   ԭ��������������ָ��
        //ͨ��malloc_handler�����ĺ���ָ�����ָ��  �Լ�ʵ�ֵ� �����ڴ治��� ����
        void * result;
        for(;;){
            //���ϳ����ͷţ�����
            new_alloc_handler = __malloc_alloc_oom_handler;//����ָ��
            if (!new_alloc_handler){//������  ���쳣   �ڴ治��Ĵ���ʧ�ܣ����쳣
                throw std::bad_alloc();// �൱�� new_alloc_handler == nullptr
            }
            (*new_alloc_handler)();//����handler ��ͼ�ͷ��ڴ�
            result = malloc(n);//��������
            if(result != nullptr){
                return result;
            }
        }
    }

    //�ڴ治��Ĵ���----realloc
    void * __malloc_alloc::oom_realloc(void *p, size_t n){
        malloc_handler new_alloc_handler;
        void * result;
        for(;;){
            //���ϵĳ�������
            new_alloc_handler = __malloc_alloc_oom_handler;
            if (!new_alloc_handler)
            {
                throw std::bad_alloc();
            }
            (*new_alloc_handler)();
            result = realloc(p,n);
            if(result){
                return result;
            }
        }
    }

    using malloc_alloc = __malloc_alloc;//һ��������������������

    //freelist�����趨
    //�����ϵ��߽磬�������ޣ�freelist����
    //enum�÷�
    enum __freelist_setting{
        __ALIGN = 8,//8bytesһ��
        __MAX_BYTES = 128,//���128bytes
        __NFREELISTS = __MAX_BYTES / __ALIGN //128/8 ��������
    };

    //����������
    //8 - 128 byts��ʹ�ÿռ�ʹ�ö���������ֱ������
    //�����ڴ�غ�ʹ������
    class __default_alloc{
    private:
        //��bytes�ϵ���8�ı���
        static size_t ROUND_UP(size_t bytes){
            return (((bytes) + static_cast<size_t>(__ALIGN) - 1) &
            ~(static_cast<size_t>(__ALIGN) - 1));
            // x & ~x  �൱��x*2��
        }

    private:
        //free_list�ڵ�
        //����union���ԣ�����Ҫռ�ö�����ڴ�ռ�
        union obj{
            union obj * free_list_link;//ָ����һ���ڵ�
            char client_data[1];    //ָ����Դ
        };
    private:
        static obj * volatile free_list[__NFREELISTS]; //volatile ���ȶ��ױ�� ���ĵĸı�
        //����ʹ�õڼ��Žڵ㣬��1����
        static size_t FREELIST_INDEX(size_t bytes){
            return (bytes + static_cast<size_t>(__ALIGN) -1) / 
                    static_cast<size_t>(__ALIGN) - 1;
        }
        //8  16  24  32  40  48  56
        //eg: 50bytes  57 / 8 -1 = 6  ����������  һ���ڵ�48bytes

        //����һ����СΪn�Ķ��󣬲��ҿ��ܼ����СΪn���������鵽free_list
        static void *refill(size_t n);
        //����һ���ռ䣬������nobjs����СΪsize������
        //����������ã�nobjs���ܻή��
        static char *chunk_alloc(size_t size,int &nobjs);

        //chunk allocation state �����״̬
        static char *start_free;  // n�ڴ����ʼλ�ã�ֻ��chunk_alloc()�б仯
        static char *end_free;    //�ڴ�ؽ���λ�ã�ֻ��chunk_alloc()�б仯
        static size_t heap_size;

    public:
        static void* allocate(size_t n);//����n��С�Ŀռ�
        static void deallocate(void* p,size_t n);//�ͷŵ�ַp   ��СΪn
        static void* reallocate(void * p, size_t old_sz,size_t new_sz);
        //�ͷ�p����СΪold�Ŀռ䣬ͬʱ���ڴ˴�����newsize�Ŀռ�
    };

    //��ʼ��
    char * __default_alloc::start_free = nullptr;//�ڴ����ʼλ��
    char * __default_alloc::end_free = nullptr;
    size_t __default_alloc::heap_size = 0;
    //freelist���ʼΪnullptr
    __default_alloc::obj * volatile __default_alloc::free_list[__NFREELISTS] = {
        nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,
        nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr ,nullptr 
    };

    //��free_list�޿�������ʱ���������ռ�
    //�¿ռ�ȡ���ڴ�أ�Ĭ�ϻ�ȡ20���ڵ㣨���飩
    //���ڴ�ز��㣬���ȡ�Ľ�С��20
    void *__default_alloc::refill(size_t n){
        int nobjs = 20;//obj��������
        //���Ե���chunk_alloc , ע��nobjs��pass_by_reference����
        //chunkָ��20������(obj)�ռ�
        char * chunk = chunk_alloc(n,nobjs);//�������飿  obj�ǽڵ�����
        obj * volatile *my_free_list;
        obj* result;
        obj* current_obj,*next_obj;

        //��ֻ��ȡ��һ�����飬��ֱ�ӷ���������ߣ�������free_list
        if(1 == nobjs) return (chunk);//�����鷵��
        my_free_list = free_list + FREELIST_INDEX(n);//n��ô���ֽڵĴ��ĸ��鿪ʼ��
        //my_free_list��Ϊ��ʼ��ô������ʼ

        //��chunk�ռ��ڽ���free_list
        result = reinterpret_cast<obj*>(chunk);
        //����free_listָ���ڴ�ط���ռ�
        //chunkָ����ڴ�ֱ�ӷָ��û���free_listָ��ʣ��(19�����)������
        *my_free_list = next_obj = reinterpret_cast<obj*>(chunk + n);//my_free_listָ�����֮��+n֮���λ�ã���ʾӦ�ð���n�������ȥ��
        for (int i = 0; ; i++){
            //����ѭ����
            current_obj = next_obj;
            next_obj = 
                reinterpret_cast<obj*>(reinterpret_cast<char*>(next_obj) + n);
            if (nobjs - 1 == i){
                current_obj->free_list_link = nullptr;//��ǰlistû�пռ���
                break;
            }else{
                current_obj->free_list_link = next_obj;
            }
        }
        return result;//chunkֱ�ӷ��ظ��û�
    }

    //Ĭ��sizeΪ8��������
    //�����
    char* __default_alloc::chunk_alloc(size_t size,int &nobjs){
        char* result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;//�ڴ��ʣ��ռ�
        if (bytes_left >= total_bytes){
            //������������
            result = start_free;//�ڴ�ص���ʼλ��
            start_free += total_bytes;//start_freeӦ���Ƿ���֮���ڴ�ص���ʼλ��
            return result;//���ڴ��ԭ��ʼλ�÷�����û���������û���Ҫ��size����С��nobjs���ռ�
        }else if (bytes_left > size){
            //û��nobjs��������Ŀǰ
            //������������һ������Ҫ��  �����ܵķ�����û�  ����nobjs��������total_bytes
            nobjs = static_cast<int>(bytes_left / size);//�ڴ����nobjs���������
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;//����֮�����ʼλ��
            return result;
        }else{
            //�ڴ��һ�����鶼�޷��ṩ
            size_t bytes_to_get =  //��ȡ�µĿռ�  2����С + ���Լ���һ���Ľڵ�
            2 * total_bytes + 
                ROUND_UP(
                    heap_size >> 4
                );//��head����ע����ڴ棬heap_size���������ô������Ӷ�����
            
            if (bytes_left > 0)
            {
                //�ڴ�ػ�Ҫһ�����ڴ棬Ϊ�˲��˷ѷ����free_list
                obj* volatile *my_free_list = 
                free_list + FREELIST_INDEX(bytes_left);
                reinterpret_cast<obj*>(start_free)->free_list_link = *my_free_list;
                *my_free_list = reinterpret_cast<obj*>(start_free);
            }
            //����heap�ռ��Բ����ڴ��
            start_free = reinterpret_cast<char *>(malloc(bytes_to_get));
            if (!start_free){//malloc error  start_free == nullptr
                //heap �ռ䲻�� mallocʧ��
                obj * volatile *my_free_list;
                obj * p;
                //��free_list �м���Ƿ��з������������ <=128
                for (size_t i = 0; i < static_cast<size_t>(__MAX_BYTES);
                    i += static_cast<size_t>(__ALIGN)){
                    my_free_list = free_list + FREELIST_INDEX(i);//��λ��ÿһ������
                    p = *my_free_list;//ÿ������Ŀ�ͷ
                    if(p != nullptr){
                        //�������Է��������
                        *my_free_list = p->free_list_link;//���뵱ǰ����
                        start_free = reinterpret_cast<char *>(p);
                        end_free = start_free + i;
                        return (chunk_alloc(
                            size,
                            nobjs));//�ݹ����������nobjs����ʱ��Ȼ����else if��֧
                    }
                }
                end_free = nullptr;//�������Ҳ��ڴ�
                //���õ�һ���������۲����ܷ�����ڴ���׳��쳣
                start_free = reinterpret_cast<char*>(malloc_alloc::allocate(bytes_to_get));
            }
            heap_size += bytes_to_get;//��ռ�õĶ��ڴ�  ���⣺�������ʧ��ҲҪ������
            end_free = start_free + bytes_to_get;
            return chunk_alloc(size,nobjs);//��������������nojbs
        }
    }
    void* __default_alloc::allocate(size_t n){
        obj * volatile * my_free_list;
        obj * result;
        //���n����128 �����һ��������
        if (n > __MAX_BYTES){
            return (malloc_alloc::allocate(n));
        }
        //ѡ����õڼ�����
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if(result == nullptr){
            //δ�ҵ�����free_list  ׼�����free_list
            void * r = refill(ROUND_UP(n));
            return r;
        }
        //����freelist
        *my_free_list = result->free_list_link;//ָ����һ���������
        return result;
    }

    void __default_alloc::deallocate(void* p,size_t n){
        //p ����Ϊnullptr
        if(n > static_cast<size_t>(__MAX_BYTES))
            malloc_alloc::deallocate(p,n);//��һ������������
        else{
            //Ѱ�Ҷ�Ӧ��free_list
            obj *volatile *my_free_list = free_list + FREELIST_INDEX(n);
            obj *q = reinterpret_cast<obj*>(p);
            //����free_list����������
            //p��Ҫ���յ���ʼλ��
            q->free_list_link = *my_free_list;//ָ���������Ŀ�ͷ��  �൱������������
            *my_free_list = q;//��ʼλ��p=q ֻ�����Ͳ�ͬ 
        }  
    }
    void * __default_alloc::reallocate(void* p,size_t old_sz,size_t new_sz){
        void *result;
        size_t copy_sz;
        if(old_sz > static_cast<size_t>(__MAX_BYTES) && 
            new_sz > static_cast<size_t>(__MAX_BYTES))
            return realloc(p,new_sz);//����128  ʹ�õ�һ��

        if(ROUND_UP(old_sz) == ROUND_UP(new_sz))//��ͬһ��������
            return p;
        result = allocate(new_sz);
        copy_sz = new_sz > old_sz ? old_sz : new_sz;
        memcpy(result,p , copy_sz);//��С�ķ����ڽ���У�ȷ������ 
        deallocate(p,old_sz);//���վɵ�
        return result;
    }
} // namespace MiniSTL

