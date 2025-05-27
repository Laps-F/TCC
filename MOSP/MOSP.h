
#ifndef __MOSP_H__
#define __MOSP_H__

#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <utility>
#include <forward_list>
#include <list>
#include <set>
#include <map>
#include <omp.h>
#include <limits.h>
#include <random>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <thread>
#include <atomic>
#include "../PTAPI-main/include/Problem.h"


struct solMOSP: public solution{ 
  std::vector<int> sol; 
  int maxNumberPiecesPerPatern;
};


class MOSP: public Problem<solMOSP>{
	private:

		int numberPatterns;
		int numberPieces;
		int maxNumberPiecesPerPatern;

		std::string fn;
		// std::vector<std::vector<bool>> incidenceMatrix;
		std::vector<int> stackSizeEvaluation;
		std::vector<std::vector<int>> patternPieces;

		
	public:
		MOSP(std::string filename, int readForm);
		~MOSP();
		solMOSP construction();
		solMOSP neighbor(solMOSP sol);
		double evaluate(solMOSP sol);
};

#endif 
