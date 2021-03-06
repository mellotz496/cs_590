//DD is implemented as in paper + complement first + backtracking

#include <clang-c/Index.h>
#include <stdlib.h>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <mlpack/core.hpp>
#include <mlpack/methods/decision_tree/decision_tree.hpp>
#include <string>
#include <unordered_set>
#include <vector>

#include "AST_manipulation.h"
#include "cursor_utils.h"
#include "model.h"
#include "options.h"
#include "report.h"
#include "rules.h"

#include "delta_debugging.h"

bool someSetSucceeding = false;

std::vector<std::vector<CXCursor>> cache;

std::vector<CXCursor> tempGlobalElements, tempFunctionBodies,
        tempAllCursors;

std::vector<CXCursor> DeltaDebugging::getImmediateChildren(CXCursor root) {
  return visitor.getImmediateChildren(root);
}

std::vector<CXCursor> DeltaDebugging::getAllCursors() {
  return visitor.getAllCursors();
}

std::vector<CXCursor> DeltaDebugging::getGlobalElements() {
  return visitor.getGlobalElements();
}

std::vector<CXCursor> DeltaDebugging::getFunctionBodies() {
  return visitor.getFunctionBodies();
}

void DeltaDebugging::removeFromAllCursors(std::vector<CXCursor> toBeRemoved,
                                          bool deepRemove) {
  std::vector<CXCursor> children;
  if (deepRemove) {
    for (auto c : toBeRemoved) {
      std::vector<CXCursor> temp = visitor.getAllChildren(c);
      children.insert(children.end(), temp.begin(), temp.end());
    }
  }
  children.insert(children.end(), toBeRemoved.begin(), toBeRemoved.end());
  for (auto x : children) {
    auto temp = visitor.getAllCursors();
    temp.erase(std::remove(temp.begin(), temp.end(), x), temp.end());
    visitor.setAllCursors(temp);
  }
}

void DeltaDebugging::handleLearning(std::vector<CXCursor> tempAllCursors,
				    std::vector<CXCursor> source,
				    std::vector<CXCursor> subset,
				    int status, int iteration
				    ) {
  if (!Option::noCache)
    cache.emplace_back(tempAllCursors);
  if (Option::decisionTree) {
    model.addForTraining(source, subset, status);
    if (Option::delayLearning) {
      if (!(iteration > 100 && iteration % (iteration / 100 + 1) != 0)) {
	if (Option::profile)
	  Report::learningProfiler.startTimer();
	model.train();
	if (Option::profile)
	  Report::learningProfiler.stopTimer();
      }
    } else {
      if (Option::profile)
	Report::learningProfiler.startTimer();
      model.train();
      if (Option::profile)
	Report::learningProfiler.stopTimer();
    }
  }//end if decisionTree
}

bool DeltaDebugging::test(std::vector<CXCursor> toBeRemoved,
                          const char *srcPath, const char *dstPath,
                          LEVEL level) {
  //std::cout << "testing...\n";
  //std::cout << "toBeRemoved:\n";
  //CursorUtils::prettyPrintSet(toBeRemoved);
  if (level == GLOBAL)
    Report::globalCallsCounter.increment();
  else
    Report::localCallsCounter.increment();
  //remove toBeRemoved (subset)
  ASTManipulation::removeElementsEfficient(srcPath, dstPath, toBeRemoved);
  std::string cmd = std::string(Option::oracleFile) + " 2> /dev/null";
  if (Option::profile)
    Report::oracleProfiler.startTimer();
  //run test
  bool status = system(cmd.c_str());
  //std::cout << "STATUS: " << status << "\n";
  if (Option::profile)
    Report::oracleProfiler.stopTimer();
  if (status == SUCCESS) {
    if (level == GLOBAL)
      Report::successfulGlobalCallsCounter.increment();
    else
      Report::successfulLocalCallsCounter.increment();

    std::string bestNow = Option::outputDir + "/best_now.c";
    if (Option::saveTemp) {
      std::string cmd = std::string("cp ") + dstPath + " " + Option::outputDir +
                        "/" + dstPath + "." +
                        std::to_string(Report::localCallsCounter.count() +
                                       Report::globalCallsCounter.count()) +
                        "." + std::to_string(level) + ".success.c";
      system(cmd.c_str());
    }
    cmd = std::string("cp ") + dstPath + " " + bestNow.c_str();
    system(cmd.c_str());
  } else {//test failed
    if (Option::saveTemp) {
      std::string cmd = std::string("cp ") + dstPath + " " + Option::outputDir +
                        "/" + dstPath + "." +
                        std::to_string(Report::localCallsCounter.count() +
                                       Report::globalCallsCounter.count()) +
                        "." + std::to_string(level) + ".fail.c";
      system(cmd.c_str());
    }
  }
  return status;
}

