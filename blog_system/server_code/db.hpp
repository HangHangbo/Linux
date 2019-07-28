//创建一些想换的类封装数据库操作

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>
namespace blog_system{

    //初始化句柄，建立连接
    static MYSQL* MYSQLInit(){
        //1.获取句柄
        MYSQL* connect_fd = mysql_init(NULL);
        //2.和数据库建立连接
        if(mysql_real_connect(connect_fd,"127.0.0.1","root","Z1679759074@q","blog_system",3306,NULL,0)==NULL){
            //连接数据库失败，通过mysql_error打印错误
            printf("连接失败！ %s\n",mysql_error(connect_fd));
            return NULL;
        }
        //3.设置字符编码
         mysql_set_character_set(connect_fd,"utf8");
         return connect_fd;
    }

    //释放句柄断开连接
    static void MySQLRelease(MYSQL* connect_fd){
        mysql_close(connect_fd);
    }



//创建一个类，用于操作博客表

class  BlogTable{
public:
    //通过构造函数获取到一个数据库的句柄
    BlogTable(MYSQL * connect_fd):mysql_(connect_fd){}

    //以下操作相关参数都统一使用json 的饭方式
    //JSON::Value jsoncpp 中最核心的类
    //JSON::Value 就代表了一个具体的Json对象
    //方便扩展
    //增
    bool Insert(const Json::Value &blog){
        const std::string& content =blog["content"].asString();
        //char *to =new char[content.size()*2+1];
        //使用unique_ptr管理开辟空间
        std::unique_ptr<char> to(new char[content.size()*2+1]);
        mysql_real_escape_string(mysql_,to.get(),content.c_str(),content.size());
        //拼接sql语句
        //char sql[1024 * 100]={0};
        std::unique_ptr<char> sql(new char[content.size()*2+4096]);
        sprintf(sql.get(),"insert into blog_table values(null,'%s','%s',%d,'%s')",
        blog["title"].asCString(),
        to.get(),
        blog["tag_id"].asInt(),
        blog["create_time"].asCString()
        );

        //执行mysql语句
        int ret=mysql_query(mysql_,sql.get());
        if(ret!=0){
            printf("执行插入博客失败! %s\n",mysql_error(mysql_));
            return false;
        }
        printf("执行插入博客成功！\n");
        return true;
    }
    //查
    //blogs 作为一个输出线参数
    //按标签查找或者全部查找
    bool SelectAll(Json::Value *blogs,const std::string& tag_id=""){
        char sql[1024*4]={0};
        if(tag_id==""){
            //不按tag查找
            sprintf(sql,"select blog_id,title,tag_id,create_time from blog_table");
        }
        else{
            //需要筛选
            sprintf(sql,"select blog_id,title,tag_id,create_time from blog_table where tag_id=%d",std::stoi(tag_id));
        }
        int ret=mysql_query(mysql_,sql);
        if(ret!=0){
            printf("执行所有查找博客失败! %s\n",mysql_error(mysql_));
            return false;
        }
        printf("执行查找博客成功！\n");
        MYSQL_RES* result = mysql_store_result(mysql_);
        int rows=mysql_num_rows(result);
        for(int i=0;i<rows;i++){
            MYSQL_ROW row=mysql_fetch_row(result);
            Json::Value blog;
            //row[]中的下标和上面select语句中写的列是对应的
            blog["blog_id"]=atoi(row[0]);
            blog["title"]=row[1];
            blog["tag_id"]=atoi(row[2]);
            blog["create_time"]=row[3];
            blogs->append(blog);
        }
        mysql_free_result(result);
        printf("执行查找博客成功！\n共查找到%d条博客\n",rows);
        return true;
    }
    bool SelectOne(int32_t blog_id,Json::Value *blog){
        char sql[1024*4]={0};
        sprintf(sql,"select blog_id,title,content,tag_id,create_time from blog_table where blog_id=%d",blog_id);
        int ret=mysql_query(mysql_,sql);
        if(ret!=0){
            printf("执行查找博客失败! %s\n",mysql_error(mysql_));
            return false;
        }
        printf("执行查找博客成功！\n");
        MYSQL_RES* result = mysql_store_result(mysql_);
        //根据获取到的结果进行校验
        int rows=mysql_num_rows(result);
        //blog_id设置的自增主键，只能查到一条
        if(rows!=1){
            printf("查找到的博客不止一条，有%d条",rows);
            return false;
        }
        MYSQL_ROW row =mysql_fetch_row(result);
        (*blog)["blog_id"]=atoi(row[0]);
        (*blog)["title"]=row[1];
        (*blog)["content"]=row[2];
        (*blog)["tag_id"]=atoi(row[3]);
        (*blog)["create_time"]=row[4];
        return true;
    }

    //改
    bool Update(const Json::Value &blog){
        const std::string& content =blog["content"].asString();
        //char *to =new char[content.size()*2+1];
        //使用unique_ptr管理开辟空间
        std::unique_ptr<char> to(new char[content.size()*2+1]);
        mysql_real_escape_string(mysql_,to.get(),content.c_str(),content.size());
        //拼接sql语句
        //char sql[1024 * 100]={0};
        std::unique_ptr<char> sql(new char[content.size()*2+4096]);
        sprintf(sql.get(),"update blog_table set title='%s',content='%s',tag_id = %d where blog_id=%d",
        blog["title"].asCString(),
        to.get(),
        blog["tag_id"].asInt(),
        blog["blog_id"].asInt()
        );

        //执行mysql语句
        int ret=mysql_query(mysql_,sql.get());
        if(ret!=0){
            printf("执行跟新博客失败! %s\n",mysql_error(mysql_));
            return false;
        }
        printf("执行跟新博客成功！\n");
        return true;
    }

    //删
    bool Delete(int32_t blog_id){
        char sql[1024 *4]={0};
        sprintf(sql,"delete from blog_table where blog_id=%d",blog_id);
        int ret=mysql_query(mysql_,sql);
        if(ret!=0){
            printf("执行删除博客失败! %s\n",mysql_error(mysql_));
            return false;
        }
        printf("执行删除博客成功！\n");
        return true;
    }

private:
    MYSQL* mysql_;
};

//操作标签表的类

class TagTable{
public:
    TagTable(MYSQL* connect_fd):mysql_(connect_fd){}
    //增
    bool Insert(const Json::Value tag){
        
        return true;
    }
    //删
    bool Delete(int32_t blod_id){
        return true;
    }

    //查
    bool SelectAll(Json::Value *tags){
        return true;
    }
    
private:
    MYSQL* mysql_;
};


}