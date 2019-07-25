//通过这个程序使用mysql api实现数据的插入功能

#include <cstdio>
#include <cstdlib>
//编译器默认从 /usr/include 目录中查找 头文件，mysql.h
//是在一个mysql的目录中
#include <mysql/mysql.h>

int main(){
    //1.创建一个数据库连接句柄
    MYSQL* connect_fd = mysql_init(NULL);

    //2.和数据库建立连接(和TCP的链接区分开，这是是在应用层建立连接)
    //链接过程需要指定一些必要的信息
    // 链接句柄  mysql_init 返回的指针 
    // 服务器的ip地址  
    // 用户名
    // 密码 
    // 数据库名
    // 端口号 
    // unix_socket 
    // client_flag
    if(mysql_real_connect(connect_fd,"127.0.0.1","root","Z1679759074@q","blog_system",3306,NULL,0)==NULL){
        //连接数据库失败，通过mysql_error打印错误
        printf("连接失败！ %s\n",mysql_error(connect_fd));
        return 1;
    }
    printf("连接成功！\n");

    //3.设置编码方式为 utf8
    //mysql server 部分最初安装的时候已经设置成utf8
    //客户端这边也要设置为utf8

    mysql_set_character_set(connect_fd,"utf8");

    //4.拼接sql语句
    char sql[1024*4] = {0};
    char title[] = "打一个嘴炮";
    char content[] = "我要找到好工作";
    int tag_id = 1;
    char data[] = "2019/07/25";
    //拼接语句
    sprintf(sql,"insert into blog_table values(null,'%s','%s',%d,'%s')",title,content,tag_id,data);
    printf("sql:  %s\n",sql);

    //5.让数据库 服务器执行 sql
    int ret=mysql_query(connect_fd,sql);
    if(ret<0){
        printf("执行 sql 失败! %s\n",mysql_error(connect_fd));
        mysql_close(connect_fd);
        return 1;
    }
    printf("插入成功！\n");
    //断开连接
    mysql_close(connect_fd);
    return 0;
}