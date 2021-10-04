# linux-system-programming

这里记录了个人学习linux系统编程的代码和一些小的功能。[common](./common)目录是一个通用目录，各部分代码需要用到的文件都在此目录中，以下是说明:point_down:

在`common`目录下

`head.h`：包含系统编程头文件，自己实现的功能的一些头文件

`thread_pool.h`: 线程池

`reactor.h`  `add_to_subreactor` :  reactor反应堆

`common.h`: 一些通用功能，比如设置文件描述符为非阻塞、实现socket创建和连接



[1.命令行参数](./01.getiopt)

[2.文件与目录](./02.file_dir)

[3.实现基于linux下的ls功能](./03.Ls)

-   这是linux下自带的`ls -al`功能

![](https://cdn.jsdelivr.net/gh/srymaker0/blogImgs@main/Img/20211002174759.png)

-   进入[03.Ls目录](./03.Ls)下执行`./a.out -al .` ，你可以看到以下效果

![](https://cdn.jsdelivr.net/gh/srymaker0/blogImgs@main/Img/20211002174919.png)



[4.进程间通信的方式](./04.IPC)

[5.进程](./05.fork)

[6.线程](./06.pthread)

[7.网络编程之进程](./07.socket)

[8.网络编程之线程](./08。socket_thread)

[9.I/O多路复用之select](./09.socket_select)

[10.文件传输](./10.socket_file)

[11.粘包问题](./11.file_transfer)
C/S文件传输的实现

[个人博客](https://srymaker0.com/)中有更详细的说明

![](https://cdn.jsdelivr.net/gh/srymaker0/blogImgs@main/Img/20211004164450.png)



[12.reactor反应堆](./12.reactor)
