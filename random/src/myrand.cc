#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

// get a random value from a PDF
int findCeil(int  arr[], int r, int l, int h)
{
        int mid;
        while (l < h)
        {
                mid = l + ((h - l) >> 1); // Same as mid = (l+h)/2
                (r > arr[mid]) ? (l = mid + 1) : (h = mid);
        }
        int va=(arr[l] >= r) ? l : -1;
        // if negative, assume no correction (center)
        if (va<0) va=(int)(h/2.0);
        //if (va<0)  va=rand() % h; // random smearing  
        return va;
}


// The main function that returns a random number from arr[] according to
// distribution array defined by freq[]. n is size of arrays.
int myRand(int arr[], int freq[], int n)
{
        // Create and fill prefix array
        int prefix[n], i;
        prefix[0] = freq[0];
        for (i = 1; i < n; ++i)
                prefix[i] = prefix[i - 1] + freq[i];

        // prefix[n-1] is sum of all frequencies. Generate a random number
        // with value from 1 to this sum
        if (prefix[n - 1] != 0) {
                int r = (rand() % prefix[n - 1]) + 1;
                //long  R1=Reta.Rndm()*RAND_MAX;
                //int r = (R1 % prefix[n - 1]) + 1;
                //cout << "Random value on [0 " << RAND_MAX << endl;
                // Find index of ceiling of r in prefix arrat
                int indexc = findCeil(prefix, r, 0, n - 1);
                return arr[indexc];
        }

          //int va=rand() % n;
           int va=(int)(n/2.0); // assume no correction when error
           return va; // random smearing  
}

