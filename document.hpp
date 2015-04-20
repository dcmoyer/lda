
#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "include.hpp"

class Document{
private:
  std::string path_to_document;
  std::vector<int> words;
  std::vector<int> topic_assignments;
public:
  Document(){;}
  Document(std::string path) : path_to_document(path){;}

  void load_document();
  void load_topics();

  //This implicitly creates and saves a topics file.
  //THIS OVERWRITES THE TOPIC FILE.
  void save_topics();

  //This assigns random topics from a unif. dist.
  void init_random_topics(int num_topics);

  //This reduces memory overhead
  void clear(){
    words.clear();
    topic_assignments.clear();
  }

  //accessors
  int get_word(int index);
  int get_word_topic(int index);
  void set_word_topic(int index, int topic);

  int num_words(){
    return words.size();
  }
};

#endif

