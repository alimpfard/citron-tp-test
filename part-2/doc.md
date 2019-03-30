# NLP Project #3
## -- Normalisation and Token Tagging
by Ali Mohammad Pur Fard

## High-level alg
+ Tokenise [interleaved with Normalise]
+ Remove stopwords
+ Pass through a state machine that tags the tokens

## Finer-grained details -- Issues encountered
+ detection of URLs was too troublesome, so we left them out

## State machine descr
states:
+ nothing : initial state
+ int     : seen a number
+ hashtag0: seen a '#'
+ float   : seen a dot after an int
+ ipaddr  : seen a dot after a float
+ ipaddr1 : 3 quartets of an IP
+ ipaddr2 : complete IP
+ hashtag1: complete hashtag

