#pragma once
#ifndef WEBCRAWLER_H
#define WEBCRAWLER_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <wininet.h>

#include <string>
#include <regex>

#include <vector>
#include <map>

#include <cctype>
#include <algorithm>

#include <time.h>
#include <thread>
#include <future>
// idk why but it does not work without it
#pragma comment(lib, "wininet.lib")
#include "dictionaries.h"

// damn
typedef std::vector<std::pair<std::string, uint_fast64_t>> keywords;
typedef std::map<std::string, std::vector<std::string*>> keywords_map;

/*
    Function that downloads webpage content
*/
std::string get_site( const std::string& url ) {
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
    std::string content( "" );
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
    Function that validates the URL
*/
inline bool validate_url( const std::string& url ) {
    std::regex urlRegex( "^(http|https)://[a-zA-Z0-9-.]+\\.[a-zA-Z]{2,}(?:/[^\\s]*)?$" );
    return std::regex_match( url, urlRegex );
}

/*
    Function that extracts
        * links
        * title
        * language
    from a HTML document
*/
void extract_info( const std::string& url, std::string& content, std::vector<std::string*>& links, std::string& title, std::string& language, bool extract_links = true ) {
    // regexes
    std::regex titleRegex( "<title>(.*?)</title>", std::regex::icase );
    std::regex langRegex( "<html(?:[^>]*\\s+)?(?:lang|xml:lang)[\\s]*=[\\s]*['\"]([^'\"]*)['\"]", std::regex::icase );
    std::smatch match;

    // searching for the title
    if ( std::regex_search( content, match, titleRegex ) )
        title = remove_diactric( match[1].str() );
    else title = "";

    // removing title's punctuation marks
    title.erase( std::remove_if( title.begin(), title.end(), []( char c ) {
        return std::ispunct( static_cast<unsigned char>(c) );
    }), title.end() );

    // changing to lowercase
    std::transform( title.begin(), title.end(), title.begin(), ::tolower );

    // searching for the language
    if ( std::regex_search( content, match, langRegex ) )
        language = match[1].str().empty() ? match[2].str() : match[1].str();
    else language = "en";

    // searching for the link
    if ( extract_links ) {
        std::regex linkRegex( "<a\\s+(?:[^>]*?\\s+)?href[\\s]*=[\\s]*['\"]([^'\"]*?)['\"][^>]*?>" );
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

            // adding the slash
            if ( link->at( link->size() - 1 ) != '/' )
                *link += "/";

            // validating the URL
            if ( validate_url( *link ) )
                links.push_back( link );

            // going further in the webpage
            text = match.suffix();
        }
    }
}

/*
    Function that extracts meta keywords
*/
std::string extract_meta_keywords( const std::string& content ) {
    std::regex keywordsRegex( "<meta\\s+name=['\"]keywords['\"]\\s+content=['\"](.*?)['\"]\\s*/?>", std::regex::icase );
    std::smatch match;

    if ( std::regex_search( content, match, keywordsRegex ) ) {
        std::string keywords( match[1].str() );

        // removing all punctuation marks
        keywords.erase( std::remove_if( keywords.begin(), keywords.end(), []( char c ) {
            return std::ispunct( static_cast<unsigned char>(c) );
        }), keywords.end() );

        // removing diactric characters
        keywords = remove_diactric( keywords );

        // changing to lowercase
        std::transform( keywords.begin(), keywords.end(), keywords.begin(), ::tolower );

        return keywords;
    } else return "";
}

