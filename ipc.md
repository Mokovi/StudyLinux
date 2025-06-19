# 信号

## **进程间通信概述**

​	进程间通信`IPC:Inter Processes Communication`进程是一个独立的资源分配单元，不同进程（这里所说的进程通常指的是用户进程)之间的资源是独立的，没有关联，不能在一个进程中直接访问另一个进程的资源（例如打开的文件描述符）。 

​	进程不是孤立的，不同的进程需要进行信息的交互和状态的传递等，因此需要进程间通信。 

进程间通信功能： 

- 数据传输：一个进程需要将它的数据发送给另一个进程。 

- 资源共享：多个进程之间共享同样的资源。 

- 通知事件：一个进程需要向另一个或一组进程发送消息，通知它们发生了某种事件。 

- 进程控制：有些进程希望完全控制另一个进程的执行（如Debug进程），此时控制进程希望能够拦截另一个进程的所有操作，并能够及时知道它的状态改变。

Linux操作系统支持的主要`IPC`

![](../source_img/Linux%20C高级编程.assets/11.png)

**进程间通信的实质：** 

​	系统只要创建一个进程，就会给当前进程分配4G的虚拟内存（32位操作系统），虚拟内存不是常说的内存条的空间，内存条的空间称之为物理内存，虚拟内存和物理内存之间存在映射关系。4G的虚拟内存分为3G的用户空间（0 ~ 3G）和1G（3 ~ 4G）的内核空间，用户空间是进程所私有的，每一个进程的用户空间只能自己访问和使用，我们之前说的栈区、堆区、数据区、代码区等都是用户空间的区域，内核空间是所有进程所公有的，也就意味着绝大多数进程间通信方式，本质就是对内核空间的操作。

**特殊的进程间通信方式：** 

- `socket`通信可以实现不同主机的进程间通信，其他六个只能在一台主机的多个进程间通信 

- 信号通信是唯一的一种异步通信机制 

- 共享内存是所有进程间通信方式中效率最高的，它是直接对物理内存进行操作 

## **信号的概述** 

### **信号的概念** 

​	信号是软件中断，它是在软件层次上对中断机制的一种模拟。信号可以导致一个正在运行的进程被另一个正在运行的异步进程中断，转而处理某一个突发事件。信号是一种异步通信方式。进程不必等待信号的到达，进程也不知道信号什么时候到达。信号可以直接进行用户空间进程和内核空间进程的交互，内核进程可以利用它来通知用户空间进程发生了哪些系统事件。 

​	每个信号的名字都以字符SIG开头。每个信号和一个数字编码相对应，在头文件`signum.h`中，这些信号都被定义为正整数。 

​	信号名定义路径：`/usr/include/x86_64-linux-gnu/bits /signum.h `

​	在Linux下，要想查看这些信号和编码的对应关系，可使用命令：`kill ­-l`

![](../source_img/Linux%20C高级编程.assets/12.png)

​	信号是由当前系统已经定义好的一些标识，每一个标识都会在特定的场合使用并且都会对进程有一定的影响，当信号产生时，会让当前信号做出相应的操作这些信号。都是已经定义好的，我们**不能自定义创造**，直接使用这些就可以。

### **产生信号的方式** 

1. 当用户按某些终端键时，将产生信号：

&#9;终端上按`Ctrl+c`组合键通常产生中断信号`SIGINT`；终端上按`Ctrl+\`键通常产生中断信号`SIGQUIT`；终端上按`Ctrl+z`键通常产生中断信号`SIGTSTP`。 

2. 硬件异常将产生信号：

&#9;除数为0，无效的内存访问等。这些情况通常由硬件检测到，并通知内核,然后内核产生适当的信号发送给相应的进程。 

3. 软件异常将产生信号：

&#9;当检测到某种软件条件已发生，并将其通知有关进程时，产生信号。 

4. 调用kill函数将发送信号：

&#9;接收信号进程和发送信号进程的所有者必须相同，或发送信号进程的所有者必须是超级用户。 

5. 运行kill命令将发送信号：

&#9;此程序实际上是使用kill函数来发送信号。也常用此命令终止一个失控的后台进程。 

### **信号的默认（缺省）处理方式** 

当进程中产生了一个信号，就会让当前进程做出一定的反应，默认处理进程的方式如下 

1. 终止进程：当信号产生后，当前进程就会立即结束 

2. 缺省处理：当信号产生后，当前进程不做任何处理 

3. 停止进程：当信号产生后，使得当前进程停止 

4. 让停止的进程恢复运行：当信号产生后，停止的进程会恢复执行（后台进程）

### **进程接收到信号后的处理方式** 

**1、执行系统默认动作** 

对大多数信号来说，系统默认动作是用来终止该进程。 

**2、忽略此信号** 

接收到此信号后没有任何动作。 

**3、执行自定义信号处理函数** 

用用户定义的信号处理函数处理该信号。

| 信号    | 值   | 性质                                                         | 默认处理方式       |
| ------- | ---- | ------------------------------------------------------------ | ------------------ |
| SIGKILL | 9    | 当产生这个信号后，当前进程会退出，                           | 退出进程           |
| SIGSTOP | 19   | 当产生这个信号后，当前进程会停止，不能被缺省和捕捉           | 停止进程           |
| SIGINT  | 2    | 键盘输入ctrl+c时产生信号                                     | 退出进程           |
| SIGQUIT | 3    | 键盘输入ctrl+\时产生信号                                     | 退出进程           |
| SIGTSTP | 20   | 键盘输入ctrl+z时产生信号                                     | 停止进程           |
| SIGCONT | 18   | 当产生当前信号后，当前停止的进程会恢复运行                   | 停止的进程恢复运行 |
| SIGALRM | 14   | 当调用alarm函数设置的时间到达时会产生当前信号                | 退出进程           |
| SIGPIPE | 13   | 当管道破裂时，会产生当前信号                                 | 退出进程           |
| SIGABRT | 6    | 当调用abort函数时会产生当前信号                              | 退出进程           |
| SIGCHLD | 17   | 当使用fork创建一个子进程时，如果子进程状态改变（退出），会产生当前信号 | 缺省               |
| SIGUSR1 | 10   | 用户自定义信号，不会自动产生，只能使用kill函数或者命令给指定的进程发送当前信号 | 缺省               |
| SIGUSR2 | 12   | 用户自定义信号，不会自动产生，只能使用kill函数或者命令给指定的进程发送当前信号 | 缺省               |

## **kill函数**

~~~c
#include <sys/types.h>
#include <signal.h>
int kill(pid_t pid, int sig);
功能：给指定的进程或者进程组发送信号
参数：
pid：
	pid>0: 将信号传送给进程ID为pid的进程。
	pid=0: 将信号传送给当前进程所在进程组中的所有进程。
	pid=‐1: 将信号传送给系统内所有的进程，除了init进程
	pid<‐1: 将信号传给指定进程组的所有进程。这个进程组号等于pid的绝对值。
sig：指定的信号
返回值：
	成功：0
	失败：‐1
~~~





~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    pid_t pid;

    pid = fork();
    if(pid < 0)
    {
        perror("fail to fork");
        exit(1);
    }
    else if(pid > 0) //父进程的代码区
    {
        while(1)
        {
            printf("父进程\n");
            sleep(1);
        }
    }
    else //子进程的代码区 
    {
        printf("子进程\n");
        //子进程在3秒之后，让父进程退出
        sleep(3);
        //使用kill给父进程发送信号，然后父进程接收到信号后直接退出就可以了
        kill(getppid(), SIGINT);
    }
	return 0;
}
~~~

## **alarm函数**

~~~c
#include <unistd.h>
unsigned int alarm(unsigned int seconds);
功能: 定时器,闹钟,当设定的时间到时,会产生SIGALRM信号
参数: second设定的秒数
返回值  
  如果alarm函数之前没有alarm设置,则返回0
  如果有,则返回上一个alarm剩余的时间(覆盖上一个定时器).
