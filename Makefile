OBJS = document.o lda.o ldaDriver.o
BOOST = -I ~/boost/boost_1_57_0
OMP = -fopenmp
GPP = g++

default: sampler

fifa_corpus:
	python make_clean_corpus.py -p data/fifa/ -f fifa -n 60

test_corpus:
	python make_clean_corpus.py -p data/test/ -f test -n 30

test_output:
	python test_output_dist.py -f testoutput_0.csv

document: document.cpp
	$(GPP) $(BOOST) $(OMP) -c -o document.o document.cpp

lda: lda.cpp
	$(GPP) $(BOOST) $(OMP) -c -o lda.o lda.cpp

ldaDriver: ldaDriver.cpp
	$(GPP) $(BOOST) $(OMP) -c -o ldaDriver.o ldaDriver.cpp

sampler: document lda ldaDriver
	$(GPP) $(BOOST) $(OMP) -o $@ $(OBJS)

clean:
	rm sampler
	rm *.csv
	rm *.o


