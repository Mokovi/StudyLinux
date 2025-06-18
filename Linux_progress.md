# **进程控制**

## **进程号**

​	每个进程都由一个进程号来标识，其类型为 pid_t，进程号的范围：0～32767。进程号是由操作系统随机给当前进程分配的，不能自己控制。进程号总是唯一的，但进程号可以重用。当一个进程终止后，其进程号就可以再次使用了

在 ubuntu 中查看当前系统中所有的开启的进程

~~~linux
ps -ajx
~~~

![](./Linux%20C高级编程.assets/2.png)

**特殊的进程号：** 

在 linux 系统中进程号由 0 开始。进程号为 0 及 1 的进程由内核创建；

- 进程号为 0 的进程通常是调度进程，常被称为交换进程(swapper)；

- 进程号为 1 的进程通常是 init 进程，init 进程是所有进程的祖先。除调度进程外，在 linux 下面所有的进程都由进程 init 进程直接或者间接创建。 

**进程号(PID)** ：标识进程的一个非负整型数。 

**父进程号(PPID)** ：任何进程(除 init 进程)都是由另一个进程创建，该进程称为被创建进程的父进程，对应的进程号称为父进程号(PPID)。 

**进程组号(PGID)** ：进程组是一个或多个进程的集合。他们之间相互关联，进程组可以接收同一终端的各种 

信号，关联的进程有一个进程组号(PGID) 。 

**COMMAND**：当前进程的名字 



**Linux 操作系统提供了三个获得进程号的函数 getpid()、getppid()、getpgid()**

~~~Linux
man getpid    man getpgid
~~~

~~~Linux
#include <sys/types.h>
#include <unistd.h>
pid_t getpid(void);
功能：获取当前进程的进程号
pid_t getppid(void);
功能：获取当前进程的父进程的进程号
pid_t getpgid(pid_t pid);
功能：获取当前进程所在进程组的 id
~~~

~~~c
#include <stdio.h>
 #include <sys/types.h>
 #include <unistd.h>

 int main(int argc, char const *argv [])
{
 //获取当前进程的进程号
 printf("pid = %d\n", getpid());
 //获取当前进程的父进程的 id
 printf("ppid = %d\n", getppid());
 //获取当前进程所在组的 id
 printf("pgid = %d\n", getpgid(getpid()));

 while(1);
  return 0;
}
~~~

![](./Linux%20C高级编程.assets/3.png)

## 进程的创建 fork 函数

~~~c
#include <sys/types.h>
#include <unistd.h>
pid_t fork(void);
功能: 在已有的进程基础上创建一个子进程.
参数无
返回值:
  成功:
   	>0 子进程的进程号, 标识父进程的代码区
  	 0  子进程的代码区
  失败:
    -1  返回给父进程, 子进程不会创建.
~~~

使用 fork 函数得到的子进程是父进程的一个复制品, 它从父进程处继承了整个进程的地址空间。

地址空间：包括进程上下文, 进程堆栈, 打开的文件描述, 信号控制设定, 进程优先级, 进程组号等。子进程所独有的只有它的进程号, 计时器, 因此, 使用 fork 函数的代价是很大的.

  **fork 函数执行完毕后父子进程的空间示意图：**

![](./Linux%20C高级编程.assets/4.png)



## 程序区域划分

在程序运行时，内存被分为不同的区域，用来存储代码、数据、堆栈等内容。

1. **栈区**（Stack Segment）

- **定义**: 栈区用于存储函数调用过程中自动分配的局部变量、函数参数和返回地址等数据。栈区的内存由系统自动分配和释放，遵循后进先出的原则。
- **特点**:
  - **自动管理**: 当函数被调用时，局部变量在栈上分配；函数返回时，栈上的局部变量会自动销毁。
  - **空间有限**: 栈的大小通常是有限的，过大的递归调用或分配过多的局部变量可能导致栈溢出（Stack Overflow）。
  - **高速存取**: 栈内存的分配和释放速度非常快。

2. **堆区**（Heap Segment）

