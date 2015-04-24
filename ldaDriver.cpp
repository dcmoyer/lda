#include "document.hpp"
#include "lda.hpp"

int main (int argc, char* argv[])
{
  if (argc < 10)
  {
    std::cout << "usage -> ./sampler location_prefix vocab_path numTopics numDocuments alphaParam betaParam burninVal thinningVal numIterations" << std::endl;
    exit(-1);
  }
  std::string file_prefix(argv[1]);
  std::string vocab_path(argv[2]);
  int numTopics = atoi(argv[3]);
  int numDocuments = atoi(argv[4]);
  double alpha = atof(argv[5]);
  double beta = atof(argv[6]);
  int burnin = atoi(argv[7]);
  int thinning = atoi(argv[8]);
  int numIters = atoi(argv[9]);

  std::vector<std::string> list_of_filenames = std::vector<std::string> ();
  for (int i = 0; i < numDocuments; ++i)
  {
    std::stringstream ss;
    ss << file_prefix << i << ".txt";
    list_of_filenames.push_back(ss.str());
  }

  LDA lda = LDA(list_of_filenames, vocab_path, numTopics, alpha, beta, burnin, thinning);
  lda.initialize();
#if 1  
  for (int j = 0; j < list_of_filenames.size(); ++j)
    std::cout << list_of_filenames[j] << std::endl;

  std::cout << "vocab_path = " << vocab_path << std::endl;
  std::cout << "numTopics = " << numTopics << std::endl;
  std::cout << "alpha = " << alpha << std::endl;
  std::cout << "beta = " << beta << std::endl;
  std::cout << "burnin = " << burnin << std::endl;
  std::cout << "thinning = " << thinning << std::endl;
  std::cout << "numIters = " << numIters << std::endl;
  lda.print_topic_dist_idx("testoutput", 0);
#else
  lda.run_iterations(numIters);
#endif 

  return 0;
}
