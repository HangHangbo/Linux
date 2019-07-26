//实现修改数据库表中数据的功能

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
    //title
    char title[]="看我改标题了";
    //content
    char content[]="看我把内容也改了";
    //taag_id
    int tag_id=4;
    //拼接语言
    sprintf(sql,"update blog_table set title='%s',content='%s',tag_id=%d Where blog_id=1",title,content,tag_id);
    printf("sql:%s\n",sql);

   //5.让数据库 服务器执行sql
    int ret=mysql_query(connect_fd,sql);
    if(ret<0){
        printf("执行sql失败！ %s\n",mysql_error(connect_fd));
        mysql_close(connect_fd);
        return 1;
    }
    printf("修改成功!\n");
    //断开连接
    mysql_close(connect_fd);

    return 0;
}