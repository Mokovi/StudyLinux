# 线程

## 线程的概念:

&#9;每个进程都有自己的数据段,代码段和堆栈段,这就造成进程在创建,切换,撤销操作时,需要较大的系统开销。为了减少系统开销,从进程中演化出了线程.

&#9;线程存在于进程中，共享进程的资源。线程是进程中独立控制流(具有独立执行的一条路径)，由环境(包括寄存器组和程序计数器)和一系列的执行指令组成。(线程就是具有独立执行的一条路径..)

&#9;每个进程都有一个地址空间和一个控制线程.

![](../source_img/Linux%20C高级编程.assets/16.png)


### 线程和进程的比较

**调度**:  
- **线程**是CPU调度和分派的基本单位。它通常不独立拥有系统资源（仅有必要的程序计数器、寄存器和栈），但可以访问其所属进程的资源（如打开的文件、I/O设备等）。
- **进程**是系统中程序执行和资源分配的基本单位。一个进程拥有独立的地址空间和资源（如内存、文件句柄等），调度时需要加载和切换完整的CPU环境。

**系统开销**:  
- 线程属于同一进程，因此共享地址空间和资源。这种共享机制使线程之间的同步和通信更为高效，系统开销较小。
- 进程切换需要保存当前进程的CPU状态并加载新进程的状态，还涉及内存管理（如切换页表等），因此系统开销相对较大。
- 线程切换只需保存和恢复少量的寄存器信息，不涉及内存地址空间的切换，因此更轻量，能更有效利用系统资源，提高系统吞吐量。

**并发性**:  
- 进程之间可以并发执行，每个进程都是独立的任务。
- 线程也可以并发执行，且多个线程可以在同一进程中共享数据和资源，从而更高效地实现并发任务。

**总结**:

- 线程通常被称为**轻量级的进程**。
- 一个进程可以创建多个线程，这些线程共享同一进程的资源。
- 进程切换涉及到用户空间的切换（每个进程默认拥有独立的4G虚拟内存，其中3G用户空间是私有的），因此开销较大。
- 相比之下，线程共享进程的地址空间，因此线程切换时无需切换这些资源，效率更高。
- 线程和进程的调度机制类似，都是通过轮询机制实现来回切换。线程在同一进程内切换运行开销小，因此适合需要频繁调度的并发任务。)

### 多线程的用处:

1. 多任务程序的设计
2. 并发程序设计
3. 网络程序设计(网络的利用效率)
4. 数据共享---(同一个进程中的不同线程共享进程的数据空间,方便不同线程间的数据共享.
5. 在多个CPU系统中,可以实现真正的并行处理

## 线程的基本操作

###   线程的创建

  ~~~C
#include <pthread.h>

   int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);
功能:创建一个新的子线程
参数:
   thread: 当前创建的线程id
   attr  :线程的属性,设置为NULL以默认的属性创建
   start_routine :线程处理函数,如果当前函数执行完毕,则表示子线程也执行完毕.
   arg : 给线程处理函数传参数用的.
返回值:
    成功: 0
    失败: 非0
    
  ~~~

 **注意**: 

1. 与fork不同的是pthread_create创建的线程不与父线程在同一点开始执行的，而是从指定的处理函数开始运行的，该函数运行完后,此线程也就退出了.

2. 线程依赖进程存在的，如果创建线程的进程结束了，那么线程也就结束了.

3. 线程函数的程序在pthread库中，故**链接时要加上参数-lpthread**。

示例:

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void  *fun1(void *arg){
    printf("子线程正在运行中\n");
}
int main()
{
     printf("主线程正在执行中\n");
    //pthread_create函数创建子线程
      pthread_t thread;
      if(pthread_create(&thread,NULL,fun1,NULL)!=0){
        perror("fail to pthread_create");
        exit(1);
      }
      //由进程结束后,进程中所有的线都会强制退出.
     while(1);
    return 0;
}
~~~

#### **设置线程属性**

设置线程属性时，可以根据需要设置以下常见属性：

1. **分离状态（Detached State）**

分离状态决定线程结束后是否自动回收资源。线程默认是“可连接”的（joinable），需要 `pthread_join()` 来回收资源。如果将其设置为“分离状态”（detached），则线程结束时会自动释放资源。

- **设置分离状态**：使用 `pthread_attr_setdetachstate()` 函数。

  ```c
  // 设置线程为分离状态
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  ```

- **分离状态的可选值**：

  - `PTHREAD_CREATE_JOINABLE`：线程可以被 `pthread_join` 等待（默认）。
  - `PTHREAD_CREATE_DETACHED`：线程结束时自动释放资源。

2. **栈大小（Stack Size）**

可以通过 `pthread_attr_setstacksize()` 来设置线程栈的大小，避免栈溢出问题。

