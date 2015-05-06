#include "lda.hpp"

//Utility function
//  Returns a number on [0,1)
double unif(){
  return (double) std::rand()/(double) RAND_MAX;
}

// initialize the static member variables
//boost::numeric::ublas::matrix<int>* LDA::topic_x_words = NULL;
//boost::numeric::ublas::matrix<int>* LDA::total_words_in_topics = NULL;

void LDA::initialize(){

  Document target;

  initialize_tables();

  std::cout << "init matrices\n";
  for(int i=0; i < filenames.size(); ++i){
    
    target = Document(filenames[i]);
    target.load_document();
    target.init_random_topics(K);
    target.save_topics();

    update_tables(target);

    target.clear();
  }

}

void LDA::initialize_tables()   {
  //zero the matrix
#if MPI_ENABLED
  for(int i=0; i < K; ++i){
    for(int j=0; j < V; ++j){
      _topic_x_words(i,j) = 0;
    }
    _total_words_in_topics(i,0) = 0;
  }
#else
  for(int i=0; i < K; ++i){
    for(int j=0; j < V; ++j){
      (*topic_x_words)(i,j) = 0;
    }
    (*total_words_in_topics)(i,0) = 0;
  }
#endif
}

void LDA::update_tables(Document doc_file)    {
    int size_of_doc = doc_file.num_words();
    int word;
    int topic;

    doc_file.load_document();
    doc_file.load_topics();

    for(int j=0; j < size_of_doc; ++j){
      word = doc_file.get_word(j);
      topic = doc_file.get_word_topic(j);
#if MPI_ENABLED
      _topic_x_words(topic,word) += 1;
      _total_words_in_topics(topic,0) += 1;
#else
      (*topic_x_words)(topic,word) += 1;
      (*total_words_in_topics)(topic,0) += 1;
#endif
    }
}

#if MPI_ENABLED
void LDA::broadcast_data(std::vector<int> data_vector, int size) {
    int mpi_ret_status = MPI_Bcast(&data_vector[0], size, MPI_INT, 0, MPI_COMM_WORLD);
    int rank;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (mpi_ret_status != MPI_SUCCESS)  {
        char err_string[BUFSIZ];
        int err_string_length;
        MPI_Error_string(mpi_ret_status, err_string, &err_string_length);
        std::cout <<"[proc "<<rank<<"]" << "MPI_Bcast failed with error: " << err_string << std::endl;
        exit(0);
    }
}
#endif

