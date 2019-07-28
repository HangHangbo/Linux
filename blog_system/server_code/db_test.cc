#include "db.hpp"

void TestBlogTable(){
    Json::StyledWriter writer;
    MYSQL *mysql=blog_system::MYSQLInit();
    blog_system::BlogTable blog_table(mysql);
    bool ret=false;

    // Json::Value blog;
    // blog["title"]="又要打嘴炮了";
    // blog["content"]="好激动";
    // blog["tag_id"]=2;
    // blog["create_time"]="2017/07/28";
    // ret = blog_table.Insert(blog);
    // printf("insert:%d\n",ret);
    
    // Json::Value blogs;
    // std::string arr="2";
    // ret=blog_table.SelectAll(&blogs,arr);
    // printf("SelectAll:%d\n",ret);
    // printf("%s\n",writer.write(blogs).c_str());    
    
    // Json::Value blog;
    // for(int i=3;i<9;i++){
    //     ret=blog_table.SelectOne(i,&blog);
    //     printf("SelectOne:%d\n",ret);
    //     printf("%s\n",writer.write(blog).c_str());
    // }

    
    // for(int i=2;i<9;i++){
    //     ret=blog_table.Delete(i);
    //     printf("Delete:%d\n",ret);
    // }

    Json::Value blog;
    blog["blog_id"]=9;
    blog["title"]="改一哈子";
    blog["content"]="再改一哈子";
    blog["tag_id"]=1;
    blog["create_time"]="1997/8/8";
    ret=blog_table.Update(blog);
    printf("update:%d\n",ret);
     blog_system::MySQLRelease(mysql);
}

int main(){

    TestBlogTable();
    return 0;
}