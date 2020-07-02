# course-selection-helper-web
Designed for UCAS course selection system. Visited by Internet explorer.  UCAS抢课工具

## 访问方式
http://114.116.225.190:9091/

## 使用说明
1、口令是123456  
2、首先打开浏览器，登录sep账号，进入sep的选课界面，打开开发者工具获取cookie。以谷歌浏览器为例，按F12进入开发者工具，进入sourse选项卡，然后刷新网页，点击network中的最上面的一条记录，名字可能是main，右侧headers中找到request headers中的cookie，获取sepuser和jsessionid的值，双引号全部去掉，记录下来。  
3、在选课界面输入口令、两条cookie、要选择的课程（学院必须输入，编号和名字不一定都要输入，只要保证其中之一是正确的即可，但不支持缩写），提交，看到提交成功的字样表明服务器已经接收到请求。  
4、进入状态查询页面查看选课状态。（有可能没有出现刚才的请求，这是由于输入有误或者后台的未知bug，可以记录下来并反馈）

## 版本更新记录
v1.0 支持课程开放状态的抢课 2020.7.1

## TODO
1、编号和名称没有时用“空”代替  
2、使用说明页面  
3、未开启选课时的逻辑  
4、可选参数（清空记录）

## 联系方式
deconst720@gmail.com
