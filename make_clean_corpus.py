
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

for i in range(options.num_files):
  f = open(options.path + options.prefix + "_raw/" +
    options.prefix + str(i) + ".txt","r")
  g = open(options.path + options.prefix + "_proc/" + 
    options.prefix + str(i) + ".txt","w")
  for line in f:
    for word in line.split():
      word = word.strip(',.').lower()
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

