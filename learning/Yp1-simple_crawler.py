#coding:utf-8
import urllib

# 实现了爬的单个页面的title的url并存入文本
def GetContent(url):
    page = urllib.urlopen(url);
    html = page.read();

    str = '<article class="article-content">';
    content = html.partition(str)[2];
    str1 = '<div class="article-social">';
    content = content.partition(str1)[0];
    return content;
    
def GetTitle(content, beg = 0):
    try:
        list = [];
        while True:
            num1 = content.index('】', beg) + 3;
            num2 = content.index('</p>', num1);
            list.append(content[num1 : num2]);
            beg = num2;
    except ValueError:
         return list;
         
def Save(title):
    with open("data.txt", "w") as fo:
        for size in range(0, len(title)):
            fo.write(title[size] +'\n');
                   
if __name__ == "__main__":
    content = GetContent("http://bohaishibei.com/post/10475/");
    title = GetTitle(content);
    Save(title);