- **设置栈大小**：

  ```c
  size_t stacksize = 1024 * 1024; // 设置为1MB
  pthread_attr_setstacksize(&attr, stacksize);
  ```

- **注意事项**：栈大小应大于系统要求的最小栈大小，否则 `pthread_attr_setstacksize` 会返回错误。

3. **栈地址（Stack Address）**

可以通过 `pthread_attr_setstack()` 设置线程栈的起始地址和栈大小。通常不需要手动设置，操作系统会自动分配栈空间，但在特定场景下可能需要自定义。

- **设置栈地址和栈大小**：

  ```c
  void *stackaddr = malloc(1024 * 1024);  // 分配1MB栈空间
  size_t stacksize = 1024 * 1024;
  pthread_attr_setstack(&attr, stackaddr, stacksize);
  ```

4. **调度策略和优先级（Scheduling Policy and Priority）**

调度策略决定线程的调度方式，可以通过 `pthread_attr_setschedpolicy()` 和 `pthread_attr_setschedparam()` 来设置。

- **调度策略的可选值**：

  - `SCHED_OTHER`：默认的非实时调度策略。
  - `SCHED_FIFO`：先到先服务的实时调度策略。
  - `SCHED_RR`：轮转的实时调度策略。

- **设置调度策略**：

  ```c
  pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
  ```

- **设置优先级**：
  通过 `pthread_attr_setschedparam()` 设置线程的优先级。

  ```c
  struct sched_param param;
  param.sched_priority = 10;  // 设置优先级为10
  pthread_attr_setschedparam(&attr, &param);
  ```

#### **完整示例**

以下代码演示了如何设置线程为分离状态，并自定义栈大小：

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* threadFunc(void* arg)
{
    printf("线程正在运行。\n");
    sleep(1);
    printf("线程结束。\n");
    return NULL;
}

