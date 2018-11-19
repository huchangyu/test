# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
from matplotlib.font_manager import FontProperties
from matplotlib.pyplot import plot,savefig
 

#plt.bar([0,1,2,3,4,5,6,7,8,9,10], [40,61,63,27,73,34,88,97,149,140,237],label='count')
plt.bar([0,1,2,3,4], [4,13,29,43,16],label='count',facecolor='g')
# facecolor='g'

# x: 条形图x轴
# y：条形图的高度
# width：条形图的宽度 默认是0.8
# bottom：条形底部的y坐标值 默认是0
# align：center / edge 条形图是否以x轴坐标为中心点或者是以x轴坐标为边缘

plt.legend()

plt.xlabel('value')
plt.ylabel('count')

plt.title(u'score=9')

savefig("score9.pdf")


# 作图