#include"tcp_pthread_server.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

using namespace std;

#define MAX_INT 0x7fffffff
#define UPLOAD_PWD "123456"
#define SUCCESS_FILENAME "success"
#define FAIL_FILENAME "fail"
#define PROCESSING_FILENAME	"processing"
#define QUEUE_FILENAME "queue"

string state[] = {"选课中", "选课成功", "选课失败"};
string filename[] = {PROCESSING_FILENAME, SUCCESS_FILENAME, FAIL_FILENAME, QUEUE_FILENAME};
string number[] = {"0", "1", "2", "3", "4", "5"};
string long_post = "";

string first_line = "HTTP/1.1 200 OK\n";

string info_body_start ="<html lang=\"zh-CN\">\
<head>\
    <meta charset=\"utf-8\">\
    <link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bootstrap@4.5.0/dist/css/bootstrap.min.css\" integrity=\"sha384-9aIt2nRpC12Uk9gS9baDl411NQApFmC26EwAOH8WgZl5MYYxFfc+NcPb1dKGj7Sk\" crossorigin=\"anonymous\">\
</head>\
<body>\
    <nav class=\"navbar navbar-expand-lg navbar-dark bg-dark\">\
        <div class=\"container\">\
            <a class=\"navbar-brand\" href=\"/\">自动选课系统</a>\
            <button class=\"navbar-toggler\" type=\"button\" data-toggle=\"collapse\" data-target=\"#navbarsExample05\" aria-controls=\"navbarsExample05\" aria-expanded=\"false\" aria-label=\"Toggle navigation\">\
                <span class=\"navbar-toggler-icon\"></span>\
            </button>\
            <div class=\"collapse navbar-collapse\" id=\"navbarsExample05\">\
            <ul class=\"navbar-nav mr-auto\">\
                <li class=\"nav-item\">\
                    <a class=\"nav-link\" href=\"/index.html\">选课</a>\
                </li>\
                <li class=\"nav-item active\">\
                    <a class=\"nav-link\" href=\"#\">状态查询</a>\
                </li>\
                <li class=\"nav-item\">\
                    <a class=\"nav-link\" href=\"/help.html\">使用帮助</a>\
                </li>\
            </ul>\
            <ul class=\"navbar-nav\">\
                <li class=\"nav-item float-right\">\
                    <a class=\"nav-link\" href=\"#\">Welcome!</a>\
                </li>\
            </ul>\
            </div>\
        </div>\
    </nav>\
    <div class=\"container\">\
        <div class=\"row\">\
            <div class=\"col mt-3\">\
                <h2>选课状态查询</h2>\
            </div>\
        </div>";

string info_body_end ="</div>\
    <script src=\"https://cdn.jsdelivr.net/npm/jquery@3.5.1/dist/jquery.slim.min.js\" integrity=\"sha384-DfXdz2htPH0lsSSs5nCTpuj/zy4C+OGpamoFVy38MVBnE+IbbVYUew+OrCXaRkfj\" crossorigin=\"anonymous\"></script>\
    <script src=\"https://cdn.jsdelivr.net/npm/popper.js@1.16.0/dist/umd/popper.min.js\" integrity=\"sha384-Q6E9RHvbIyZFJoft+2mJbHaEWldlvI9IOYy5n3zV9zzTtmI3UksdQRVvoxMfooAo\" crossorigin=\"anonymous\"></script>\
    <script src=\"https://cdn.jsdelivr.net/npm/bootstrap@4.5.0/dist/js/bootstrap.min.js\" integrity=\"sha384-OgVRvuATP1z7JjHLkuOU7Xw704+h835Lr+6QL9UvYjZE3Ipu6Tp75j7Bh/kR0JKI\" crossorigin=\"anonymous\"></script>\
</body>\
</html>";