int main()
{
    pthread_t thread;
    pthread_attr_t attr;
    // 初始化线程属性
    pthread_attr_init(&attr);
    // 设置分离状态
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    // 设置栈大小
    size_t stacksize = 1024 * 1024; // 1MB
    pthread_attr_setstacksize(&attr, stacksize);
    // 创建线程并使用自定义属性
    if (pthread_create(&thread, &attr, threadFunc, NULL) != 0)
    {
        perror("fail to create thread.");
        exit(1);
    }
    // 销毁属性对象
    pthread_attr_destroy(&attr);
    // 主线程休眠一会，确保子线程输出
    sleep(2);
    printf("主线程结束。\n");
    return 0;
}
```

### 线程调度机制

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void  *fun1(void *arg){
    printf("子线程A正在运行中\n");
    sleep(1);
    printf("---------------A--------------\n");
}
void  *fun2(void *arg){
    printf("子线程B正在运行中\n");
    sleep(1);
    printf("---------------B--------------\n");
}
int main(){

     printf("主线程main正在运行中\n");
    pthread_t thread1,thread2;
    if(pthread_create(&thread1,NULL,fun1,NULL)!=0){
       perror("fail to pthread_create");
    }
    if(pthread_create(&thread2,NULL,fun2,NULL)!=0){
       perror("fail to pthread_create");
    }
    while(1);
    return 0;
}

~~~



~~~C
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void  *fun1(void *arg){
    printf("子线程A正在运行中\n");
    for(int i=0;i<100;i++){
        printf("i===%d\n",i);
    }
    sleep(1);
    printf("---------------A--------------\n");
}
void  *fun2(void *arg){
    printf("子线程B正在运行中\n");
      for(int j=0;j<100;j++){
        printf("j===%d\n",j);
    }
    sleep(1);
    printf("---------------B--------------\n");
}
int main(){

     printf("主线程main正在运行中\n");
    pthread_t thread1,thread2;
    if(pthread_create(&thread1,NULL,fun1,NULL)!=0){
       perror("fail to pthread_create");
    }
    if(pthread_create(&thread2,NULL,fun2,NULL)!=0){
       perror("fail to pthread_create");
    }
     while(1);
    return 0;
}

~~~

**总结**: 

   线程处理函数是并行执行的,是来回交替执行,但是普通函数是一定按照先后调用函数顺序执行.

### 线程处理函数的传参.



~~~c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int n=200;

void  *fun1(void *arg){
      printf("子线程A n=%d\n",n);
      n++;
      int k=*(int *)arg;
      printf("子线程A  K=%d\n",k);
      *(int *)arg=555;
}
void  *fun2(void *arg){
    sleep(1);
     printf("子线程B n=%d\n",n);
     int k=*(int *)arg;
     printf("子线程B  K=%d\n",k);
}
int main(){
    printf("主线程main正在运行中\n");
    int k=100;
    pthread_t thread1,thread2;
    if(pthread_create(&thread1,NULL,fun1,(void *)&k)!=0){
       perror("fail to pthread_create");
    }
    if(pthread_create(&thread2,NULL,fun2, (void *)&k)!=0){
       perror("fail to pthread_create");
    }
     while(1);
    return 0;
}

~~~

## 线程的等待

~~~c
#include <pthread.h>
int pthread_join(pthread_t thread, void **retval);
功能: 等待子线程结束,并回收子线程资源.
参数:
    thread: 被等待的线程号.
    retval: 用来存储线程退出状态的指针的地址.
返回值:
   成功返回  0,  失败返回非 0
~~~

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void  *fun1(void *arg){
    printf("子线程正在运行中\n");
    sleep(3);
    printf("子线程要退出了....\n");
}

int main(){

    printf("主线程正在运行中..\n");

    pthread_t thread;
    if(pthread_create(&thread,NULL,fun1,NULL)!=0){
        perror("fail to pthread_create");
        exit(1);
    }
    //通过用pthread_join函数阻塞等待子程线退出
    if(pthread_join(thread,NULL)!=0){
        perror("fail to pthread_join");
        exit(1);
    }
    printf("进程要退出了\n");
    return  0;
}
~~~

通过`pthread_join`获取子线程的退出状态值

~~~
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void  *fun1(void *arg){
    static int k=200;
    printf("子线程正在运行中\n");
    sleep(3);
    printf("子线程要退出了....\n");
    //子线程如果要返回退出状态,可以通过返回值或者pthread_exit函数
    return   (void *)&k;
}

int main(){
    printf("主线程正在运行中..\n");
    pthread_t thread;
    if(pthread_create(&thread,NULL,fun1,NULL)!=0){
        perror("fail to pthread_create");
        exit(1);
    }
    //通过用pthread_join函数阻塞等待子程线退出
    int *num;
    if(pthread_join(thread,(void **)&num)!=0){
        perror("fail to pthread_join");
        exit(1);
    }
    printf("retvalue==%d\n",*num);
    printf("进程要退出了\n");
    return  0;
}
~~~

## 线程分离

线程的结合态和分离态：

&#9;linux线程执行和windows不同，`pthread`有两种状态： 可结合的（`joinable`）或者是分离的（`detached`），线程默认创建为可结合态。 

- 如果线程是`joinable`状态，当线程函数自己返回退出时或`pthread_exit`时都不会释放线程所占用堆栈和线程描述符（总计8K多）。只有当你调用了`pthread_join` 之后这些资源才会被释放。 
- 若是`detached`状态的线程，这些资源在线程函数退出时或`pthread_exit`时自动会 被释放，使用`pthread_detach`函数将线程设置为分离态。 

创建一个线程后应回收其资源，但使用`pthread_join`函数会使调用者阻塞，故Linux提供了线程分离函数：`pthread_detach`

~~~c
#include <pthread.h>
int pthread_detach(pthread_t thread);
功能: 使调用线程与当前线程分离,使其成为一个独立的线程.该线程终止时,系统会自动回收它的资源.
参数:
   thread : 指定的子线程的id
返回值:
     成功: 0
     失败: 非0
~~~

**线程分离状态的特性**

- **资源自动回收**: 分离线程的资源（如线程栈和线程控制块）会在线程结束时自动释放，避免了资源泄漏。
- **无法使用 `pthread_join`**: 对于已经分离的线程，不能再使用 `pthread_join` 来等待线程结束并获取其返回值。
- **返回值**: 分离线程的返回值（即线程的退出状态）不能被获取或访问。线程的退出状态通常由 `pthread_exit` 函数设置，线程退出时，该状态会被丢弃，因为没有线程在等待它。

示例:

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *fun1(void *arg){
   printf("子线程正在运行中...\n");
   sleep(3);
   printf("子线程即将退出了\n");
}

int main(){

     printf("主线程正在运行中...\n");
     pthread_t thread;
     if(pthread_create(&thread,NULL,fun1,NULL) !=0){
        perror("fail to pthread_create");
        exit(1);
     }
     if(pthread_detach(thread)!=0){
        perror("fail to pthread_detach");
        exit(1);
     }
     /*
       如果子线程已经设置了分离态,那么就不要再使用pthread_join函数了,因为这个函数是个阻塞函数,如果子线程不退出,那么就会导致当前线程(main主线无法继续执行,这样大大限制了代码运行效率)
     if(pthread_join(thread,NULL) !=0){
        perror("fail to pthread_join");
        exit(1);
     }
     */
     while(1){
        printf("hello linuxc\n");
        sleep(1);
     }


    return 0;
}

~~~

## 线程退出

  线程退出函数

```c
#include <pthread.h>
void pthread_exit(void *retval);
功能: 退了正在执行的线程.
参数:
   retval : 当前线程的退出状态值.
   这个值可以被调用pthread_join函数的线程接收到.
返回值: 无
 
