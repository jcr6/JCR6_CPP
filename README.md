![](https://avatars3.githubusercontent.com/u/47812464?s=200&v=4)

# JCR6 for C++

## NOTICE!

This is still a work in progress! It will not be fully functional what you find here (if in fact you find anything functional at all), and although I will not stop you from cloning this repository already. The zlib license this project will have will ONLY count once this notice has been removed! Until that time, you may only clone/download/view this code for evaluation/testing/studying purposes.


## STATUS

- All this library can now do properly is recognize a JCR6 file as such... Big deal, that was the EASY part... :P
- A heading config can be read now, but at the current time, nearly no JCR6 files have this, so I cannot test this.
- The file table can be read now, however:
  - Patching is not yet possible, so JCR6 files with dependencies can get crazy results
  - I didn't yet write any compression drivers, so currently JCR6 files with a packed file table cannot be read, but... Technically speaking it should be possible already to write such a driver, and if you are crazy enough to do so, it SHOULD work.... :) (I will study how to do that properly myself in C++, but my prime concern is getting the core to work, so this will come later).
- I've put in a function which will load an entire JCR6 entry into a string, a routine I've used a lot in other languages, and since it was easy to implement in C++ it simply had to be there.
- And I also did a function which will put all lines of a text file into a vector<string>. Similar functions have also been used in other languages by me. Some of my GINI parsers have also been based on this.
- Patching implemented although not fully tested yet. This should cover all the reading stuff, as far as non-packed files are concerned. What the best way is to do packed files is another chapter in this story.
- zlib support.... I will investigate if I can include lzma as well.
