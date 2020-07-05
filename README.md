# course-selection-helper-web
Designed for UCAS course selection system. Visited by Internet explorer.  UCAS抢课工具

## 访问方式
http://114.116.225.190:9091/

## 使用说明
1、口令是123456。  
2、首先打开浏览器，登录sep账号，进入sep的选课界面，打开开发者工具获取cookie。以谷歌浏览器为例，按F12进入开发者工具，进入sourse选项卡，然后刷新网页，点击network中的最上面的一条记录，名字可能是main，右侧headers中找到request headers中的cookie，获取sepuser和jsessionid的值，双引号全部去掉，记录下来。  
3、在选课界面输入口令、两条cookie、要选择的课程（学院必须输入，编号和名字不一定都要输入，只要保证其中之一是正确的即可，但不支持缩写），提交，看到提交成功的字样表明服务器已经接收到请求。  
4、进入状态查询页面查看选课状态。（有可能没有出现刚才的请求，这是由于输入有误或者后台的未知bug，可以记录下来并反馈）  
5、支持的管理员指令包括empty（清空）、tp x（x为012，表明跳转页面）、ts x（x是整数，设置时间戳），以空格分隔不同的指令。

## 更多的说明
1、使用在北京的服务器，虽然网络带宽不高，但更加稳定。  
2、可以与本地浏览器同时访问选课网站，不会互相占用。  
2、直接发送http请求，而不是用浏览器模拟，效率更高，整个选课过程不超过一秒。  
3、使用临时会话的cookie进行登录，并且本程序不会保留这些信息，可以放心使用。  

## 版本更新记录
v1.0 支持课程开放状态的抢课 2020.7.1  
v1.1 完善了编号和名称空缺的文件读写，增加了使用说明页面，增加了未开启选课时的逻辑，增加了登录失败的错误提示，增加了可选参数，支持清空记录、页面跳转、设置时间戳 2020.7.4

## TODO
1、fork进程回收、python程序超时退出  
2、完善使用说明页面  
3、一些报错提示（如管理员指令）  
4、更多的测试，提升鲁棒性

## 联系方式
deconst720@gmail.com