~~~

~~~c
#include <stdio.h>
#include <unistd.h>
int main(){

   unsigned int second;
    second=alarm(5);
    sleep(3);
    second=alarm(7);

    printf("second= %d\n",second);
    while(10){
        printf("AAAAAAA\n");
        sleep(1);
    }
}
~~~

## **raise 函数**

~~~c
#include <signal.h>
int raise(int sig);
功能: 给调用的进程本身发送信号.
参数: sig 指定的信号
返回值:
	成功:  0
	失败:  非0
~~~

~~~c
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <sys/types.h>
  #include <signal.h>
  int main(){
           int count=0;
          while(1){
             printf("aaaa\n");
             sleep(1);
             count++;
             if(count==5){
                //raise(SIGINT);
                raise(SIGALRM);
             }
          }
  }
~~~

## **pause函数**

~~~c
#include <unistd.h>
int pause(void);
功能：阻塞等待一个信号的产生/判断信号是否已经到达
返回值：
	当有信号产生时，函数返回‐1
~~~

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    pid_t pid;

    pid = fork();
    if(pid < 0)
    {
        perror("进程创建失败");
        exit(1);
    }
    else if(pid > 0) //父进程的代码区
    {
        printf("父进程在运行中...\n");
        //使用pause阻塞等待捕捉信号
        pause();
    }
    else //子进程的代码区 
    {
        printf("子进程在运行中...\n");
        sleep(3);
        kill(getppid(), SIGINT);
    }
	return 0;
}
~~~

## signal 函数

~~~c
#include <signal.h>
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
功能: 当进程中产生某个信号时,对当前信号进行处理.
参数:
sig :指定要处理的信号
handler :处理方式
	SIG_IGN: 当信号产生时,以缺省(忽略)的方式处理
	SIG_DFL: 当信号产生时,以当前信号默认的方式处理.
        
void handler(int sig) :当信号产生时,通过信号处理函数自定义方式处理,函数名可以随便写.
参数 表示当前的信号
返回值: 
	成功: 返回函数的地址,该地址为此信号上一次注册的信号处理函数的地址.
	失败: SIG_ERR
~~~

小结：`signal()` 函数并不会阻塞调用它的代码，它的行为更类似于 Qt 中的 `connect()`，它只是在后台注册一个信号处理函数，并不影响代码的正常执行。

### signal函数的使用

~~~c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
void handler(int sig);
int main(){
    //以默认的方式处理信号
    #if 0
    if(signal(SIGINT,SIG_DFL)==SIG_ERR){
        perror("fail to signal");
        exit(1);
    }
     if(signal(SIGQUIT,SIG_DFL)==SIG_ERR){
        perror("fail to signal");
        exit(1);
    }
     if(signal(SIGTSTP,SIG_DFL)==SIG_ERR){
        perror("fail to signal");
        exit(1);
    }
    #endif
     //以忽略的方式来处理信号
     #if 0
     if(signal(SIGINT,SIG_IGN)==SIG_ERR){
        perror("fail to signal");
        exit(1);
    }
     if(signal(SIGQUIT,SIG_IGN)==SIG_ERR){
        perror("fail to signal");
        exit(1);
    }
     if(signal(SIGTSTP,SIG_IGN)==SIG_ERR){
        perror("fail to signal");
        exit(1);
    }
   #endif 
    //以用户自定义方式处理信号
        if(signal(SIGINT,handler)==SIG_ERR){
        perror("fail to signal");
        exit(1);
    }
    if(signal(SIGQUIT,handler)==SIG_ERR){
        perror("fail to signal");
        exit(1);
    }
     if(signal(SIGTSTP,handler)==SIG_ERR){
        perror("fail to signal");
        exit(1);
    }
    /*  SIGKILL与SIGSTOP这两个信号是不能忽略的方式处理,只能以默认的方式处理.
     if(signal(SIGKILL,SIG_IGN)==SIG_ERR){
        perror("fail to signal");
        exit(1);
    }
     if(signal(SIGSTOP,SIG_IGN)==SIG_ERR){
        perror("fail to signal");
        exit(1);
    }
    */
    while(1){
        printf("AAA\n");
        sleep(1);
       
    }
    return 0;
}
void handler(int sig){
    if(sig==SIGINT){
        printf("SIGINT正在处理!!!\n");
    }
     if(sig==SIGQUIT){
        printf("SIGQUIT正在处理!!!\n");
    }
     if(sig==SIGTSTP){
        printf("SIGTSTP正在处理!!!\n");
    }
}
~~~

### signal函数的使用

~~~c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void *ret_handler;

void handler(int sig){
    printf("AAAAAAAAAA!\n");
    if(signal(SIGINT,ret_handler)==SIG_ERR){
        perror("fail to signal...");
        exit(1);
    }
}

int main(){
    if((ret_handler=signal(SIGINT,handler))==SIG_ERR){
        perror("fail to signal");
        exit(1);
    }
    while(1){
        printf("hello!\n");
        sleep(1);
    }
    return 0;
}

~~~

## 可重入函数

可重入函数是指函数可以由多个任务并发使用,而不必担心数据错误。可重入函数就可以被中断的函数,当前函数可在任何时刻中断它,并执行另一块代码,当执行完毕后,回到原来的代码还可以正常继续运行.

编写可重入函数:

1. 不使用静态的数据,全局变量(除非信号量互斥)

2. 不调用动态内存分配,释放的函数

3. 不调用任何不可重入的函数(标准的IO函数)

常见的可重入函数列表:

![](../source_img/Linux%20C高级编程.assets/14.png)



~~~c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void handler(int sig){
    printf("AAAAA\n");
}

int main(){

    signal(SIGINT,handler);
    //sleep是一个可重入函数,但是当执行信号处理函数之后,不会回到原来的位置继续睡眠.
   // sleep(6);
    alarm(10);
    while(1){
        printf("hello\n");
        sleep(1);
    }
}
~~~

### 为什么sleep函数也被称为可重入函数

`sleep()` 函数被称为**可重入函数**的原因是，它符合可重入函数的定义。可重入函数可以在任意时刻被中断，并安全地被重新调用而不会引发数据错误。这意味着即使在执行过程中发生信号中断或函数被另一个线程调用，也不会产生副作用或数据不一致的问题。

以下是为什么 `sleep()` 被认为是可重入函数的具体原因：

1. **不使用全局或静态数据**
   - `sleep()` 函数不会使用全局或静态数据。它仅涉及局部变量或者系统调用，因此每次调用 `sleep()` 都是独立的，不会因为并发调用产生冲突。
   
2. **不会修改共享资源**
   - `sleep()` 不修改任何共享资源。它只是简单地使当前线程或进程进入睡眠状态，期间不涉及修改全局状态或者跨进程、跨线程的数据。
   
3. **原子操作**

   - `sleep()` 通常只是一个系统调用，操作系统会挂起调用它的进程或线程，直到指定的时间过去为止。这种操作是原子的，不会产生部分执行的状态，即使在中断之后也不会影响再次调用的安全性。

4. **可中断且安全**

   - 在某些操作系统中，`sleep()` 可以被信号中断，比如接收到 `SIGALRM` 等信号时，`sleep()` 会提前终止。然而，提前终止后系统仍然会安全地返回控制权，而不破坏程序状态或引发不一致的行为。

**可重入函数的定义**

可重入函数是一类特殊的函数，具有以下特征：

- **没有依赖全局或静态变量**：函数内部使用的所有数据都通过局部变量或参数传递。
- **不修改共享资源**：函数不会修改全局状态或者共享的外部资源。
- **可以被中断和再次调用**：函数在执行时可以被信号中断并且可以在中断后重新调用，而不会影响程序的正确性。

由于 `sleep()` 符合这些条件，它可以被认为是一个可重入函数。



