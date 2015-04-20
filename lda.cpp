
#include "lda.hpp"

void LDA::initialize(){

  //zero the matrix
  for(int i=0; i < K; ++i){
    for(int j=0; j < V; ++j){
      (*topic_x_words)(i,j) = 0;
    }
    (*total_words_in_topics)(i,0) = 0;
  }

  for(int i=0; i < filenames.size(); ++i){
    
    Document target(filenames[i]);
    target.load_document();
    target.init_random_topics(K);
    
    int size_of_doc = target.num_words();
    int word;
    int topic;
    for(int j=0; j < size_of_doc; ++j){
      word = target.get_word(j);
      topic = target.get_word_topic(j);
      (*topic_x_words)(topic,word) += 1;
      (*total_words_in_topics)(topic,0) += 1;
    }
  }

}


//void LDA::run_iterations(int num_iterations);
//void LDA::print_topic_dist(std::string topic_file_name);
//void LDA::print_doc_dist(int index);
//void LDA::load_topic_dist(std::string topic_file_name);



