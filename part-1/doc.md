# Text Processing -- Keyword Evaluation
### _(Project #2)_

## High-level alg
+ Tokenise
+ Remove stopwords and punctuation
+ Calculate "categorised" TF-IDF for each requested word
+ Merge back
+ Test scores and report error-rate

## Finer grained details -- Issues encountered
+ terms cannot be matched against, requires a window the size of the term
 + not requested anyway

### Tokenisation
    + _Reported at project #1_

### Stopword removal
    + Stopwords were retrieved from source: github.com/...
    + They're detected in `isStopword` and filtered away in `analyse:`

### "categorised" TF-IDF