```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void handler(int sig){
    printf("AAAAA\n");
}

int main(){
    
    signal(SIGINT,handler);
     /*read也是可重入函数,在等待终端输入时,如果产生信号并执行信号处理函数,信号处理完之后,可以继续输入数据,*/
    char buffer[30]=" ";
    if(read(0,buffer,20)==-1){
        perror("fail to read");
        exit(1);
    }
    printf("buffer=[%s]\n",buffer);
    return 0;
}
```



## 信号集

信号集是用来表示多个信号的数据类型.

~~~c
#include <signal.h>
int sigemptyset(sigset_t *set);
功能:初始化由set指向的信号集,清除其中所有的信号即初始化一个空集信号集.
参数: set 集号集标识的地址,以后操作此信号集,对set进行操作就可以了.
返回值:
	成功返回  0
	失败返回 -1

int sigfillset(sigset_t *set);
功能: 初始化信号集set,将信号集合设置为所有信号的集合.
参数:   set 集号集标识的地址,以后操作此信号集,对set进行操作就可以了.
返回值:
	成功返回  0
	失败返回 -1
        
int sigaddset(sigset_t *set, int signum);
功能: 将信号signum 加入到信号集合 set之中.
参数:
	set : 信号集标识的地址.
	signum : 信号的编号
返回值:
	成功返回  0
	失败返回 -1

int sigdelset(sigset_t *set, int signum);
功能: 将信号signum 从信号集合 set中删除.
参数:
	set : 信号集标识的地址.
	signum : 信号的编号
返回值:
	成功返回  0
	失败返回 -1

int sigismember(const sigset_t *set, int signum);
功能: 查询signum标识的信号是否在信号集合set之中.
参数:
	set : 信号集标识的地址.
	signum : 信号的编号
返回值:
	成功返回  在信号集中返回1  不在信号集中返回0
	失败返回  返回-1
~~~



~~~c
#include <signal.h>
#include <stdio.h>
int main(){
    //创建一个信号集;
    sigset_t set;
    int ret=0;

    //初始化一个空的集号集;
    sigemptyset(&set);
    ret=sigismember(&set,SIGINT);
    if(ret == 0){
        printf("SIGINT不在信号集中%d\n",ret);
    }
    //添加指定的信号集
    sigaddset(&set,SIGINT);
    sigaddset(&set,SIGQUIT);
    ret=sigismember(&set,SIGINT);
    if(ret == 1){
        printf("SIGINT信号在信号集中....%d\n",ret);
    }
    return 0;


}
~~~



### sigset_t等操作系统相关显示未定义

打开C++配置文件，将Cstandard中C17改为GNU17。

在 C 编程中，`C17` 和 `GNU17` 是两种不同的标准选项，具体差异如下：

1. **C17 标准**

   - `C17` 是 ISO C 标准的一个版本，它是 C11 的一个小修订版（C17 的正式标准是 ISO/IEC 9899:2018）。与 C11 相比，C17 主要修正了一些错误和不一致的地方，没有引入新功能。
   - 当你在编译器中选择 `C17` 标准时，编译器会遵循 ISO/IEC C 标准规范，不包含任何额外的扩展。这意味着只提供纯粹的标准 C 库功能，遵循最严格的标准兼容性。
   - 因此，使用 `C17` 时，一些在 GNU 扩展中定义的类型和函数可能不可用，比如 `sigset_t`。

2. **GNU17 标准**
   - `GNU17` 是 GCC（GNU 编译器）的一种扩展模式，基于 C17 标准，但同时引入了一些 GNU 扩展。GNU 扩展是一些超出 ISO C 标准规范的附加功能和库函数，用于提供额外的灵活性和实用性。
   - 当你选择 `GNU17` 时，除了遵循 C17 标准之外，你还可以使用 GNU 特定的扩展功能，比如 POSIX 相关的类型和函数，包括 `sigset_t` 之类的类型。
   - 这意味着在 `GNU17` 模式下，编译器会更宽松地允许使用一些在标准 C 中未定义的功能。

结论

- **C17** 代表了纯粹的 ISO C 标准（不带任何扩展），因此某些非标准类型和函数可能无法使用。
- **GNU17** 则在遵循 C17 标准的基础上，增加了 GNU 特有的扩展功能，使得在编译过程中可以使用如 `sigset_t` 这样的 POSIX 类型。

因此，当你使用 `C17` 标准时，由于该标准没有包含 POSIX 扩展，所以 `sigset_t` 类型未定义。而使用 `GNU17` 时，编译器启用了 GNU 扩展，所以 `sigset_t` 可以正常使用。



## 信号阻塞集

​	每个进程都有一个阻塞集,它用来描述哪些信号递送到该进程的时候被阻塞(在信号发生时记住它,直到进程准备好时再将信号通知进程)。所谓的阻塞并**不是禁止**传送信号,而是**暂缓**信号的传送.若将被阻塞的信号从信号阻塞集中删除,且对应的信号在被阻塞时发生了,进程会收到相应的信号。

~~~c
 int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
 功能:检查或修改信号阻塞集,根据how指定的方法对进程的阻塞集合进行修改,新的信号阻塞集由set指定,而原先的信号阻塞集合由oldset保存.
 参数:
    how : 信号阻塞集合的修改方法
    SIG_BLOCK :向信号阻塞集中添加set 信号集
    SIG_UNBLOCK: 从信号阻塞集合中删除set 集合
    SIG_SETMASK : 将信号阻塞集合设为set 集合
    set :  要操作的信号集地址
    oldset : 保存原先信号集地址:
    
    注意: 若set 为NULL,则不会改变信号阻塞集合,函数只把当前信号阻塞集合保存到oldset中.
返回值:
    成功:  返回0
    失败:  返回-1
    
~~~



~~~c
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(){

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set,SIGINT);
    
    while(1){
        //将set信号集添加到信号阻塞集中
        sigprocmask(SIG_BLOCK,&set,NULL);
        for(int i=0;i<6;i++){
             printf("SIGINT 信号集在阻塞集中\n");
             sleep(1); 
        }
        //将set集号集从信号阻塞集中删除
        sigprocmask(SIG_UNBLOCK,&set,NULL);
          for(int i=0;i<6;i++){
             printf("SIGINT 信号集在阻塞集中删除\n");
             sleep(1); 
        } 
    }
}
~~~

---

# 无名管道

## **无名管道概述** 

&#9;管道(pipe)又称无名管道.无名管道是一种特殊类型的文件，在应用层体现为两个打开的文件描述符。 

&#9;任何一个进程在创建的时候，系统都会给他分配4G的虚拟内存，分为3G的用户空间和1G的内核空间，内核空间是所有进程公有的，**无名管道就是创建在内核空间的**，多个进程知道同一个无名管道的空间，就可以利用他来进行通信。

![](../source_img/Linux%20C高级编程.assets/13.png)

&#9;管道是最古老的UNIX IPC方式，其特点是:

1. 半双工，数据在同一时刻只能在一个方向上流动。 

2. 数据只能从管道的一端写入，从另一端读出。 

3. 写入管道中的数据遵循先入先出(`FIFO`)的规则。 

4. 管道所传送的数据是无格式的，这要求管道的读出方与写入方必须事先约定好数据的格式，如多少字节算一个消息等。 

5. 管道不是普通的文件，不属于某个文件系统，其只存在于内存中。 

6. 管道在内存中对应一个缓冲区。不同的系统其大小不一定相同。 

7. 从管道读数据是一次性操作，数据一旦被读走，它就从管道中被抛弃，释放空间以便写更多的数据。 

8. 管道没有名字，只能在具有公共祖先的进程之间使用。

## **无名管道的创建 -- pipe函数**

~~~c
#include <unistd.h>
int pipe(int pipefd[2]);
功能：创建一个无名管道，返回两个文件描述符负责对管道进行读写操作
参数：
	pipefd：int型数组的首地址，里面有两个元素
	pipefd[0] 负责对管道执行读操作
	pipefd[1] 负责对管道执行写操作