```

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *fun1(void *arg){
    static char buffer[]="this is a book";
    printf("子线程正在运行中...\n");
    for(int k=0;k<10;k++){
        if(k==5){
            //退出当前线程
            //pthread_exit(NULL);
            pthread_exit(buffer);
        }
        printf("==========================\n");
        sleep(1);
    }

}
int main(){

    printf("主线程正在运行中...\n");
    pthread_t thread;
    if(pthread_create(&thread,NULL,fun1,NULL) !=0){
        perror("fail to pthread_create");
        exit(1);
    }
   // pthread_join(thread,NULL);
    char *str;
     pthread_join(thread,(void **)&str);
     printf("str====%s\n",str);
    printf("进程要退出了....\n");
    return 0;
}
~~~

**注意**: 

&#9;  一个进程中的多个线程是共享该进程的数据段,因此,通常线程退出后所占用的资源并不会释放,如果释放资源,结合态需要通过pthread_join函数, 分离态则自动释放.

## 线程的取消

~~~c
 #include <pthread.h>

  int pthread_cancel(pthread_t thread);
功能:取消线程
参数:  
   thread:要销毁的线程的id
返回值:
    成功:  0
    失败:  非 0
    
~~~

&#9;`pthread_cancel`函数的实质是发信号给目标线程`thread`,使目标线程退出.此函数只是发送终止信号给目标线程,不会等待取消目标线程执行完才返回.

 &#9;然而发送成功并不意味着目标线程一定就会终止,线程被取消时,线程的取消属性会决定线程能否被取消以及何时被取消.

**线程的取消状态** ： 线程能不能被取消

**线程取消点**： 即线程被取消的地方

**线程的取消类型**：在线程能被取消的状态下,是立马被取消结束，还是执行到取消点的时候被取消结束.

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
void *fun1(void *arg){
     while(1){
        printf("子线程正在运行中....\n");
        sleep(1);
     }
}
int main(){

     pthread_t thread;
     if(pthread_create(&thread,NULL,fun1,NULL) !=0){
        perror("fail to pthread_create");
        exit(1);
     }
     sleep(5);
     pthread_cancel(thread);
     pthread_join(thread,NULL);
    printf("进程即将结束\n");

    return 0;
}
~~~

### 线程的取消状态

在Linux 系统中,线程默认可以被取消,编程时可以通过pthread_setcancelstate函数设置线程是否可以被取消.

~~~c
#include <pthread.h>
int pthread_setcancelstate(int state, int *oldstate);
功能:  设置线程是否被取消
参数: 
state: 新的状态
    PTHREAD_CANCEL_DISABLE :不可以被取消
    PTHREAD_CANCEL_ENABLE : 可以被取消
oldstate: 保存调用线程原来的可取消状态的内存地址
返回值:
    成功:  0
    失败:  非0
~~~

### 线程的取消点

   线程被取消后,该线程并不是马上终止,默认情况下线程执行到取消点时才被终止,编程时可以通过`pthread-testcancel`函数设置线程的取消点.

~~~c
#include <pthread.h>

void pthread_testcancel(void);
功能: 设置线程的取消点
~~~



### 线程的取消类型

&#9;线程被取消后,该线程并不是马上终止,默认情况线程执行到取消点时才被终止,编程时可以通过`pthread_setcanceltype`函数设置线程是否可以立即被取消.

~~~c
#include <pthread.h>
int pthread_setcanceltype(int type, int *oldtype);
type:
    PTHREAD_CANCEL_ASYNCHRONOUS:  立即取消
    PTHREAD_CANCEL_DEFERRED : 不立即取消
oldtype:
    保存调用线程原来的可以取消类型的内存地址
~~~

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void* func1(void *arg)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);//设置可以取消，（默认可以取消）
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);//不立即取消 （默认不立即取消）
    printf("子线程进行中1.\n");
    sleep(1);
    printf("子线程进行中2.\n");
    sleep(1);
    printf("子线程进行中3.\n");
    sleep(1);
    printf("子线程进行中4.\n");
    sleep(1);
    pthread_testcancel(); // 检查取消请求
}

int main()
{
    pthread_t thread;
    if(pthread_create(&thread, NULL, func1, NULL) != 0)
    {
        perror("fail to create thread.");
        exit(1);
    }
    pthread_cancel(thread);
    pthread_join(thread,NULL);
    printf("进程结束.\n");
    return 0;
}

~~~

运行结果：

![](../source_img/Linux%20C高级编程.assets/32.png)

**注意：**输出结果与预想的不一致。这是因为在`POSIX线程库`中，像`sleep()`这样的函数内部通常会包含隐式的取消点。这意味着当一个线程在`sleep()`调用中等待时，它是可能被取消的。取消点包括但不限于`sleep()`,` pthread_cond_wait(),` `sem_wait()`等阻塞调用，也包括显式调用的`pthread_testcancel()`。

​	

## 线程退出清理函数

 和进程的退出清理一样,线程也可以注册它的退出时要调用的函数,这样的函数称为线程清理处理程序. 

``` c
  #include <pthread.h>

  void pthread_cleanup_push(void (*routine)(void *), void *arg);
  功能:将清除函数压栈,即注册清理函数.
  参数
  routine : 线程清理函数的指针
  arg  传给线程清理函数的参数.
 返回值:无
  
                                
  void pthread_cleanup_pop(int execute);
  功能: 将清理函数弹栈,即删除清理函数
  参数:
     execute : 线程清理函数执行标志位.
      非0 弹出清理函数,且立即执行清理函数
      0  弹出清理函数,但不执行清理函数.
  返回值:  无