- **定义**: 堆区用于动态分配内存，通常由程序员手动分配和释放（例如通过 `new` 和 `delete` 等函数在 C/C++ 中进行操作）。堆的内存可以在程序运行时动态增长或缩减。
- **特点**:
  - **手动管理**: 程序员负责内存的分配和释放，使用不当可能导致内存泄漏或碎片化。
  - **较大空间**: 与栈相比，堆的内存空间通常更大，可以用于分配大块的内存。
  - **灵活性高**: 适合不确定大小的数据结构，例如链表、树等。

3. **BSS** 区（BSS Segment）

- **定义**: BSS 区（Block Started by Symbol）用于存放未初始化的全局变量和静态变量。该区域在程序加载时由操作系统初始化为零。
- **特点**:
  - **零初始化**: 所有在 BSS 区的变量在程序运行前都会被自动清零。
  - **只存储未初始化数据**: 如果全局或静态变量在定义时没有显式初始化，它们就会被放在 BSS 区。
  - **节省空间**: 未初始化的变量并不占用可执行文件中的空间，它们只是在程序运行时才占用内存。

4. **数据区**（Data Segment）

- **定义**: 数据区用于存放已初始化的全局变量和静态变量。这些变量的初始值会随程序一起加载进内存，并在整个程序的生命周期内存在。
- **特点**:
  - **全局存储**: 包含了所有程序中已初始化的全局变量和静态变量。
  - **可读写**: 数据区中的变量在程序运行期间是可读写的，不会随函数的调用和结束而消失。
  - **持久性**: 这些变量在整个程序运行期间都存在，并且可以跨函数访问。

5. **代码区**（Text Segment）

- **定义**: 代码区（又称文本区）存储程序的机器指令，即程序的代码部分。它是只读的，以防止程序在运行时意外修改自己的指令。
- **特点**:
  - **只读**: 通常代码区是不可写的，防止修改代码。
  - **共享性**: 代码区可以在多线程或多进程之间共享，比如多个进程可以运行同一个可执行文件的不同实例，它们可以共享代码区。
  - **不可变性**: 在程序运行时，代码区的数据（指令）不会改变。

**内存布局总结**

通常程序的内存布局可以按以下顺序从高地址到低地址分布：

1. **栈区（Stack）**: 由高地址向低地址增长。
2. **堆区（Heap）**: 由低地址向高地址增长，动态分配的内存位于这里。
3. **BSS 区（BSS）**: 存储未初始化的全局变量和静态变量，初始值为 0。
4. **数据区（Data）**: 存储已初始化的全局变量和静态变量。
5. **代码区（Text）**: 存储程序的代码，通常是只读的。

## 创建子进程

1.创建子进程

~~~c
//执行一次 fork，就会在原有的进程基础上创建一个新的子进程
//而且如果 fork 之后不区分父子进程的代码区，则后面所有的代码都会执行
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(){
	//fflush(stdout);
	fork();
	printf("hello world");
   // fflush(NULL);
	while(1){
    }
}
~~~

2.创建区分主进程与子进程

~~~c
//fork 后区分父子进程
//使用 fork()创建子进程标准代码
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
int main(){
   
    pid_t pid;
    printf(" [%d]:Begin!\n",getpid());    //一定是父进程打印的
    pid = fork();
    if(pid < 0){
        perror("fork 创建进程错误!");
        exit(1);
    }
    if(pid == 0){
        printf(" [%d]:子进程正在工作...\n",getpid());
    }else{
        sleep(1);
        printf(" [%d]:父进程正在工作...\n",getpid());
    }
     printf("[%d] End!\n", getpid());
   //  getchar();
    exit(0);
}
~~~

``` c
//验证系统进程调度方式
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
int main(){
     
     pid_t pid;
     pid=fork();
     if(pid <0){
        perror("fail to fork...");
        exit(1);
     }
     if(pid ==0){
        //子进程代码区
        printf("[%d]子进程正在运行中....\n",getpid());
        for(int i=0;i<100;i++){
            printf("子进程i=====%d\n",i);
        }
     }else{
        //父进程代码区
        printf("[%d]父进程正在运行中....\n",getpid());
        for(int k=0;k<100;k++){
            printf("父进程k=====%d\n",k);
        }
     }
     while(1);
    return 0;
}
```