返回值：
	成功：0
	失败：‐1
~~~

### pipe示例1：

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int pipefd[2];
    if(pipe(pipefd) == -1)
    {
        perror("fail to create pipe.");
        exit(1);
    }
    printf("pipefd[0] == %d\n", pipefd[0]);
    printf("pipefd[1] == %d\n", pipefd[1]);
    return 0;
}
```

**为何pipefd[0]，pipefd[1]输出的是3与4，3与4表示什么？**

在上述代码中，`pipefd[0]` 和 `pipefd[1]` 分别输出了 `3` 和 `4`，这是因为它们是新创建的文件描述符（file descriptors），在 Linux 和 Unix 系统中，文件描述符是用于表示打开文件或 I/O 资源的整数。

1. **文件描述符的基本概念**

   - 在 Unix/Linux 系统中，每个进程都有一个文件描述符表，用于管理该进程打开的文件或其他 I/O 资源。
   - 文件描述符是一个非负整数，通常从 0 开始。
   - 常见的标准文件描述符：
     - `0`: 标准输入（stdin）
     - `1`: 标准输出（stdout）
     - `2`: 标准错误输出（stderr）

   这些是默认分配的文件描述符，当你打开新的文件或创建新的 I/O 通道（如管道）时，系统会分配未使用的最小整数作为新的文件描述符。

2. **`pipe()` 函数的行为**

   - `pipe()` 函数用于创建一个匿名管道，它会生成两个文件描述符，一个用于读取数据（`pipefd[0]`），另一个用于写入数据（`pipefd[1]`）。
   - 当你调用 `pipe(pipefd)` 时，系统会分配两个新的文件描述符来表示管道的读端和写端。
   - 由于文件描述符 `0`, `1`, `2` 已经分别用于标准输入、标准输出和标准错误输出，所以系统会从 `3` 开始为新创建的资源分配文件描述符。

3. **为什么是 `3` 和 `4`**

   - 在你的程序中，管道创建后，系统为其分配了文件描述符 `3` 和 `4`，分别表示管道的读端和写端。这是因为文件描述符 `0`, `1`, `2` 已经被占用了，所以系统从 `3` 开始分配给新的 I/O 资源。
   - 具体来说：
     - `pipefd[0] = 3`: 这是管道的读端，用于从管道中读取数据。
     - `pipefd[1] = 4`: 这是管道的写端，用于向管道中写入数据。

### pipe示例2:

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    //使用pipe创建一个无名管道
    int fd_pipe[2];
    if(pipe(fd_pipe) == -1)
    {
        perror("fail to pipe");
        exit(1);
    }

    printf("fd_pipe[0] = %d\n", fd_pipe[0]);
    printf("fd_pipe[1] = %d\n", fd_pipe[1]);

    //对无名管道执行读写操作
    //由于无名管道给当前用户进程两个文件描述符，所以只要操作这两个文件
    //描述符就可以操作无名管道，所以通过文件IO中的read和write函数对无名管道进行操作

    //通过write函数向无名管道中写入数据
    //fd_pipe[1]负责执行写操作
    //如果管道中有数据，再次写入的数据会放在之前数据的后面，不会把之前的数据替换
    if(write(fd_pipe[1], "I am a teacher", 14) == -1)
    {
        perror("fail to write");
        exit(1);
    }

    write(fd_pipe[1], "this is a book", strlen("this is a book")+1);

    //通过read函数从无名管道中读取数据
    //fd_pipe[0]负责执行读操作
    //读取数据时，直接从管道中读取指定个数的数据，如果管道中没有数据了，则read函数会阻塞等待
    char buf[32] = "";
    ssize_t bytes;
    if((bytes = read(fd_pipe[0], buf, 20)) == -1)
    {
        perror("fail to read");
        exit(1);
    }

    printf("buf = [%s]\n", buf);
    printf("bytes = %ld\n", bytes);
 
    bytes = read(fd_pipe[0], buf, sizeof(buf));
    printf("buf = [%s]\n", buf);
    printf("bytes = %ld\n", bytes);
   
    bytes = read(fd_pipe[0], buf, sizeof(buf));
    printf("buf = [%s]\n", buf);
    printf("bytes = %ld\n", bytes);

    return 0;
}

~~~

## **无名管道实现进程间通信**

**注意：** 

&#9;利用无名管道实现进程间的通信，都是父进程创建无名管道，然后再创建子进程，子进程继承父进程的无名管道的文件描述符，然后父子进程通过读写无名管道实现通信.

### 匿名管道进程通信规范示例

``` c
/*
编写一个程序，实现父子进程间通过无名管道传递一个整数数组。父进程生成一个随机整数数组，并将该数组通过管道发送给子进程，子进程接收并计算数组元素的和。
*/
//原始：
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

const int Length = 10;

int main()
{
    pid_t pid;
    pid = fork();
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("fail to create pipe.");
        exit(1);
    }
    if (pid < 0)
    {
        perror("fail to fork.");
        exit(1);
    }
    if (pid == 0)
    {
        printf("子进程: \n");
        int getNumArray[Length];
        int sum = 0;
        for(int i=0; i<Length; i++)
        {
            read(pipefd[0], getNumArray+i, sizeof(int));
            sum += getNumArray[i];
        }
        printf("sum = %d", sum);
    }
    else
    {
        printf("父进程: \n");
        srand(time(NULL));
        // 生成一个随机数
        int randomNumber;
        for(int i=0; i<Length; i++)
        {
            randomNumber = rand();
            write(pipefd[1], &randomNumber, sizeof(randomNumber));
        }
    }
}
//规范版：
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

const int Length = 10;