std::vector<CXCursor> DeltaDebugging::backtrack(std::vector<CXCursor> source_,
						std::vector<CXCursor> subset,
						std::vector<std::vector<CXCursor>> subsets,
						int i,
						arma::uvec order,
						const char *srcPath,
						const char *dstPath,
						LEVEL level,
						int iteration,
						std::vector<CXCursor> children) {
  std::vector<CXCursor> toBeRemoved;
  std::vector<CXCursor> temp = CursorUtils::subtract(source_, subset);
  int status = FAIL;
  int j = i + 1;
  while(status == FAIL && j < subsets.size()) {
    std::vector<CXCursor> toAdd;
    tempAllCursors = visitor.getAllCursors();
    tempGlobalElements = visitor.getGlobalElements();
    tempFunctionBodies = visitor.getFunctionBodies();
    children.clear();
    if (Option::decisionTree) {
      toAdd = subsets[order(j)];
    } else {
      toAdd = subsets[j];
    }
    for(int k = 0; k < subsets.size(); k++) {
      if(Option::decisionTree) {
	if(k != order(i) && k != order(j))
	  children = addToRemove(children, subsets[k]);
      } else {
	if(k != i && k != j)
	  children = addToRemove(children, subsets[k]);
      }
    }
    removeCursors(children, level);
    toBeRemoved = CursorUtils::subtract(temp, toAdd);
    //std::cout << "testing subset " << i << " and subset " << j << "\n";
    status = test(toBeRemoved, srcPath, dstPath, level);
    handleLearning(tempAllCursors, source_, toBeRemoved, status, iteration); 
    if(status == SUCCESS) {//std::cout << "subset " << i << " and subset " << j << " passed\n";
      resetCursors(level);
      someSetSucceeding = true;
      return CursorUtils::subtract(source_, toBeRemoved);
    }
    j++;
  }
  return source_;
}

bool DeltaDebugging::cacheContains(std::vector<std::vector<CXCursor>> cache,
                                   std::vector<CXCursor> vec) {
  for (auto c : cache) {
    if (c == vec)
      return true;
  }
  return false;
}

std::vector<std::vector<CXCursor>>
DeltaDebugging::filterSubsets(std::vector<std::vector<CXCursor>> subsets,
                              LEVEL level) {
  // Removing redundant subsets
  std::vector<std::vector<CXCursor>> finalSubsets;
  std::vector<CXCursor> tempAllCursors, children;
  for (auto subset : subsets) {
    tempAllCursors = visitor.getAllCursors();
    children.clear();
    for (auto c : subset) {
      std::vector<CXCursor> temp = visitor.getAllChildren(c);
      children.insert(children.end(), temp.begin(), temp.end());
    }
    children.insert(children.end(), subset.begin(), subset.end());
    tempAllCursors = CursorUtils::subtract(tempAllCursors, children);

    bool isInCache = false;
    if (!Option::noCache)
      isInCache = cacheContains(cache, tempAllCursors);
    auto f = visitor.getFunction(visitor.getAllCursors(), subset[0]);
     if (!((Option::globalDep && level == GLOBAL && Rule::mainExists(subset)) ||
      (isInCache && !Option::noCache) ||
      ((Option::globalDep || Option::localDep) &&
       Rule::isTypeDependencyBroken(tempAllCursors, subset)) ||
      (Option::localDep && level > GLOBAL &&
       Rule::isUninitialized(tempAllCursors)) ||
      (Option::localDep && level > GLOBAL &&
       !Rule::containsOneReturn(children, f, visitor.getAllChildren(f))))) {
     finalSubsets.emplace_back(subset);
    }
  }
  return finalSubsets;
}

std::vector<CXCursor> DeltaDebugging::addToRemove(std::vector<CXCursor> children, std::vector<CXCursor> subset) {
  for (auto c : subset) {
    std::vector<CXCursor> temp = visitor.getAllChildren(c);
    //add temp(begin - end) to children at the end
    children.insert(children.end(), temp.begin(), temp.end());
  }
  //add subset(begin - end) to children at the end
  children.insert(children.end(), subset.begin(), subset.end());
  return children;
}

void DeltaDebugging::removeCursors(std::vector<CXCursor> children, int level) {
  //remove children from tempAllCursors
  tempAllCursors = CursorUtils::subtract(tempAllCursors, children);
  //subtract children from tempGlobalElements and tempFunctionBodies
  if (level == GLOBAL) {
    tempGlobalElements = CursorUtils::subtract(tempGlobalElements, children);
    tempFunctionBodies = CursorUtils::subtract(tempFunctionBodies, children);
  }
}