3.父进程拥有独立的地址空间.

~~~c
//父子进程拥有独立的地址空间
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int x = 10;
int main()
{
    static int y = 10;
    int z = 10;
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        perror("创建进程失败");
        return -1;
    }
    if (pid > 0)
    {
        // 父进程的代码区
        printf("父进程....\n");
        x++;
        y++;
        z++;
        printf("x =%d, y =%d, z =%d\n", x, y, z);
    }
    else
    {
        // 子进程的代码区
        sleep(1);
        printf("子进程...\n");
        printf("x =%d, y =%d, z =%d\n", x, y, z);
    }

    while (1);
    return 0;
}
~~~

执行结果:

![](./Linux%20C高级编程.assets/7.png)

4.子进程继承父进程的空间

   子进程会继承父进程的一些公有的区域, 比如磁盘空间，内核空间。文件描述符的偏移量保存在内核空间中, 所以父进程改变偏移量, 则子进程获取的偏移量是改变之后的.所以子进程继承父进程的空间.

~~~c
//子进程会继承父进程的一些公有的区域。
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(){
    int fd;
    if((fd = open("myfile.txt", O_RDONLY))==-1){
        perror("open file fail...");
        return -1;
    }
    pid_t pid;
    pid = fork();
    if(pid < 0){
        perror("fork fail....");
        return -1;
    }
    if(pid > 0 ){
        printf("父进程....\n");
        char buffer [32] = "";
        if(read(fd, buffer,20) == -1){
            perror("read file fail...");
            return -1;
        }
        printf("buffer = [%s]\n", buffer);
    }else{
         printf("子进程....\n");
        char buffer [32] = "";
        if(read(fd, buffer,20) == -1){
            perror("read file fail...");
            return -1;
        }
        printf("buffer = [%s]\n", buffer);
    }
    while(10){

    
    }
    return 0;
}
~~~

运行效果图:

![](./Linux%20C高级编程.assets/8.png)

## 进程 sleep 睡眠状态

~~~c
#include <unistd.h>
unsigned int sleep(unsigned int seconds);
功能：进程在一定的时间内没有任何动作，称为进程的挂起（进程处于等待态）
参数：
seconds：指定要挂起的秒数
返回值：
若进程挂起到 second 指定的时间则返回 0，若有信号中断则返回剩余秒数
注意：
进程挂起指定的秒数后程序并不会立即执行，系统只是将此进程切换到就绪态
~~~

~~~c
#include <stdio.h>
#include <unistd.h>

int main()
{
    while(1)
    {
        printf("hello\n");
        //当运行到 sleep 函数后，程序会在此位置等待设定的秒数，当秒数到达后，代码会接着执行
        //sleep 运行时进程为等待态，时间到达后会先切换到就绪态，如果代码继续运行，再切换到运行态
        sleep(1);
    }
    
    return 0;
}
~~~

## **进程的等待 wait 函数**

~~~c
#include <sys/types.h>
#include <sys/wait.h>
pid_t wait(int *status);
功能：等待子进程终止，如果子进程终止了，此函数会回收子进程的资源。调用 wait 函数的进程会挂起，直到它的一个子进程退出或收到一个不能被忽视的信号时才被唤醒。若调用进程没有子进程或它的子进程已经结束，该函数立即返回。
    
参数：status：函数返回时，参数 status 中包含子进程退出时的状态信息。子进程的退出信息在一个 int 中包含了多个字段，用宏定义可以取出其中的每个字段。子进程可以通过 exit 或者_exit 函数发送退出状态。
    
返回值：
	成功：子进程的进程号。
	失败：‐1
~~~

 `WIFEXITED(status)`：取出子进程的退出信息。

​	如果子进程是正常终止的，取出的字段值非零。 

`WEXITSTATUS(status)`：返回子进程的退出状态。

​	返回子进程的退出状态，退出状态保存在 status 变量的 8~16 位。 在用此宏前应先用宏 WIFEXITED 判断子进程是否正常退出，正常退出才可以使用此宏。 