int main()
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("fail to create pipe.");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fail to fork.");
        exit(1);
    }

    if (pid == 0)
    {
        // 子进程：读取数据并计算和
        close(pipefd[1]);  // 子进程关闭写端

        int getNumArray[Length];
        int sum = 0;

        // 读取整个数组
        read(pipefd[0], getNumArray, sizeof(getNumArray));
        for (int i = 0; i < Length; i++)
        {
            sum += getNumArray[i];
        }
        printf("子进程: 数组元素的和 = %d\n", sum);
        
        close(pipefd[0]);  // 关闭读端
        _exit(0);  // 子进程退出
    }
    else
    {
        // 父进程：生成随机数数组并发送
        close(pipefd[0]);  // 父进程关闭读端
        int randomNumberArray[Length];
        srand(time(NULL));
        printf("父进程: 生成的随机数组为: ");
        for (int i = 0; i < Length; i++)
        {
            randomNumberArray[i] = rand() % 100;  // 生成100以内的随机数
            printf("%d ", randomNumberArray[i]);
        }
        printf("\n");

        // 将整个数组写入管道
        write(pipefd[1], randomNumberArray, sizeof(randomNumberArray));

        close(pipefd[1]);  // 关闭写端
        wait(NULL);// 等待子进程结束
    }
    return 0;
}
```

**改进要点总结：**

1. **关闭不必要的文件描述符**：确保父进程和子进程只关闭不需要的管道端。
2. **父进程等待子进程结束**：使用 `wait()` 或 `waitpid()` 防止子进程变成僵尸进程。
3. **使用块读写操作**：避免每次只读写一个字节或一个整数，可以使用整个数组的大小进行一次性读写。
4. **防止僵尸进程**：确保子进程使用 `_exit(0)` 正常退出。

这样做可以避免很多潜在的错误并提高程序的健壮性。



## **无名管道的读写规律**

### **读写端都存在，只读不写**

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    int pipefd[2];
    if(pipe(pipefd) == -1)
    {
        perror("fail to pipe");
        exit(1);
    }

    write(pipefd[1], "hello world", 11);

    char buf[128] = "";
    if(read(pipefd[0], buf, sizeof(buf)) == -1)
    {
        perror("fail to read");
        exit(1);
    }

    printf("buf = %s\n", buf);

    if(read(pipefd[0], buf, sizeof(buf)) == -1)
    {
        perror("fail to read");
        exit(1);
    }

    printf("buf = %s\n", buf);

    return 0;
}
~~~

**总结** ：读写端都存在，只读不写 如果管道中有数据，会正常读取数据；如果管道中没有数据，则读操作会阻塞等待，直到有数据为止

### **读写端都存在，只写不读**

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    int pipefd[2];
    if(pipe(pipefd) == -1)
    {
        perror("fail to pipe");
        exit(1);
    }

    //读写端都存在，只写不读
    //如果一直执行写操作，则无名管道对应的缓冲区会被写满，写满之后，write函数也会阻塞等待
    //默认无名管道的缓冲区64K字节

    int num = 0;
    while(1)
    {
        if(write(pipefd[1], "6666", 1024) == -1)
        {
            perror("fail to write");
            exit(1);
        }
        num++;
        printf("num = %d\n", num);
    }

    return 0;
}
~~~

**总结**：读写端都存在，只写不读，如果一直执行写操作，则无名管道对应的缓冲区会被写满，写满之后，**write函数也会阻塞等待**。默认无名管道的缓冲区64K字节

### **只有读端，没有写端**

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    int pipefd[2];
    if(pipe(pipefd) == -1)
    {
        perror("fail to pipe");
        exit(1);
    }

    write(pipefd[1], "hello world",11);

  
    close(pipefd[1]);

    char buf[128] = "";
    ssize_t bytes;
    if((bytes = read(pipefd[0], buf, sizeof(buf))) == -1)
    {
        perror("fail to read");
        exit(1);
    }

    printf("bytes = %ld\n", bytes);
    printf("buf = %s\n", buf);

    //清除字符串中的内容
    memset(buf, 0, sizeof(buf));

    if((bytes = read(pipefd[0], buf, sizeof(buf))) == -1)
    {
        perror("fail to read");
        exit(1);
    }

    
    printf("bytes = %ld\n", bytes);
    printf("buf = %s\n", buf);

    return 0;
}
~~~

  **总结**：关闭写文件描述符，只有读端。如果原本管道中有数据，则读操作正常读取数据，如果管道中没有数据，则**read函数会返回0**。

### **只有写端，没有读端**

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handler(int sig)
{
    printf("SIGPIPE信号产生了，管道破裂了\n");
}

int main(int argc, char const *argv[])
{
    signal(SIGPIPE, handler);

    int pipefd[2];
    if(pipe(pipefd) == -1)
    {
        perror("fail to pipe");
        exit(1);
    }
    close(pipefd[0]);
    int num = 0;
    while(1)
    {
        if(write(pipefd[1], "hello world", 1024) == -1)
        {
            perror("fail to write");
            exit(1);
        }
        num++;
        printf("num = %d\n", num);
    }

    return 0;
}
~~~

   **总结**:  关闭读操作文件描述符，只有写端，如果关闭读端，一旦执行写操作，就会产生一个信号**SIGPIPE（管道破裂）**，这个信号的默认处理方式是退出进程。

## **通过fcntl函数设置文件的阻塞特性** 

**设置为阻塞：** 

`fcntl(fd, F_SETFL, 0); `

**设置为非阻塞：** 

`fcntl(fd, F_SETFL, O_NONBLOCK); `

&#9;如果是阻塞，管道中没有数据，read会一直等待，直到有数据才会继续运行，否则一直等待。 

&#9;如果是非阻塞，`read`函数运行时，会先看一下管道中是否有数据，如果有数据，则正常运行读取数据，如果管道中没有数据，则read函数会立即返回，继续下面的代码运行。

~~~c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int fd_pipe[2];
	char buf[] = "hello world";
	pid_t pid;
	
	if (pipe(fd_pipe) < 0)
	{
		perror("fail to pipe");
		exit(1);
	}

	pid = fork();
	if (pid < 0)
	{
		perror("fail to fork");
		exit(0);
	}
	if (pid == 0)
	{
		while(1)
		{
			sleep(5);
			write(fd_pipe[1], buf, strlen(buf));
		}
	}
	else
	{
		//将fd_pipe[0]设置为阻塞
		//fcntl(fd_pipe[0], F_SETFL, 0);
		//将fd_pipe[0]设置为非阻塞
		//fcntl(fd_pipe[0], F_SETFL, O_NONBLOCK);
		while(1)
		{
			memset(buf, 0, sizeof(buf));
			read(fd_pipe[0], buf, sizeof(buf));
			printf("buf=[%s]\n", buf);
			sleep(1);
		}
	}
	return 0;
}
~~~

## **文件描述符概述** 

&#9;文件描述符（`File Descriptor`, 简称` FD`）是操作系统中用来表示打开的文件、管道、网络连接等的抽象标识符。它是一个非负整数，由内核维护，用于管理对文件和其他资源的访问。在 `Unix `和` Linux `系统中，文件描述符是系统调用接口中的核心概念，几乎所有的` I/O `操作（如读、写、打开、关闭文件）都基于文件描述符进行。

### **文件描述符的基本概念：**

1. **整数标识符**: 文件描述符是一个简单的非负整数（如 0, 1, 2 等），它在进程打开一个文件或其他 I/O 资源时由内核分配。每个进程有自己独立的文件描述符表，用来记录该进程打开的文件。

2. **标准文件描述符**: 每个进程在创建时，都会默认打开三个标准文件描述符：
   - **标准输入（stdin）**: 文件描述符 0，通常指向键盘输入。
   - **标准输出（stdout）**: 文件描述符 1，通常指向终端输出。
   - **标准错误（stderr）**: 文件描述符 2，通常指向终端的错误输出。

3. **抽象层**: 文件描述符作为一个抽象层，隐藏了底层 I/O 的实现细节。无论是操作文件、管道、设备、还是网络套接字，系统都会为其分配文件描述符，使得这些资源的访问方式保持一致。

**文件描述符的生命周期**

1. **分配**: 当进程使用 `open()` 或 `socket()` 等系统调用打开文件或网络连接时，内核为该资源分配一个文件描述符。
2. **使用**: 进程可以通过 `read()`、`write()` 等系统调用对文件描述符执行 I/O 操作。
3. **关闭**: 当不再需要使用文件描述符时，进程调用 `close()` 释放它。关闭文件描述符是必须的操作，否则会导致资源泄漏。

**文件描述符表**

每个进程维护一个文件描述符表，它是一个数组，其中每个元素都包含一个指向内核文件表项的指针。文件表项包含文件的实际状态信息，如文件偏移量、访问模式（读、写）、引用计数等。

- **文件描述符表**: 记录每个打开文件的描述符，进程通过文件描述符表来访问文件。

- **文件表项**: 文件表项是一个内核数据结构，保存了有关打开文件的详细信息。所有进程共享文件表项，它们通过文件描述符访问文件表项的内容。其包含了：

  - **文件位置指针（File Offset）**: 当前文件读写位置的偏移量，用于文件的读写操作。每个进程的文件描述符表中的文件描述符可能会有不同的偏移量。
  - **文件状态标志（File Status Flags）**: 文件的状态信息，例如只读、读写模式等。
  - **文件控制块（File Control Block）**: 包括指向文件系统中的 inode 的指针或索引，表示文件的实际位置和属性。
  - **引用计数（Reference Count）**: 表示有多少文件描述符或进程正在使用这个文件表项。当引用计数为零时，文件表项会被释放。

  > 注意：多个文件描述符可以指向同一个文件表项。例如，`dup()` 系统调用创建的文件描述符就是指向同一个文件的副本，它们共享文件表项。

![](../source_img/Linux%20C高级编程.assets/31.png)

**常见场景**

1. **重定向**: 文件描述符可以用于重定向标准输入、输出。例如，将标准输出重定向到文件或其他设备：

   ```c
   int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
   dup2(fd, 1);  // 将文件描述符 1（stdout）重定向到文件
   ```

2. **管道（Pipe）**: 管道是一种特殊的文件描述符，允许进程间通信。创建管道时会产生两个文件描述符，一个用于读取（`pipefd[0]`），一个用于写入（`pipefd[1]`）。

3. **网络编程**: 套接字（Socket）是网络连接的抽象，也通过文件描述符进行操作。网络连接的建立、数据的收发等都依赖文件描述符。

**小结**

- **文件描述符** 是一个整数，用于标识打开的文件或其他 I/O 资源。
- 通过文件描述符，进程可以进行读、写、关闭等操作。
- 文件描述符在操作系统中提供了一个统一的接口，用于管理文件、设备、管道、网络连接等各种资源。
- 每个进程都有独立的文件描述符表，且系统在打开文件时分配的文件描述符是进程范围内唯一的。

**注意：** 

`Linux`中一个进程最多只能打开`NR_OPEN_DEFAULT`（即1024）个文件，故当文件不再使用时应及时调用close函数关闭文件。

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include  <sys/wait.h>
#include <fcntl.h>
int main(){
     int fd;
     while(1){
        if((fd = open("test.txt", O_RDONLY | O_CREAT, 0664)) < 0){
            perror("fail to open");
            exit(1);
        }
        printf("fd=%d\n",fd);

     }
     return 0;
}
~~~



`open`、`read` 和 `write` 是 C 语言中用于文件操作的系统调用。它们常用于直接操作文件描述符进行低级别文件操作。

### `open` 函数

`open` 函数用于打开文件，并返回一个文件描述符（file descriptor），后续对文件的操作通过该文件描述符进行。

**函数原型**：

```c
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

