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

