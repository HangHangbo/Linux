#include "db.hpp"

void TestBlogTable(){
    // Json::StyledWriter writer;
    // MYSQL *mysql=blog_system::MYSQLInit();
    // blog_system::BlogTable blog_table(mysql);
    // bool ret=false;

    // Json::Value blog;
    // blog["title"]="又要打嘴炮了";
    // blog["content"]="好激动";
    // blog["tag_id"]=3;
    // blog["author_id"]=3;
    // blog["create_time"]="2017/07/28";
    // ret = blog_table.Insert(blog);
    // printf("insert:%d\n",ret);
    
    // Json::Value blogs;
    // std::string arr="2";
    // ret=blog_table.SelectALLAu(&blogs);
    // printf("SelectAll:%d\n",ret);
    // printf("%s\n",writer.write(blogs).c_str());    
    
    // Json::Value blog;
    // for(int i=1;i<4;i++){
    //     ret=blog_table.SelectOne(i,&blog);
    //     printf("SelectOne:%d\n",ret);
    //     printf("%s\n",writer.write(blog).c_str());
    // }

    
    // for(int i=1;i<2;i++){
    //     ret=blog_table.Delete(i);
    //     printf("Delete:%d\n",ret);
    // }

    // Json::Value blog;
    // blog["blog_id"]=2;
    // blog["title"]="改一哈子";
    // blog["content"]="再改一哈子";
    // blog["author_id"]=5;
    // blog["tag_id"]=1;
    // blog["create_time"]="1997/8/8";
    // ret=blog_table.Update(blog);
    // printf("update:%d\n",ret);
     

    //标签
    // Json::StyledWriter writer;
    // MYSQL *mysql=blog_system::MYSQLInit();
    // blog_system::TagTable tag_table(mysql);
    // bool ret=false;

    // // 增
    // Json::Value tag;
    // tag["tag_name"] ="JAVA";
    // ret=tag_table.Insert(tag);
    // printf("insert:%d\n",ret);

    //删
    // ret=tag_table.Delete(3);
    // printf("Delete:%d",ret);
    
    //查
    // Json::Value tags;
    // ret=tag_table.SelectAll(&tags);
    // printf("Select:%d\n",ret);
    // printf("%s\n",writer.write(tags).c_str());
    
    Json::StyledWriter writer;
    MYSQL *mysql=blog_system::MYSQLInit();
    blog_system::AuthorTable author_table(mysql);
    bool ret=false;


    // Json::Value author;
    // author["user_name"]="卢本伟";
    // author["passwords"]="8848";
    // ret=author_table.Insert(author);
    // printf("insert:%d",ret);
    // blog_system::MySQLRelease(mysql);

    // Json::Value author;
    // ret=author_table.SelectAll(&author);
    // printf("selsct:%d\n",ret);
    // printf("%s\n",writer.write(author).c_str());

    Json::Value author;
    ret=author_table.SelectOne(&author,1);
    printf("selsct:%d\n",ret);
    printf("%s\n",writer.write(author).c_str());

}

int main(){

    TestBlogTable();
    return 0;
}