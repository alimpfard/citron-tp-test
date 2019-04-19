# NLP Project 5 - Spellcheck
_by Ali MohammadPur Fard_

## Toplevel

0. The input text is first lexed to produce the tokens
1. For each given token, the word-tree containing the dictionary is filtered to produce probable mistake correction (the filtering is skipped if the given word exists)
2. The suggestions are sorted with minimum Levenstein Edit Distance from the given token.
3. All of the sorted array is reported as probable corrections

## About the filtering method
_(see the theory behind it $\href{theory/doc.pdf}{here}$)_

The filtering method works by modelling a keyboard and finding incremental probabilities for words, and culling the lower probabilities (cutoff is designed to always be at 50%).

This produces anywhere from 0 (no way the given word is right) to around half of the tree (the given word is made of characters spaced exactly at the middle of the keyboard, and all the words in the tree are too); with the norm being around 1/1000th of the entire tree.

The filtering is highly dependant on the heuristic data, with worst-case being $O(n)$ where $n$ is the max number of characters in a word, essentially constant-time in respect to the number of words saved in the tree.
