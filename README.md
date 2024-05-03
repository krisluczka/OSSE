# WebCrawler

Crawler goes through every link that will be encountered in a given document, and will estimate it's keywords.
It stores them by a method called 'reverse-index' (where keyword functions as a key and the values are URL's).
This makes it significantly faster than normal indexing, because search engines are using keywords in order to
search for websites.

Currently has no algorithm for ranking pages.
