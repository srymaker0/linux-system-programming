/*************************************************************************
	> File Name: server.cpp
	> Author: 
	> Mail: 
	> Created Time: Wed 16 Mar 2022 03:36:28 PM CST
 ************************************************************************/

#define _GNU_SOURCE 1
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <poll.h>

#define USER_LIMIT 5
#define BUFFER_SIZE 64
#define FD_LIMIT 65535

struct client_data
{
    sockaddr_in address;
    char* write_buf;
    char buf[ BUFFER_SIZE ];
};

int setnonblocking( int fd )
{
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

int main( int argc, char* argv[] )
{
    if( argc <= 2 )
    {
        printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi( argv[2] );

    int ret = 0;
    struct sockaddr_in address;
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons( port );
    address.sin_addr.s_addr = inet_addr("0.0.0.0");

    int listenfd = socket( PF_INET, SOCK_STREAM, 0 );
    assert( listenfd >= 0 );

    ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );
    assert( ret != -1 );

    ret = listen( listenfd, 5 );
    assert( ret != -1 );

    client_data* users = new client_data[FD_LIMIT];
    pollfd fds[USER_LIMIT+1];
    int user_counter = 0;              // 表示已连接的用户数量     
    for( int i = 1; i <= USER_LIMIT; ++i )
    {
        fds[i].fd = -1;
        fds[i].events = 0;
    }
    fds[0].fd = listenfd;
    fds[0].events = POLLIN | POLLERR;  // 告诉poll监听可读和错误的事件
    fds[0].revents = 0;                // 由内核修改，实际发生的事件 

    while( 1 )
    {
        ret = poll( fds, user_counter+1, -1 );   // -1 永远阻塞直到某个事件发生
        if ( ret < 0 )
        {
            printf( "poll failure\n" );
            break;
        }
        // 对每一个已连接的用户
        for( int i = 0; i < user_counter+1; ++i )
        {
            // 新用户连接
            if( ( fds[i].fd == listenfd ) && ( fds[i].revents & POLLIN ) )
            {
                struct sockaddr_in client_address; // 保存用户的地址信息（IP，PORT）
                socklen_t client_addrlength = sizeof( client_address );
                // connfd 与用户通讯的连接sockfd
                int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
                if ( connfd < 0 )
                {
                    printf( "errno is: %d\n", errno );
                    continue;
                }
                // 如果用户数量大于设置的值，则提示客户端用户数量过多
                if( user_counter >= USER_LIMIT )
                {
                    const char* info = "too many users\n";
                    printf( "%s", info );
                    send( connfd, info, strlen( info ), 0 );
                    close( connfd );
                    continue;
                }

                user_counter++;                                                 // 用户数量需要+1
                users[connfd].address = client_address;                         // 使用connfd唯一标识用户数据，connfd每次产生的是最小未用的
                setnonblocking( connfd );                                       // 设置非阻塞
                fds[user_counter].fd = connfd;                                  // 标识poll fds数组 —— 初始化   
                fds[user_counter].events = POLLIN | POLLRDHUP | POLLERR;        
                fds[user_counter].revents = 0;
                printf( "comes a new user, now have %d users\n", user_counter );
            }
            else if( fds[i].revents & POLLERR )                                 // revents是内核标识的，出现错误
            {
                printf( "get an error from %d\n", fds[i].fd );
                char errors[ 100 ];
                memset( errors, '\0', 100 );
                socklen_t length = sizeof( errors );
                if( getsockopt( fds[i].fd, SOL_SOCKET, SO_ERROR, &errors, &length ) < 0 )
                {
                    printf( "get socket option failed\n" );
                }
                continue;
            }
            else if( fds[i].revents & POLLRDHUP )                               // TCP连接被对方关闭——通常情况下的recv<0，关闭写操作    
            {
                // std::cout << fds[i].fd << "   " << fds[user_counter].fd << std::endl;
                users[fds[i].fd] = users[fds[user_counter].fd];                 // 将关闭的connfd的user空间指向最后一个用户的user空间
                close( fds[i].fd );                                             // 关闭当前的connfd
                // std::cout << i << "   " << user_counter << std::endl;
                fds[i] = fds[user_counter];                                     // 当前fds[i] 指向的poll实例指向最后一个用户
                i--;
                user_counter--;
                printf( "a client left\n" );
            }
            else if( fds[i].revents & POLLIN )
            {
                int connfd = fds[i].fd;
                // std::cout << connfd << "  " << i << std::endl;
                memset( users[connfd].buf, '\0', BUFFER_SIZE );
                ret = recv( connfd, users[connfd].buf, BUFFER_SIZE-1, 0 );      // 接收用户发送过来的数据
                printf( "get %d bytes of client data %s from %d\n", ret, users[connfd].buf, connfd );
                if( ret < 0 )
                {
                    if( errno != EAGAIN )
                    {
                        close( connfd );
                        users[fds[i].fd] = users[fds[user_counter].fd];
                        fds[i] = fds[user_counter];
                        i--;
                        user_counter--;
                    }
                }
                else if( ret == 0 )
                {
                    printf( "code should not come to here\n" );
                }
                else
                {
                    // 向其他用户发送消息
                    for( int j = 1; j <= user_counter; ++j )
                    {
                        if( fds[j].fd == connfd )
                        {
                            continue;
                        }

                        fds[j].events |= ~POLLIN;
                        fds[j].events |= POLLOUT;                               // 设置当前poll监听可写事件
                        users[fds[j].fd].write_buf = users[connfd].buf;         // 将需要接收消息的用户的buf指向发送信息的用户的buf
                    }
                }
            }
            else if( fds[i].revents & POLLOUT )
            {
                // 把单个用户的信息发送给其他用户
                int connfd = fds[i].fd;
                if( ! users[connfd].write_buf )
                {
                    continue;
                }
                ret = send( connfd, users[connfd].write_buf, strlen( users[connfd].write_buf ), 0 );
                users[connfd].write_buf = NULL;
                fds[i].events |= ~POLLOUT; 
                fds[i].events |= POLLIN;                // 写完数据需要重新注册fds[i]上的可读事件
            }
        }
    }

    delete [] users;
    close( listenfd );
    return 0;
}
