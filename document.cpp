
#include "document.hpp"

//Loads a document from file.
//Assumes the path has already been set.
void Document::load_document(){
  
  words.clear();
  std::ifstream s((path_to_document).c_str(), std::ifstream::in);
  int temp;
  
  while(!s.eof()){
    s >> temp;
    words.push_back(temp);
  }
}

void Document::load_topics(){
  
  topic_assignments.clear();
  std::ifstream s((path_to_document + "_topics.txt").c_str(), std::ifstream::in);
  int temp;
  
  while(!s.eof()){
    s >> temp;
    topic_assignments.push_back(temp);
  }
}

//This implicitly creates and saves a topics file.
//THIS OVERWRITES THE TOPIC FILE.
void Document::save_topics(){
  
  std::ofstream s((path_to_document + "_topics.txt").c_str(), std::ifstream::out);
  int len = topic_assignments.size();

  for(int i = 0; i < len; ++i){
    s << " " << topic_assignments[i];
  }
}

//This assigns random topics from a unif. dist.
void Document::init_random_topics(int num_topics){

  int len = words.size();
  topic_assignments.clear();

  for(int i = 0; i < len; ++i){
    topic_assignments.push_back(std::rand() % num_topics);
  }
}

int Document::get_word(int index){
  assert(index < words.size());
  return(words[index]);
}

int Document::get_word_topic(int index){
  assert(index < topic_assignments.size());
  return topic_assignments[index];
}

void Document::set_word_topic(int index, int topic){
  assert(index < topic_assignments.size());
  topic_assignments[index] = topic;
}


