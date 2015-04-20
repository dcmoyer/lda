
#include "lda.cpp"

LDA::initialize(){
  
  for(int i=0; i < filenames.size(); ++i){
    
    Document target(filenames[i]);
    target.load_document();
    target.init_random_topics(K);

  }

}


  void run_iterations(int num_iterations);

  void print_topic_dist(std::string topic_file_name);
  void print_doc_dist(int index);
  void load_topic_dist(std::string topic_file_name);



