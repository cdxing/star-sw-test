#include "LKH.h"

/*
 * Each time a trial has resulted in a shorter tour the candidate set is
 * adjusted (by AdjustCandidateSet). The ResetCandidates function resets
 * the candidate set. The original order is re-established (using, and 
 * edges with Alpha == INT_MAX are excluded.
 *
 * The function is called from FindTour and OrderCandidates.
 */

void ResetCandidateSet()
{
    Node *From;
    Candidate *NFrom, *NN, Temp;

    From = FirstNode;
    /* Loop for all nodes */
    do {
        if (!From->CandidateSet)
            continue;
        /* Reorder the candidate array of From */
        for (NFrom = From->CandidateSet; NFrom->To; NFrom++) {
            if (InInputTour(From, NFrom->To))
                NFrom->Alpha = 0;
            Temp = *NFrom;
            for (NN = NFrom - 1;
                 NN >= From->CandidateSet &&
                 (Temp.Alpha < NN->Alpha ||
                  (Temp.Alpha == NN->Alpha && Temp.Cost < NN->Cost)); NN--)
                *(NN + 1) = *NN;
            *(NN + 1) = Temp;
        }
        NFrom--;
        /* Remove included edges */
        while (NFrom >= From->CandidateSet && NFrom->Alpha == INT_MAX)
            NFrom--;
        NFrom++;
        NFrom->To = 0;
    }
    while ((From = From->Suc) != FirstNode);
}
