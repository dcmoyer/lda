
import sys, urllib2, re, string, time, threading, string

def remove_braces( target ):
  pos = 0
  braket = 0
  while(pos < len(target) - 1):
    if(target[pos] == "{" and target[pos+1] == "{"):
      braket += 1
      pos2 = pos + 2
      while(pos2 < len(target) - 1):
        if(target[pos2] == "{" and target[pos2+1] == "{"):
          braket += 1
          pos2 += 1
        elif(target[pos2] == "}" and target[pos2+1] == "}"):
          braket -= 1
          pos2 += 1
        pos2 += 1
        if(braket == 0):
          break
      if(braket > 0):
        raise Exception("Unmatched Braket")
      target = target[:pos] + target[pos2:]
    else:
      pos += 1
  return target

def remove_brakets_file( target ):
  pos = 0
  braket = 0
  while(pos < len(target) - 6):
    if(target[pos] == "[" and target[pos+1] == "[" and 
      target[(pos+2):(pos+6)] == "File"):
      braket += 1
      pos2 = pos + 2
      while(pos2 < len(target) - 1):
        if(target[pos2] == "[" and target[pos2+1] == "["):
          braket += 1
          pos2 += 1
        elif(target[pos2] == "]" and target[pos2+1] == "]"):
          braket -= 1
          pos2 += 1
        pos2 += 1
        if(braket == 0):
          break
      if(braket > 0):
        raise Exception("Unmatched Braket")
      target = target[:pos] + target[pos2:]
    else:
      pos += 1
  return target


def replace_links_with_text( target ):
  pos = 0
  while(pos < len(target) - 1):
    if(target[pos] == "[" and target[pos+1] == "["):
      pos2 = pos + 2
      pos3 = pos + 2
      while( pos2 < len(target) - 1):
        text = False
        if(target[pos2] == "]" and target[pos2+1] == "]"):
          target = target[:pos] + target[pos3:(pos2)] + target[(pos2+2):]
          break
        elif(target[pos2] == "|"):
          pos3 = pos2 + 1
        pos2 += 1
    else:
      pos += 1
  return target

def get_wikipedia_article( article_title ):
  """
  Downloads a particular Wikipedia article
  and strips out (most of) the formatting, links, etc. 

  Taken from Matt Hoffman's Random Wikipedia functions
  """
  failed = True
  while failed:
    failed = False
    try:
      req = urllib2.Request('http://en.wikipedia.org/w/index.php?title=Special:Export/%s&action=submit' \
                    % (article_title),
                  None, { 'User-Agent' : 'x'})
      f = urllib2.urlopen(req)
      all = f.read()
    except (urllib2.HTTPError, urllib2.URLError):
      print 'oops. there was a failure downloading %s. retrying...' \
        % article_title
      failed = True
      continue
    print 'downloaded %s. parsing...' % article_title
    
    try:
      all = re.search(r'<text.*?>(.*)</text', all, flags=re.DOTALL).group(1)
      all = remove_braces( all )
      #remove references
      all = re.sub(r'&lt;','<',all)
      all = re.sub(r'&gt;','>',all)
      all = re.sub(r'<ref>([\s\S]*?)(</ref>?)', '', all)
      all = re.sub(r'<ref name=(.*?)/>', '', all)
      all = re.sub(r'<ref name=(.*?)>([\s\S]*?)</ref>', '', all)
      #remove comments
      all = re.sub(r'<!--(.*?)-->', '', all)
      #formatting and double quotes
      
      all = re.sub(r'\n', ' ', all)
      all = re.sub(r"''", '', all)
      #replace &amp;nbsp; with a space
      all = re.sub(r'&amp;nbsp;', ' ', all)
      #all = re.sub(r'\{\{.*?\}\}', r'', all)
      all = re.sub(r'\[\[Category:.*', '', all)
      all = re.sub(r'==\s*[Ss]ource\s*==.*', '', all)
      all = re.sub(r'==\s*[Rr]eferences\s*==.*', '', all)
      all = re.sub(r'==\s*[Ee]xternal [Ll]inks\s*==.*', '', all)
      all = re.sub(r'==\s*[Ee]xternal [Ll]inks and [Rr]eferences==\s*', '', all)
      all = re.sub(r'==\s*[Ss]ee [Aa]lso\s*==.*', '', all)
      all = re.sub(r'http://[^\s]*', '', all)
      all = re.sub(r'\[\[Image:.*?\]\]', '', all)
      all = re.sub(r'Image:.*?\|', '', all)
      all = re.sub(r'&quot;', '', all)
      all = remove_brakets_file(all)
      #all = re.sub(r'\[\[File:.*?\]\]', '', all)
      all = replace_links_with_text(all)
      all = re.sub(r'={2,}','',all)
      all = re.sub(r"'",'',all)
      #all = re.sub(r'\[\[.*?\|*([^\|]*?)\]\]', r'\1', all)
      #all = re.sub(r'\&lt;.*?&gt;', '', all)
      all = filter(lambda x: x in string.printable, all)
    except:
      # Something went wrong, try again. (This is bad coding practice.)
      print 'oops. there was a failure parsing %s. retrying...' \
        % article_title
      failed = True
      continue
    print(all)
  return(all)

if __name__ == "__main__":
  #print(remove_braces("{{hello}}, what is {up daw{{g}}} {{ asdf }} "))
  get_wikipedia_article("Dog")

