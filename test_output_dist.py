
from optparse import OptionParser as OP

parser = OP()
#parser.add_option("-f","--filename",dest="filename", help="filename")
parser.add_option("-f","--file-prefix",dest="prefix", help="prefix of dirs/files")
parser.add_option("-p","--path", dest="path", help="path to directory")

##
## Parse
##

(options,args) = parser.parse_args()

##
##
##

vocab = {}
v = open(options.path + options.prefix + '_proc/vocab.txt', 'r')
for line in v:
  line = line.strip().split()
  vocab[line[0]] = line[1]
v.close()

f = open(options.path + options.prefix + '_proc/' + options.prefix + 'topic_dist_0.csv', 'r')
topics = {}
sorted_topics = {}
i = 0

for line in f:
  line = line.strip(',\n').split(',')
  topics[i] = []
  for wid in line:
    topics[i].append(int(wid))
  sorted_topics[i] = sorted(topics[i], reverse=True)
  i += 1

f.close()

g = open(options.path + options.prefix + '_proc/hot_words_for_topics.csv', 'w')
for key in sorted(topics.keys()):
  j = 0
  jj = 0
  while j < 20:
    wid = sorted_topics[key][j]
    indices = [i for i, x in enumerate(topics[key]) if x == wid]
    j += len(indices)
    for index in indices:
      if jj >= 20:
        break
      g.write(vocab[str(index)] + ',')
      jj += 1
  g.write('\n')

g.close()

