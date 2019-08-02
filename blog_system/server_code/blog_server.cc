#include <signal.h>
#include "httplib.h"
#include "db.hpp"
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
    server.listen("0.0.0.0",9093);
    return 0;
}
