# Open Source Search Engine

## Search engine
Due to reverse-indexing, the sites are stored in such way, that the keywords are used as keys. Thus, whenever you type a word, the search engine will spit every indexed by this word site.

## Web crawler
Crawler in order to see the web will divide the work into threads. Then it goes through every link that will be encountered in a given document, and will estimate it's keywords. It stores them by a method called 'reverse-index' (where keyword functions as a key and the values are URL's). This makes it significantly faster than normal indexing, because search engines are using keywords in order to search for websites.
Currently there is no algorithm for ranking pages.