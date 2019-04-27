# Project 6
## NGram
_by Ali Mohammad Pur Fard_

### Toplevel
1. Sentences are detected and yielded as streams of tokens that end in dots (more punctuation could easily be added)
2. A Sparse Array of dimensionality N is created to store the n-gram count with the below format:
  + dimension indices: [0: count] [1: start] ... [x: end] ... [|v|: some_token]
3. a single pass over the entire corpus' sentences is performed with a sliding window of size N, incrementing the value of the window, and the count (value with the last dimension set to 0)
4. once this pass is over, the values of the last dimension are extracted at index 0 (count) and the count array is divided by the extracted values to get the n-gram probability table

### Bonus round : sparse array operations
Sparse Array operations are performed by my Sparse-NDArray library ($\href{https://github.com/alimpfard/sparse-ndarray}{repository}$)
which is reasonably fast and doesn't use up silly amounts of RAM.

## Part 0 (bigram on dummy texts)
19170 different sentences made of tokens 'a', 'b', 'c' and 'd' are generated through the below expression, and are later shuffled and split into 191 documents of roughly 100 sentences.
```hs
writeFile "corpusData.txt" $
  intersperse ' ' $
  intercalate ".\n" $
  concat $ nub $
    (combinations <$> [1..7]) <*> [concat $ take 5 $ repeat "abcd"]
```

## Part 1 (bigram on real texts)
Absolutely no change is required in this transition, since the tokens are detected and indexed as data flows in.

## Part 2 (ngram on real texts)
A simple parameter change realises this requirement (`ngram-N -> 3`)

## Calculating the probability of a given sentence
Is performed by applying the toplevel algorithm, but multiplying a pr value with the window probability values.