**参数**：

- **pathname**: 文件路径（字符串）。
- **flags**: 文件的打开方式和标志，如 `O_RDONLY`（只读）、`O_WRONLY`（只写）、`O_RDWR`（读写）、`O_CREAT`（文件不存在时创建它）、`O_TRUNC`（截断文件为0字节）。
- **mode**（可选）: 当使用 `O_CREAT` 标志创建新文件时，必须指定文件权限，如 `0644` 表示用户有读写权限，组和其他用户只有读权限。

**返回值**：

- 成功时返回文件描述符，是一个非负整数。
- 失败时返回 `-1`，并设置 `errno` 来指示错误原因。

**示例**：

```c
int fd = open("example.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
if (fd == -1) {
    perror("Failed to open file");
}
```

### `read` 函数

`read` 函数用于从文件或其他文件描述符中读取数据，将读取的数据存储到指定的缓冲区中。

**函数原型**：

```c
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count);
```

**参数**：

- **fd**: 文件描述符。
- **buf**: 读取数据存放的缓冲区（指针）。
- **count**: 读取的字节数。

**返回值**：

- 成功时返回读取的字节数（可以小于 `count`，表示已经读取到文件末尾）。
- 返回 `0` 表示到达文件末尾。
- 失败时返回 `-1`，并设置 `errno`。

**示例**：

```c
char buffer[128];
ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
if (bytes_read == -1) {
    perror("Failed to read file");
}
```

###  `write` 函数

`write` 函数用于将数据写入文件或其他文件描述符。

**函数原型**：

```c
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t count);
```

**参数**：

- **fd**: 文件描述符。
- **buf**: 写入的数据（指针）。
- **count**: 要写入的字节数。

**返回值**：

- 成功时返回实际写入的字节数。
- 失败时返回 `-1`，并设置 `errno`。

**示例**：

```c
const char *data = "Hello, world!";
ssize_t bytes_written = write(fd, data, strlen(data));
if (bytes_written == -1) {
    perror("Failed to write to file");
}
```

**综合使用示例**：

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    int fd = open("example.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Failed to open file");
        return 1;
    }

    const char *data = "Hello, world!";
    ssize_t bytes_written = write(fd, data, strlen(data));
    if (bytes_written == -1) {
        perror("Failed to write to file");
        close(fd);
        return 1;
    }

    close(fd);  // 关闭文件
    return 0;
}
```

### `close`函数

`close` 函数用于关闭文件描述符，当不再需要访问文件时，应该调用此函数来释放文件描述符资源。文件描述符是系统级别的资源，通常一个进程最多可以打开一定数量的文件描述符，所以合理关闭不再使用的文件描述符非常重要。

**函数原型**：

```c
#include <unistd.h>
int close(int fd);
```

**参数**：

- **fd**: 文件描述符，需要关闭的文件或设备的标识符，由 `open` 或其他系统调用返回。

**返回值**：

- 成功时返回 `0`。
- 失败时返回 `-1`，并设置 `errno` 来指示错误原因。常见错误包括：
  - `EBADF`: 无效的文件描述符（可能已经关闭或从未打开）。
  - `EINTR`: `close` 被信号中断，调用可能需要重试。

**作用**：

- **释放资源**: 关闭文件描述符后，内核会释放与文件描述符关联的资源，如缓冲区、文件表项等。
- **写入缓冲区内容**: 对于以 `O_WRONLY` 或 `O_RDWR` 方式打开的文件，`close` 会确保将缓冲区中剩余的数据刷新到磁盘（类似 `fflush` 的功能）。
- **同步到文件系统**: 当关闭一个打开的文件时，文件系统会确保所有的写操作都同步到磁盘上，从而保证数据的一致性。
- **更新引用计数**: 在 Unix 系统中，多个文件描述符可能指向同一个文件表项（如通过 `dup` 或 `fork` 创建的文件描述符）。在这种情况下，`close` 函数减少文件表项的引用计数，当引用计数降为零时，才真正释放文件相关的资源。

**注意事项**：

- **重复关闭**:  对同一个文件描述符多次调用 `close` 会导致错误，第二次及之后的调用会返回 `-1`，并设置 `errno` 为 `EBADF`（Bad File Descriptor）。。
- **关闭标准输入/输出/错误**: 如果不小心关闭了标准输入 (`stdin`)、标准输出 (`stdout`) 或标准错误 (`stderr`)，可以使用 `dup2` 重新分配这些文件描述符，或通过 `freopen` 函数重新打开它们。

**示例**：

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    // 打开文件
    int fd = open("example.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Failed to open file");
        return 1;
    }

    // 向文件写入数据
    const char *data = "Hello, world!";
    ssize_t bytes_written = write(fd, data, 13);
    if (bytes_written == -1) {
        perror("Failed to write to file");
        close(fd);  // 关闭文件描述符
        return 1;
    }

    // 关闭文件
    if (close(fd) == -1) {
        perror("Failed to close file");
        return 1;
    }

    printf("File closed successfully.\n");
    return 0;
}
```

**细节解析**：

1. **`close(fd)`**：
   - 调用 `close` 时，内核会检查文件描述符 `fd` 是否有效。如果有效，则关闭文件，并释放与此文件描述符相关的资源。
   - 成功时，`close` 返回 0；如果出现错误，返回 `-1`，并设置 `errno`。

2. **关闭缓冲区**：
   - 对于输出操作，如果使用了缓冲区，`close` 会确保将缓冲区中的数据写入文件或设备。
   - 因此，调用 `close` 不仅是释放资源，还包括确保数据完整性。

3. **多次 `close` 的问题**：
   - 如果一个文件描述符已经关闭，再次调用 `close` 会导致 `EBADF` 错误。更糟糕的是，如果文件描述符已经被另一个文件或资源重新使用，错误的 `close` 操作可能会关闭其他不相关的资源，导致难以调试的问题。

4. **关闭管道和套接字**：
   - 对于管道或套接字，`close` 的作用不仅仅是关闭文件描述符，还可能通知对端通信结束。例如，关闭写端可能会使得对端读端收到 `EOF`。

## **文件描述符的复制**

