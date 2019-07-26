//实现删除数据库表中数据的功能

#include <cstdio>
#include <cstdlib>
#include <mysql/mysql.h>

int main(){
    //1.创建一个数据库连接句柄
    MYSQL *connect_fd = mysql_init(NULL);

    //2.和数据库建立连接
    if(mysql_real_connect(connect_fd,"127.0.0.1","root","Z1679759074@q","blog_system",3306,NULL,0)==NULL){
        //连接数据库失败，打印错误
        printf("连接失败！ %s\n",mysql_error(connect_fd));
        return 1;
    }
    printf("连接成功！\n");

    //3.设置编码为utf8

    mysql_set_character_set(connect_fd,"utf8");

    //4.拼接sql语句
    char sql[1024*4]={0};
    sprintf(sql,"delete from blog_table Where blog_id=1");
    printf("sql:%s\n",sql);

   //5.让数据库 服务器执行sql
    int ret=mysql_query(connect_fd,sql);
    if(ret<0){
        printf("执行sql失败！ %s\n",mysql_error(connect_fd));
        mysql_close(connect_fd);
        return 1;
    }
    printf("删除成功!\n");
    //断开连接
    mysql_close(connect_fd);

    return 0;
}