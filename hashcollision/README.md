### Introduction
Code for comparing the Probability of Hash Collision minimizer and SubseqHash with different ordering. 


### Installation
```
git clone https://github.com/Shao-Group/subseqhash.git
cd subseqhash/hashcollision
g++ -o simulate simulate.cpp -std=c++11
g++ -o hashcollision hashcollision.cpp -std=c++11
```
### Usage
- To simulate pairs of strings:
  - Run `./simulate k n e data_directory` to simulate pairs of strings. `k` is the length of random generated string in each pair. `n` is the number of pairs. `e` is maximal edit distance between two strings in each pair. `data_directory` is the path of the directory that save `e` different files named from 1 to e. In file `i`, there are 2 * n lines, every two lines contain two strings, the first one is of lenth `k` and the edit distance of the two strings is `i`.

- To get the probability of hash collision:
  - Run `./hashcollision k d data_file` to get the probability of hash collision of SubseqHash on the pairs of strings in the input file `data_file`. `k` and `d` are the parameter for SubseqHash with A/B/C ordering. It will first randomly generate A/B/C tables. The output format is `probability of hash collision`, `number of hash collision`, `number of valid pairs`, `number of unvalid pairs`. (Unvalid pairs means the length of one string in the pairs is smaller than `k`)