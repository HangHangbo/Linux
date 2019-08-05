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

    //以下操作相关参数都统一使用json 的方式
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
        sprintf(sql.get(),"insert into blog_table values(null,'%s','%s',%d,%d,'%s')",
        blog["title"].asCString(),
        to.get(),
        blog["tag_id"].asInt(),
        blog["author_id"].asInt(),
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
            sprintf(sql,"select blog_id,title,author_id,tag_id,create_time from blog_table");
        }
        else{
            //需要筛选
            sprintf(sql,"select blog_id,title,author_id,tag_id,create_time from blog_table where tag_id=%d",std::stoi(tag_id));
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
            blog["author_id"]=atoi(row[2]);
            blog["tag_id"]=atoi(row[3]);
            blog["create_time"]=row[4];
            blogs->append(blog);
        }
        mysql_free_result(result);
        printf("执行查找博客成功！\n共查找到%d条博客\n",rows);
        return true;
    }
    bool SelectALLAu(Json::Value *blogs,const std::string& author_id=""){
        char sql[1024*4]={0};
        if(author_id==""){
            sprintf(sql,"select blog_id,title,author_id,tag_id,create_time from blog_table");
        }
        else{
            sprintf(sql,"select blog_id,title,author_id,tag_id,create_time from blog_table where author_id=%d",std::stoi(author_id));
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
            blog["author_id"]=atoi(row[2]);
            blog["tag_id"]=atoi(row[3]);
            blog["create_time"]=row[4];
            blogs->append(blog);
        }
        mysql_free_result(result);
        printf("执行查找博客成功！\n共查找到%d条博客\n",rows);
        return true;
    }
    bool SelectOne(int32_t blog_id,Json::Value *blog){
        char sql[1024*4]={0};
        sprintf(sql,"select blog_id,title,content,author_id,tag_id,create_time from blog_table where blog_id=%d",blog_id);
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
        (*blog)["author_id"]=row[3];
        (*blog)["tag_id"]=atoi(row[4]);
        (*blog)["create_time"]=row[5];
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
        sprintf(sql.get(),"update blog_table set title='%s',content='%s',author_id=%d,tag_id = %d where blog_id=%d",
        blog["title"].asCString(),
        to.get(),
        blog["author_id"].asInt(),
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
    bool Insert(const Json::Value &tag){
        //1.拼接sql语句
        char sql[1024*4]={0};
        sprintf(sql,"insert into tag_table values(null,'%s')",
        tag["tag_name"].asCString());
       
        //2.执行mysql语句
        int ret = mysql_query(mysql_,sql);
        if(ret!=0){
            printf("插入标签失败! %s\n",mysql_error(mysql_));
            return false;
        }
        printf("插入标签成功！\n");
        return true;
    }
    //删
    bool Delete(int32_t tag_id){
        char sql[1024 * 4]={0};
        sprintf(sql,"delete from tag_table where tag_id = %d",tag_id);
        int ret=mysql_query(mysql_,sql);
        if(ret!=0){
            printf("执行删除标签失败! %s\n",mysql_error(mysql_));
            return false;
        }
        printf("执行删除标签成功！\n");
        return true;
    }

    //查
    bool SelectAll(Json::Value *tags){
        char sql[1024 * 4]={0};
        //拼接sql语句
        sprintf(sql,"select tag_id,tag_name from tag_table");
        //执行mysql语句
        int ret= mysql_query(mysql_,sql);
        if(ret!=0){
            printf("执行sql失败！%s\n",mysql_error(mysql_));
            return false;
        }
        MYSQL_RES* result = mysql_store_result(mysql_);
        int rows=mysql_num_rows(result);
        for(int i=0;i<rows;i++){
            MYSQL_ROW row=mysql_fetch_row(result);
            Json::Value tag;
            tag["tag_id"] =atoi(row[0]);
            tag["tag_name"] = row[1];
            tags->append(tag);
        }
        mysql_free_result(result);
        printf("执行查找标签成功！\n共查找到%d条标签\n",rows);
        return true;
    }
    
private:
    MYSQL* mysql_;
};

//作者表
    class AuthorTable{
    public:
        AuthorTable(MYSQL* connect_fd):mysql_(connect_fd){}
         bool Insert(const Json::Value &author){
            char sql[1024*4]={0};
            sprintf(sql,"insert into author_table values(null,'%s','%s')",
            author["user_name"].asCString(),
            author["passwords"].asCString()
            );
            int ret = mysql_query(mysql_,sql);
            if(ret != 0){
                printf("执行增加用户失败！\n%s\n",mysql_error(mysql_));
                return false;
            }
            printf("执行成功增加一名用户成功！%s\n",author["user_name"].asCString());
            return true;
        }

        //查

        //查看用户列表
        bool SelectAll(Json::Value* authors){
            char sql[1024*4]={0};
            sprintf(sql,"select author_id,user_name from author_table");
            int ret = mysql_query(mysql_,sql);
            if(ret!=0){
                printf("执行查找失败！\n%s\n",mysql_error(mysql_));
                return false;
            }
            printf("执行查找作者列表成功！");
            MYSQL_RES* result = mysql_store_result(mysql_);
            int rows = mysql_num_rows(result);
            for(int i=0;i<rows;i++){
                MYSQL_ROW row = mysql_fetch_row(result);
                Json::Value author;
                author["author_id"]=atoi(row[0]);
                author["user_name"]=row[1];
                authors->append(author);
            }
            printf("查看作者列表完成！\n共查到%d条信息",rows);
            mysql_free_result(result);
            return true;
        } 

        //根据id查用户 名字 密码 登陆时用
        bool SelectOne(Json::Value* authors,int32_t author_id){
            printf("有用户请求登陆！\n");
            char sql[1024*4]={0};
            sprintf(sql,"select user_name,passwords from author_table where author_id=%d",author_id);
            int ret = mysql_query(mysql_,sql);
            if(ret!=0){
                printf("用户名不存在!\n");
                return false;
            }
            printf("用户名存在！\n");
            MYSQL_RES* result=mysql_store_result(mysql_);
            int rows = mysql_num_rows(result);
            if(rows!=1){
                printf("查找出多条数据！%d\n",rows);
                return false;
            }
            MYSQL_ROW row = mysql_fetch_row(result);
            (*authors)["user_name"]=row[0];
            (*authors)["passwords"]=row[1];
            mysql_free_result(result);
            printf("查找成功！");
            return true; 
        }


        //用账号密码查id
        bool Selsectid_log(Json::Value& author,int32_t &author_id){
            printf("id 用户登陆!\n");
            //拼接sql语句
            char sql[1024*4]={0};
            //printf("user_name:%s\npassword:%s",author["user_name"].asCString(),author["passwords"].asCString());
            sprintf(sql,"select author_id from author_table where user_name='%s'and passwords='%s'",
            author["user_name"].asCString(),
            author["passwords"].asCString());
            //printf("%s\n",sql);
            //执行sql语句
            int ret=mysql_query(mysql_,sql);
            if(ret!=0){
                printf("执行sql语句失败！\n");
                return false;
            }
            printf("找到用户！\n");
            //获取sql语句执行结果
            MYSQL_RES* result=mysql_store_result(mysql_);
            int rows=mysql_num_rows(result);
            if(rows!=1){
                printf("该用户已存在！\n");
                return false;
            }
            printf("该用户为新用户！\n");
            MYSQL_ROW row=mysql_fetch_row(result);
            author_id=atoi(row[0]);
            return true;
        }

        //查看用户数是否存在，存在则为 false 不存在 true
        bool Selsectid(Json::Value& author){
            printf("查看用户是否已经存在!\n");
            //拼接sql语句
            char sql[1024*4]={0};
            //printf("user_name:%s\npassword:%s",author["user_name"].asCString(),author["passwords"].asCString());
            sprintf(sql,"select author_id from author_table where user_name='%s'and passwords='%s'",
            author["user_name"].asCString(),
            author["passwords"].asCString());
            //printf("%s\n",sql);
            //执行sql语句
            int ret=mysql_query(mysql_,sql);
            if(ret!=0){
                printf("执行sql语句失败！\n");
                return false;
            }
           
            //获取sql语句执行结果
            MYSQL_RES* result=mysql_store_result(mysql_);
            int rows=mysql_num_rows(result);
            //printf("rows:%d\n",rows);
            if(rows!=0){
                printf("该用户未注册！\n");
                return false;
            }
            printf("此用户可以注册！\n");
            return true;
        }


    private:
        MYSQL* mysql_;
    };
}