`dup` 和 `dup2` 是两个用于复制文件描述符的系统调用。它们可以将一个现有的文件描述符复制到一个新的文件描述符中，以实现多个文件描述符指向同一文件或设备。

### `dup` 函数

`dup` 函数用于创建一个新的文件描述符，该文件描述符是传入的文件描述符的副本。

**函数原型**：

```c
#include <unistd.h>

int dup(int oldfd);
```

**参数**：

- **oldfd**: 要复制的文件描述符。

**返回值**：

- 成功时返回新的文件描述符。
- 失败时返回 `-1`，并设置 `errno` 来指示错误原因。

**行为**：

- 新的文件描述符是当前可用的最小整数。
- 新的文件描述符与 `oldfd` 共享相同的文件表项，因此它们共享同样的文件偏移量、文件状态标志（如 `O_RDONLY`, `O_WRONLY`），以及其他文件属性。

**示例**：

```c
int fd1 = open("example.txt", O_RDONLY);
int fd2 = dup(fd1);
```

``` c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(void)
{
    //通过dup函数复制一个文件描述符
    int fd;
    //dup执行后给返回值文件描述符分配的值是文件描述符表中最小可用的文件描述符
    fd = dup(1);
    printf("fd = %d\n", fd);
    //由于通过dup函数将1这个文件描述符复制了一份为fd，所以fd现在就相当于1，所以写数据就是想终端写入数据
    write(fd, "this ia a book\n", strlen("this ia a book\n"));

    return 0;
}
```

``` c
// 实现 输出重定向,先往文件输出,再回到标准输出
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include  <sys/wait.h>
#include <fcntl.h>
int main(){
    int fd1;
    int fd2;
    fd2=dup(1);
    printf("fd2===%d\n",fd2);
    fd1=open("mm.txt",O_RDWR |O_CREAT,0664);
    printf("fd1===%d\n",fd1);
    close(1);  //关闭标准输出文件描述符
    int fd3=dup(fd1);
    printf("AAAAAAAAAA\n");
    printf("fd3===%d\n",fd3);
    
    close(1);
    int fd4=dup(fd2);
    printf("BBBBBBBB\n");
    printf("fd4===%d\n",fd4);

}
```



### `dup2` 函数

`dup2` 函数也是用于复制文件描述符，但它允许将 `oldfd` 复制到一个指定的文件描述符 `newfd` 上。

**函数原型**：

```c
#include <unistd.h>

int dup2(int oldfd, int newfd);
```

**参数**：

- **oldfd**: 要复制的文件描述符。
- **newfd**: 目标文件描述符。如果 `newfd` 已经被打开，那么它将被关闭，然后 `oldfd` 会复制到 `newfd` 上。

**返回值**：

- 成功时返回 `newfd`。
- 失败时返回 `-1`，并设置 `errno`。

**行为**：

- 如果 `newfd` 等于 `oldfd`，则 `dup2` 什么也不做，直接返回 `newfd`。
- 如果 `newfd` 已经被打开，则在复制前会被自动关闭，避免文件描述符泄露。

**示例**：

```c
int fd1 = open("example.txt", O_RDONLY);
int fd2 = dup2(fd1, 10);  // 将 fd1 复制到文件描述符 10
```

``` c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(void)
{
	int fd1;
	int fd2;
  
	
	fd1 = open("abc.txt", O_CREAT | O_WRONLY, 0664);
	if (fd1 < 0)
	{
		perror("fail to open");
		exit(1);
	}
    
    //首先关闭1文件描述符，然后将fd1复制给1，意味着1和fd1都标识test.txt文件,返回值跟1是一样的
	fd2 = dup2(fd1, 1);
    printf("hello world\n");
	printf("fd2 = %d\n", fd2);
    
	return 0;
}
```

``` c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int fd1;
    //如果使用dup2，则需要实现给第二个参数对应的变量赋一个初始值
	int fd2 = 3;
	
    //将1复制一份为fd2，所以fd2标识的是标准输出
	dup2(1, fd2);
	printf("fd2 = %d\n", fd2);
	fd1 = open("test.txt", O_CREAT | O_RDWR, 0664);
    //输出重定向：关闭文件描述符1，将fd1复制一份为1，所以1此时标识的是test.txt文件
	dup2(fd1, 1);
	printf("hello world\n");
    //再次实现标准输出：关闭文件描述符1，将fd2复制一份为1，所以1此时标识的是标准输出
	dup2(fd2, 1);
	printf("你好\n");
	return 0;
}
```



3. **相同点**

- **文件描述符复制**: 无论是 `dup` 还是 `dup2`，它们最终都会创建一个新的文件描述符，指向与 `oldfd` 相同的文件或设备。复制的文件描述符共享文件的偏移量、文件状态标志等。

- **文件属性共享**: 新的文件描述符与原始文件描述符共享同样的文件属性，如文件偏移量、访问模式（读、写）、锁等。这意味着通过其中一个文件描述符进行的操作会影响另一个文件描述符（例如，读写位置的改变）。

4. **不同点**

- **目标文件描述符的选择**:
  - `dup`: 新的文件描述符是当前系统中可用的最小整数。
  - `dup2`: 允许用户指定目标文件描述符 `newfd`，并且 `newfd` 可能是任意有效的文件描述符。如果 `newfd` 已经被占用，`dup2` 会关闭它并重新分配。

- **行为差异**:
  - `dup` 只能分配新的文件描述符，而 `dup2` 则可以将文件描述符复制到一个指定的目标位置。如果目标位置已经打开，`dup2` 会在复制前关闭该文件描述符。

- **安全性**:
  - `dup2` 可以避免文件描述符泄露的问题，因为它会在需要时关闭 `newfd`。而 `dup` 则只是简单地返回一个新的文件描述符。

**总结**

- `dup` 和 `dup2` 都是用于复制文件描述符的系统调用。
- **`dup`** 自动分配一个新的文件描述符，是系统中最小的可用整数。
- **`dup2`** 允许将文件描述符复制到一个指定的位置，如果该位置已经被占用，它会先关闭该文件描述符，然后再进行复制。

通过这两个函数，可以实现进程内多个文件描述符指向同一个文件或设备的场景，比如重定向标准输入、输出等。

# 有名管道

## 有名管道

有名管道`(FIFO)`和管道`(PIPE)`基本上相同,但也有一些不同点:

**特点**:

1. 半双工,数据在同一时刻只能在一个方向上流动.
2. 写入`FIFO`中的数遵循先入先出的规则.
3. `FIFO`所传送的数据是无格式的,这要求对`FIFO`的读出方与写入方必须事先约定好数据的格式,比如说多少字节算一个消息等.

4. `FIFO`在文件系统中作为一个特殊文件而存在并且在文件系统中可见,所以有名管道可以实现不相关的进程间通信,但`FIFO`中的内容却可以存放在内存中.
5. 管道在内存中对应一个缓冲区,不同的系统其大小不一定相同.
6. 从`FIFO`读数据是一次性操作,数据一旦被读,它就从`FIFO`中被抛弃,释放空间以便写多的数据.
7. 当使用`FIFO`的进程退出后,`FIFO`文件将继续保存在文件系统中以便以后使用.
8. `FIFO`有名字,不相关的进程可以通过打开命名管道进行通信.



## 有名管道的创建

​      

1. 方法一: 用shell 命令mkfifo创建有名管道    mkfifo myfifo  