void LDA::run_iterations(int num_iterations){
  
  Document target;
  int first_file_idx, last_file_idx;

#if MPI_ENABLED
    int rank, namelen, num_procs;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Get_processor_name(processor_name, &namelen);

    //sync nodes before broadcast
    //MPI_Barrier(MPI_COMM_WORLD);

    //distribute the initial tables to all nodes
    broadcast_data(topic_x_words, K*V);
    broadcast_data(total_words_in_topics, K);

    if (rank == 0)    {
      std::cout <<"[proc "<<rank<<"]" << "Tables sent to children" << std::endl;
    }

    int num_files_per_proc = ceil((float)filenames.size() / num_procs);
    first_file_idx = rank * num_files_per_proc;
    last_file_idx = first_file_idx + num_files_per_proc - 1;

    if (rank == num_procs - 1)
        last_file_idx = filenames.size() - 1;

      std::cout <<"[proc "<<rank<<"]" << "My first index is "<<first_file_idx<<std::endl;
      std::cout <<"[proc "<<rank<<"]" << "My last index is "<<last_file_idx<<std::endl;
#else
    first_file_idx = 0; last_file_idx = filenames.size();
#endif

  for(int iter_idx=0; iter_idx < num_iterations; ++iter_idx){
#if MPI_ENABLED
    if (rank == 0)    { 
      std::cout <<"[proc "<<rank<<"]" << "Iteration " << iter_idx << std::endl;
    }
#else
    std::cout << "Iteration " << iter_idx << std::endl;
    first_file_idx = 0; last_file_idx = filenames.size();
#endif

    //Big loop of iteration over files
    //TODO: MPI Goes Here

    for(int file_idx=first_file_idx; file_idx < last_file_idx; ++file_idx){
      target = Document(filenames[file_idx]);
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
        assert( topic>=0 && topic<=K );
        document_x_topic(0,topic) += 1;
      }

      //actual gibbs sampling
      //this is where OpenMP would be nice.
      //TODO: OpenMP atomic or barrier/syncs
    int word_idx = 0;
#if OMP_ENABLED 
    int threadCount = 4;
    omp_set_num_threads(threadCount);
    #pragma omp parallel shared(target, document_x_topic, size_of_doc) private(word_idx)
#endif 
    {
#if OMP_ENABLED
      #pragma omp for
#endif 
      for(word_idx=0; word_idx < size_of_doc; ++word_idx){
        //getword
        int word = target.get_word(word_idx);
        //gettopic
        int topic = target.get_word_topic(word_idx);

        //update dists.
        //comment out the following since with OMP, topic_x_words could be poisoned a little bit and go below 1
        //assert((*topic_x_words)(topic,word) > 0);
#if MPI_ENABLED
        _topic_x_words(topic,word) -= 1;
        _total_words_in_topics(topic,0) -= 1;
#else
        (*topic_x_words)(topic,word) -= 1;
        (*total_words_in_topics)(topic,0) -= 1;
#endif
        assert(document_x_topic(0,topic) > 0);
        document_x_topic(0,topic) -= 1;
        boost::numeric::ublas::matrix<double> topic_dist(K,1);
        for(int topic_idx=0;topic_idx < K; ++topic_idx){
#if MPI_ENABLED
          double topic_word_prob = ((double) _topic_x_words(topic_idx,word) + beta)/
            ((double)_total_words_in_topics(topic_idx,0) + V*beta);

#else
          double topic_word_prob = ((double) (*topic_x_words)(topic_idx,word) + beta)/
            ((double)(*total_words_in_topics)(topic_idx,0) + V*beta);
#endif
          double topic_doc_prob = ((double)(document_x_topic(0,topic_idx) + alpha)/
            ((double) size_of_doc + K*alpha/*CHECKTHIS*/));

          assert (topic_idx>=0 && topic_idx<=K);
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
          prob -= topic_dist(new_topic,0);
        }
        assert(new_topic < K);

        //assign_topic
        target.set_word_topic(word_idx,new_topic);

        //update dists
#if MPI_ENABLED
        _topic_x_words(new_topic,word) += 1;
        _total_words_in_topics(new_topic,0) += 1;
#else
        (*topic_x_words)(new_topic,word) += 1;
        (*total_words_in_topics)(new_topic,0) += 1;
#endif
        document_x_topic(0,new_topic) += 1;
      }
      target.save_topics();
    } //omp parallel

    }

#if MPI_ENABLED
    if (iter_idx % sync_frequency == 0)  {
      if (rank == 0)  {
        // recount the tables from current topic assignments
        initialize_tables();

        for (int i = 0; i<filenames.size(); i++)    {
            Document tmp_doc = Document(filenames[i]);
            update_tables(tmp_doc);
        }
      }

        // send tables to all processes in the pool
      broadcast_data(topic_x_words, K*V);
      broadcast_data(total_words_in_topics, K);
    }

    if (rank == 0) {
      if(iter_idx % thinning == 0){
        if(iter_idx < burnin){
          continue;
        }
        print_neg_log_likelihood(vocab_path.substr(0, vocab_path.length()-9) + "neg_log_like.csv");
        //TODO: PRINT
      }
    }
#else
    if(iter_idx % thinning == 0){
      if(iter_idx < burnin){
        continue;
      }
      print_neg_log_likelihood(vocab_path.substr(0, vocab_path.length()-9) + "neg_log_like.csv");
      //TODO: PRINT
    }
#endif
  } // outer for loop
}

void LDA::print_topic_dist(std::string topic_file_name) {
  std::ofstream s(topic_file_name.c_str(), std::ofstream::out);
  for(int i=0; i < K; ++i){
    for(int j=0; j < V; ++j){
#if MPI_ENABLED
        s << _topic_x_words(i,j) << ",";
#else
        s << (*topic_x_words)(i,j) << ",";
#endif
    }
    s << "\n";
  } 
  s.close();
}
void LDA::print_topic_dist_idx(std::string doc_dist_file_name, int index) { 
  std::stringstream ss;
  ss << doc_dist_file_name << "_" << index << ".csv";
  print_topic_dist(ss.str());
}

void LDA::print_neg_log_likelihood(std::string file_name){
  double log_L=0,temp_prob=0;
  int word;
  //calculate neg log likelihood
  Document target;
  for(int file_idx=0; file_idx < filenames.size(); ++file_idx){
      
    target = Document(filenames[file_idx]);
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
    for(int word_idx=0; word_idx < size_of_doc; ++word_idx){
      word = target.get_word(word_idx);
      temp_prob = 0;
      for(int topic_idx = 0; topic_idx < K; ++topic_idx){
#if MPI_ENABLED
        double topic_word_prob = ((double) _topic_x_words(topic_idx,word) + beta)/
          ((double)_total_words_in_topics(topic_idx,0) + V*beta);
#else
        double topic_word_prob = ((double) (*topic_x_words)(topic_idx,word) + beta)/
          ((double)(*total_words_in_topics)(topic_idx,0) + V*beta);
#endif
        double topic_doc_prob = ((double)(document_x_topic(0,topic_idx) + alpha)/
          ((double) size_of_doc + K*alpha/*CHECKTHIS*/));
        temp_prob += topic_word_prob * topic_doc_prob;
      }
      //std::cout << temp_prob << std::endl;
      if(temp_prob == 0){
        std::cout << "WTF";
        temp_prob = 0.000001;
      }
      log_L += log2(temp_prob)/((double) filenames.size());
    }

  }

  //print it
  std::ofstream s(file_name.c_str(), std::ofstream::app);
  s << -log_L << std::endl;  
 
}

//void LDA::print_doc_dist(int index);
//void LDA::load_topic_dist(std::string topic_file_name);



