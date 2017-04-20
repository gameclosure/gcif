#include "BinaryModeModel.h"
#include <math.h>

void BinaryModeModel::update(int index) {
//  if (incr == 1)
    AdaptiveModel::update(index); 

}

void BinaryModeModel::init(int ns) {
  AdaptiveModel::init(ns);
  incr = 8192;
}


void ContinuousModeModel::init(int ns) {
  AdaptiveModel::init(ns);

  int i, cum;

  int freqCount = (MaxFrequency / 2);
  double *freqPercentage = new double[ns];

  double sum = 0.0;
  double temp;
  #pragma omp parallel for private(temp) reduction(+:sum)
  for (int i = 0; i < ns; i++) 
  {
    //prevent false sharing and double indirect addressing overhead using a `temp`orary variable
    temp = pow(15.0, 0.4 * (double)(-i-1));  
    freqPercentage[i] = temp;  
    sum += temp; 
    //temp avoids data dependency issues instead of freqPercentage
  }

  #pragma omp parallel for shared(freqPercentage)
  for (int i = 0; i < ns; i++) 
  {
    //Get and store probabiliity
      freqPercentage[i] /= sum;
  }

  for (i = 1; i <= ns; i++) 
  {
    
     //Absolute frequency
    freq[i] = (int)((double)freqCount * freqPercentage[i-1]);
    if (freq[i] == 0)
      freq[i] = 1;
  }
  
  //Cumulative Frequency => loop dependencies => prefix sum
cum_freq[ns] = 0;
 int j;
  #pragma omp parallel for schedule(dynamic)
  for ( i = ns - 1; i >= 0; i--) 
  {   
    cum_freq[i] = 0;
    //#pragma omp parallel for reduction is an overhead
    for(j=ns;j>=i;j--)
    {
        cum_freq[i] += freq[j];
    }
    
  }

  delete [] freqPercentage;
}

