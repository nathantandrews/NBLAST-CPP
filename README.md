# NBLAST-CPP
The goal of this project is to automate the comparison and matching of fly brain neurons. NeuronBLAST, otherwise known as NBLAST (not to be confused with Nucleotide BLAST) is an obfuscated, outdated R package for scoring pairs of neurons based upon their topological similarity. NBLAST works by decomposing a query and target neuron into short segments. The segments are matched using nearest neighbor and cosine similarity and scored using a log-likelihood ratio scoring matrix empirically defined by the statistics of known matches and non-matches. Our work reproduces the core NBLAST methodology in modern C++, improving portability, performance, and extensibility. When NBLAST-CPP(our reimplementation) is completed and tested, the program will be run on the BANC (Brain and Nerve Cord) dataset and the FAFB (Full Adult Fly Brain) dataset, piping its results to seed SANA (Simulating Annealed Network Aligner). SANA then aligns the datasets together and determines high quality matches in a fraction of the time.

# Differences in NBLAST-CPP
A notable difference from the original NBLAST implementation is the usage of midpoints between segments as the comparison points for matching segments together. NBLAST-CPP also uses nanoflann’s implementation (instead of the original, nabor) of a KD-tree for nearest neighbor lookups in nlog(n) time.

# Modes
We have implemented two modes: Query mode and Generator mode.

Query mode compares two or more neurons using the command: 

```nblast++ -q MatchMatrix,RandomMatrix QueryNeuron.swc TargetNeuron1.swc```

MatchMatrix specifies the empirical log-likelihood scoring matrix computed from known neuron matches, whereas RandomMatrix specifies the corresponding matrix computed from randomly sampled neuron pairs.

Generator mode generates both of the matrices to use for Query mode using the command on the dataset:

```nblast++ -g *.swc```

# In Progress
- The generator mode argument parsing is implemented but needs to be integrated with the project
- Testing the KD-Tree’s effectiveness in cutting runtime
- Retrofitting the official NBLAST log-odds ratio matrix to use it for scoring

