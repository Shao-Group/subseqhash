### Introduction
Code for comparing SubseqHash with minimizer on the seeding step of bucketing related sequences. For this purpose, sequences that share at least one common seed are put into the same bucket.

The `sample-reads` subdirectory contains read-files as well as ground truth pairs of datasets used in the paper.
The `tables` subdirectory contains all ABC tables used.

### Installation
```
git clone https://github.com/Shao-Group/subseqhash.git
cd subseqhash/bucketing
make product
```
### Usage
- To generate bucketing pairs using SubseqHash:
  - Run `./genSubseqSeeds.out readFile n k d tableName` to generate seeds for a read-file, change `NUMTHREADS` in `genSubseqSeeds.cpp` to allow parallelization. Example: `./genSubseqSeeds.out sample-reads/SRX533603-sample10k.efa 30 19 11 tables/ABC30191.table`. This call creates a directory `sample-reads/SRX533603-sample10k-seeds-ABC30191.table-n30-k19-d11/` which contains all the seed-files, one for each read in the read-file.
  - Run `./bucketBySeeds.out seedDir numReads` to bucket the reads according to the seed-files. Example: `./bucketBySeeds.out sample-reads/SRX533603-sample10k-seeds-ABC30191.table-n30-k19-d11/ 10000`. This call creates a file named `bucket-n10000.all-pair` in the given seed-directory.
  
- To generate bucketing pairs using minimizer: Run `./bucketByMinimizer.out readFile w k`. Note that `w` here is the number of `k`-mers in a window. Example: `./bucketByMinimizer.out sample-reads/SRX533603-sample10k.efa 6 10`. This call creates a file named `SRX533603-sample10k-bucketMinimizer-w6-k10.all-pair`, it corresponds to the right most point of `Minimizer 15` of Figure 8(a) in [our paper]().

- To generate bucketing pairs using plain kmer: Run `./bucketByKMer.out readFile k`. Example: `./bucketByKMer.out sample-reads/SRX533603-sample10k.efa 15`. This call creates a file named `SRX533603-sample10k-bucketKMer-k15.all-pair`.
  
- The `bucketFileOp.js` script can be used to take union/intersection of bucketing pair results. It requires the [k8](https://github.com/attractivechaos/k8) library.
  - Example for obtaining the union of two result files (used for repeating SubseqHash): `./bucketFileOp.js union SRX533603-ABC30191.table/bucket-n10000.all-pair SRX533603-ABC30192.table/bucket-n10000.all-pair > union2.all-pair`.
  - Example for obtaining the intersection of two files (used for comparing bucketing results versus the ground truth): `./bucketFileOp.js intersection sample-reads/SRX533603-sample10k.truepairs SRX533603-ABC30191.table/bucket-n10000.all-pair > ABC30191.intersection`.
