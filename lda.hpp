
#ifndef LDA_CLASS_H
#define LDA_CLASS_H

#include "include.hpp"
#include "document.hpp"

class LDA{
private:
  std::vector<std::string> filenames;
  std::string vocab_path;
  int K;
  int V;
  double alpha;
  double beta;

  boost::numeric::ublas::matrix<int>* topic_x_words;
  boost::numeric::ublas::matrix<int>* total_words_in_topics;
public:
  LDA();
  LDA(std::vector<std::string>& list_of_filenames, std::string _path, int _K
    double _alpha, double _beta){
    for(int i = 0; i < list_of_filenames.size(); ++i){
      filenames.push_back(list_of_filenames[i]);
    }
    vocab_path = _path;
    K = _K;
    alpha = _alpha;
    beta = _beta;

    int word;
    std::string true_word;
    std::ifstream s((vocab_path).c_str(), std::ifstream::in);
    V = 0;

    while(!s.eof()){
      s >> true_word >> word;
      V++;
    }

    topic_x_words = new boost::numeric::ublas::matrix<int>(K,V);
    total_words_in_topics = new boost::numeric::ublas::matrix<int>(K,1);
  }

  ~LDA(){
    delete topic_x_words;
    delete total_words_in_topics;
  }

  void initialize();
  void run_iterations(int num_iterations);

  void print_topic_dist(std::string topic_file_name);
  void print_doc_dist(int index);
  void load_topic_dist(std::string topic_file_name);

};

#endif

