
#ifndef LDA_CLASS_H
#define LDA_CLASS_H

#include "include.hpp"
#include "document.hpp"

class LDA{
private:
  std::vector<std::string> filenames;
  std::string vocab_path;
  std::string prefix;
  int K;
  int V;
  int burnin;
  int thinning;
  double alpha;
  double beta;
  boost::numeric::ublas::matrix<int>* topic_x_words;
  boost::numeric::ublas::matrix<int>* total_words_in_topics;

public:
  // have to make them public and static since OMP does not allow class member to be shared among threads 
  // looks like OMP will make variables to be shared by default if not specified explicitly
  // so we may not need the following to be static
  // leave it here in case we need to re-enable them 
  //static boost::numeric::ublas::matrix<int>* topic_x_words;
  //static boost::numeric::ublas::matrix<int>* total_words_in_topics;
  LDA();
  LDA(std::vector<std::string>& list_of_filenames, std::string _prefix,
    std::string _path, int _K,
    double _alpha, double _beta, int _burnin, int _thinning){
    for(int i = 0; i < list_of_filenames.size(); ++i){
      filenames.push_back(list_of_filenames[i]);
    }

    prefix = _prefix;
    vocab_path = _path;
    K = _K;
    alpha = _alpha;
    beta = _beta;
    burnin = _burnin;
    thinning = _thinning;

    int word;
    std::string true_word;
    char c;
    std::ifstream s((vocab_path).c_str(), std::ifstream::in);
    V = 0;

    while(!s.eof()){
      s >> word >> true_word;
      s.get(c);
      V++;
    }
    std::cout << "all but mats\n";
    topic_x_words = new boost::numeric::ublas::matrix<int>(K,V);
    total_words_in_topics = new boost::numeric::ublas::matrix<int>(K,1);
    std::cout << "mats\n";
  }

  ~LDA(){
    delete topic_x_words;
    delete total_words_in_topics;
  }

  void initialize();
  void run_iterations(int num_iterations);

  void print_topic_dist(std::string topic_file_name);
  void print_topic_dist_idx(std::string topic_file_name, int index);
  void print_doc_dist(int index);
  void load_topic_dist(std::string topic_file_name);
  void print_neg_log_likelihood(std::string file_name);
};

#endif

