# -*- coding:utf-8 -*-  

import string, urllib2, re, csv
review = re.compile(r'<div class="row review_table_row">.*?div.*?div.*?/div.*?div.*?/div.*?div.*?/div.*?/div.*?div.*?div.*?div.*?/div.*?/div.*?/div.*?/div>')
findname=re.compile(r'style="word-wrap:break-word">(.*?)</span>')
findtime=re.compile(r'class="fr small subtle">(.*?20.*?)</span>')
findreview=re.compile(r'<div class="scoreWrapper">.*?</div>(.*?)</div>')
findstar=re.compile(r'<span class="glyphicon glyphicon-star">')
findsuprev=re.compile(r'Super Reviewer')
findhalfstar=re.compile(r'&frac12')
#replace=re.compile(r'<span class="glyphicon glyphicon-star">')
#replaceone=re.compile(r'<.*?>')
replace=re.compile(r'&#39;')
class Spider:
    # 申明相关的属性
    def __init__(self,url1,url2):  
        self.urlone=url1
        self.urltwo=url2
        self.datas = []
        self.result =[]
        print u'已经初始化'


    def begin(self):
        out = open('result.csv','a+')
        for i in range(1,52):
            url=url1+str(i)+url2
            mypage = urllib2.urlopen(url).read()
           
            self.datas=review.findall(mypage)
            for data in self.datas:
            #   m=re.match(r'<div class="row review_table_row">.*?div.*?div.*?div.*?/div.*?div.*?/div.*?div(.*?)/div.*?/div.*?div(.*?)div.*?div.*?/div(.*?)/div.*?/div.*?/div>',data)
                
                print data
                tem=findname.findall(data)
                self.result=self.result+tem
                tem=findtime.findall(data)
                self.result=self.result+tem

                temano=findstar.findall(data)
                print len(temano)
                num=2*len(temano)
                temanoano=findsuprev.findall(data)
                print len(temanoano)
                num=num-len(temanoano)
                temanoanoano=findhalfstar.findall(data)
                print len(temanoanoano)
                num=num+len(temanoanoano)
                self.result.append(num)

                tem=findreview.findall(data)
                self.result=self.result+tem
                print self.result
                #csv 写入
                
                #打开文件，追加a
                
                #设定写入模式
                csv_write = csv.writer(out,dialect='excel')
                #写入具体内容
                csv_write.writerow(self.result)

                print ("write over")

                self.result =[]   
             #  self.result.append(tem1.group(1))
                
             #  tem2=re.match(r'>(.*?20.*?)</span>',data)
             #  self.result.append(tem2.group(1))
             #  tem3=re.match(r'<div class="scoreWrapper">.*?</div>(.*?)</div>',data)
             #  self.result.append(tem3.group(1))
             #  self.result
             
            print str(i)
        out.close()   
        
        #<span class="glyphicon glyphicon-star"></span>
        #Super Reviewer
            

print u'已开始运行'
url1 = 'https://www.rottentomatoes.com/m/a_star_is_born_2018/reviews/?page='
url2 = '&type=user&sort='

#调用
mySpider = Spider(url1,url2)
mySpider.begin()