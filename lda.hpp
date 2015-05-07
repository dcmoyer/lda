
#ifndef LDA_CLASS_H
#define LDA_CLASS_H

#include "include.hpp"
#include "document.hpp"

#if MPI_ENABLED
#include <mpi.h>
#endif
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
  int sync_frequency;
  int inner_thread_count;
  int outer_thread_count;

#if MPI_ENABLED
  std::vector<int> topic_x_words;
  std::vector<int> total_words_in_topics;
#else
  boost::numeric::ublas::matrix<int>* topic_x_words;
  boost::numeric::ublas::matrix<int>* total_words_in_topics;
#endif


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
    double _alpha, double _beta, int _burnin, int _thinning, 
    int _outer_loop_num_threads, int _inner_loop_num_threads){
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
    inner_thread_count = _inner_loop_num_threads;
    outer_thread_count = _outer_loop_num_threads;
  
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
#if MPI_ENABLED
    topic_x_words = std::vector<int> (K*V);
    total_words_in_topics = std::vector<int> (K);
#else
//    std::cout << "all but mats\n";
    topic_x_words = new boost::numeric::ublas::matrix<int>(K,V);
    total_words_in_topics = new boost::numeric::ublas::matrix<int>(K,1);
//    std::cout << "mats\n";
#endif
  }

  ~LDA(){
#if MPI_ENABLED
    ;
#else
    delete topic_x_words;
    delete total_words_in_topics;
#endif
  }

#if MPI_ENABLED
  inline int& _topic_x_words(int r, int c)
  {
     size_t index = V * r + c;
     return topic_x_words[index];
  }

  inline int& _total_words_in_topics(int r, int c)
  {
     size_t index = 1 * r + c;
     return total_words_in_topics[index];
  }
#endif

  void initialize();
  void run_iterations(int num_iterations);
  void run_iterations_mpi(int num_iterations);
  void initialize_tables();
  void update_tables(Document target);
  void broadcast_data(std::vector<int> data_vector, int size);

  void print_topic_dist(std::string topic_file_name);
  void print_topic_dist_idx(std::string topic_file_name, int index);
  void print_doc_dist(int index);
  void load_topic_dist(std::string topic_file_name);
  void print_neg_log_likelihood(std::string file_name);
};

#endif

