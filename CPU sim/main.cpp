//
// CPU scheduling sim
//

#include <iostream>     // std::cout
#include <iomanip>      // std::fixed and setprecision
#include <algorithm>    // std::next_permutation
#include <cmath>        // std::tgamma (for factorial)
#include <iterator>     // std::size for arrays
#include <string>       // std::string, std::rfind
#include <vector>       // std::vector
using namespace std;

//
// Global structures, constants, and variables
//

// Declare a structure to represent a process.
struct Process
{
   char id;
   int  cycles;
   int  arrivalTime;
};

const int _NumProcesses = 3;  // TODO: Remove this and make it dynamic.
bool _DEBUG = false;


//
// Support Routines
//
bool checkTimelineValidity(Process processes[],
                           string  timeline)
{
   // A timeline is valid if it never trues to schedule a process into a time slot before it has arrived.
   bool isValid = true;
   // Iterate over all of the processes, checking the timeline validity for each.
   for (int i = 0; i < _NumProcesses; i++ ) {
      if (_DEBUG) {
         cout << "Checking process " << processes[i].id << " for validity." << endl;
      };
      // If this process ID occurrs in the timeline BEFORE its arrival time then it's NOT VALID.
      for (int j = 0; j < processes[i].arrivalTime; j++) {
         if (timeline[j] == processes[i].id) {
            isValid = false;
         }
      }
   }
   return isValid;
}

float calcAverageTurnaroundTime(Process processes[],
                                string  timeline) {
   float ttSum = 0.0;
   // Iterate over all of the processes, computing the turnaround time for each.
   for (int i = 0; i < _NumProcesses; i++ ) {
      // Find the LAST occurrence of this processes' ID in the timeline string.
      int finishTime = timeline.rfind(processes[i].id);
      // Subtract if from the arrival time, and avoid an off-by-one error.
      int turnaroundTime = (finishTime - processes[i].arrivalTime) + 1; // +1 because time 0 counts.
      if (_DEBUG) {
         cout << "Turnaround time for process " << processes[i].id << ":" << turnaroundTime << endl;
      };
      // Accumulate the sum so we can compute the average later.
      ttSum += turnaroundTime;
   }
   return (ttSum / _NumProcesses);
}


//
// Main entry point for this program.
//
int main(int argc, char* argv[])
{
   int retCode = 0;

   cout.setf(ios::fixed);  // Set floating point precision.

   // Check the command line args.
   // Note: argv[0] is the executable name. The parameters begin with index 1.
   if (argc >= 2) {
      if (strcmp(argv[1],"debug") == 0) {  // Needing that strcmp() instead of just == is why people hate C++.
         _DEBUG = true;
         cout << "Running in DEBUG mode." << endl;
      } else {
         cout << "Bad argument [" << argv[1] << "] ignored." << endl;
      }
   } // end if

   // Define processes. TODO: Make dynamic. Read from a file.
   Process pA;
   pA.id          = 'A';
   pA.cycles      = 3;
   pA.arrivalTime = 0;

   Process pB;
   pB.id          = 'B';
   pB.cycles      = 2;
   pB.arrivalTime = 0;

   Process pC;
   pC.id          = 'C';
   pC.cycles      = 1;
   pC.arrivalTime = 0;

   // Put those process structures in an array Processes.
   Process processes[_NumProcesses] = {pA, pB, pC};

   // Other initializations
   string timeline = "";
   int denominator = 1;
   vector<string> validTimelines;

   // Note the processes.
   cout << "Processes:" << endl;
      for (int i = 0; i < 3; i++) {
      cout << processes[i].id << " of length " << processes[i].cycles << " arriving at time " << processes[i].arrivalTime << endl;
      for (int j = 0; j < processes[i].cycles; j++) {
         timeline += processes[i].id;
      };
      denominator =  denominator * tgamma( processes[i].cycles+1 );
   }
   cout << endl;

   cout << "Base execution timeline: " << timeline << endl;

   // How many unique TOTAL permutations are there?
   int numerator = tgamma( timeline.length()+1 );  // denominator declared and computed above.
   cout << numerator << " / " << denominator << " = " << (numerator/denominator) << " total permutations:" << endl;
   bool timelineIsValid = true;
   bool thereAreMorePermutations = true;
   while (thereAreMorePermutations) {
      // Is this timeline possibility valid? I.e., Does it use a process BEFORE it arrives? If so, it's not valid.
      timelineIsValid = checkTimelineValidity(processes, timeline);
      cout << timeline << " ";
      if (timelineIsValid) {
         cout << "valid";
         validTimelines.push_back(timeline);
      } else {
         cout << "NOT valid";
      }
      cout << endl;
      // Rearrange the timeline into its next permutation. Returns false is there are no more.
      thereAreMorePermutations = next_permutation(timeline.begin(),timeline.end());
   }
   cout << endl;

   float avgTurnaroundTime = 0.0;
   cout << validTimelines.size() << " Valid Timelines:" << endl;
   for (int i = 0; i < validTimelines.size(); i++) {
      avgTurnaroundTime = calcAverageTurnaroundTime(processes, validTimelines[i]);
      cout << validTimelines[i] << ": avgTT = " << setprecision(3) << avgTurnaroundTime << endl;
   }

   return retCode;
}