```

**注意**:

- 线程可以建立多个清理处理程序，**处理程序在栈中,所有它们执行顺序与它们注册时顺序相反**

- 由于这**两个函数是宏**，因此两个函数**必须成对出现**并且出现在同一个代码块中，不能跨越函数或者代码块。在编译时，它们会被展开为一个代码块。

- `pthread_cleanup_push` 注册一个清理函数，该函数将在以下情况之一发生时被调用：

  - 线程被取消（例如通过 `pthread_cancel`）。

  - 线程正常退出（例如调用 `pthread_exit` 或者函数返回）。

  - 显式调用 `pthread_cleanup_pop` 时，如果参数为 `1`。

  **示例1**：调用`pthread_exit`退出

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

void myclean(void *arg){
    printf("myclean ptr=%s\n",(char *)arg);
    free((char *)arg);
}

void *fun1(void *arg){
    printf("子线程正在运行中....\n");
    char *ptr=NULL;
    ptr=(char*)malloc(100);
    pthread_cleanup_push(myclean,(void *)(ptr));
    bzero(ptr,100);
    //bzero是memset的下位替代，除非有特定需求或兼容旧代码，否则推荐优先使用memset，因为它更加灵活且已被现代标准所支持。
    strcpy(ptr,"this is a book");
    printf("注册清理函数!\n");
    sleep(3);
    pthread_exit(NULL);//此时退出，其后面的代码不会再起作用，但是由于是正常退出，cleanup函数仍然会调用.
    printf("删除清理函数\n");
    pthread_cleanup_pop(1);
}

int main(){
    pthread_t thread;
    if(pthread_create(&thread,NULL,fun1,NULL)!=0){
        perror("fail to pthread_create");
        exit(1);
    }
    pthread_join(thread,NULL);
    printf("进程即将要结束....\n");
}
~~~

  **示例2**：调用`pthread_cancel`退出

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

void myclean(void *arg){
    printf("myclean ptr=%s\n",(char *)arg);
    free((char *)arg);
}

void *fun1(void *arg){
    printf("子线程正在运行中....\n");
    char *ptr=NULL;
    ptr=(char*)malloc(100);
    pthread_cleanup_push(myclean,(void *)(ptr));
    bzero(ptr,100);
    strcpy(ptr,"this is a book");
    printf("注册清理函数!\n");
    sleep(3);
    printf("删除清理函数\n");
    pthread_cleanup_pop(1);
}

int main(){
    pthread_t thread;
    if(pthread_create(&thread,NULL,fun1,NULL)!=0){
        perror("fail to pthread_create");
        exit(1);
    }
    //取消线程
    pthread_cancel(thread);
    pthread_join(thread,NULL);
    printf("进程即将要结束....\n");
}
~~~

  **示例3**：正常退出，多个清理函数查看执行顺序

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

void fun1(void *arg){
     printf("fun1...\n");
    printf("myclean ptr=%s\n",(char *)arg);
    free((char *)arg);
}

void fun2(void *arg){
     printf("fun2...\n");
}

void *fun(void *arg){
    printf("子线程正在运行中....\n");
    char *ptr=NULL;
    ptr=(char*)malloc(100);
    pthread_cleanup_push(fun1,(void *)(ptr));
    pthread_cleanup_push(fun2, NULL);
    bzero(ptr,100);
    strcpy(ptr,"this is a book");
    printf("注册清理函数!\n");
    sleep(3);
    printf("删除清理函数\n");
    printf("clearup1...\n");
    pthread_cleanup_pop(1);
    printf("clearup2...\n");
    pthread_cleanup_pop(1);
}

int main(){


    pthread_t thread;
    if(pthread_create(&thread,NULL,fun,NULL)!=0){
        perror("fail to pthread_create");
        exit(1);
    }
    //取消线程
  //  pthread_cancel(thread);
    pthread_join(thread,NULL);
    printf("进程即将要结束....\n");

}
~~~

##   同步与互斥概念

&#9;在多任务操作系统中，同时运行的多个任务可能都需要访问/使用同一种资源，多个任务之间有依赖关系，某个任务的运行依赖于另一个任务，同步和互斥就是用于解决这两个问题的。 

**互斥:** 

&#9;一个公共资源同一时刻只能被一个进程或线程使用，多个进程或线程不能同时使用公共资源。POSIX标准中进程和线程同步和互斥的方法,主要有信号量和互斥锁两种方式。 

**同步:** 

&#9;两个或两个以上的进程或线程在运行过程中协同步调，按预定的先后次序运行。 同步就是在互斥的基础上有顺序

## 互斥锁

&#9;`mutex`是一种简单的加锁的方法来控制对共享资源的访问，`mutex`只有两种状态,即上 锁(`lock`)和解锁(`unlock`)。 在访问该资源前，首先应申请`mutex`，如果`mutex`处于`unlock`状态，则会申请到`mutex`并立即`lock`； 如果`mutex`处于`lock`状态，则默认阻塞申请者。 `unlock`操作应该由`lock`者进行。 

### **互斥锁的操作** 

初始化互斥锁

`mutex`用`pthread_mutex_t`数据类型表示，在使用互斥锁前,必须先对它进行初始化。 

**静态分配的互斥锁：** 

`pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; `

**动态分配互斥锁:** 

`pthread_mutex_t mutex; `

`pthread_mutex_init(&mutex, NULL); `

在所有使用过此互斥锁的线程都不再需要使用时候，应调用`pthread_mutex_destroy`销毁 

**互斥锁初始化**

```c
#include <pthread.h>
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
功能：初始化一个互斥锁
参数：
    mutex：指定的互斥锁
    mutexattr：互斥锁的属性，为NULL表示默认属性
