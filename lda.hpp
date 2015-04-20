
#ifndef LDA_CLASS_H
#define LDA_CLASS_H

#include "include.hpp"
#include "document.hpp"

class lda{
private:
  std::vector<std::string> filenames;
  Document current_document;
  std::string directory_path;
public:
  lda();
  lda(std::vector& list_of_filenames){
    for(int i = 0; i < list_of_filenames.size(); ++i){
      filenames.push_back(list_of_filename[i]);
    }
  }

  void initialize();
  void run_iterations();
  
  void print_topic_dist();
  void print_doc_dist(int index);

};

#endif

