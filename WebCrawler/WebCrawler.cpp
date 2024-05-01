/*
    WebCrawler project, Krzysztof Łuczka 2024, MIT license
*/
#include <iostream>
#include <windows.h>
#include <wininet.h>
#include <fstream>
#include <vector>
#include <regex>
#include <string>
#include <map>
#include <sstream>
#include <cctype>
#include <algorithm>
// idk why but it does not work without it
#pragma comment(lib, "wininet.lib")
#include "dictionaries.h"

// damn
typedef std::vector<std::pair<std::string, uint_fast64_t>> keywords;

/*
    Function that downloads webpage content
*/
std::string getSite( const std::string& url ) {
    // session opening
    HINTERNET hInternet = InternetOpenA( "Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0 );
    if ( !hInternet ) {
        std::cerr << "The session wasn't started correctly." << std::endl;
        return "";
    }

    // opening given URL in a session
    HINTERNET hUrl = InternetOpenUrlA( hInternet, url.c_str(), "Accept: text/html", 0, INTERNET_FLAG_RELOAD, 0 );
    if ( !hUrl ) {
        std::cerr << "Invalid URL." << std::endl;
        InternetCloseHandle( hInternet );
        return "";
    }

    // the content
    std::string content("");
    // the buffer
    constexpr DWORD bufferSize( 1024 );
    char buffer[bufferSize];
    DWORD bytesRead( 0 );

    // reading (idk if it is truly optimized)
    while ( InternetReadFile( hUrl, buffer, bufferSize - 1, &bytesRead ) && bytesRead > 0 ) {
        buffer[bytesRead] = '\0'; // null terminator
        content.append( buffer, bytesRead );
    }

    // don't forget this!
    InternetCloseHandle( hUrl );
    InternetCloseHandle( hInternet );

    return content;
}

/*
    Function that extracts every link from a given HTML page
*/
std::vector<std::string*> extractLinks( const std::string& content, const std::string& url ) {
    // regex to find the <a></a>
    std::regex linkRegex( "<a\\s+(?:[^>]*?\\s+)?href[\\s]*=[\\s]*['\"]([^'\"]*?)['\"][^>]*?>" );

    // links vector
    std::vector<std::string*> links;

    // searching for the link
    std::smatch match;
    auto text( content ); // it doesn't work without this lol
    std::string* link;
    while ( std::regex_search( text, match, linkRegex ) ) {
        link = new std::string( match[1].str() ); // extracting

        // validating the url and correcting errors
        if ( link->substr( 0, 7 ) != "http://" && link->substr( 0, 8 ) != "https://" ) {
            if ( link->substr( 0, 2 ) == "./" ) {
                *link = url + link->substr( 2 );
            } else if ( link->find( "/" ) == 0 ) {
                *link = url + link->substr( 1 );
            } else if ( link->find( "www." ) == std::string::npos ) {
                *link = "http://www." + *link;
            } else {
                *link = "http://" + *link;
            }
        }

        links.push_back( link );

        // going further in the webpage
        text = match.suffix();
    }

    return links;
}

/*
    Function that returns the most occuring words in a string
*/
keywords getKeywords( const std::string& text, uint_fast64_t top ) {
    std::map<std::string, uint_fast64_t> wordCount;
    std::istringstream iss( text );
    std::string word;

    // iterating through the whole content
    while ( iss >> word ) {
        // removing all punctuation marks
        word.erase( std::remove_if( word.begin(), word.end(), []( char c ) {
            return std::ispunct( static_cast<unsigned char>(c) );
        }), word.end() );

        // changing to lowercase
        std::transform( word.begin(), word.end(), word.begin(), ::tolower );

        // checking if document's language has the dictionary prepared
        if ( dictionary.find( "pl" ) != dictionary.end() ) {
            const std::vector<std::string>& ignoredWords = dictionary.at( "pl" );
            // if the word is in the dictionary, ignore it and move to the next
            if ( std::find( ignoredWords.begin(), ignoredWords.end(), word ) != ignoredWords.end() )
                continue;
        }
        wordCount[word]++;
    }

    // keywording the keywords
    keywords words;
    for ( const auto& pair : wordCount ) {
        words.push_back( pair );
    }

    // hate to use this way of sorting but it really is better than by hand
    std::sort( words.begin(), words.end(), []( const auto& a, const auto& b ) {
        return a.second > b.second;
    });

    // preventing the potential going out of scope
    if ( top > words.size() ) {
        top = words.size();
    }

    // returning sorted array of keywords
    return keywords( words.begin(), words.begin() + top );
}

/*
    Function that removes specified HTML tags and their content
*/
std::string removeTags( const std::string& text, const std::string& tag ) {
    std::string result;
    std::string startTag( "<" + tag );
    std::string endTag( "</" + tag + ">" );
    bool insideTag( false );

    // iterating through every character
    for ( size_t i = 0; i < text.size(); ++i ) {
        if ( !insideTag ) {
            // checking whether we found the opening tag
            if ( text.substr( i, startTag.size() ) == startTag ) {
                insideTag = true;
                // moving the pointer beyond the tag
                i += startTag.size() - 1;
            } else {
                // appending the character to the text
                result += text[i];
            }
        } else {
            // checking whether we found the closing tag
            if ( text.substr( i, endTag.size() ) == endTag ) {
                insideTag = false;
                // moving the pointer beyond the tag
                i += endTag.size() - 1;
                // adding the space because sometimes the words would concatenate
                result += " ";
            }
        }
    }

    return result;
}

/*
    Function that removes all HTML tags,
    and leaves plain text
*/
std::string rawText( std::string text ) {
    bool insideTag( false );
    std::string result;
    size_t pos( 0 );
    
    // I will change this later, promise!
    text = removeTags( text, "style" );
    text = removeTags( text, "script" );
    text = removeTags( text, "noscript" );
    
    // removing all other html tags
    for ( char c : text ) {
        if ( c == '<' ) {
            insideTag = true;
        } else if ( c == '>' ) {
            insideTag = false;
            result += " ";
        } else if ( !insideTag ) {
            result += c;
        }
    }

    // removing all html special characters
    std::regex htmlSpecialChars( "&[a-zA-Z0-9#]+;" );
    return std::regex_replace( result, htmlSpecialChars, "" );
}

/*
    Function that crawls through given URL
*/
void crawl( std::string url, uint_fast64_t depth = 1 ) {
    // downloading site's content
    std::string content( getSite( url ) );
    
    // i hope there is no memory leak
    std::vector<std::string*> links( extractLinks( content, url ) );

    // deHTMLing
    content = rawText( content );

    // extracting keywords
    keywords topWords( getKeywords( content, 10 ) );

    // debugging
    std::cout << depth << " " << links.size() << "\n";
    for ( const auto& pair : topWords ) {
        std::cout << pair.first << " : " << pair.second << std::endl;
    }

    /*
        * checking to which sites it refers (to increase their score)
        * saves this data (in a file named by hashed link)
        * boom search engine searches
    */

    // going deeper
    --depth;
    if ( depth > 0 ) {
        for ( std::string* l : links ) {
            // crawl my spiders!
            crawl( *l, depth );
        }
    }

    // yes yes, remember to delete your pointers, kids!
    for ( std::string* l : links ) {
        delete l;
    }
}

int main() {
    std::string url;
    uint_fast64_t depth;
    std::cout << "The starting URL >> ";
    std::cin >> url;
    std::cout << "Depth >> ";
    std::cin >> depth;

    crawl( url, depth );
}
