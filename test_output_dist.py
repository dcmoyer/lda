
from optparse import OptionParser as OP

parser = OP()
parser.add_option("-f","--filename",dest="filename", help="filename")

##
## Parse
##

(options,args) = parser.parse_args()

##
##
##

vocab = {}
v = open('data/fifa/fifa_proc/vocab.txt', 'r')
for line in v:
  line = line.strip().split()
  vocab[line[0]] = line[1]
v.close()

f = open(options.filename, 'r')
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

g = open('hot_words_for_topics.csv', 'w')
for key in sorted(topics.keys()):
  j = 0
  while j < 20:
    wid = sorted_topics[key][j]
    indices = [i for i, x in enumerate(topics[key]) if x == wid]
    j += len(indices)
    for index in indices:
      g.write(vocab[str(index)] + ',')
  g.write('\n')

g.close()

