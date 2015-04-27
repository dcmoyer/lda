OBJS = document.o lda.o ldaDriver.o
BOOST = -I ~/boost/boost_1_57_0

default: sampler

fifa_corpus:
	python make_clean_corpus.py -p data/fifa/ -f fifa -n 60

document: document.cpp
	g++ $(BOOST) -c -o document.o document.cpp

lda: lda.cpp
	g++ $(BOOST) -c -o lda.o lda.cpp

ldaDriver: ldaDriver.cpp
	g++ $(BOOST)  -c -o ldaDriver.o ldaDriver.cpp

sampler: document lda ldaDriver
	g++ $(BOOST) -o $@ $(OBJS)

clean:
	rm sampler
	rm *.csv
	rm *.o


