//
// CPU Scheduling Permutation Evaluator
//

// g++ -std=c++20 main.cpp

#include <iostream>     // cout, endl
using std::cout;
using std::endl;

#include <iomanip>      // setprecision, ios
using std::setprecision;
using std::ios;         // I/O base class

#include <string>       // string, to_string
using std::string;
using std::to_string;

#include <vector>       // vector
using std::vector;

#include <algorithm>    // next_permutation
#include <cmath>        // tgamma (for factorial)
#include <iterator>     // size for arrays


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
bool _CSV   = false;


//
// Support Routines
//
bool checkTimelineValidity(Process processes[],
                           string  timeline)
{
   // A timeline is valid if it never tries to schedule a process into a time slot before it has arrived.
   bool isValid = true;
   // Iterate over all of the processes, checking the timeline validity for each.
   for (int i = 0; i < _NumProcesses; i++ ) {
      if (_DEBUG) {
         cout << "Checking process " << processes[i].id << " for validity. " << endl;
      }
      // If this process ID occurs in the timeline BEFORE its arrival time then it's NOT VALID.
      for (int j = 0; j < processes[i].arrivalTime; j++) {
         if (timeline[j] == processes[i].id) {
            isValid = false;
            if (_DEBUG) {
               cout << "   NOT valid because process " << processes[i].id << " cannot be scheduled at time " << j << ". " << endl;
            }
         }
      }
   }
   return isValid;
}

float calcAverageTurnaroundTime(Process processes[],
                                const string& timeline) {
   u_long ttSum = 0;
   // Iterate over all of the processes, computing the turnaround time for each.
   for (int i = 0; i < _NumProcesses; i++ ) {
      // Find the LAST occurrence of this process's ID in the timeline string.
      u_long finishTime = timeline.rfind(processes[i].id);
      // Subtract if from the arrival time, and avoid an off-by-one error.
      u_long turnaroundTime = (finishTime - processes[i].arrivalTime) + 1; // +1 because time slot 0 counts.
      if (_DEBUG) {
         cout << "Turnaround time for process " << processes[i].id << ":" << turnaroundTime << endl;
      }
      // Accumulate the sum so we can compute the average later.
      ttSum += turnaroundTime;
   }
   return (static_cast<float>(ttSum) / _NumProcesses);
}

float calcAverageWaitTime(Process processes[],
                          string  timeline) {
   u_long wtSum = 0;
   // Iterate over all of the processes, computing the wait time for each.
   for (int i = 0; i < _NumProcesses; i++ ) {
      int waitTime = 0;
      // Find the LAST occurrence of this process's ID in the timeline string.
      u_long finishTimeIndex = timeline.rfind(processes[i].id);
      // Count the time slots from process arrival time to its finish where this process ID is NOT executing (and therefore ready/waiting).
      for (int j = processes[i].arrivalTime; j < finishTimeIndex; j++) {
         if (timeline[j] != processes[i].id) {
            waitTime++;
         }
      }
      if (_DEBUG) {
         cout << "Wait time for process " << processes[i].id << ":" << waitTime << endl;
      }
      // Accumulate the sum so we can compute the average later.
      wtSum += waitTime;
   }
   return (static_cast<float>(wtSum) / _NumProcesses);
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
      if (strcmp(argv[1],"debug") == 0) {  // Needing strcmp() here instead of just == is why people hate C++.
         _DEBUG = true;
         cout << "Running in DEBUG mode." << endl;
      } else if (strcmp(argv[1],"csv") == 0) {
         _CSV = true;
         cout << "Running in CSV output mode." << endl;
      } else {
         cout << "Bad argument [" << argv[1] << "] ignored." << endl;
      }
   } // end if

   // Define processes. TODO: Make dynamic. Read from a file.
   Process pA{};
   pA.id          = 'A';
   pA.cycles      = 4;
   pA.arrivalTime = 0;

   Process pB{};
   pB.id          = 'B';
   pB.cycles      = 4;
   pB.arrivalTime = 1;

   Process pC{};
   pC.id          = 'C';
   pC.cycles      = 4;
   pC.arrivalTime = 2;

   // Put those process structures in an array Processes.
   Process processes[_NumProcesses] = {pA, pB, pC};

   // Other initializations
   string timeline = "";
   double denominator = 1;
   vector<string> validTimelines;

   // Display the processes.
   cout << "Processes:" << endl;
   for (auto & process : processes) {
      cout << process.id << " of length " << process.cycles << " arriving at time " << process.arrivalTime << endl;
      for (int j = 0; j < process.cycles; j++) {
         timeline += process.id;
      }
      denominator =  denominator * tgamma( process.cycles+1 );
   }
   cout << endl;

   cout << "Execution timeline ingredients: " << timeline << endl;

   // How many unique TOTAL permutations are there?
   double numerator = tgamma( timeline.length()+1 );  // denominator declared and computed above.
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

   // Output the results.
   vector<string> CSVdata;
   CSVdata.emplace_back("timeline,avg_tt,avg_wt");

   float avgTurnaroundTime = 0.0;
   float avgWaitTime       = 0.0;
   cout << validTimelines.size() << " Valid Timelines:" << endl;
   for (auto & validTimeline : validTimelines) {
      avgTurnaroundTime = calcAverageTurnaroundTime(processes, validTimeline);
      avgWaitTime       = calcAverageWaitTime(processes, validTimeline);
      cout << validTimeline << ": avgTT = " << setprecision(3) << avgTurnaroundTime
           << "  avgWT = " << avgWaitTime << endl;
      if (_CSV) {
         CSVdata.push_back( validTimeline + "," + to_string(avgTurnaroundTime) + "," + to_string(avgWaitTime) );
      }
   }

   // Output the CSV results if necessary.
   if (_CSV) {
      cout << endl << endl;
      for (auto & line : CSVdata) {
         cout << line << endl;
      }
   }


   return retCode;
}