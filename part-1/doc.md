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
    + A frequency matrix is first generated for Documents x Words
    + Each document is tagged with its (known) label
    + A selective TF is performed over the requested keywords
    + The resulting TF rows are divided by the number of labels containing them, not the specific documents (ITF - Inverse Tag Frequency?)

### Merging
    + The resulting categorised TF-IDFs are merged back into a NxL matrix with a simple sum

### Evaluation
    + A sum pass is performed on the merged matrix and a confidence value is calculated
