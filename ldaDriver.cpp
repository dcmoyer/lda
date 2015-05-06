
#include "lda.hpp"
#include "include.hpp"

// taken from earlier HWs
double read_timer()
{
  static bool initialized = false;
  static struct timeval start;
  struct timeval end;
  if( !initialized ) {
    gettimeofday( &start, NULL );
    initialized = true;
  }
  
  gettimeofday( &end, NULL );
  
  return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}


int main(int argc, char* argv[]){
  
  std::string prefix, path_prefix,vocab_path, output_path; 
  std::vector<std::string> filenames;
  int K, N;
  std::stringstream ss;
  int rank = 0;

  int rank;

#if MPI_ENABLED
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
#endif

  //cmd args
  if(argc != 4){
    std::cout << "<filename>:" + std::string(argv[0])
       + ": ERROR wrong number of args";
    return 1;
  }
 
  prefix = argv[1];
  vocab_path ="data/" + prefix + "/" + prefix + "_proc/vocab.txt" ;
  path_prefix = "data/" + prefix + "/" + prefix + "_proc/" + prefix;

  std::istringstream iss(argv[2]);
  if(iss >> K){
#ifndef MPI_ENABLED
    std::cout << K << " blocks" << std::endl;
#endif
  } else {
    std::cout << "ERROR IN READ";
    return 1;
  }
  
  std::istringstream issN(argv[3]);
  if(issN >> N){
#ifndef MPI_ENABLED
    std::cout << K << " files" << std::endl;
#endif
  } else {
    std::cout << "ERROR IN READ";
    return 1;
  }

  //std::clock_t start;
 
  for(int i=0; i < N; ++i){

    ss << i;
    std::string s = ss.str();
    filenames.push_back(path_prefix + s + ".txt");
#if MPI_ENABLED
  if (rank == 0)  
    std::cout << path_prefix + s + ".txt" << std::endl;
#else
    std::cout << path_prefix + s + ".txt" << std::endl;
#endif
    ss.str("");
  }
  
  LDA lda(filenames,
                prefix,
                vocab_path,
                K,
                1,//alpha,
                1,//beta,
                2,//burnin
                1,//thinning
                50//sync frequency
              );
  lda.initialize();
#if 0
  /*for (int j = 0; j < list_of_filenames.size(); ++j)
    std::cout << list_of_filenames[j] << std::endl;

  std::cout << "vocab_path = " << vocab_path << std::endl;
  std::cout << "numTopics = " << numTopics << std::endl;
  std::cout << "alpha = " << alpha << std::endl;
  std::cout << "beta = " << beta << std::endl;
  std::cout << "burnin = " << burnin << std::endl;
  std::cout << "thinning = " << thinning << std::endl;
  std::cout << "numIters = " << numIters << std::endl;
  lda.print_topic_dist_idx("testoutput", 0);*/
#else
  //start = std::clock();
  double start = read_timer();
  lda.run_iterations(100);
  double end = read_timer();
#endif

#if MPI_ENABLED
  if (rank == 0) {
    std::cout << "Runtime: " << (end - start) << std::endl;
    lda.print_topic_dist_idx(path_prefix + "topic_dist",0);
  }

  MPI_Finalize();
#else
    std::cout << "Runtime: " << (end - start) << std::endl;
    lda.print_topic_dist_idx(path_prefix + "topic_dist",0);
#endif
}