string index_body_start ="<html lang=\"zh-CN\">\
<head>\
    <meta charset=\"utf-8\">\
    <link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bootstrap@4.5.0/dist/css/bootstrap.min.css\" integrity=\"sha384-9aIt2nRpC12Uk9gS9baDl411NQApFmC26EwAOH8WgZl5MYYxFfc+NcPb1dKGj7Sk\" crossorigin=\"anonymous\">\
</head>\
<body>\
    <nav class=\"navbar navbar-expand-lg navbar-dark bg-dark\">\
        <div class=\"container\">\
            <a class=\"navbar-brand\" href=\"#\">自动选课系统</a>\
            <button class=\"navbar-toggler\" type=\"button\" data-toggle=\"collapse\" data-target=\"#navbarsExample05\" aria-controls=\"navbarsExample05\" aria-expanded=\"false\" aria-label=\"Toggle navigation\">\
                <span class=\"navbar-toggler-icon\"></span>\
            </button>\
            <div class=\"collapse navbar-collapse\" id=\"navbarsExample05\">\
            <ul class=\"navbar-nav mr-auto\">\
                <li class=\"nav-item active\">\
                    <a class=\"nav-link\" href=\"#\">选课</a>\
                </li>\
                <li class=\"nav-item\">\
                    <a class=\"nav-link\" href=\"/info.html\">状态查询</a>\
                </li>\
                <li class=\"nav-item\">\
                    <a class=\"nav-link\" href=\"/help.html\">使用帮助</a>\
                </li>\
            </ul>\
            <ul class=\"navbar-nav\">\
                <li class=\"nav-item float-right\">\
                    <a class=\"nav-link\" href=\"#\">Welcome!</a>\
                </li>\
            </ul>\
            </div>\
        </div>\
    </nav>\
    <div class=\"container\">\
        <div class=\"row\">\
            <div class=\"col mt-3\">\
                <h2>选课信息填写</h2>\
                <form name=\"queryForm\" action=\"index.html\" method=\"post\" enctype=\"multipart/form-data\">\
                    <div class=\"form-row\">\
                        <div id=\"div-from\" class=\"form-group col-md-6\">\
                            <label for=\"input-from\">口令</label>\
                            <input id=\"input-from\" class=\"form-control\" type=\"password\" name=\"password\">\
                        </div>\
                    </div>\
                    <div class=\"form-row\">\
                        <div id=\"div-from\" class=\"form-group col-md-6\">\
                            <label for=\"input-from\">Cookie sepuser</label>\
                            <input id=\"input-from\" class=\"form-control\" type=\"text\" name=\"cookie_sepuser\">\
                        </div>\
                    </div>\
                    <div class=\"form-row\">\
                        <div id=\"div-from\" class=\"form-group col-md-6\">\
                            <label for=\"input-from\">Cookie JSESSIONID</label>\
                            <input id=\"input-from\" class=\"form-control\" type=\"text\" name=\"cookie_jsessionid\">\
                        </div>\
                    </div>\
                    <div class=\"form-row\">\
                        <div id=\"div-priority\" class=\"form-group col-md-6\">\
                            <label for=\"input-priority\">开课学院</label>\
                            <select id=\"input-priority\" class=\"custom-select\" name=\"deptld\">\
                                <option value=\"\" selected>无</option>\
                                <option value=\"910\">数学学院</option>\
                                <option value=\"911\">物理学院</option>\
                                <option value=\"957\">天文学院</option>\
                                <option value=\"912\">化学学院</option>\
                                <option value=\"928\">材料学院</option>\
                                <option value=\"913\">生命学院</option>\
                                <option value=\"914\">地球学院</option>\
                                <option value=\"921\">资环学院</option>\
                                <option value=\"951\">计算机学院</option>\
                                <option value=\"952\">电子学院</option>\
                                <option value=\"958\">工程学院</option>\
                                <option value=\"917\">经管学院</option>\
                                <option value=\"945\">公共管理学院</option>\
                                <option value=\"927\">人文学院</option>\
                                <option value=\"964\">马克思主义学院</option>\
                                <option value=\"915\">外语系</option>\
                                <option value=\"954\">中丹学院</option>\
                                <option value=\"955\">国际学院</option>\
                                <option value=\"959\">存济医学院</option>\
                                <option value=\"946\">体育教研室</option>\
                                <option value=\"961\">微电子学院</option>\
                                <option value=\"962\">未来技术学院</option>\
                                <option value=\"963\">网络空间安全学院</option>\
                                <option value=\"968\">心理学系</option>\
                                <option value=\"969\">人工智能学院</option>\
                                <option value=\"970\">纳米科学与技术学院</option>\
                                <option value=\"971\">艺术中心</option>\
                                <option value=\"972\">光电学院</option>\
                                <option value=\"967\">创新创业学院</option>\
                                <option value=\"973\">核学院</option>\
                                <option value=\"974\">现代农业科学学院</option>\
                                <option value=\"975\">化学工程学院</option>\
                                <option value=\"976\">海洋学院</option>\
                                <option value=\"977\">航空宇航学院</option>\
                            </select>\
                        </div>\
                    </div>\
                    <div class=\"form-row\">\
                        <div id=\"div-from\" class=\"form-group col-md-4\">\
                            <label for=\"input-from\">课程编号</label>\
                            <input id=\"input-from\" class=\"form-control\" type=\"text\" name=\"course_id1\">\
                        </div>\
                        <div id=\"div-from\" class=\"form-group col-md-4\">\
                            <label for=\"input-from\">课程名称</label>\
                            <input id=\"input-from\" class=\"form-control\" type=\"text\" name=\"course_name1\">\
                        </div>\
                        <div id=\"div-priority\" class=\"form-group col-md-4\">\
                            <label for=\"input-priority\">次修/辅修</label>\
                            <select id=\"input-priority\" class=\"custom-select\" name=\"coursec1\">\
                                <option value=\"\" selected>否</option>\
                                <option value=\"1\">是</option>\
                            </select>\
                        </div>\
                    </div>\
                    <div class=\"form-row\">\
                        <div id=\"div-from\" class=\"form-group col-md-4\">\
                            <label for=\"input-from\">课程编号</label>\
                            <input id=\"input-from\" class=\"form-control\" type=\"text\" name=\"course_id2\">\
                        </div>\
                        <div id=\"div-from\" class=\"form-group col-md-4\">\
                            <label for=\"input-from\">课程名称</label>\
                            <input id=\"input-from\" class=\"form-control\" type=\"text\" name=\"course_name2\">\
                        </div>\
                        <div id=\"div-priority\" class=\"form-group col-md-4\">\
                            <label for=\"input-priority\">次修/辅修</label>\
                            <select id=\"input-priority\" class=\"custom-select\" name=\"coursec2\">\
                                <option value=\"\" selected>否</option>\
                                <option value=\"1\">是</option>\
                            </select>\
                        </div>\
                    </div>\
                    <div class=\"form-row\">\
                        <div id=\"div-from\" class=\"form-group col-md-4\">\
                            <label for=\"input-from\">课程编号</label>\
                            <input id=\"input-from\" class=\"form-control\" type=\"text\" name=\"course_id3\">\
                        </div>\
                        <div id=\"div-from\" class=\"form-group col-md-4\">\
                            <label for=\"input-from\">课程名称</label>\
                            <input id=\"input-from\" class=\"form-control\" type=\"text\" name=\"course_name3\">\
                        </div>\
                        <div id=\"div-priority\" class=\"form-group col-md-4\">\
                            <label for=\"input-priority\">次修/辅修</label>\
                            <select id=\"input-priority\" class=\"custom-select\" name=\"coursec3\">\
                                <option value=\"\" selected>否</option>\
                                <option value=\"1\">是</option>\
                            </select>\
                        </div>\
                    </div>\
                    <div class=\"form-row\">\
                        <div id=\"div-from\" class=\"form-group col-md-4\">\
                            <label for=\"input-from\">课程编号</label>\
                            <input id=\"input-from\" class=\"form-control\" type=\"text\" name=\"course_id4\">\
                        </div>\
                        <div id=\"div-from\" class=\"form-group col-md-4\">\
                            <label for=\"input-from\">课程名称</label>\
                            <input id=\"input-from\" class=\"form-control\" type=\"text\" name=\"course_name4\">\
                        </div>\
                        <div id=\"div-priority\" class=\"form-group col-md-4\">\
                            <label for=\"input-priority\">次修/辅修</label>\
                            <select id=\"input-priority\" class=\"custom-select\" name=\"coursec4\">\
                                <option value=\"\" selected>否</option>\
                                <option value=\"1\">是</option>\
                            </select>\
                        </div>\
                    </div>\
                    <div class=\"form-row\">\
                        <div id=\"div-from\" class=\"form-group col-md-4\">\
                            <label for=\"input-from\">课程编号</label>\
                            <input id=\"input-from\" class=\"form-control\" type=\"text\" name=\"course_id5\">\
                        </div>\
                        <div id=\"div-from\" class=\"form-group col-md-4\">\
                            <label for=\"input-from\">课程名称</label>\
                            <input id=\"input-from\" class=\"form-control\" type=\"text\" name=\"course_name5\">\
                        </div>\
                        <div id=\"div-priority\" class=\"form-group col-md-4\">\
                            <label for=\"input-priority\">次修/辅修</label>\
                            <select id=\"input-priority\" class=\"custom-select\" name=\"coursec5\">\
                                <option value=\"\" selected>否</option>\
                                <option value=\"1\">是</option>\
                            </select>\
                        </div>\
                    </div>\
                    <div class=\"form-row\">\
                        <div id=\"div-from\" class=\"form-group col-md-12\">\
                            <label for=\"input-from\">可选参数</label>\
                            <input id=\"input-from\" class=\"form-control\" type=\"text\" name=\"command\">\
                        </div>\
                    </div>\
                    <div class=\"text-center\">\
                        <button type=\"submit\" class=\"btn btn-primary\">提交</button>\
                    </div>";