**注意**： 此 status 是 wait 的参数指向的整型变量。 

~~~
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	pid_t pid;
	pid = fork();
	if(pid < 0)
	{
		perror("进程创建失败");
        return -1;
    }
	if(pid == 0)
	{
		int i = 0;
		for(i = 0; i < 3; i++)
		{
			printf("子进程\n");
			sleep(1);
		}
		//使用 exit 退出当前进程并设置退出状态
		exit(3);
	}
	else 
	{
		//使用 wait 在父进程中阻塞等待子进程的退出
		//不接收子进程的退出状态
		//wait(NULL);
		//接收子进程的退出状态，子进程中必须使用 exit 或者_exit 函数退出进程是发送退出状态
		int status = 0;
		wait(&status);
		if(WIFEXITED(status) != 0)
		{
			printf("子进程返回状态: %d\n", WEXITSTATUS(status));
		}
		printf("父进程\n");	
	}
	return 0;
}
~~~

## **waitpid 函数**

~~~c
#include <sys/types.h>
#include <sys/wait.h>
pid_t waitpid(pid_t pid, int *status, int options)
功能：等待子进程终止，如果子进程终止了，此函数会回收子进程的资源。
参数：
pid：指定的进程或者进程组
	pid > 0：等待进程 ID 等于 pid 的子进程。
	pid = 0：等待同一个进程组中的任何子进程，如果子进程已经加入了别的进程组，waitpid 不会等待它。
	pid =‐1：等待任一子进程，此时 waitpid 和 wait 作用一样。
	pid <‐1：等待指定进程组中的任何子进程，这个进程组的 ID 等于 pid 的绝对值
status：保存子进程退出时的状态信息
options：选项
	0：同 wait，阻塞父进程，等待子进程退出。
	WNOHANG：没有任何已经结束的子进程，则立即返回。非阻塞形式
	WUNTRACED：如果子进程暂停了则此函数马上返回，并且不予以理会子进程的结束状态。（跟踪调试，很少用到）
返回值：
	成功：返回状态改变了的子进程的进程号；如果设置了选项 WNOHANG 并且 pid 指定的进程存在则返回 0。
	失败：返回‐1。当 pid 所指示的子进程不存在，或此进程存在，但不是调用进程的子进程，waitpid 就会出错返回，这时 errno 被设置为 ECHILD。
wait(status) <==> waitpid(‐1, status, 0)
~~~

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv [])
{
	pid_t pid;
	
	pid = fork();
	if(pid < 0)
    {
		perror("创建进程失败");
        return -1;
    }
	if(pid == 0)
	{
		int i = 0;
		for(i = 0; i < 5; i++)
		{
			printf("子进程\n");
			sleep(1);
		}
		exit(0);
	}
	else 
	{		
		waitpid(pid, NULL, 0);
		printf("父进程\n");
	}
	return 0;
}
~~~

- **僵尸进程** : 进程已经结束, 但进程的占用的资源未被回收, 这样的进程称为僵尸进程.子进程已经运行结束, 父进程未调用 `wait` 或 `waitpid` 函数回收子进程的资源使子进程变为僵尸进程的原因.

- **孤儿进程**: 父进程运行结束, 但子进程未运行结束的子进程.

- **守护进程**: 守护进程是个特殊的孤儿进程, 这种进程脱离终端, 在后台运行.



## **进程的终止 exit/_exit**

~~~c
#include <unistd.h>
void exit(int status);
功能：退出当前进程
参数：
status：退出状态，由父进程通过 wait 函数接收这个状态
	一般失败退出设置为非 0; 成功退出设置为 0
~~~

 

**_exit 函数** 

~~~c
#include <stdlib.h>
void _exit(int status);
功能：退出当前进程
参数：
status：退出状态，由父进程通过 wait 函数接收这个状态
一般失败退出设置为非 0, 成功退出设置为 0
返回值：
	无
~~~

exit 和_exit 函数的区别： 

exit 为库函数，而_exit 为系统调用 

