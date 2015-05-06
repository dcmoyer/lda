OBJS = document.o lda.o ldaDriver.o
BOOST = -I ~/boost/boost_1_57_0
OMP = -fopenmp
OPTIONS = -g
GPP = g++
CXX = mpicxx

default: sampler

fifa_corpus:
	python make_clean_corpus.py -p data/fifa/ -f fifa -n 60 

test_corpus:
	python make_clean_corpus.py -p data/test/ -f test -n 30

wiki_corpus:
	python make_clean_corpus.py -p data/wiki/ -f wiki -n 90

test_output_fifa:
	python test_output_dist.py -p data/fifa/ -f fifa

test_output_test:
	python test_output_dist.py -p data/test/ -f test

document: document.cpp
	$(CXX) $(BOOST) $(OMP) $(OPTIONS) -c -o document.o document.cpp

lda: lda.cpp
	$(CXX) $(BOOST) $(OMP) $(OPTIONS) -c -o lda.o lda.cpp

ldaDriver: ldaDriver.cpp
	$(CXX) $(BOOST) $(OMP) $(OPTIONS) -c -o ldaDriver.o ldaDriver.cpp

sampler: document lda ldaDriver
	$(CXX) $(BOOST) $(OMP) -o $@ $(OBJS)

clean:
	rm sampler
	rm *.o


