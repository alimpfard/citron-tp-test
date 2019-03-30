## NLP Optional Project 0 -- Hamshahri Collection :: Keyword Detection

### Description

An exact copy of my project 2 submission, except 5 keywords are kept instead of 1.

### Algorithm Description

+ Tokenising
    + A normalisation pass is performed on the text
    + All non-word characters are padded with spaces
    + All newlines and tabs are transformed into spaces
    + The text is split with spaces

+ TF-IDF calculation
    + A word is skipped if it is punctuation or a stopword
    + An array of `documents count` ([1, 0...0]) is generated where index 0 keeps the total TF, and index 1 keeps the DF of the word, per new word encountered
    + The specific entry for a word in a document is incremented if it is encountered
    + A final pass is performed on the resulting map-of-lists that calculates TF[idx]/TF[1]

+ Keyword detection
    + Two arrays of length N (where N is the number of keywords needed) is kept, one for the keyword, and one for its max TF-IDF
    + Every iteration over the keyword set, a history of maximums is generated as such:
        + the list max is scanned for the largest TF-IDF smaller than the current TF-IDF
        + if such an entry is found, it is swapped with the current one, and the keyword is placed in the second array in its index
    + The resulting array contains the top N keywords of the document


### Issues

+ The stopword set may contain abnormal text, to resolve this, that is also normalised
+ The stupid number of documents takes forever to process, and uses up entirely too much RAM. can't really solve this


### Code heirarchy
+ Tokeniser - `Lexer` 
    * `normalise:` -- Normalises the given text
    * `split:` -- splits the given text to tokens (normalises if `collate:` has been called)
    * `collate:` -- sets the collation (normalisation set)

+ Token Analyser -- `TokenAnalyser`
    * `with:` -- constructor
    * `split:` -- alias for `lexer::'split:'`
    * `isRemovalCandidate:` -- checks if a token should be removed
    * `tag:pre:post:` -- irrelevant
    * `tagPhrases:pre:post:` -- irrelevant
    * `consolidate:pre:post:` -- Splits, and calculates TF-IDF for the given documents
    * `detectKeywords:forDocument:pre:post:` -- Finds the 5 most frequent keywords in all the documents