exit 会刷新缓冲区，但是_exit 不会刷新缓冲区 

一般会使用 exit  



![](./Linux%20C高级编程.assets/9.png)

~~~c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
void fun(){
    printf("BBB\n");
    return;
    printf("CCC\n");
}
int main(){

   printf("AAA\n");
   fun();
   printf("DDD\n");
   return 0;

}

~~~

return 除了返回值以外, 在主函数中可以退出进程, 但是在子函数中使用只能退出当前函数.

exit(0) 可以退出一个进程, 且可以刷新缓冲区.

_exit(0) 可以退出一个进程, 但是不会刷新缓冲区.

## **进程退出清理** atexit

~~~c
#include <stdlib.h>
int atexit(void (*function)(void));
功能: 注册进程正常结束前调用的函数, 进程退出执行注册函数.
参数: function: 进程结束前, 调用函数的入口地址.
    一个进程中可以多次调用 atexit 函数注册清理函数.
    正常结束前调用函数的顺序和注册时的顺序相反.
返回值:
   成功: 0
   失败: 非 0
~~~

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void fun1(){
    printf("clear fun1...\n");
}
void fun2(){
    printf("clear fun2...\n");
}
void fun3(){
    printf("clear fun3...\n");
}
int main(){
    //atexit 函数在进程结束时, 才会去执行参数对应的回调函数.
    atexit(fun1);
    atexit(fun2);
    atexit(fun3);
    printf(" **** **** **** *****\n ");
    sleep(3);
    /*
    while(1){

    }
    */
    return 0;
}
~~~

## **进程的创建--vfork 函数**

~~~c
 #include <sys/types.h>
 #include <unistd.h>
 pid_t vfork(void);
 功能: vfork 函数和 fork 函数一样都是在已有的进程中创建一个新的进程, 但是它们创建的子进程是有区别的.vfork 保证子进程先运行, 在它调用 exec, exit 或者正常结束之后, 父进程才可能被调度运行.
 参数: 无
 返回值:
   成功:  子进程中返回 0, 父进程中返回子进程 ID
   失败:  -1
~~~

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(){

     pid_t pid;
     //使用 vfork 函数创建子进程
     pid = vfork();
     if(pid < 0){
        perror("子进程创建失败...");
        exit(1);
     }
     if(pid == 0){
         int i = 0;
         for(i = 0; i < 6; i++){
            printf("子进程在运行中...\n");
            sleep(1);
         }
         exit(0);
     }else{
         //父进程代码区
         while(10){
            printf("父进程运行中....\n");
            sleep(1);
         }
     }
     return 0;

}
~~~

## **子进程和父进程共享同一块空间(= vfork 创建)**

~~~c
//fork 创建
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int x = 10;
int main(){
     static int y = 10;
     int z = 10;
     pid_t pid;
     pid = fork();
     if(pid < 0){
        perror("创建进程失败");
        return -1;
     }
     if(pid > 0){
        //父进程的代码区
        printf("父进程....\n");
        x++;
        y++;
        z++;
        printf("x =%d, y =%d, z =%d\n", x, y, z);
     }else{
        //子进程的代码区
         sleep(1);
         printf("子进程...\n");
         printf("x =%d, y =%d, z =%d\n", x, y, z);
     }
    
     while(10){

     }
    return 0;
}
~~~

```c
//vfork 创建
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
int x=100;
int main(){
     static int y=200;
     int z=300;
     pid_t pid;
     //子进程会复制父进程 fork 之前的所有内容, 但是 fork 之后, 父子进程完全独立, 所以不管双方怎么改变(堆区, 栈区, 数据区), 都不会影响对方
     pid=vfork();
     if(pid<0){
        perror("创建进程失败!");
        return -1;
     }
     if(pid>0){
        //父进程的代码区
        printf("父进程正在运行中...\n");
      
        printf("x==%d,y==%d,z==%d\n",x,y,z);
     }else{
        //子进程的代码区
       
         printf("子进程正在运行中...\n");
           x++;
           y++;
           z++;
        
        printf("x==%d,y==%d,z==%d\n",x,y,z);
        exit(0);
     }

      while(1);



    return 0;
}
```