/*
    Function that returns the most occuring words in a string
*/
keywords extract_keywords( const std::string& content, std::string& title, std::string& language, std::string& meta_keywords, uint_fast64_t top ) {
    std::map<std::string, uint_fast64_t> wordScores;
    std::istringstream cstream( content ), tstream( title ), mstream( meta_keywords );
    std::string word;
    uint_fast64_t additional_size( 0 );

    // iterating through the whole content
    while ( cstream >> word ) {
        // removing all punctuation marks
        word.erase( std::remove_if( word.begin(), word.end(), []( char c ) {
            return std::ispunct( static_cast<unsigned char>(c) );
        }), word.end() );

        // changing to lowercase
        std::transform( word.begin(), word.end(), word.begin(), ::tolower );

        // checking if document's language has the dictionary prepared
        if ( dictionary.find( language ) != dictionary.end() ) {
            const std::vector<std::string>& ignoredWords( dictionary.at( language ) );
            // if the word is in the dictionary, ignore it and move to the next
            if ( std::find( ignoredWords.begin(), ignoredWords.end(), word ) != ignoredWords.end() )
                continue;
        }

        // counting up the occurences
        if ( word != "" )
            wordScores[word]++;
    }

    // keywording the keywords
    keywords words;
    for ( const auto& pair : wordScores ) {
        words.push_back( pair );
    }

    // hate to use this way of sorting but it really is better than by hand
    std::sort( words.begin(), words.end(), []( const auto& a, const auto& b ) {
        return a.second > b.second;
    });

    // preventing the potential going out of scope
    if ( top > words.size() )
        top = words.size();

    // removing unnecessary words
    keywords result( words.begin(), words.begin() + top );

    // appending title words
    while ( tstream >> word ) {
        // checking if document's language has the dictionary prepared
        if ( dictionary.find( language ) != dictionary.end() ) {
            const std::vector<std::string>& ignoredWords( dictionary.at( language ) );
            // if the word is in the dictionary, ignore it and move to the next
            if ( std::find( ignoredWords.begin(), ignoredWords.end(), word ) != ignoredWords.end() )
                continue;
        }
        
        // appending the title
        if ( word != "" ) {
            result.push_back( std::make_pair( word, 1000 ) );
            ++additional_size;
        }
    }

    // appending meta keywords
    while ( mstream >> word ) {
        // checking if document's language has the dictionary prepared
        if ( dictionary.find( language ) != dictionary.end() ) {
            const std::vector<std::string>& ignoredWords( dictionary.at( language ) );
            // if the word is in the dictionary, ignore it and move to the next
            if ( std::find( ignoredWords.begin(), ignoredWords.end(), word ) != ignoredWords.end() )
                continue;
        }

        // appending the title
        if ( word != "" ) {
            result.push_back( std::make_pair( word, 1000 ) );
            ++additional_size;
        }
    }

    top += additional_size;

    // returning sorted array of keywords
    return result;
}

/*
    Function that extracts raw content from HTML
    To rewrite!
*/
std::string extract_content( const std::string& content ) {
    std::regex tagsRegex( "<(?:script|style|noscript)[^>]*>[\\s\\S]*?<\\/\\s*(?:script|style|noscript)>" );
    std::regex htmlRegex( "<[^>]*>" );
    std::regex specialRegex( "&[a-zA-Z0-9#]+;" );

    // removing <script>, <style> and <noscript>
    std::string result( std::regex_replace( content, tagsRegex, " " ) );

    // removing other tags
    result = std::regex_replace( result, htmlRegex, " " );

    // removing special characters
    result = std::regex_replace( result, specialRegex, " " );

    return remove_diactric( result );
}

/*
    Function that crawls through given URL
*/
uint_fast64_t crawl( std::string url, uint_fast64_t depth, std::vector<std::string*>& all_time_links, keywords_map& index_map ) {
    // downloading site's content
    std::string content( get_site( url ) );

    // extracting info about the document
    std::vector<std::string*> links;
    std::string language;
    std::string title;
    uint_fast64_t amount( 0 );
    extract_info( url, content, links, title, language, (depth > 1) );

    // extracting meta keywords
    std::string meta_keywords( extract_meta_keywords( content ) );
    // deHTMLing
    content = extract_content( content );
    // extracting keywords
    keywords topWords( extract_keywords( content, title, language, meta_keywords, 5 ) );

    // saving indexed site's data
    std::cout << url << "\n\n";
    for ( const auto& pair : topWords ) {
        // if the vector from map doesn't exist, we create
        if ( index_map.find( pair.first ) == index_map.end() )
            index_map[pair.first] = std::vector<std::string*>();

        // probably it would have been easier if i used unordered_set
        // whoops
        auto it = std::find_if( index_map[pair.first].begin(), index_map[pair.first].end(),
            [&url]( const std::string* l ) {
                return *l == url;
            } );

        // pushin
        if ( it == index_map[pair.first].end() )
            index_map[pair.first].push_back( new std::string( url ) );
    }

    --depth;
    // crawl my spiders!
    if ( depth > 0 ) {
        for ( std::string* l : links ) {
            // searching whether link was already indexed
            auto it = std::find_if( all_time_links.begin(), all_time_links.end(),
                [l]( std::string* ptr ) {
                    return *ptr == *l;
                } );

            // if it wasn't
            if ( it == all_time_links.end() ) {
                std::string* new_link( new std::string );
                *new_link = *l;
                all_time_links.push_back( new_link );
                crawl( *l, depth, all_time_links, index_map );
            }
        }
        amount = links.size();
    } else amount = 1;

    // yes yes, remember to delete your pointers, kids!
    for ( std::string* l : links )
        delete l;

    return amount;
}