2. 方法二: 使用函数mkfifo

   ```c
   #include <sys/types.h>
   #include <sys/stat.h>
   int mkfifo(const char *pathname, mode_t mode);
   功能:创建一个有名管道,产生一个本地文件系统可见的pathname
   参数:
      pathname: 有名管道创建后生成的文件,可以带路径
      mode :管道文件的权限,一般通过八进制数设置即可 : 比如: 0664
      返回值:
       成功:   0
       失败   -1
   ```

   ```c
   #include <stdio.h>
   #include <stdlib.h>
   #include <unistd.h>
   #include <string.h>
   #include <sys/types.h>
   #include <sys/stat.h>
   #include  <sys/wait.h>
   #include <fcntl.h>
   #include <errno.h>
   int main(){
       if(mkfifo("myfifo_file",0664)==-1){
          // printf("errno====%d",errno);
          if(errno!=EEXIST){
           perror("fifo_file创建失败");
           exit(1);
       }
       }
       return 0;
   }
   ```

   

## 有名管道的读写操作:

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include  <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
int main(){
    //创建mkfifo函数创建有名管道
    if(mkfifo("myfifo2",0664)== -1){
          if(errno!=EEXIST){
        perror("fifo_file创建失败");
        exit(1);
       }
    }
    int fd;
    fd=open("myfifo2",O_RDWR);
    if(fd==-1){
        perror("fail to open!");
        exit(1);
    }
    //往管道写数据
    if(write(fd,"this is a book",strlen("this is a book")) ==-1){
        perror("fail to write");
        exit(1);
    }
    write(fd,"I am a teacher",strlen("I am a teacher"));
    //读管道数据
    char buffer[32]="";
    if(read(fd,buffer,sizeof(buffer))==-1){
        perror("fail to read");
        exit(1);
    }
    printf("buffer===[%s]\n",buffer);

       if(read(fd,buffer,sizeof(buffer))==-1){
        perror("fail to read");
        exit(1);
    }
    printf("buffer===[%s]\n",buffer);
    //close函数关闭文件描述符;
    close(fd);
    return 0;
}
~~~



## 有名管道实现进程间通信

由于有名管道在本地创建了一个管道文件,所以不相关的进程间也可以实现通信.

### 发送端 send

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include  <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
int main(){
    /**
       创建了二个有名管道;
    */
    if(mkfifo("myfifo3",0664)==-1){
        if(errno !=EEXIST){
            perror("fail to mkfifo..");
            exit(1);
        }

    }
     if(mkfifo("myfifo4",0664)==-1){
        if(errno !=EEXIST){
            perror("fail to mkfifo..");
            exit(1);
        }
    }
    //打开两个有名管道得到文件描述符
    int fd_w,fd_r;
    if((fd_w=open("myfifo3",O_WRONLY))== -1){
        perror("fail to open");
        exit(1);
    }
    if((fd_r=open("myfifo4",O_RDONLY))== -1){
        perror("fail to open");
        exit(1);
    }
    char buffer[128]="";
    ssize_t bytes;
    while(1){
        fgets(buffer,sizeof(buffer),stdin);
        buffer[strlen(buffer)-1]='\0';
        //send进程负责将数据写入myfifo3,接着从myfifo4中读取数据.
        if((write(fd_w,buffer,sizeof(buffer)))==-1){
            perror("fail to writer");
            exit(1);
        }
        if((bytes=read(fd_r,buffer,sizeof(buffer)))==-1){
            perror("fail to read.");
            exit(1);
        }
        printf("form recv: %s\n",buffer);
    }

    return  0;
}
~~~

### 接收端recv

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include  <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
int main(){
    /**
       创建了二个有名管道;
    */
    if(mkfifo("myfifo3",0664)==-1){
        if(errno !=EEXIST){
            perror("fail to mkfifo..");
            exit(1);
        }

    }
     if(mkfifo("myfifo4",0664)==-1){
        if(errno !=EEXIST){
            perror("fail to mkfifo..");
            exit(1);
        }

    }
    //打开两个有名管道得到文件描述符
    int fd_w,fd_r;
     if((fd_r=open("myfifo3",O_RDONLY))== -1){
        perror("fail to open");
        exit(1);
    }

    if((fd_w=open("myfifo4",O_WRONLY))== -1){
        perror("fail to open");
        exit(1);
    }
   
    char buffer[128]="";
    ssize_t bytes;
    while(1){
      
        if((bytes=read(fd_r,buffer,sizeof(buffer)))==-1){
            perror("fail to read.");
            exit(1);
        }
        printf("form send: %s\n",buffer);
         fgets(buffer,sizeof(buffer),stdin);
        buffer[strlen(buffer)-1]='\0';
         write(fd_w,buffer,sizeof(buffer));

    }

    return  0;
}
~~~

##       有名管道的读写规律

###      读取端都存在,只读不写.

**读取端存在，只读不写**: 进程可以读取数据，若管道为空，读取操作会阻塞，直到有数据写入或管道关闭

``` c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
int main()
{
    if (mkfifo("myfifo5", 0664) == -1)
    {
        if (errno != EEXIST)
        {
            perror("fail to mkfifo");
            exit(1);
        }
    }
    int fd;
    if ((fd = open("myfifo5", O_RDWR)) == -1)
    {
        perror("fail to open");
        exit(1);
    }
    char buffer[128] = "";
    write(fd, "hello world", 11);
    read(fd, buffer, sizeof(buffer));
    printf("buffer= %s\n", buffer);
    // 再读一次
    read(fd, buffer, sizeof(buffer));
    printf("buffer= %s\n", buffer);
    return 0;
}
```

### 读写端都存在,只写不读

**读写端都存在，只写不读**: 进程可以写入数据，写入端关闭后，读取端会收到 `EOF`。当有名管道的缓冲区写满后,`write`函数会发生阻塞。默认有名管道的缓冲区为`64k`字节.

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include  <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
int main(){
 if(mkfifo("myfifo6",0664)== -1){
     if(errno !=EEXIST){
         perror("fail to mkfifo");
         exit(1);
     }
 }
 int fd;
 if((fd=open("myfifo6",O_RDWR))==-1){
     perror("fail to open");
     exit(1);
 }
 int count=0;
 while(1){
     write(fd,"",1024);
     count++;
     printf("count= %d\n",count);
 }
 return 0;
~~~

### 只有读端,没有写端

**只有读端，没有写端**: 若其他进程打开了fifo的写端，本进程可以读取数据，但读取操作会阻塞直到有数据写入或管道关闭；若没有其他进程，会阻塞在open，直到有进程打开写端。FIFO 的设计是为了实现进程间的通信，因此读取操作会等待写入操作的到来。

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
int main()
{
    if (mkfifo("myfifo7", 0664) == -1)
    {
        if (errno != EEXIST)
        {
            perror("fail to mkfifo");
            exit(1);
        }
    }
    printf("===============================\n");
    // 在一个进程中,只有读端,没有这写端
    // 会在open函数位置阻塞.
    int fd;
    if ((fd = open("myfifo7", O_RDONLY)) == -1)
    {
        perror("fail to open");
        exit(1);
    }
    printf("********************\n");
    char buffer[128] = "";
    ssize_t bytes;
    while (1)
    {
        if (bytes = read(fd, buffer, sizeof(buffer)) == -1)
        {
            perror("fail to read..");
            exit(1);
        }
        printf("bytes= %ld\n", bytes);
        printf("buffer== %s\n", buffer);
    }
    return 0;
}
~~~

### 只有写端,没有读端

**只有写端，没有读端**: 若其他进程打开了fifo的读端，本进程可以写入数据，但数据无法在本进程被读取；若没有其他进程，则会阻塞在open，直到有进程打开读端。

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
int main()
{
    if (mkfifo("myfifo7", 0664) == -1)
    {
        if (errno != EEXIST)
        {
            perror("fail to mkfifo");
            exit(1);
        }
    }
    printf("===========================\n");
    /*
       在一个进程中,只有写端,没有读端
       也会在open函数位置阻塞.
    */
    int fd;
    if ((fd = open("myfifo7", O_WRONLY)) == -1)
    {
        perror("fail to open");
        exit(1);
    }
    printf("********************");
    while (1)
    {
        write(fd, "hello linuxc", 12);
        printf("ok!!!\n");
        sleep(1);
    }
    return 0;
}
~~~