## vfork 与 fork 的区别

`vfork()` 和 `fork()` 都是用于创建子进程的系统调用，但它们之间有一些重要的区别，特别是在父进程和子进程之间共享内存空间的情况上。以下是两者的区别：

1. **内存空间**
   - **`fork()`：**
     - `fork()` 创建一个新的子进程，子进程拥有父进程的 **完整拷贝**，包括内存地址空间、文件描述符等。
     - 在现代操作系统中，`fork()` 通常使用 **写时拷贝** (copy-on-write) 机制，即只有当父子进程中的某一方修改数据时，操作系统才会为子进程创建该数据的副本。这减少了不必要的内存开销。
     - 因此，子进程最初看似共享父进程的内存空间，但实际上两者是独立的，只有在数据发生变化时才分开。

   - **`vfork()`：**
     - `vfork()` 主要用于优化性能。与 `fork()` 不同的是，`vfork()` 不会立即复制父进程的内存空间。子进程会 **直接共享父进程的地址空间**，直到子进程调用 `exec()` 或 `exit()`。
     - 在 `vfork()` 之后、调用 `exec()` 或 `exit()` 之前，父进程会被阻塞，等待子进程完成。因为子进程和父进程共享同一个地址空间，子进程对内存的修改会直接影响父进程。
     - 这种行为带来了一些风险：如果子进程在调用 `exec()` 或 `exit()` 之前修改了内存内容，父进程的数据可能会被破坏。因此，使用 `vfork()` 时要小心，避免在子进程中执行不安全的操作。

2. **性能**

   - **`fork()`：**
     - 由于 `fork()` 创建了父进程内存空间的副本，尽管使用了写时拷贝技术，其开销还是比 `vfork()` 大一些，尤其是在内存较大的进程中。

   - **`vfork()`：**
     - `vfork()` 的性能更好，因为它不需要复制父进程的内存空间。子进程直接使用父进程的内存，因此减少了内存和 CPU 的开销。
     - 这种方式在创建短时间内就要调用 `exec()` 的子进程时非常高效。

3. **执行顺序**

   - **`fork()`：**
     - 在 `fork()` 之后，父进程和子进程是独立执行的，谁先执行取决于调度器。父进程并不会等待子进程。

   - **`vfork()`：**
     - 在 `vfork()` 之后，父进程会被阻塞，直到子进程调用 `exec()` 或 `exit()` 完成。这样确保了子进程优先执行，而父进程不会在子进程完成之前继续运行。

4. **应用场景**

   - **`fork()`：**
     - 适合需要在父子进程中保持独立内存空间的情况，尤其是当子进程需要继续运行而不立即执行新程序时。

   - **`vfork()`：**
     - 适合性能敏感的场景，特别是在创建子进程后马上调用 `exec()` 执行新程序的场合。避免了不必要的内存拷贝。

**总结**

- `fork()`：子进程和父进程在创建时拥有独立的内存空间，虽然初始状态下看起来共享，但修改时内存会被复制，属于“写时拷贝”。
- `vfork()`：子进程在调用 `exec()` 或 `exit()` 之前，和父进程共享同一个内存空间，使用时要小心，防止子进程修改父进程的数据。



## **进程的替换 exec**

exec 函数族，是由六个 exec 函数组成的。 

- exec 函数族提供了六种在进程中启动另一个程序的方法。 

- exec 函数族可以根据指定的文件名或目录名找到可执行文件。 

- 调用 exec 函数的进程并不创建新的进程，故调用 exec 前后，进程的进程号并不会改变，其执行的程序完全由新的程序替换，而新程序则从其 main 函数开始执行。 

exec 函数族取代调用进程的数据段、代码段和堆栈段

![](./Linux%20C高级编程.assets/10.png)

一个进程调用 exec 后，除了进程 ID，进程还保留了下列特征不变： 

​	父进程号、进程组号 、控制终端、根目录、当前工作目录、进程信号屏蔽集、未处理信号

**exec 函数族**

