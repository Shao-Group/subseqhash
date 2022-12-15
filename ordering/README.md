### Introduction
Code for comparing SubseqHash with minimizer on the seeding step of bucketing related sequences. For this purpose, sequences that share at least one common seed are put into the same bucket.

The read-files (.efa) and ground truth pairs (.truepairs) 
of all datasets used in the paper as well as the random tables used are
available [here]().

### Installation
```
git clone https://github.com/Shao-Group/subseqhash.git
cd subseqhash/bucketing
make product
```
### Usage
- To generate bucketing pairs using SubseqHash:
  - Run `./genSubseqSeeds.out readFile n k d tableName` to generate seeds for a read-file, change `NUMTHREADS` in `genSubseqSeeds.cpp` to allow parallelization. Example: `./genSubseqSeeds.out SRX533603.efa 30 19 11 ABC30191.table`. This call creates a directory `SRX533603-seeds-ABC30191.table-n30-k19-d11/` which contains all the seed-files, one for each read in the read-file.
  - Run `./bucketBySeeds.out seedDir numReads` to bucket the reads according to the seed-files. Example: `./bucketBySeeds.out SRX533603-seeds-ABC30191.table-n30-k19-d11/ 10000`. This call creates a file named `bucket-n10000.all-pair` in the given seed-directory.
  
- To generate bucketing pairs using minimizer: Run `./bucketByMinimizer.out readFile w k`. Note that `w` here is the number of `k`-mers in a window. Example: `./bucketByMinimizer.out SRX533603.efa 6 10`. This call creates a file named `SRX533603-bucketMinimizer-w6-k10.all-pair`, it corresponds to the right most point of `Minimizer 15` of Figure 8(a) in [our paper]().

- To generate bucketing pairs using plain kmer: Run `./bucketByKMer.out readFile k`. Example: `./bucketByKMer.out SRX533603.efa 15`. This call creates a file named `SRX533603-bucketKMer-k15.all-pair`.
  
- The `bucketFileOp.js` script can be used to take union/intersection of bucketing pair results. It requires the [k8](https://github.com/attractivechaos/k8) library.
  - Example for obtaining the union of two result files (used for repeating SubseqHash): `./bucketFileOp.js union SRX533603-seeds-ABC30191.table/bucket-n10000.all-pair SRX533603-seeds-ABC30192.table/bucket-n10000.all-pair > union2.all-pair`.
  - Example for obtaining the intersection of two files (used for comparing bucketing results versus the ground truth): `./bucketFileOp.js intersection SRX533603-sample10k.truepairs SRX533603-seeds-ABC30191.table/bucket-n10000.all-pair > ABC30191.intersection`.
