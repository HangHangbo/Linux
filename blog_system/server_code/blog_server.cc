#include <signal.h>
#include "httplib.h"
#include "db.hpp"
#include <iostream>
MYSQL *mysql = NULL;
int main(){
    using namespace httplib;
    using namespace blog_system;
    //1.先和服务器建立好连接
    mysql=blog_system::MYSQLInit();
    signal(SIGINT,[](int){MySQLRelease(mysql);exit(0);});
    //2.创建相关数据库处理对象
    BlogTable blog_table(mysql);
    TagTable tag_table(mysql);
    AuthorTable author_table(mysql);
    //3.创建服务器，并设置“路由”（HTTP服务器中的路由，和IP协议中的路由不太一样），此处的路由是指把 方法+path =>哪个处理函数相关联
    Server server;
    //新增博客
    server.Post("/blog",[&blog_table](const Request& req,Response& resp){
        printf("新增博客！\n");
        //1.获取到请求中的boby并解析成json

        //字符串 -》json对象 reader
        //json  -> 字符串  writer
        Json::Reader reader;
        Json::FastWriter writer;
        Json::Value req_json;
        Json::Value resp_json;
        bool ret=reader.parse(req.body,req_json);
        if(!ret){
            //说明解析出错，给用户提示
            printf("解析失败！%s\n",req.body.c_str());
            //构造一个响应对象，告诉客户客户端出错了
            resp_json["ok"]=false;
            resp_json["reason"]="input data parse falied!";
            resp.status = 400;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }
        //2.对数据进行校验
        if(req_json["title"].empty()||
            req_json["content"].empty()||
            req_json["tag_id"].empty()||
            req_json["create_time"].empty()){
                  printf("请求数据格式错误！%s\n",req.body.c_str());
                //构造一个响应对象，告诉客户客户端出错了
                resp_json["ok"]=false;
                resp_json["reason"]="input data farmat error!";
                resp.status = 400;
                resp.set_content(writer.write(resp_json),"application/json");
                return;
        }
        //3.真正的调用Mysql接口操作
        ret = blog_table.Insert(req_json);
        if(!ret){
            printf("博客插入失败！\n");
            resp_json["ok"]=false;
            resp_json["reason"] = "blog insert failed!";
            resp.status=500;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }
        //4.构造一个正确的响应给客户端即可
        printf("博客插入成功！\n");
        resp_json["ok"]=true;
        resp.set_content(writer.write(resp_json),"application/json");
        return;
    });

    //查看所有博客列表
    //按照tag_id查询
    server.Get("/blog",[&blog_table](const Request& req,Response& resp){
        printf("查看所有博客列表！\n");
        Json::FastWriter writer;
        //不需要解析响应了
        //如果有tag_id 那就再query_string中获取
        //1.尝试获取 tag_id,如果tag_id这个参数不存在，返回空字符串
        const std::string&tag_id =req.get_param_value("tag_id");
        //2.调用数据库来获取所有的博客结果
        Json::Value resp_json;
        bool ret=blog_table.SelectAll(&resp_json, tag_id);
        if(!ret){
            resp_json["ok"] =false;
            resp_json["reason"] = "select all failed!";
            resp.status = 500;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }
        //3.构造相应结果
        resp.set_content(writer.write(resp_json),"application/json");
        return;
    });

    //按照author_id查询
    server.Get("/blog",[&blog_table](const Request& req,Response& resp){
        printf("查看所有博客列表！\n");
        Json::FastWriter writer;
        //不需要解析响应了
        //如果有author_id 那就再query_string中获取
        //1.尝试获取 author_id,如果tag_id这个参数不存在，返回空字符串
        const std::string&  author_id =req.get_param_value("author_id");
        //2.调用数据库来获取所有的博客结果
        Json::Value resp_json;
        bool ret=blog_table.SelectALLAu(&resp_json, author_id);
        if(!ret){
            resp_json["ok"] =false;
            resp_json["reason"] = "select all failed!";
            resp.status = 500;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }
        //3.构造相应结果
        resp.set_content(writer.write(resp_json),"application/json");
        return;
    });
    //查看每个博客
    server.Get(R"(/blog/(\d+))",[&blog_table](const Request& req,Response& resp){
        Json::FastWriter writer;
        //1.解析获取到的blog_id
        int32_t blog_id = std::stoi(req.matches[1].str());
        printf("查看id为 %d 的博客！\n",blog_id);
        //2.直接调用数据库操作
        Json::Value resp_json;
        bool ret=blog_table.SelectOne(blog_id,&resp_json);
        if(!ret){
            resp_json["ok"]=false;
            resp_json["reason"]="查看指定博客失败："+ std::to_string(blog_id);
            resp.status = 404;
            resp.set_content(writer.write(resp_json),"application/json");
            
            return;
        }
        //3.包装一个执行正确的响应
        resp_json["ok"] = true;
        resp.set_content(writer.write(resp_json),"application/json");
        return;
    });

    //修改
    server.Put(R"(/blog/(\d+))",[&blog_table](const Request& req,Response& resp){
        //1.先获取博客id
        int32_t blog_id=std::stoi(req.matches[1].str());
        printf("修改id为%d的博客！\n",blog_id);
        //2.获取到请求并解析结果
        Json::Reader reader;
        Json::FastWriter writer;
        Json::Value req_json;
        Json::Value resp_json;
        bool ret = reader.parse(req.body,req_json);
        if(!ret){
            printf("请求解析失败！%s\n",req.body.c_str());
            //请求解析失败
            resp_json["ok"] = false;
            resp_json["reason"]="update blog parse request failed";
            resp.status = 400;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }
        //3.对数据进行校验
        if(req_json["title"].empty()||
            req_json["content"].empty()||
            req_json["tag_id"].empty()){
                  printf("请求数据格式错误！%s\n",req.body.c_str());
                //构造一个响应对象，告诉客户客户端出错了
                resp_json["ok"]=false;
                resp_json["reason"]="update blog request farmat error!";
                resp.status = 400;
                resp.set_content(writer.write(resp_json),"application/json");
                return;
        }
        //4.调用数据库操作跟新博客操作
        //从path中获取到的id 设置到json对象中
        req_json["blog_id"]=blog_id;
        ret =blog_table.Update(req_json);
        if(!ret){
            resp_json["ok"] =false;
            resp_json["reason"] = "update blog database failed!";
            resp.status = 500;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }
        //5.构造一个正确的返回结果
        resp_json["ok"]=true;
        resp.set_content(writer.write(resp_json),"application/json");
        return;
    });
    
    //删除
    server.Delete(R"(/blog/(\d+))",[&blog_table](const Request& req,Response& resp){
        //1.获取博客id
        int32_t blog_id=std::stoi(req.matches[1].str());
        printf("删除id为%d的博客！\n",blog_id);
        //2.调用数据库操作
        Json::Value resp_json;
        Json::FastWriter writer;
        bool ret = blog_table.Delete(blog_id);
        if(!ret){
            printf("数据路操作失败！\n");
            resp_json["ok"] =false;
            resp_json["reason"] = "delete blog_table failed!";
            resp.status = 500;
            resp.set_content(writer.write(resp_json),"application/json");
            return; 
        }
        //3.构造一个正确的响应
        resp_json["ok"]=true;
        resp.set_content(writer.write(resp_json),"application/json");
        return;
    });
    //新增标签
    server.Post("/tag",[&tag_table](const Request& req,Response& resp){
        Json::Reader reader;
        Json::FastWriter writer;
        Json::Value req_json;
        Json::Value resp_json;
        //1.解析请求
        bool ret = reader.parse(req.body,req_json);
        if(!ret){
            printf("解析失败！%s\n",req.body.c_str());
            resp_json["ok"] =false;
            resp_json["reason"] = "input tag parse failed!";
            resp.status = 400;
            resp.set_content(writer.write(resp_json),"application/json");
            return; 
        }
        //2.对数据进行校验
        if(req_json["tag_name"].empty()){
            printf("请求格式出错！%s\n",req.body.c_str());
            resp_json["ok"] =false;
            resp_json["reason"] = "input tag parse error!";
            resp.status = 400;
            resp.set_content(writer.write(resp_json),"application/json");
            return; 
        }
        //3.调用mysql 接口操作
        ret= tag_table.Insert(req_json);
        if(!ret){
            printf("标签插入失败！\n");
            resp_json["ok"] = false;
            resp_json["reason"] = "tag insert failed!";
            resp.status = 500;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }
        //4.构造一个成功的响应给客户端
        printf("博客插入成功！\n");
        resp_json["ok"]=true;
        resp.set_content(writer.write(resp_json),"application/json");
        return;
    });
    //删除标签
    server.Delete(R"(/tag/(\d+))",[&tag_table](const Request& req,Response& resp){
        //1.从正则表达式中获取标签id
        int32_t tag_id=std::stoi(req.matches[1].str());
        printf("删除id为%d的博客！\n",tag_id);
        //2.调用数据库操作
        Json::Value resp_json;
        Json::FastWriter writer;
        bool ret =tag_table.Delete(tag_id);
        if(!ret){
            printf("数据库操作失败！\n");
            resp_json["ok"] =false;
            resp_json["reason"]="delete tag_table failed!";
            resp.status = 500;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }
        //3.构造正确的响应
        resp_json["ok"]=true;
        resp.set_content(writer.write(resp_json),"applicatin/json");
        return;
    });
    //查看所有标签
    server.Get("/tag",[&tag_table](const Request& req,Response& resp){
        printf("查看所有标签！\n");
        Json::FastWriter writer;
        Json::Value resp_json;
        Json::Value tags;
        bool ret =tag_table.SelectAll(&tags);
        if(!ret){
            printf("数据库查找所有标签失败！\n");
            //构造响应
            resp_json["ok"] =false;
            resp_json["reason"] ="select all tag failed!";
            resp.status=500;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }
        //3.构造正确响应
        resp.set_content(writer.write(tags),"application/json");
        return;
    });

    //新增用户
    // server.Post("/author",[&author_table](const Request &req,Response& resp){
    //     Json::Reader reader;
    //     Json::FastWriter writer;
    //     Json::Value req_json;
    //     Json::Value resp_json;
    //     //获取请求中的数据，http请求Post方法数据存放在body中
    //     //并讲数据转为json格式
    //     bool ret=reader.parse(req.body,req_json);
    //     //parse解析成功返回0，失败返回非0
    //     if(!ret){
    //         //解析出错
    //         printf("解析失败！%s\n",req.body.c_str());
    //         //构造响应对象
    //         resp_json["ok"]=false;
    //         resp_json["reason"] = "input data parse falied!";
    //         resp.status = 400;
    //         resp.set_content(writer.write(resp_json),"application/json");
    //         return;
    //     }
    //     //2.对数据进行校验
    //     if(req_json["user_name"].asString().empty()||
    //         req_json["passwords"].asString().empty()){
    //             printf("请求数据格式错误！%s\n",req.body.c_str());
    //             //构造异常响应
    //             resp_json["ok"]=false;
    //             resp_json["reason"]="input data farmat error!";
    //             resp.set_content(writer.write(resp_json),"application/json");
    //             return;
    //     }
    //     //3.调用Mysql接口操作
    //     ret = author_table.Insert(req_json);
    //     if(!ret){
    //         printf("添加新用户失败！\n");
    //         resp_json["ok"] = false;
    //         resp_json["reason"] = "blog insert faild!";
    //         resp.set_content(writer.write(resp_json),"application/json");
    //         return;
    //     }
    //     //4.构造一个正确的响应返回给客户端
    //     printf("新用户插入成功！\n");
    //     resp_json["ok"]= true;
    //     resp.set_content(writer.write(resp_json),"application/json");
    //     return;
    // });

    //查看用户列表
    server.Get("/author",[&author_table](const Request& req,Response &resp){
        printf("查看用户列表！\n");
        Json::FastWriter writer;
        Json::Value resp_json;
        Json::Value authors;
        //管理者查看用户列表
        bool ret=author_table.SelectAll(&authors);
        if(!ret){
            printf("数据库查找用户列表失败！\n");
            //构造异常响应
            resp_json["ok"]=false;
            resp_json["reason"]="select all author falied!";
            resp.status=500;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }
        resp.set_content(writer.write(authors),"application/json");
        return;
    });


    // //实现用户登陆时的密码校验
    // server.Post(R"(/author/(\d+))",[&author_table](const Request& req,Response &resp){
    //     //1.从正则表达式中获取到用户id author_id
    //     int32_t author_id=std::stoi(req.matches[1].str());
    //     printf("用户ID 为%d请求登陆！\n",author_id);
    //     //2.获取到数据并解析结果
    //     Json::Reader reader;
    //     Json::FastWriter writer;
    //     Json::Value req_json;
    //     Json::Value resp_json;
    //     std::cout<<author_id<<std::endl;
    //     bool ret =reader.parse(req.body,req_json);
    //     if(!ret){
    //         printf("请求解析失败！%s\n",req.body.c_str());
    //         //请求解析失败
    //         resp_json["ok"] = false;
    //         resp_json["reason"]="请求解析失败";
    //         resp.status = 400;
    //         resp.set_content(writer.write(resp_json),"application/json");
    //         return;
    //     }
    //     //3.对数据进行校验
    //     if(req_json["passwords"].asString().empty()){
    //         printf("用户输入密码格式有误！\n");
    //         resp_json["ok"]=false;
    //             resp_json["reason"]="密码为空!";
    //             resp.status = 400;
    //             resp.set_content(writer.write(resp_json),"application/json");
    //             return;
    //     }
    //     //4.获取用户密码,及请求中的密码
    //     Json::Value true_author;
    //     const std::string passwords=req_json["passwords"].asString();
    //     ret=author_table.SelectOne(&true_author,author_id);
    //     const std::string true_passwords=true_author["passwords"].asString();
    //     if(passwords==true_passwords){
    //         resp_json["ok"]=true;
    //         resp.set_content(writer.write(resp_json),"application/json");
    //         return;
    //     }
    //     else{
    //         printf("输入密码有误！");
    //          resp_json["ok"] =false;
    //         resp_json["reason"] = "账号密码输入有误！\n";
    //         resp.status = 500;
    //         resp.set_content(writer.write(resp_json),"application/json");
    //         return; 
    //     }
    // });

    //根据author/XX判断是进行新增用户操作还是进行用户登录操作
    server.Post(R"(/author/(\d+))",[&author_table](const Request& req,Response &resp){
        //1 通过正则表达式判断此请求是进行新增用户还是进行登录操作
        int32_t flag= std::stoi(req.matches[1].str());
        Json::Reader reader;
        Json::FastWriter writer;
        Json::Value req_json;
        Json::Value resp_json;
        bool ret=reader.parse(req.body,req_json);
        //parse解析成功返回0，失败返回非0
        if(!ret){
            //解析出错
            printf("解析失败！%s\n",req.body.c_str());
            //构造响应对象
            resp_json["ok"]=false;
            resp_json["reason"] = "input data parse falied!";
            resp.status = 400;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }

        
        if(flag == 1){
        //查看用户是否已经存在
        
        ret=author_table.Selsectid(req_json);
        
        //用户已存在注册失败
        if(!ret){
            printf("用户已经存在！\n");
            resp_json["ok"] = false;
            resp_json["reason"] = "user already exists!";
            resp.status = 500;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }

        //用户不存在，可以注册
        //3.调用Mysql接口操作
        ret = author_table.Insert(req_json);
        if(!ret){
            printf("添加新用户失败！\n");
            resp_json["ok"] = false;
            resp.status = 500;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }

        //数据库中插入数据成功，返回用户id
        int32_t author_id;
        ret=author_table.Selsectid_log(req_json,author_id);
        if(!ret){
            printf("查找用户id失败！\n");
            resp_json["ok"] = false;
            resp.status = 500;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }
        //4.构造一个正确的响应返回给客户端
        printf("新用户插入成功！\n");
        resp_json["ok"]= true;
        resp_json["author_id"]=author_id;
        resp.set_content(writer.write(resp_json),"application/json");
        return;
        }
        else if(flag == 2){
        //登陆设计，为使用aurhor_id和密码登陆
        //req::json author_id  password
            int32_t author_id = req_json["author_id"].asInt();
         if(!ret){
            printf("请求解析失败！%s\n",req.body.c_str());
            //请求解析失败
            resp_json["ok"] = false;
            resp_json["reason"]="请求解析失败";
            resp.status = 400;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }
        //3.对数据进行校验
        if(req_json["passwords"].asString().empty()){
            printf("用户输入密码格式有误！\n");
            resp_json["ok"]=false;
                resp_json["reason"]="密码为空!";
                resp.status = 400;
                resp.set_content(writer.write(resp_json),"application/json");
                return;
        }
        //4.获取用户密码,及请求中的密码
        Json::Value true_author;
        const std::string passwords=req_json["passwords"].asString();
        ret=author_table.SelectOne(&true_author,author_id);
        const std::string true_passwords=true_author["passwords"].asString();
        if(passwords==true_passwords){
            resp_json["ok"]=true;
            resp_json["reason"] = "用户登陆成功！";
            resp.set_content(writer.write(resp_json),"application/json");
            return;
        }
        else{
            printf("输入密码有误！");
            resp_json["ok"] =false;
            resp_json["reason"] = "账号密码输入有误！\n";
            resp.status = 500;
            resp.set_content(writer.write(resp_json),"application/json");
            return; 
        }
        }
    });
    server.listen("0.0.0.0",9093);
    return 0;
}
