# Progress Log
This log will be used to track what I am doing, different methods that I try and used to save links and other resources that I use. 
## Getting the data from Wikipedia
Wikipedia provides lots of different data formats to download data. However the site is very confusing to use. I eventually found a way to downlaod just a current snapshot of all english pages which takes up around 16GB

1) Downloaded the XML dump files from https://dumps.wikimedia.org/enwiki/20240401/ (pages articles multistream)
2) Verified files against checksum to ensure nothing is corrupt 
3) Uncompress the bz2 file which took around half an hour. It expaneded to a size of 100GB
3) Need to find a way to extract information from the giant xml file.

## Selecting a suitable XML parser

- I initally selected [PUGIXML](https://pugixml.org/) as the XML parser of choice due to it being very small, simple, and well documented. However, after doing further research, I found that using a DOM based parser would not be suitable due to the size of the files. and instead a SAX based parser is required.
- Instead, I will use libxml2 and [libxml++](https://libxmlplusplus.sourceforge.net/) (a C++ wrapper for libxml2). Looks like a pain to install.