返回值：
	成功：0
```

**互斥锁上锁**

~~~c
#include <pthread.h>
int pthread_mutex_lock(pthread_mutex_t *mutex);
功能：对互斥锁上锁，若已经上锁，则调用者一直阻塞到互斥锁解锁
参数：
mutex：指定的互斥锁
返回值：
	成功：0
	失败：非0

#include <pthread.h>
int pthread_mutex_trylock(pthread_mutex_t *mutex);
功能：对互斥锁上锁，若已经上锁，则上锁失败，函数立即返回。
参数：
	mutex：互斥锁地址。
返回值：
    成功：0
    失败：非0。
~~~

**互斥锁解锁**

~~~c
#include <pthread.h>
int pthread_mutex_unlock(pthread_mutex_t * mutex);
功能：对指定的互斥锁解锁。
参数：
	mutex：互斥锁地址。
返回值：
    成功：0
    失败：非0
~~~

**销毁互斥锁**

~~~c
#include <pthread.h>
int pthread_mutex_destroy(pthread_mutex_t *mutex);
功能：销毁指定的一个互斥锁。
参数：
	mutex：互斥锁地址。
返回值：
    成功：0
    失败：非0。
~~~

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
//共享数据
 int money=10000;
  //创建互斥锁(二个线程要同时使用--使用全局变量)
 pthread_mutex_t mymutex;
 void *fun1(void *arg){
     int takemoney,balance,actualmoney;
     takemoney=10000;
     //第三步: 对共享资源的操作进行上锁
     pthread_mutex_lock(&mymutex);
     printf("jack正在查询余额...!\n");
     sleep(1);
     balance=money;
     printf("jack正在取钱中....\n");
     sleep(1);
     if(takemoney>balance){
        actualmoney=0;
     }else{
           actualmoney=takemoney;
           balance=balance-takemoney;
           money=balance;
     }

     printf("jack想取%d元,实际取了%d元,余额为%d元\n",takemoney,actualmoney,balance);
     //第四步: 当共享资源的操作执行完毕后,对互斥锁执行解锁操作.
     pthread_mutex_unlock(&mymutex);
     pthread_exit(NULL);
 }
void *fun2(void *arg){
     int takemoney,balance,actualmoney;
     takemoney=10000;
     //第三步: 对共享资源的操作进行上锁
     pthread_mutex_lock(&mymutex);
     printf("lucy正在查询余额...!\n");
     sleep(1);
     balance=money;
     printf("lucy正在取钱中....\n");
     sleep(1);
     if(takemoney>balance){
        actualmoney=0;
     }else{
           actualmoney=takemoney;
           balance=balance-takemoney;
           money=balance;
     }
     printf("lucy想取%d元,实际取了%d元,余额为%d元\n",takemoney,actualmoney,balance);
    //第四步: 当共享资源的操作执行完毕后,对互斥锁执行解锁操作.
     pthread_mutex_unlock(&mymutex);
     pthread_exit(NULL);
 }
int main(){
     //第二步初始化互斥锁
     pthread_mutex_init(&mymutex,NULL);
     pthread_t t1,t2;
     if(pthread_create(&t1,NULL,fun1,NULL) !=0){
        perror("fail to pthread_create");
        exit(1);
     } 
     if(pthread_create(&t2,NULL,fun2,NULL) !=0){
        perror("fail to pthread_create");
        exit(1);
     } 
     pthread_join(t1,NULL);
     pthread_join(t2,NULL);
     //第五步:当互斥锁使用完毕后,要销毁
     pthread_mutex_destroy(&mymutex);

    return 0;
}
~~~

### 线程综合题

编写一个程序，使用多线程实现字符串的拼接。要求每个线程拼接一部分字符串，然后将结果合并。使用互斥锁控制

``` c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