string index_body_end ="</form>\
            </div>\
        </div>\
    </div>\
    <script src=\"https://cdn.jsdelivr.net/npm/jquery@3.5.1/dist/jquery.slim.min.js\" integrity=\"sha384-DfXdz2htPH0lsSSs5nCTpuj/zy4C+OGpamoFVy38MVBnE+IbbVYUew+OrCXaRkfj\" crossorigin=\"anonymous\"></script>\
    <script src=\"https://cdn.jsdelivr.net/npm/popper.js@1.16.0/dist/umd/popper.min.js\" integrity=\"sha384-Q6E9RHvbIyZFJoft+2mJbHaEWldlvI9IOYy5n3zV9zzTtmI3UksdQRVvoxMfooAo\" crossorigin=\"anonymous\"></script>\
    <script src=\"https://cdn.jsdelivr.net/npm/bootstrap@4.5.0/dist/js/bootstrap.min.js\" integrity=\"sha384-OgVRvuATP1z7JjHLkuOU7Xw704+h835Lr+6QL9UvYjZE3Ipu6Tp75j7Bh/kR0JKI\" crossorigin=\"anonymous\"></script>\
</body>\
</html>";

string help_begin ="<html lang=\"zh-CN\">\
<head>\
    <meta charset=\"utf-8\">\
    <link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bootstrap@4.5.0/dist/css/bootstrap.min.css\" integrity=\"sha384-9aIt2nRpC12Uk9gS9baDl411NQApFmC26EwAOH8WgZl5MYYxFfc+NcPb1dKGj7Sk\" crossorigin=\"anonymous\">\
</head>\
<body>\
    <nav class=\"navbar navbar-expand-lg navbar-dark bg-dark\">\
        <div class=\"container\">\
            <a class=\"navbar-brand\" href=\"/\">自动选课系统</a>\
            <button class=\"navbar-toggler\" type=\"button\" data-toggle=\"collapse\" data-target=\"#navbarsExample05\" aria-controls=\"navbarsExample05\" aria-expanded=\"false\" aria-label=\"Toggle navigation\">\
                <span class=\"navbar-toggler-icon\"></span>\
            </button>\
            <div class=\"collapse navbar-collapse\" id=\"navbarsExample05\">\
            <ul class=\"navbar-nav mr-auto\">\
                <li class=\"nav-item\">\
                    <a class=\"nav-link\" href=\"/index.html\">选课</a>\
                </li>\
                <li class=\"nav-item\">\
                    <a class=\"nav-link\" href=\"/info.html\">状态查询</a>\
                </li>\
                <li class=\"nav-item active\">\
                    <a class=\"nav-link\" href=\"#\">使用帮助</a>\
                </li>\
            </ul>\
            <ul class=\"navbar-nav\">\
                <li class=\"nav-item float-right\">\
                    <a class=\"nav-link\" href=\"#\">Welcome!</a>\
                </li>\
            </ul>\
            </div>\
        </div>\
    </nav>\
    <div class=\"container\">\
        <div class=\"row\">\
            <div class=\"col mt-3\">\
                <h2>使用帮助</h2>\
                <p>\
                    <span style=\"font-size:18px\">\
                    1、具备选课开启前等待抢课、选课开启后捡漏等功能。所有操作都需要输入正确的口令。\
                    </span>\
                </p>\
                <p>\
                    <span style=\"font-size:18px\">\
                    2、选课操作使用临时会话cookie登录。获取方式：以谷歌浏览器为例，首先打开sep的选课界面，按F12进入开发者工具，进入network选项卡，然后刷新网页，点击network列表中的最上面的一条记录，名字可能是main，右侧headers中找到request headers中的cookie，获取sepuser和jsessionid的值，双引号全部去掉，记录下来。\
                    </span>\
                </p>\
                <p>\
                    <span style=\"font-size:18px\">\
                    3、在选课界面输入口令、两条cookie、要选择的课程（学院必须输入，编号和名字不一定都要输入，只要保证其中之一是正确的即可，但不支持任何形式的缩写，名字的符号如括号等一般是中文符号），提交，看到提交成功的字样表明服务器已经接收到请求。\
                    </span>\
                </p>\
                <p>\
                    <span style=\"font-size:18px\">\
                    4、进入状态查询页面查看选课状态，包括选课中、选课成功、选课失败（包括登录失败、找不到课程、选课时间冲突或未知原因导致的选课失败）。（如果没有出现刚才的请求，请检查你的输入或者向我们反馈）\
                    </span>\
                </p>\
                <p>\
                    <span style=\"font-size:18px\">\
                    5、获取口令、源代码、更详细的说明以及联系方式请访问<a href=\"https://github.com/Deconsts/course-selection-helper-web\">github页面</a>。\
                    </span>\
                </p>\
            </div>\
        </div>\
    </div>\
    <script src=\"https://cdn.jsdelivr.net/npm/jquery@3.5.1/dist/jquery.slim.min.js\" integrity=\"sha384-DfXdz2htPH0lsSSs5nCTpuj/zy4C+OGpamoFVy38MVBnE+IbbVYUew+OrCXaRkfj\" crossorigin=\"anonymous\"></script>\
    <script src=\"https://cdn.jsdelivr.net/npm/popper.js@1.16.0/dist/umd/popper.min.js\" integrity=\"sha384-Q6E9RHvbIyZFJoft+2mJbHaEWldlvI9IOYy5n3zV9zzTtmI3UksdQRVvoxMfooAo\" crossorigin=\"anonymous\"></script>\
    <script src=\"https://cdn.jsdelivr.net/npm/bootstrap@4.5.0/dist/js/bootstrap.min.js\" integrity=\"sha384-OgVRvuATP1z7JjHLkuOU7Xw704+h835Lr+6QL9UvYjZE3Ipu6Tp75j7Bh/kR0JKI\" crossorigin=\"anonymous\"></script>\
</body>\
</html>";

