import re
from optparse import OptionParser as OP

parser = OP()
parser.add_option("-p","--path", dest="path", help="path to directory")
parser.add_option("-n","--num-files",dest="num_files", help="number of files", type="int") 
parser.add_option("-f","--file-prefix",dest="prefix", help="prefix of dirs/files")

##
## Parse
##

(options,args) = parser.parse_args()

##
##
##

dict_of_words = {}
vocab_idx = 0

STOP_WORDS = ('the', 'a', 'an', 'i', 'we', 'they', 'you', 'he', 'she', 'it', \
              'his', 'her', 'their', 'its', 'your', 'my', 'from', 'of', 'to', \
              'and', 'however', 'but', 'into', 'for', 'after', 'before', 'in', \
              'out', 'is', 'am', 'are', 'was', 'were', 'that', 'this', 'these', \
              'those', 'by', 'on', 'with', 'as', 'not', 'over', 'at', 'have', \
              'has', 'had', 'been', 'then', 'when', 'what', 'who', 'do', 'did', \
              'do', 'did', 'him')

for i in range(options.num_files):
  f = open(options.path + options.prefix + "_raw/" +
    options.prefix + str(i) + ".txt","r")
  g = open(options.path + options.prefix + "_proc/" + 
    options.prefix + str(i) + ".txt","w")
  for line in f:
    for word in line.split():
      word = word.strip(',.').lower()
      match = re.match("^[a-z\-]*$", word)
      if word in STOP_WORDS or not match or len(word) < 2:
        continue
      try:
        dict_of_words[word]
      except:
        dict_of_words[word] = vocab_idx
        vocab_idx += 1
      g.write(" " + str(dict_of_words[word]))
  g.close()
  f.close()

g = open(options.path + options.prefix + "_proc/vocab.txt",'w')
for key in dict_of_words.keys():
  g.write(str(dict_of_words[key]) + " " + key + "\n")
g.close()

