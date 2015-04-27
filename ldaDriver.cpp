
#include "lda.hpp"
#include "include.hpp"

int main(int argc, char* argv[]){
  
  std::string prefix, path_prefix,vocab_path; 
  int flag;
  std::vector<std::string> filenames;
  int K, N;
  std::stringstream ss;

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
    std::cout << K << " blocks" << std::endl;
  } else {
    std::cout << "ERROR IN READ";
    return 1;
  }
  
  std::istringstream issN(argv[3]);
  if(issN >> N){
    std::cout << K << " files" << std::endl;
  } else {
    std::cout << "ERROR IN READ";
    return 1;
  }

  std::clock_t start;
 
  for(int i=0; i < N; ++i){

    ss << i;
    std::string s = ss.str();
    if(flag == 0){
      filenames.push_back(path_prefix + s + ".txt");
    }
    std::cout << path_prefix + s + ".txt" << std::endl;
    ss.str("");
  }
  
  LDA lda(filenames,
                vocab_path,
                K,
                1,//alpha,
                1,//beta,
                20,//burnin,
                20//thinning
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
  start = std::clock();
  lda.run_iterations(100);
#endif

  std::cout << "Runtime: " << ((double) std::clock() - start) / (double)CLOCKS_PER_SEC
     << std::endl;

  lda.print_topic_dist_idx("testoutput",0);
}


