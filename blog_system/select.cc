#include <cstdio>
#include <cstdlib>
#include <mysql/mysql.h>
int main(){
    //1.创建一个句柄
    MYSQL* connect_fd = mysql_init(NULL);
    
    //2.和数据库建立连接
    if(mysql_real_connect(connect_fd,"127.0.0.1","root","Z1679759074@q","blog_system",3306,NULL,0)==NULL){
        //连接数据库失败，通过mysql_error打印错误
        printf("连接失败！ %s\n",mysql_error(connect_fd));
        return 1;
    }
    printf("连接成功！\n");

    //3.设置编码方式为 utf8
    mysql_set_character_set(connect_fd,"utf8");

    //4.拼接sql语句
    char sql[1024*4] = "select * from blog_table";

    //5.执行sql语句
     int ret=mysql_query(connect_fd,sql);
    if(ret<0){
        printf("执行 sql 失败! %s\n",mysql_error(connect_fd));
        mysql_close(connect_fd);
        return 1;
    }

    //6.遍历结构集合
    //mysql_store_result 获取结果集合 api 所产生的结果集合会再内存中进行malloc 动态申请内存 需要手动释放 否则出现内存泄漏
    MYSQL_RES* result = mysql_store_result(connect_fd);
    // a)获取到结果集合中的行数（mysql_num_rows）和列数(mysql_num_fields)
    int rows = mysql_num_rows(result);
    int fields = mysql_num_fields(result);
    // b)根据行数和列数来遍历结果
    for(int i = 0;i < rows; ++i){
        //一次获取一行数据(mysql_fatch_row)
        MYSQL_ROW row =mysql_fetch_row(result);
        for(int j = 0;j<fields;++j){
            //将每一行数据按列打印
            printf("%s\t",row[j]);
        }
        printf("\n");
    }
    //释放结果集合
    mysql_free_result(result);

    mysql_close(connect_fd);
    return 0;
}