static int currentOrder = 1;
char string[1024];
const unsigned int threadNum = 3;
#define STRLEN 30
typedef struct threadInputPara
{
    unsigned int order;
    char str[STRLEN];
    bool finishFlag;
}tpara;

pthread_mutex_t mutex;

void *spliceStr(void *arg)
{
    tpara *para = (tpara *)arg;
    while (1) {
        pthread_mutex_lock(&mutex);
        if (currentOrder == para->order) {
            if (para->order == 1) {
                strcpy(string, para->str);  // 第一个线程直接拷贝
            } else {
                strcat(string, para->str);  // 之后的线程拼接
            }
            currentOrder++;  // 更新顺序
            pthread_mutex_unlock(&mutex);
            break;  // 跳出循环，完成拼接
        }
        pthread_mutex_unlock(&mutex);
        usleep(100);  // 避免忙等，稍作休眠
    }
    para->finishFlag = true;
}

int main()
{
    pthread_mutex_init(&mutex, NULL);
    tpara para[threadNum];
    printf("please input %d str to splice: \n", threadNum);
    int readByte;
    for (int i = 0; i < threadNum; i++)
    {
        readByte = read(0, (para + i)->str, STRLEN);
        para[i].str[readByte-1] = '\0'; //将最后一个/n改为/0
        para[i].order = i + 1;
    }
    pthread_t thread[threadNum];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // 设置为分离态
    for (int i = 0; i < threadNum; i++)
    {
        if (pthread_create(thread + i, &attr, spliceStr, (void *)(para + i)) != 0)
        {
            perror("fail to create thread.");
            exit(1);
        }
    }
    pthread_attr_destroy(&attr);
    do
    {
        printf("正在拼接中....\n");
        sleep(1);
    } while (!(para[0].finishFlag) || !(para[1].finishFlag) || !(para[2].finishFlag));
    printf("拼接结果: %s\n", string);
    return 0;
}
```



## 信号量的概念

&#9;信号量广泛用于进程或线程间的同步和互斥. 信号量本质上是一个非负的整数计数器,它被用来控制对公共资源的访问.在编程中可根据操作信号量的结果判断是否对公共资源具有访问的权限,当信号量值大于0时,可以访问,否则将阻塞.

&#9;信号量又称为PV操作,PV原语是对信号量的操作,一次P操作使信号量sem减1, 一次V操作使信号量sem加1,对于P操作,如果信号量的sem值为小于等于0,则P操作会阻塞,如果信号量的值大于0,才可以执行P操作进行减1.

信号量主要用于进程或线程间的同步和互斥这两种情况.

1. 若用于互斥,几个进程或线程往往只设置一个信号量.

![](../source_img/Linux%20C高级编程.assets/17.png)



2. 若用于同步操作,往往会设置多个信号量,并且安排不同的初始值,来实现它们之间的执行顺序.

![](../source_img/Linux%20C高级编程.assets/18.png)

### 信号量的操作

  信号量的初始化

~~~c
#include <semaphore.h>
int sem_init(sem_t *sem, int pshared, unsigned int value);
功能: 初始化一个信号量
参数: 指定的信号量
pshared :是否在线程间或者进程间共享
     0  线程间共享
     1  进程间共享
value : 信号量的初始值
返回值 :
   成功:  0
   失败:  -1
       
~~~

 信号量P操作

 ~~~c
 #include <semaphore.h>

 int sem_wait(sem_t *sem);
功能: 将信号量的值减1,若信号量的值小于0,此函数会引用调用者阻塞
参数:
    sem :指定的信号量
返回值:
    成功   0
    失败  -1
        
 int sem_trywait(sem_t *sem);
功能: 将信号量的值减1,若信号量的值小于0,则对信号量的操作失败,函数立即返回.
  参数:
    sem :指定的信号量
返回值:
    成功   0
    失败  -1  

 ~~~

 信号量V操作

~~~c
   #include <semaphore.h>

       int sem_post(sem_t *sem);
 功能:  执行V操作,执行一次,信号量的值加1.
   参数:
    sem :指定的信号量
返回值:
    成功   0
    失败  -1  
~~~

 

获得信号量的计数值

~~~c
 #include <semaphore.h>

 int sem_getvalue(sem_t *sem, int *sval);
功能:获取sem标识的信号量的值,保存在sval中
参数:
   sem : 信号量的地址
   sval  保存信号量值的地址
   返回值:
    成功   0
    失败  -1  
~~~

信号量的销毁

~~~c
   #include <semaphore.h>

  int sem_destroy(sem_t *sem);
功能: 删除sem标识的信号量
参数: 
   sem :信号量地址
 返回值:
    成功   0
    失败  -1  
~~~

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
//创建一个信号量
sem_t sem;
void print(char *str){
    //执行p操作
    //由于信号量实现互斥,信号量的初始值为1,则两个线程执行P操作,
    //先执行P操作的线程继续执行,后执行P操作先阻塞等待.
    sem_wait(&sem);
    while(*str){
        putchar(*str);
        fflush(stdout);
        str++;
        sleep(2);

    }
    //第四步:实现V操作
    sem_post(&sem);
}

void *fun1(void *arg){
    char *p1="hello";
    print(p1);

}
void *fun2(void *arg){
    char *p2="world";
    print(p2);
}
int main(){
    //第二步,初始化信号量
    sem_init(&sem,0,1);
      pthread_t t1,t2;
     if(pthread_create(&t1,NULL,fun1,NULL) !=0){
        perror("fail to pthread_create");
        exit(1);
     } 
     if(pthread_create(&t2,NULL,fun2,NULL) !=0){
        perror("fail to pthread_create");
        exit(1);
     } 
     pthread_join(t1,NULL);
     pthread_join(t2,NULL);
     printf("\n");
     sem_destroy(&sem);
     return 0;
}
~~~



~~~c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
//创建一个信号量
sem_t sem_p,sem_v;
char c='a';

void *fun1(void *arg){
  
   while(1){
    sem_wait(&sem_v);   
    c++;
    sleep(1);
    sem_post(&sem_p);
   }

}
void *fun2(void *arg){
  
    while(1){
     sem_wait(&sem_p);
     printf("%c",c);
     fflush(stdout);
     sleep(1);
     sem_post(&sem_v);
    }
}
int main(){
    //第二步,初始化信号量
     sem_init(&sem_v,0,0);
     sem_init(&sem_p,0,1);
      pthread_t t1,t2;
     if(pthread_create(&t1,NULL,fun1,NULL) !=0){
        perror("fail to pthread_create");
        exit(1);
     } 
     if(pthread_create(&t2,NULL,fun2,NULL) !=0){
        perror("fail to pthread_create");
        exit(1);
     } 
     pthread_join(t1,NULL);
     pthread_join(t2,NULL);
     printf("\n");
     sem_destroy(&sem_v);
     sem_destroy(&sem_p);
     return 0;
}
~~~

### 线程综合题

编写一个程序，使用多线程实现字符串的拼接。要求每个线程拼接一部分字符串，然后将结果合并。使用信号量控制

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define STRLEN 30
#define THREADNUM 3 // 线程数量
char string[1024];  // 拼接的最终字符串

typedef struct threadInputPara
{
    unsigned int order; // 线程的顺序
    char str[STRLEN];   // 该线程要拼接的字符串
} tpara;

sem_t semaphores[THREADNUM]; // 信号量数组

void *spliceStr(void *arg)
{
    tpara *para = (tpara *)arg;

    // 等待当前线程的信号量
    sem_wait(&semaphores[para->order - 1]);

    // 拼接字符串
    if (para->order == 1)
    {
        strcpy(string, para->str); // 第一个线程直接拷贝
    }
    else
    {
        strcat(string, para->str); // 其他线程拼接
    }

    // 唤醒下一个线程
    if (para->order < THREADNUM)
    {
        sem_post(&semaphores[para->order]);
    }

    return NULL;
}

int main()
{
    tpara para[THREADNUM];
    pthread_t threads[THREADNUM];

    // 初始化信号量，第一个信号量初始值为1，其他为0
    for (int i = 0; i < THREADNUM; i++)
    {
        sem_init(&semaphores[i], 0, (i == 0) ? 1 : 0);
    }

    // 输入字符串
    printf("Please input %d strings to splice:\n", THREADNUM);
    for (int i = 0; i < THREADNUM; i++)
    {
        fgets(para[i].str, STRLEN, stdin);
        para[i].str[strcspn(para[i].str, "\n")] = '\0'; // 去掉换行符
        para[i].order = i + 1;
    }

    // 创建线程
    for (int i = 0; i < THREADNUM; i++)
    {
        if (pthread_create(&threads[i], NULL, spliceStr, (void *)&para[i]) != 0)
        {
            perror("fail to create thread");
            exit(1);
        }
    }

    // 等待所有线程完成
    for (int i = 0; i < THREADNUM; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // 打印拼接结果
    printf("拼接结果: %s\n", string);

    // 销毁信号量
    for (int i = 0; i < THREADNUM; i++)
    {
        sem_destroy(&semaphores[i]);
    }

    return 0;
}

```

