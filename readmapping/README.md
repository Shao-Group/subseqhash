### Introduction
Code for comparing read mapping between minimizer and SubseqHash with A/B/C ordering. 


### Installation
```
git clone https://github.com/Shao-Group/subseqhash.git
cd subseqhash/readmapping
g++ -o gettable gettable.cpp -std=c++11
g++ -c seeding.cpp -std=c++11
g++ -o refseeding refseeding.cpp seeding.o -std=c++11
g++ -o readseeding readseeding.cpp seeding.o -std=c++11
g++ -o seedmatch seedmatch.cpp seeding.o -std=c++11
```
### Usage
- Data format:
  There are two input files. The first one is the reference genome. It can have multiple sequences from different chromosome. Each sequence takes two lines. First line is the name of it. Second line is the sequence. Here is an example `./data/ref`. The second file is the read file. Each read takes three lines, the first line is the sequence, the second line is the ground-truth alignment, the i-th number `$x_i$` in the second line means the i-th char of the read should be aligned to the position `$x_i$`. The thrid line is the name of reference genome sequence that read should be mapped to (the name must be the same as the one in reference genome file). Here is also an example `./data/input`.

- Generate A/B/C tables: There will be `num` different tables saved in `table_folder` named from 1 to `num`.

```
./gettable k d num table_folder
```

- Do seeding for reference genome. `n`, `k` and `d` are the parameter for SubseqHash with A/B/C ordering. `ref_input` is the input file for reference genome. `table` and `tablenum` can point to the table that would be used. Each sequence in `ref_input` will have a corresponding output file named `name_of_refseq`\_`tablenum` in `ref_seed_folder`.

```
./refseeding n k d ref_input table_folder table_num ref_seed_folder
```

- Do seeding for reads. `read_input` is the input file for reference genome. `table` and `tablenum` can point to the table that would be used. There is only one output file named `tablenum` in `ref_seed_folder`.

```
./readseeding n k d read_input table_folder table_num read_seed_folder
```

- To get seed matches results. `r` is the number of repeating times. It will take the seeds from `ref_seed_folder` and `read_seed_folder`. The seeds must be from table 1 to table `r`. The output format is n/k/d/t, number of seed-matches, number of true seed-matches, ratio of true seed-matches, coverage of true seed-matches, coverage of false seed-matches.

```
./seedmatch n k d r ref_input ref_seed_folder read_seed_folder < read_input
```

- An example on sample data:

```
mkdir ./table
./gettable 15 11 2 ./table

mkdir ./refseed
./refseeding 20 15 11 ./data/ref ./table 1 ./refseed
./refseeding 20 15 11 ./data/ref ./table 2 ./refseed

mkdir ./readseed
./readseeding 20 15 11 ./data/input ./table 1 ./readseed
./readseeding 20 15 11 ./data/input ./table 2 ./readseed

./seedmatch 20 15 11 2 ./data/ref ./refseed ./readseed < ./data/input
```