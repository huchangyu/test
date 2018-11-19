# -*- coding:utf-8 -*-  

import string, urllib2, re, csv
review = re.compile(r'<div class="row review_table_row">.*?div.*?div.*?/div.*?div.*?/div.*?div.*?/div.*?/div.*?div.*?div.*?div.*?/div.*?/div.*?/div>')
replace=re.compile(r'<span class="glyphicon glyphicon-star">')
replaceone=re.compile(r'<.*?>')
replacetwo=re.compile(r'&#39;')
class Spider:
    # 申明相关的属性
    def __init__(self,url1,url2):  
        self.urlone=url1
        self.urltwo=url2
        self.datas = []
        print u'已经初始化'


    def begin(self):
        f = open('result.txt','a+')
        for i in range(1, ):
            url=url1+str(i)+url2
            mypage = urllib2.urlopen(url).read()
            print mypage
            self.datas=review.findall(mypage)
            for data in self.datas:
                data=re.sub(replace,'1',data)
                data=re.sub(replaceano,'',data) 
                f.writelines(data)
                f.writelines('\n')
            print str(i)
        
        f.close()
            

print u'已开始运行'
url1 = 'https://www.rottentomatoes.com/m/a_star_is_born_2018/reviews/?page='
url2 = '&type=user&sort='

#调用
mySpider = Spider(url1,url2)
mySpider.begin()