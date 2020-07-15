# course-selection-helper-web
Designed for UCAS course selection system. Visited by Internet explorer.  UCAS抢课工具

## 访问方式
http://114.116.225.190:9091/

## 代码运行环境
Linux 系统  
C++ 11 标准  
Python 3.7

## 使用说明
1、口令是123456。  
2、首先打开浏览器，登录sep账号，进入sep的选课界面，打开开发者工具获取cookie。以谷歌浏览器为例，按F12进入开发者工具，进入network选项卡，然后刷新网页，点击network列表中的最上面的一条记录，名字可能是main，右侧headers中找到request headers中的cookie，获取sepuser和jsessionid的值，双引号全部去掉，记录下来。  
3、在选课界面输入口令、两条cookie、要选择的课程（学院必须输入，编号和名字不一定都要输入，只要保证其中之一是正确的即可，但不支持任何形式的缩写，名字的符号如括号等一般是中文符号），提交，看到提交成功的字样表明服务器已经接收到请求。  
4、进入状态查询页面查看选课状态，包括选课中、选课成功、选课失败（包括登录失败、找不到课程、选课时间冲突或未知原因导致的选课失败）。如果没有出现刚才的请求，检查你的输入。  
5、支持四条可选指令。包括empty（清空所有记录）、tp x（x为0或1或2，表明跳转页面）、ts x（x是时间戳，设置选课开始的时间）、mode x（x是整数，只能与选课请求一同提交，当次有效，在人数已满等特殊情况下循环选课，每秒选课一次，循环x次，上限为6000次），以空格分隔不同的操作数或指令。

## 更多的说明
1、使用在北京的服务器，虽然网络带宽不高，但更加稳定。  
2、可以与本地浏览器同时访问选课网站，不会互相占用。  
3、直接发送http请求，而不是用浏览器模拟，效率更高。  
4、使用临时会话的cookie进行登录，主动退出登录或者过一小段时间就会失效，并且本程序不会保留这些信息，可以放心使用。  

## 注意事项
1、所有除访问以外的操作都需要输入正确密码。  
2、每一次请求都会fork一个子进程，在这个进程中，所填写的若干次选课是顺序执行的，所以一次至多提交五个选课请求避免后面的课选不上。可以尝试将一次请求拆分成多次，那么选课过程将会并行的。  
3、可以设置选课开始的时间，但是建议比通知的时间略早一点，避免学校系统的时间与本地的时间差。在这个时间之前或者系统未开启选课时，选课状态始终保持为选课中。选课中状态至多保持十分钟，在选课前十分钟以上提交的请求很可能会选课失败。  

## 版本更新记录
v1.0 支持课程开放状态的抢课 2020.7.1  
v1.1 完善了编号和名称空缺的文件读写，增加了使用说明页面，增加了未开启选课时的逻辑，增加了登录失败的错误提示，增加了可选参数，支持清空记录、页面跳转、设置选课开始时间 2020.7.4  
v1.2 完善了使用说明页面，增加了选课开始时间的显示，修复了部分非法指令导致的异常退出  2020.7.7  
v1.3 修复了未开始选课时直接显示登录失败的问题，增加了辅修和次修的选课，一次提交只接受同一学院的请求，提高了运行效率，优化了网页长post请求的逻辑 2020.7.13  
v1.4 修复了僵尸进程未被回收的问题，增加了选课脚本十分钟超时退出的机制，增加了循环选课指令和功能，优化了html生成的逻辑 2020.7.15

## 联系方式
deconst720@gmail.com
