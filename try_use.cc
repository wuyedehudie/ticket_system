//
// Created by 86178 on 2024/5/3.
//
#include <bits/stdc++.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>

void func0(){
    std::cout << "This is func0 " << std::endl;
}

void func1(){
    std::cout << "This is func1" << std::endl;
}

using namespace std;

int main(){

    std::function<void(void)> fa = func0;
    std::function<void(void)> fb = func1;

    std::cout << "====交换前====" << std::endl;
    fa();
    std::cout << "fa address: " << &fa << std::endl;
    fb();
    std::cout << "fb address: " << &fb << std::endl;

    //开始交换
    fa.swap(fb);

    std::cout << "====交换后====" << std::endl;
    fa();
    std::cout << "fa address: " << &fa << std::endl;
    fb();
    std::cout << "fb address: " << &fb << std::endl;

    if (fa) {
        std::cout << "fa target address: " << fa.target<void(void)>() << std::endl;
    } else {
        std::cout << "fa has no valid target" << std::endl;
    }



    /*std::string host;
    int fd1= ::socket(AF_INET,SOCK_STREAM,0);
    cout<<fd1<<endl;
    int fd2= ::socket(AF_INET,SOCK_STREAM,0);
    ::close(fd1);
    cout<<fd2<<endl;
    int fd3= ::socket(AF_INET,SOCK_STREAM,0);
    cout<<fd3<<endl;
    int fd4=::open("/home/ciji/test.cc",O_RDONLY);
    cout<<fd4<<endl;
    int fd5=::epoll_create();
    cout<<fd5;*/
}
