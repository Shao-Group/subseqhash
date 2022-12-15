### Introduction
Code for comparing pairwise sequence alignment between minimizer and SubseqHash with A/B/C ordering. 


### Installation
```
git clone https://github.com/Shao-Group/subseqhash.git
cd subseqhash/pairwisealignment
g++ -o seqsimulation seqsimulation.cpp -std=c++11
g++ -c seeding.cpp -std=c++11
g++ -o subseqhash subseqhash.cpp seeding.o -std=c++11
```
### Usage
- To simulate sequences:
  - Run `./seqsimulation l e n > data_file` to simulate pairs of sequences. `l` is the length of random generated sequence in each pair. `n` is the number of pairs. `e` is a double number which is the mutation rate for each position. `data_file` is the path to save the generated data. In the file, there are 3 * n lines. In every three lines, the first two line are two sequences, the third line is the ground-truth alignment. There are `l` numbers in the third line. The i-th number `$x_i$` means the i-th char in first sequence should be aligned to the `$x_i$`-th number in second sequence. If the number is -1, it means this char is replaced or deleted.
 
- To get the results:
  - Run `./subseqhash n k d r < data_file` to get the sequence alignment results using SubseqHash. `n`, `k` and `d` are the parameter for SubseqHash with A/B/C ordering. 
  `t` is the number of repeating times. It will first randomly generate A/B/C tables. Then calculate the seed-matches, true/false seed-matches, and the coverage of true/false seed-matches. The output format is n/k/d/t, number of seed-matches, number of true seed-matches, ratio of true seed-matches, coverage of true seed-matches, coverage of false seed-matches.