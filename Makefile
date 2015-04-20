
fifa_corpus:
	python make_clean_corpus.py -p data/fifa/ -f fifa -n 60

document: document.cpp
	g++ -c -o document.o document.cpp

lda: lda.cpp
	g++ -c -o lda.o lda.cpp

clean:
	rm *.o
	rm *.exe


