
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
  int num_threads;

#if MPI_ENABLED
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
#endif

#if OMP_ENABLED
  //cmd args
  if(argc != 5){
#else
  if (argc!=4){
#endif
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
    //std::cout << path_prefix + s + ".txt" << std::endl;
#endif
    ss.str("");
  }
 
#if OMP_ENABLED
  std::istringstream issT(argv[4]);
  issT >> num_threads;
//  num_threads = atoi(argv[4]);

  LDA lda(filenames,
                prefix,
                vocab_path,
                K,
                1,//alpha,
                1,//beta,
                2,//burnin
                1,//thinning
                num_threads,//num_threads outer loop
                1//num_threads inner loop
              );
  lda.initialize();
#elif MPI_ENABLED 
  LDA lda(filenames,
                prefix,
                vocab_path,
                K,
                1,//alpha,
                1,//beta,
                2,//burnin
                1,//thinning
                1//sync frequency
              );
  // only the master node should initialize using all files
  // otherwise, it will cause race condition as all nodes
  // are trying to read the same files and causing the app
  // to crash
  if (rank == 0)
  {
    lda.initialize();
  }
#else
  LDA lda(filenames,
                prefix,
                vocab_path,
                K,
                1,//alpha,
                1,//beta,
                2,//burnin
                1//thinning
              );
  lda.initialize();
#endif

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
#if MPI_ENABLED
  lda.run_iterations_mpi(100);
#else
  lda.run_iterations(100);
#endif
  double end = read_timer();
#endif

#if MPI_ENABLED
  if (rank == 0) {
    std::cout << "For "<<num_threads<<" threads, the runtime is: " << (end - start) 
        << " on the "<<prefix<<" dataset"<< std::endl;
    lda.print_topic_dist_idx(path_prefix + "topic_dist",0);
  }

  MPI_Finalize();
#else
    std::cout << "For "<<num_threads<<" threads, the runtime is: " << (end - start)
        << " on the "<<prefix<<" dataset"<< std::endl;
    lda.print_topic_dist_idx(path_prefix + "topic_dist",0);
#endif
}


