### Introduction
SubseqHash is a seeding method that uses minimized subsequence as seeds. This repository provides source code for 5 different experiments in different folders.
The folder `ordering` contains code for generating an A/B/C order and computing the similarity of neighboring strings in an order. 
`hashcollision` contains code for computing the probability of hash collision of different methods.
`pariwisealignment` has code for using SubseqHash in pairwise sequence alignment. `readmaping` has code for mapping reads to reference genome with SubseqHash.
`overlap-detection` provides code for comparing SubseqHash with minimizer on the seeding step of overlap detection.

### Usage

The repository can be installed with following command. The detailed instruction of each experiment can be found in the corresponding folder.

```
git clone https://github.com/Shao-Group/subseqhash.git
``` 