void handler(int num) {   
    //接受到了SIGCHLD信号
    int status;
    int pid = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("The child %d exit with code %d\n", pid, WEXITSTATUS(status));
    }
}

string printtime()
{
    ifstream in;
    in.open("timestamp", ios::in);
	string st;
    int stampTime;
	if (in >> st){
        stampTime = stoi(st);
    }
    else{
        return "";
    }
    in.close();
	time_t tick = (time_t)stampTime;
	struct tm tm;
	char s[100];
	tm = *localtime(&tick);
	strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);
    string time = "<div class=\"row\">\
            <div class=\"col mt-3\">\
                <h3>\
                选课开始时间\
                </h3>\
                <h5>" + string(s) + "</h5>\
            </div>\
        </div>";
	return time;
}

string print_state_HTML(int i) {
	string HTML_string = "<div class=\"row\">\
            <div class=\"col mt-3\">\
                <h3>"
        + state[i] +
        "</h3>\
            <p>\
                <table class=\"table\">\
                    <thead>\
                        <tr><th scope=\"col\">用户</th>\
                        <th scope=\"col\">课程编号</th>\
                        <th scope=\"col\">课程名称</th>\
                        <th scope=\"col\">状态</th></tr>\
                    </thead>\
                    <tbody>";
	ifstream in;
	in.open(filename[i], ios::in);
	string key;
	while (in >> key) {
		HTML_string += "<tr><td scope=\"col\">";
		HTML_string += key;
        in >> key;
		HTML_string += "</td><td>";
		HTML_string += key;
        in >> key;
		HTML_string += "</td><td>";
		HTML_string += key;
        in >> key;
		HTML_string += "</td><td>";
		HTML_string += key;
		HTML_string += "</td></tr>";
	}
	HTML_string += "</tbody></table></p></div></div>";
    in.close();
	return HTML_string;
}

