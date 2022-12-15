### Introduction
Code for comparing SubseqHash with different ordering. First generated the order with tables A/B/C, then compute the similarity of neighboring strings in an order.


### Installation
```
git clone https://github.com/Shao-Group/subseqhash.git
cd subseqhash/ordering
g++ -o getABCorder getABCorder.cpp -std=c++11
g++ -o statistics statistics.cpp -std=c++11
```
### Usage
- To generate an order and tables A/B/C:
  - Run `./getABCorder k d m path_to_stringlist path_to_table` to generate an order and tables A/B/C. Only first m strings in the ordering will be saved to `path_to_stringlist`. `path_to_stringlist` contains m lines, each line contains a string of length k, a double value $\omega$ and an int value $\phi$. `path_to_table` saves tables A/B/C. Table A contains `k` matrix, each matrix is `d` * 4. Table B also contains `k` matrix, each matrix is also `d` * 4. Table C only has one matrix, the size of it is `k` * 4.

- To get the statistics results of the generated order:
  - Run `./statistics k m path_to_stringlist`. `m` must be equal to or less than the number of the strings saved in `path_to_stringlist`.