/*
    Function that indexes and crawls through
    the Internet
*/
uint_fast64_t index( uint_fast64_t depth, uint_fast64_t threads = 0 ) {
    // getting the amount of threads to run
    SYSTEM_INFO sysInfo;
    GetSystemInfo( &sysInfo );
    // times two just for experiments
    threads = sysInfo.dwNumberOfProcessors;

    // important stuff
    keywords_map index_map;
    std::vector<std::string*> searched_links;
    std::string line, key, word;

    // copying files just in case
    std::ifstream urls_o( "urls.txt", std::ios::binary );
    std::ofstream urls_c( "urls_copy.txt", std::ios::binary );
    std::ifstream index_o( "index.txt", std::ios::binary );
    std::ofstream index_c( "index_copy.txt", std::ios::binary );
    urls_c << urls_o.rdbuf();
    index_c << index_o.rdbuf();
    urls_o.close();
    urls_c.close();
    index_o.close();
    index_c.close();

    // opening files to read
    std::fstream index( "index.txt", std::ios::in );
    std::fstream urls( "urls.txt", std::ios::in );

    // loading indexed links
    while ( std::getline( urls, line ) )
        searched_links.push_back( new std::string( line ) );

    // loading web index
    while ( std::getline( index, line ) ) {
        std::istringstream iss( line );
        // loading the first word (the keyword)
        if ( iss >> key ) {
            // if the vector from map doesn't exist, we create
            if ( index_map.find( key ) == index_map.end() )
                index_map[key] = std::vector<std::string*>();

            // loading next words
            while ( iss >> word )
                index_map[key].push_back( new std::string( word ) );
        }
    }

    index.close();
    urls.close();

    // opening files to save
    std::fstream indexx( "index.txt", std::ios::out );
    std::fstream urlss( "urls.txt", std::ios::out );

    /*
        The threads of crawling

        They take a random URL's from already
        indexed sites and crawl through them with given depth.
    */
    uint_fast64_t links_amount( 0 );
    if ( searched_links.size() ) {
        std::uint_fast64_t id;
        std::future<uint_fast64_t>* links_amounts( new std::future<uint_fast64_t>[threads] );
        for ( uint_fast64_t i( 0 ); i < threads; ++i ) {
            id = rand() % searched_links.size();
            links_amounts[i] = std::async( std::launch::async, crawl, *searched_links[id], depth, std::ref( searched_links ), std::ref( index_map ) );
        }
        for ( uint_fast64_t i( 0 ); i < threads; ++i )
            links_amount += links_amounts[i].get();
        delete[] links_amounts;
    } else {
        links_amount = crawl( "https://www.wierszespodtaboreta.pl/", depth, searched_links, index_map );
    }


    // saving index map file and cleaning afterwards
    for ( auto& pair : index_map ) {
        indexx << pair.first << " ";
        for ( const std::string* l : pair.second ) {
            indexx << *l << " ";
            delete l;
        }
        indexx << "\n";
        pair.second.clear();
    }
    index_map.clear();
    indexx.flush();
    indexx.close();

    // saving urls and cleaning afterwards
    for ( std::string* l : searched_links ) {
        urlss << *l << "\n";
        delete l;
    }
    searched_links.clear();
    urlss.flush();
    urlss.close();

    // returning the total amount of sites indexed
    return links_amount;
}

#endif