int total_length;
int pwd_valid;
int pwd_not_empty;
int pwd_e_valid;
int current_page;
string current_file;
string boundary;


void HttpProcess(string& req, string* resp)
{
    int n, m;
    int page_to_choose = 0;
    int req_length = req.size();
    string res = "";
    string txt = "";
    string pwd = "";
    string smode = "";
    // 计算长度

    if(total_length == 0){
        long_post = req;
    }
    else if(total_length > 0){
        total_length -= req_length;
        long_post += req;
    }

    // 长post请求处理
    if((n = req.find("Content-Length")) != string::npos){
        long_post = req;
        if((m = req.find("boundary=")) != string::npos){
            boundary="--";
            m+=9;
            for(;req.at(m)!='\r';m++){
                boundary.append(1,req.at(m));
            }
        }
        total_length=0;
        n+=16;
        for(;req.at(n)!='\r';n++){
            total_length*=10;
            total_length+=req.at(n)-'0';
        }
        cout<<"total_length:"<<total_length<<endl;
        if ((m = req.find(boundary,m)) != string::npos){
            cout<<"total_length:"<<m<<endl;
            total_length+=m;
            total_length-=req_length;
        }
        total_length--;
        cout<<"total_length:"<<total_length<<endl;
    }
        cout<<"total_length:"<<total_length<<endl;

    // 获取请求的页面信息
    if((n = req.find("GET")) == string::npos){
        if((n = req.find("POST")) != string::npos){
            n++;
        }
    };
    if(n != string::npos){
        n+=7;
    }
    if(n == string::npos && total_length<0){
        page_to_choose = current_page;
    }
    else if(n == string::npos){
        page_to_choose = 3;
    }
    else if (req.at(n)=='l'){
        page_to_choose = 2;
    }
    else if (req.at(n)=='f'){
        page_to_choose = 1;
    }
    else if (req.at(n)=='d' || req.at(n)=='T'){
        page_to_choose = 0;
    } 
    else{
        page_to_choose = 0;
    }
    cout<<"\npage_to_choose:"<<page_to_choose<<endl;
    if(page_to_choose!=3){
        current_page = page_to_choose;
    }
    if(total_length > 0){
        page_to_choose = 3;
    }
    cout<<"\nfinal_page:"<<page_to_choose<<endl;

    // post未结束
    if(total_length > 0){
        goto index_fail;
    }

    // post 分析作用域
    {
    // index页密码
    if((n = long_post.find("name=\"password\"")) != string::npos){
        pwd_not_empty=1;
        n+=19;
        cout<<"password:"<<endl;
        for(;long_post.at(n)!='\r';n++){
            pwd.append(1,long_post.at(n));
        }
        cout<<pwd.c_str()<<endl;
        if(pwd == UPLOAD_PWD){
            pwd_valid=1;
        }
        else{
            pwd_valid=0;
        }
    }

    // 调用算法实现具体功能
    // index页信息
    if(!pwd_valid){
        goto index_fail;
    }
    // index 作用域
    {
    string cookie_sepuser = "", cookie_jsessionid = "", deptld = "", course_id = "", course_name = "", coursec = "", command = "", timestamp = "", request_info = "";

    if((n = long_post.find("command")) != string::npos){
        n+=12;
        for(;long_post.at(n)!='\r';n++){
            command.append(1,long_post.at(n));
        }
        if (command.find("empty") != string::npos){
            for (int i = 0; i < 3; i++){
                ofstream outfile;
                outfile.open(filename[i], ios::out);
	            outfile << "";
                outfile.close();
            }
        }
        if (command.find("tp 0") != string::npos){
            page_to_choose = 0;
        }
        else if (command.find("tp 1") != string::npos){
            page_to_choose = 1;
        }
        else if (command.find("tp 2") != string::npos){
            page_to_choose = 2;
        }
        if ((n = command.find("ts")) != string::npos){
            n+=3;
            for(;n < command.size() && command.at(n)!=' ' && command.at(n) <= '9' && command.at(n) >= '0' ;n++){
                timestamp.append(1,command.at(n));
            }
            ofstream outfile;
            outfile.open("timestamp", ios::out);
	        outfile << timestamp;
            outfile.close();
        }
        if ((n = command.find("mode")) != string::npos){
            n+=5;
            for(;n < command.size() && command.at(n)!=' ' && command.at(n) <= '9' && command.at(n) >= '0' ;n++){
                smode.append(1,command.at(n));
            }
            //TODO
        }
    }
    if((n = long_post.find("cookie_sepuser")) != string::npos){
        n+=19;
        for(;long_post.at(n)!='\r';n++){
            cookie_sepuser.append(1,long_post.at(n));
        }
        if(!cookie_sepuser.empty()){
            if((n = long_post.find("cookie_jsessionid")) != string::npos){
                n+=22;
                for(;long_post.at(n)!='\r';n++){
                    cookie_jsessionid.append(1,long_post.at(n));
                }
                if(!cookie_jsessionid.empty()){
                    if((n = long_post.find("deptld")) != string::npos){
                        n+=11;
                        for(;long_post.at(n)!='\r';n++){
                            deptld.append(1,long_post.at(n));
                        }
                        if(!deptld.empty()){
                            request_info += cookie_sepuser + "\n" + cookie_jsessionid + "\n" + deptld + "\n" + smode + "\n";
                        }
                    }
                }
            }
        }
    }

    for(int i = 1; i < 6; i++){
        course_id = ""; course_name = ""; coursec = "";
        if((n = long_post.find("course_id" + number[i])) != string::npos){
            n+=15;
            for(;long_post.at(n)!='\r';n++){
                course_id.append(1,long_post.at(n));
            }
        }
        if((n = long_post.find("course_name" + number[i])) != string::npos){
            n+=17;
            for(;long_post.at(n)!='\r';n++){
                course_name.append(1,long_post.at(n));
            }
        }
        if((n = long_post.find("coursec" + number[i])) != string::npos){
            n+=13;
            for(;long_post.at(n)!='\r';n++){
                coursec.append(1,long_post.at(n));
            }
            if(!course_id.empty() || !course_name.empty()){
                request_info += coursec + "\n" + course_id + "\n" + course_name + "\n";
            }
        }
	}

    ofstream outfile;
	outfile.open(filename[3], ios::out);
	outfile << request_info.c_str();
    outfile.close();
    outfile.close();
    request_info = "";
    pid_t pid = fork();
    cout << "pid:" << pid << endl;
	if(pid == 0){
        execlp("python3","python3","course.py",0);
        exit (0);
    }

    }}
index_fail:
    string info_body_result ="";
    string index_body_check = "";

    // 读取文件
    txt = printtime() + print_state_HTML(0) + print_state_HTML(1) + print_state_HTML(2);
    info_body_result += txt;

    // 检查密码正确性的反馈
    if(pwd_not_empty){
        if(pwd_valid){
            index_body_check +="<div class=\"text-center\">\
                        <font color=\"green\">提交成功！</font>\
                    </div>";
        }
        else{
            index_body_check +="<div class=\"text-center\">\
                        <font color=\"red\">密码错误！</font>\
                    </div>";
        }
    }

    // password记录清空
    if(total_length<0){
        total_length=0;
        pwd_not_empty=0;
    }
    if(total_length<=0 && pwd_valid){
        pwd_valid=0;
    }

    string body;
    if(page_to_choose == 0){
        body = index_body_start + index_body_check + index_body_end;
    }
    else if(page_to_choose == 1){
        body = info_body_start + info_body_result + info_body_end;
    }
    else if(page_to_choose == 2){
        body = help_begin;
    }
    else if(page_to_choose == 3){
        body = "";
    }

    string header = "Content-Type: text/html\ncharset: gbk\nContent-Length:"
            +to_string(body.size())+"\n\n";
    if(page_to_choose == 3){
        *resp = "HTTP/1.1 100 Continue\n\n";
    }
    else{
        *resp = first_line + header + body;
    }

    cout<<"\nProcess done...\n";
}

int main()
{
    signal(SIGCHLD, handler);
    TcpPthreadServer server("0.0.0.0",9091);
    server.Start(HttpProcess);
    return 0;
}