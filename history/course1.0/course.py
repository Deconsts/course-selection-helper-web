import requests,re

# write to file
def write(filename, content):
    with open(filename, 'w', encoding='utf-8') as f:
        f.write(content)

# read from file
def read(filename):
    with open(filename, 'r', encoding='utf-8') as f:
        content = f.read()
    return content

# read lines from file
def readlines(filename):
    with open(filename, 'r', encoding='utf-8') as f:
        content = f.readlines()
    return content

# get course page
def course_main(headers):
    url = "http://jwxk.ucas.ac.cn/courseManageBachelor/main"
    return requests.get(url, headers = headers).text

# find course sid
def find_course(text, course_id, course_name):
    if course_id:
        sid = re.findall('<span id="courseCode_(.*)">'+ course_id +'</span>', text)
        if sid:
            return sid[0]
    if course_name:
        course_index = re.findall('/course/coursetime/(.*)" target="_blank">'+ course_name, text)
        if course_index:
            sid = re.findall(course_index[0] + '" target="_blank"><span id="courseCode_(.*)"', text)
            if sid:
                return sid[0]
    return "0"

# get course sid
def course_info(header, course_id, course_name, deptlds):
    url = "http://jwxk.ucas.ac.cn/courseManageBachelor/selectCourse"
    data = {
        "deptlds": deptlds,
        "sb": "0"
    }
    response = requests.post(url, headers = header, data = data).text
    return find_course(response, course_id, course_name)
    
# post for course
def course_post(header, deptlds, sids):
    url = "http://jwxk.ucas.ac.cn/courseManageBachelor/saveCourse"
    data = {
        "deptlds": deptlds,
        "sids": sids
    }
    requests.post(url, headers = header, data = data)

# check state
def check_course(headers, course_id, course_name):
    text = course_main(headers)
    if re.findall(course_id, text):
        return "选课成功"
    if re.findall(course_name, text):
        return "选课成功"
    return "选课失败"

# entry of course selection
def select_course(header, course_id, course_name, deptld):
    sid = course_info(header, course_id, course_name, deptld)
    if sid == '0':
        return "找不到课程"
    course_post(header, deptld, sid)
    return check_course(header, course_id, course_name)

# entry
def entry():
    courses = readlines("queue")
    for i in range(len(courses)):
        courses[i] = courses[i][: -1]
    cookie_sepuser = courses[0]
    cookie_jsessionid = courses[1]
    header = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.116 Safari/537.36",
        "Cookie": "sepuser=\""+cookie_sepuser+"\"; JSESSIONID="+cookie_jsessionid
    }
    print("load file successfully")

    result = "选课中"
    content = read("processing")
    for i in range(2, len(courses), 3):
        if courses[i + 1] or courses[i + 2]:
            content = cookie_sepuser + "\t" + courses[i + 1] + "\t" + courses[i + 2] + "\t" + result + "\n" + content
    write("processing", content)
    print("start processing...")

    for i in range(2, len(courses), 3):
        if courses[i + 1] or courses[i + 2]:
            result = select_course(header, courses[i + 1], courses[i + 2], courses[i])
            print(courses[i + 1] + courses[i + 2] + result)
            if result == "选课成功":
                content = read("success")
                content = cookie_sepuser + "\t" + courses[i + 1] + "\t" + courses[i + 2] + "\t" + result + "\n" + content
                write("success", content)
            else:
                content = read("fail")
                content = cookie_sepuser + "\t" + courses[i + 1] + "\t" + courses[i + 2] + "\t" + result + "\n" + content
                write("fail", content)
            content = readlines("processing")
            result = "选课中"
            index = content.index(cookie_sepuser + "\t" + courses[i + 1] + "\t" + courses[i + 2] + "\t" + result + '\n')
            content = content[: index] + content[index + 1:]
            write("processing", ''.join(content))
    print("done!")

if __name__ == "__main__":
    #header = {
    #    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.116 Safari/537.36",
    #    "Cookie": "sepuser=\"bWlkPTBhYmI5YmVhLTdmZTctNDNjOC05MzdhLTg2ZjVlYTYxYjdlMg==  \"; JSESSIONID=73BB211F883B16EE534C0524C5CB1F8C"
    #}
    #print(check_course(header, "B0912019Y", "人机交互"))
    entry()