
#include "lda.hpp"

//Utility function
//  Returns a number on [0,1)
double unif(){
  return (double) std::rand()/(double) RAND_MAX;
}


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


void LDA::run_iterations(int num_iterations){
  
  for(int iter_idx=0; iter_idx < num_iterations; ++iter_idx){

    //Big loop of iteration over files
    for(int file_idx=0; file_idx < filenames.size(); ++file_idx){
      
      Document target(filenames[file_idx]);
      target.load_document();
      target.load_topics();

      //loop of iteration over words
      int size_of_doc = target.num_words();
      
      //create_document_topic_distribution
      boost::numeric::ublas::matrix<double> document_x_topic(1,K);

      //initialize dist
      for(int i=0; i<K; ++i){
        document_x_topic(0,i) = 0;
      }
      
      //fill distribution
      for(int word_idx=0; word_idx < size_of_doc; ++word_idx){
        int topic = target.get_word_topic(word_idx);
        document_x_topic(0,topic) += 1;
      }

      //actual gibbs sampling
      //this is where OpenMP would be nice.
      for(int word_idx=0; word_idx < size_of_doc; ++word_idx){
        //getword
        int word = target.get_word(word_idx);
        //gettopic
        int topic = target.get_word_topic(word_idx);

        //update dists.
        assert((*topic_x_words)(topic,word) > 0);
        (*topic_x_words)(topic,word) -= 1;
        (*total_words_in_topics)(topic,0) -= 1;
        assert(document_x_topic(0,topic) > 0);
        document_x_topic(0,topic) -= 1;

        boost::numeric::ublas::matrix<double> topic_dist(K,1);
        for(int topic_idx=0;topic_idx < K; ++topic_idx){
          double topic_word_prob = ((double) (*topic_x_words)(topic_idx,word) + beta)/
            ((double)(*total_words_in_topics)(topic_idx,0) + V*beta);
          double topic_doc_prob = ((double)(document_x_topic(0,topic) + alpha)/
            ((double) size_of_doc + K*alpha/*CHECKTHIS*/));
          topic_dist(topic_idx,0) = topic_word_prob * topic_doc_prob;
        }

        //sum of the topic dist vector
        double normalizing_constant = sum(prod(
          boost::numeric::ublas::scalar_vector<double>(topic_dist.size1()),
          topic_dist));
        for(int topic_idx=0; topic_idx < K; ++topic_idx){
          topic_dist(topic_idx,0) = topic_dist(topic_idx,0)/normalizing_constant;
        }
        double prob = unif();
        int new_topic = -1;
        
        while(prob > 0){
          new_topic += 1;
        }
        assert(new_topic < K);

        //update dists
        (*topic_x_words)(new_topic,word) += 1;
        (*total_words_in_topics)(new_topic,0) += 1;
        document_x_topic(0,topic) += 1;
      }
    }
    if(iter_idx % thinning == 0){
      if(iter_idx < burnin){
        continue;
      }
      //TODO: PRINT
    }
  }
}
//void LDA::print_topic_dist(std::string topic_file_name);
//void LDA::print_doc_dist(int index);
//void LDA::load_topic_dist(std::string topic_file_name);



