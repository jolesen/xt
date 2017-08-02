import re
import urllib

def GetPageHtml(url):
    page = urllib.urlopen(url);
    html = page.read();
    return html;

def GetImage(html):
    reg = r'src="(.+?\.jpg)" pic_ext';
    listImage = re.findall(re.compile(reg), html);
    x = 0;
    for image in listImage:
        print "GET " + image;
        urllib.urlretrieve(image, '%s.jpg' % x)
        x += 1;

if __name__ == "__main__":
    html = GetPageHtml("http://tieba.baidu.com/p/2460150866");
    GetImage(html);
