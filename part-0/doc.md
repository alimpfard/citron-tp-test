# Basic Persian Lexer

## General lexing method

0. The text is first cleaned
    + \\n -\> ' '
    + \\t -\> ' '

1. Then it is split with the given PCRE regex [1]()
    + ```[،.,:;'"\!\@\#\$\%\^\&\*\(\)؟_\+=\-0987654321\`]|[\p{L}\p{N}\d-[،\.,:;'"\!\@\#\$\%\^\&\*\(\)؟_\+=\-0987654321\`]]+```

## The issues that were encountered

0. grouping non-words together caused the symbols to be grouped as well
    + e.g. `'!!!'` was seen as a single token, not as three `'!'` tokens

1. half-spaces proved to be annoying, so we simply consider them part of a word, but not part of punctuation

2. splitting in such a manner proved annoying (some parts of the text were simply too specific to capture in a regex)
    + Our response to this issue was to simply add spaces around parts we had detected, and leave the structure of the text intact
    + The resulting text would be easily split by spaces, as we have already introduced spaces around punctuation

3. Emojis proved fatal, since the Zero-Width Non-Joiner character was considered a single token, so we simply added that to the list
    + This, however, would cause some specially crafted text to appear as a single token: `word <ZWNJ> word` would be a single token