``` c
#include <unistd.h>
int execl(const char *path, const char *arg, .../* (char *) NULL */);
int execlp(const char *file, const char *arg, .../* (char *) NULL */);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execle(const char *path, const char *arg, .../*, (char *) NULL*/, char * const envp[] );
int execvpe(const char *file, char *const argv[], char *const envp[]);
```

**功能**：在一个进程里面执行另一个程序，主要用于执行命令
**参数**：

- path：命令或者程序的路径

- l：如果是带 l 的函数，对应的命令或者程序是通过每一个参数进行传递的，最后一个为 NULL 表示结束。例如：`"ls", "‐l", NULL`

- v: 如果是带 v 的函数，对应的命令或者程序是通过一个指针数组来传递的，指针数组的最后一个元素为 NULL 标识结束。例如：`char *str[] = {"ls", "‐l", NULL};`

- p：如果是不带 p 的函数，第一个参数必须传当前命令或者程序的绝对路径，如果是带 p 的函数，第一个参数既可以是绝对路径，也可以是相对路径

- e:  允许你传递一个自定义的环境变量数组（`envp[]`），因此可以为新程序指定不同于当前进程的环境变量。自定义的环境变量数组 `envp[]` 使得你可以在调用 `execve()` 或 `execvpe()` 时，为新进程传递特定的环境变量。通过这种方式，你能够控制新程序的运行环境，与当前进程的环境隔离开来。这在需要运行特定环境设置的子进程时非常有用。

**返回值**：成功：1；失败：‐1



~~~c
例子 1.
 //调用 exec 函数族中的函数，执行其他命令或者程序
 //查看命令的路径：whereis 命令或者 which 命令
 //exec 函数族可以调用 shell 命令
 //不带 p 的函数，命令的路径一定要用绝对路径
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(int argc, char const *argv [])
{
    pid_t pid;
    if((pid = fork()) < 0)
    {
        perror("fail to fork");
        exit(1);
    }
    else if(pid > 0) //父进程
    {
        printf("父进程在运行中\n");
        wait(NULL);
        printf("子进程运行结束\n");
    }
    else //子进程
    {
        printf("子进程运行中\n");
        if(execl("/bin/ls", "ls", "-l", NULL) == -1)
        {
            perror("fail to execl");
            exit(1);
        }
        printf("hello\n");
    }
    return 0;
}
第二种方式:
 //带 p 的函数，第一个参数既可以是相对路径，也可以是绝对路径

 if(execlp("ls", "ls", "-l", NULL) == -1)
        {
            perror("fail to execlp");
            exit(1);
        }
第三种方式:
//带 v 的函数需要使用指针数组来传递
    char *str [] = {"ls", "-l", NULL};
    if(execv("/bin/ls", str) == -1)
     {
         perror("fail to execv");
         exit(1);
     }
 第四种方式:
 
    //**** **** **** ***exec 函数族调用可执行文件*** **** **** *******     
        if(execlp("./hello", "./hello", NULL) == -1)
        {
            perror("fail to execlp");
            exit(1);
        }
第五种方式:
   //**** **** **** ***exec 函数族调用 shell 脚本*** **** **** *******       
        if(execlp("./myshell.sh", "./myshell.sh", NULL) == -1)
        {
            perror("fail to execl");
            exit(1);
        }
~~~

``` c
示例2
print_env.c:

#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("USER: %s\n", getenv("USER"));
    printf("PATH: %s\n", getenv("PATH"));
    return 0;
}

execve.c:

#include <unistd.h>
#include <stdio.h>

int main() {
    char *args[] = {"./print_env", NULL};
    char *envp[] = {
        "USER=custom_user",
        "PATH=/custom/path",
        NULL
    };

    execve("./print_env", args, envp);

    // 如果 execve 执行成功，下面的代码不会被执行
    perror("execve failed");
    return 1;
}
通过gcc print_env.c -o print_env 创建可执行文件后，运行execve.c
```

## **system 函数**

