#include "common.h"
#include "utils.h"

Dna getRevCompl(Dna const & nucleotide)
{
        if (nucleotide == (Dna)'A')
                return (Dna)'T';
        if (nucleotide == (Dna)'T')
                return (Dna)'A';
        if (nucleotide == (Dna)'C')
                return (Dna)'G';
        if (nucleotide == (Dna)'G')
                return (Dna)'C';
        return (Dna)'N';
}

Dna5String doRevCompl(Dna5String seq)
{
        Dna5String allSeq;
        append(allSeq,seq);
        allSeq += "NNN";
        Dna5String revComplGenome;
        resize(revComplGenome, length(seq));
        for (unsigned i = 0; i < length(seq); ++i)
        {
                revComplGenome[length(seq) - 1 - i] = getRevCompl(seq[i]);
        }
        allSeq += revComplGenome;
        return allSeq;
}

String<Dna5String> defineKmers(int kmerlength)
{
        String<Dna5String> bases;
        appendValue(bases, "A");
        appendValue(bases, "G");
        appendValue(bases, "C");
        appendValue(bases, "T");

        String<Dna5String> kmers;

        kmers = bases;

        for(int j = 0; j < kmerlength-1; j++)
        {
                String<Dna5String> temp;

                for(int k = 0; k < length(kmers); k++)
                {
                        for(int m = 0; m < length(bases); m++)
                        {
                                String<Dna> kmer = bases[m];
                                kmer += kmers[k];
                                appendValue(temp,kmer);
                        }
                }
                kmers = temp;
                clear(temp);
        }

        return kmers;
}

void count(String<Dna5String> kmers, Dna5String seq, int klen, int counts[])
{
        typedef boost::unordered_map<string, int> unordered_map;
        unordered_map map;
        //puts all the kmers into an unordered map
        for(int i = 0; i < length(kmers); i++)
        {
                string meh;
                assign(meh,kmers[i]);
                map.emplace(meh, 0);
        }

        for(int i = 0; i <= length(seq)-klen; i++)
        {
                string meh;
                assign(meh,infix(seq, i, i+klen));
                map[meh]++;
        }

        for(int i = 0; i < length(kmers); i++)
        {
                string meh;
                assign(meh,kmers[i]);
                counts[i] = (int)map[meh];
        }
}

void count(String<Dna5String> kmers, Dna5String seq, int klen, long long int counts[])
{
        typedef boost::unordered_map<string, long long int> unordered_map;
        unordered_map map;
        //puts all the kmers into an unordered map
        for(int i = 0; i < length(kmers); i++)
        {
                string meh;
                assign(meh,kmers[i]);
                map.emplace(meh, 0);
        }

        for(int i = 0; i <= length(seq)-klen; i++)
        {
                string meh;
                assign(meh,infix(seq, i, i+klen));
                map[meh]++;
        }

        for(int i = 0; i < length(kmers); i++)
        {
                string meh;
                assign(meh,kmers[i]);
                counts[i] = (long long int)map[meh];
        }
}

void markov(String<Dna5String> kmers, Dna5String seq, int klen, long long int counts[], int markovOrder, double kmerProb[])
{
        //count the kmers using markov order
        int newmarkov = markovOrder + 1;
        String<Dna5String> markovKmers = defineKmers(newmarkov);

        //count the occurances of the markov kmers
        int markovKcounts [length(markovKmers)];
        count(markovKmers, seq, newmarkov, markovKcounts);

        //sum the total number of kmers
        int sumCounts = 0;
        for(int i = 0; i < length(markovKmers); i++)
                sumCounts = sumCounts + markovKcounts[i];

        //calculate frequency of kmers
        double markovFreq [length(markovKmers)];
        for(int i = 0; i < length(markovKmers); i++)
        {
                markovFreq[i] = (double)markovKcounts[i] / (double)sumCounts;
        }

        double total = 0.0;

        //for each kmer in kmers
        for(int i = 0; i < length(kmers); i++)
        {
                double prob = 1.0;
                Dna5String kmer = kmers[i];

                for(int l = 0; l < length(kmer); l++)
                {
                        int j = l + markovOrder + 1;
                        Infix<Dna5String>::Type inf = infix(kmer,l,j);

                        for(int m = 0; m < length(markovKmers); m++)
                        {
                                if(inf == markovKmers[m])
                                {
                                        prob = prob * markovFreq[m];
                                } else {
                                }
                        }

                        if(j == length(kmer))
                        {
                                break;
                        }
                }
                kmerProb[i] = prob;
                total = total + prob;
        }
}

//I want it to return an array with element 0 is the smallest
void recordall(int nohits, double hits[], double value, int seqcurrpos, int hitpos[])
{
        //if value is smaller than the current largest, lets knock it off
        if(value < hits[nohits-1]){

                hits[nohits-1] = value;
                hitpos[nohits-1] = seqcurrpos; //copy position move
                int j;
                double temp;
                int temppos;

                //now, lets see if our new value is smaller than any of the others
                //iterate through the rest of the elements
                int i = nohits-1;
                while(i >= 0 && hits[i] < hits[i-1])
                {
                        temp = hits[i-1];
                        temppos = hitpos[i-1];

                        hits[i-1] = hits[i];
                        hitpos[i-1] = hitpos[i];

                        hits[i] = temp;
                        hitpos[i] = temppos;

                        i--;
                }
        }
}