void DeltaDebugging::resetCursors(int level) {
  visitor.setAllCursors(tempAllCursors);
  if (level == GLOBAL) {
    visitor.setGlobalElements(tempGlobalElements);
    visitor.setFunctionBodies(tempFunctionBodies);
  }
}

void DeltaDebugging::ddmin(std::vector<CXCursor> source, const char *srcPath,
                           const char *dstPath, LEVEL level) {//std::cout << "ddmin\n";
  int status = FAIL;
  if (source.size() >= 1) {
    std::vector<CXCursor> source_ = source;
    int n = 2;
    std::vector<CXCursor> children, complement, backtracked;
    if (Option::decisionTree) {
      model.addForTraining(source, source, SUCCESS);
    }
    int iteration = 0;
    while (static_cast<int>(source_.size()) >= 2) {
      std::vector<std::vector<CXCursor>> subsets =
          CursorUtils::split(source_, n);
      someSetSucceeding = false;
      arma::uvec order;
      arma::mat probs;

      //apply some general rules to reduce number of subsets
      subsets = filterSubsets(subsets, level);

      if (Option::decisionTree) {
        if (Option::profile)
          Report::learningProfiler.startTimer();
        model.addForPredicting(source, subsets, probs);
        if (Option::profile)
          Report::learningProfiler.stopTimer();
        order = sort_index(probs.row(0), "descend");
      }
      
      //loop through the subsets
      for (int i = 0; i < subsets.size(); i++) { 
        iteration++;
        std::vector<CXCursor> subset;
        if (Option::decisionTree) {
          subset = subsets[order(i)];
        } else {
          subset = subsets[i];
        }
        status = FAIL;
	tempAllCursors = visitor.getAllCursors();
	tempGlobalElements = visitor.getGlobalElements();
	tempFunctionBodies = visitor.getFunctionBodies();
	children.clear();
	children = addToRemove(children, subset);
	removeCursors(children, level);
        //run the test on the complement of the current subset (subset is toBeRemoved)
	//std::cout << "testing complement of subset " << i << "\n";
	status = test(subset, srcPath, dstPath, level);
	handleLearning(tempAllCursors, source, subset, status, iteration);
	 
	if(status == SUCCESS) {//std::cout << "complement of subset " << i << " passed\n";
	  //complement of subset passed the test, so reset all to the subset
	  resetCursors(level);
	  //subtract subset from source
	  source_ = CursorUtils::subtract(source_, subset);
	  //reset n to n - 1 (or 2 min)
	  n = std::max(n - 1, 2);
	  someSetSucceeding = true;
	  break;
	} else if(level == GLOBAL) {//complement didn't pass, so check the subset
	  complement = CursorUtils::subtract(source_, subset);
	  tempAllCursors = visitor.getAllCursors();
	  tempGlobalElements = visitor.getGlobalElements();
	  tempFunctionBodies = visitor.getFunctionBodies();
	  children.clear();
	  for(int j = 0; j < subsets.size(); j++) {
	    if(Option::decisionTree) {
	      if(j != order(i))
		children = addToRemove(children, subsets[j]);
	    } else {
	      if(j != i)
		children = addToRemove(children, subsets[j]);
	    }
	  }
	  removeCursors(children, level);

	  //run the test on the subsetj
	  //std::cout << "testing subset " << i << "\n";
	  status = test(complement, srcPath, dstPath, level);
	  handleLearning(tempAllCursors, source, complement, status, iteration);
	  
	  //subset passed the test, so reset all to the subset
	  if(status == SUCCESS) {//std::cout << "subset " << i << " passed\n";
	    resetCursors(level);
	    source_ = subset;
	    n = 2;
	    someSetSucceeding = true;
	    break;
	  } else if (n > 2 && level == GLOBAL) {//backtrack
	    backtracked = backtrack(source_, subset, subsets, i, order, srcPath, dstPath, level, iteration, children);
	    
	    if(someSetSucceeding) {
	      n = 3;
	      source_ = backtracked;
	      break;
	    } 
	  }//end else
	}//end else
      }//end for

      //no set succeeded
      if (!someSetSucceeding) {//std::cout << "no set succeeded\n";
	//until n = size of source
        if (n == static_cast<int>(source_.size()))
          break;

	//start over, increasing granularity
        n = std::min(n * 2, static_cast<int>(source_.size()));
      }
    }//end while
  }//end if (source size > 1)
  if (Option::decisionTree) {
    model.clear();
  }
}
  
void DeltaDebugging::fin() { visitor.fin(); }

void DeltaDebugging::init(const char *inputPath) {
  visitor.clear();
  cache.clear();

  visitor.init(inputPath);
}