~~~c
#include <stdlib.h>
int system(const char *command);
功能：执行一个 shell 命令（shell 命令、可执行文件、shell 脚本）
system 会调用 fork 函数产生子进程，
子进程调用 exec 启动/bin/sh ‐c string
来执行参数 string 字符串所代表的命令，
此命令执行完后返回原调用进程
参数：
	command：要执行的命令的字符串
返回值：
	如果 command 为 NULL，则 system()函数返回非 0，一般为 1。
	如果 system()在调用/bin/sh 时失败则返回 127，其它失败原因返回‐1
~~~

~~~
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv [])
{
    //使用 system 执行 shell 命令
    system("clear");
    system("ls -l");
    system("./myhello");
    return 0;
}
~~~

### system 与 exec 的区别与相同点

`system()` 和 `exec()` 系列函数都是用于在 C 程序中执行外部命令或程序的函数，但它们在行为、功能和用途上有显著的区别。以下是两者的区别和相同点：

**相同点**

1. **执行外部命令或程序**：
   - 两者都可以用于执行外部的命令或程序。例如，调用 shell 命令、运行可执行文件等。

2. **基于操作系统的函数**：
   - `system()` 和 `exec()` 都是操作系统提供的函数，它们的行为依赖于操作系统的实现。

**不同点**

1. **执行方式**

   - `system()`：

     - `system()` 会调用一个 shell 来执行指定的命令。它首先创建一个新的子进程，然后在该子进程中执行 shell 命令。执行完命令后，子进程会终止，父进程会等待子进程结束并返回其退出状态。
     - `system()` 函数会阻塞父进程的执行，直到命令执行完毕。
       - `system()` 返回值是子进程的退出状态，如果命令执行失败，返回值可能是 -1。
       
     
    - `exec()`：
        - `exec()` 系列函数不调用 shell，而是直接用指定的程序替换当前进程的映像。这意味着调用 `exec()` 后，当前进程的代码和数据会被新程序的代码和数据替代，不再返回到调用 `exec()` 的代码中。
        - `exec()` 不会创建新进程，而是在当前进程中直接加载并执行新程序。
        
      - `exec()` 系列有多个变体（如 `execl()`, `execv()`, `execvp()` 等），用于根据不同的需求传递参数。无论使用哪个变体，`exec()` 都不会返回，除非发生错误。
   
2. **进程关系**

   - `system()`：
     - `system()` 创建一个新的子进程执行命令，而父进程保持不变。子进程完成后，父进程会继续执行原来的代码。
     - 父进程和子进程之间没有共享数据，子进程完成任务后就会退出。

   - `exec()`：
     - `exec()` 不创建子进程，而是用新程序替换当前进程。调用 `exec()` 的进程会直接被新程序替代，不再返回。由于没有创建新进程，因此调用 `exec()` 后，当前进程的进程 ID 不变，但执行的代码已经是新程序的内容。

3. **使用场景**
   - `system()`：
     - 适合简单地调用外部命令或脚本，且希望父进程在子进程执行完毕后继续执行的场景。
     - 例如：在程序中调用 shell 脚本或系统命令，并等待其完成后继续执行后续代码。
   
   - `exec()`：
     - 适合用当前进程执行另一个程序的场景，特别是当你不再需要执行当前进程的代码时。
     - 常见用法是在 `fork()` 之后调用 `exec()`，子进程调用 `exec()` 来执行新程序，而父进程继续执行其他任务。
   
4. **返回行为**

   - `system()`：
     - `system()` 执行完命令后会返回，返回值是命令的退出状态，因此父进程可以检查命令的执行结果。

   - `exec()`：
     - `exec()` 函数不会返回，除非发生错误。如果执行成功，当前进程被新程序替换；如果失败，`exec()` 会返回 -1，并设置 `errno` 以指示错误原因。

**总结**

- `system()`：在子进程中执行外部命令，父进程等待子进程完成并返回退出状态。适合简单的命令执行场景。
- `exec()`：用新程序替换当前进程，不会返回。适合需要完全替换当前进程执行另一个程序的场景。

如果你的任务是执行一个命令并继续当前程序的运行，使用 `system()` 更方便；如果你需要完全替换当前进程为另一个程序，使用 `exec()` 是更好的选择。
