### Introduction
Code for comparing SubseqHash with minimizer on the seeding step of overlap detection. For this purpose, sequences that share at least one common seed are considered overlapping.

The read-files (.efa) and ground truth pairs (.truepairs) 
of all datasets used in the paper as well as the random tables used are
available [here]().

### Installation
```
git clone https://github.com/Shao-Group/subseqhash.git
cd subseqhash/overlap-detection
make product
```
### Usage
- To generate overlapping pairs using SubseqHash:
  - Run `./genSubseqSeeds.out readFile n k d tableName` to generate seeds for a read-file, change `NUMTHREADS` in `genSubseqSeeds.cpp` to allow parallelization. Example: `./genSubseqSeeds.out SRX533603.efa 30 19 11 ABC30191.table`. This call creates a directory `SRX533603-seeds-ABC30191.table-n30-k19-d11/` which contains all the seed-files, one for each read in the read-file.
  - Run `./overlapBySeeds.out seedDir numReads` to identify overlapping reads according to the seed-files. Example: `./overlapBySeeds.out SRX533603-seeds-ABC30191.table-n30-k19-d11/ 10000`. This call creates a file named `overlap-n10000.all-pair` in the given seed-directory.
  
- To generate overlapping pairs using minimizer: Run `./overlapByMinimizer.out readFile w k`. Note that `w` here is the number of `k`-mers in a window. Example: `./overlapByMinimizer.out SRX533603.efa 6 10`. This call creates a file named `SRX533603-overlapMinimizer-w6-k10.all-pair`, it corresponds to the right most point of `Minimizer 15` of Figure 8(a) in [our paper]().

- To generate overlapping pairs using plain kmer: Run `./overlapByKMer.out readFile k`. Example: `./overlapByKMer.out SRX533603.efa 15`. This call creates a file named `SRX533603-overlapKMer-k15.all-pair`.
  
- The `overlapFileOp.js` script can be used to take union/intersection of overlap detection results. It requires the [k8](https://github.com/attractivechaos/k8) library.
  - Example for obtaining the union of two result files (used for repeating SubseqHash): `./overlapFileOp.js union SRX533603-seeds-ABC30191.table/overlap-n10000.all-pair SRX533603-seeds-ABC30192.table/overlap-n10000.all-pair > union2.all-pair`.
  - Example for obtaining the intersection of two files (used for comparing reported overlapping pairs versus the ground truth): `./overlapFileOp.js intersection SRX533603-sample10k.truepairs SRX533603-seeds-ABC30191.table/overlap-n10000.all-pair > ABC30191.intersection`.
