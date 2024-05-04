/*
    Open Source Search Engine - OSSE
    
    Krzysztof Łuczka 2024, MIT license
*/
#include <iostream>
#include <chrono>
#include <windows.h>
#include <time.h>

#include "webcrawler.h"
#include "searchengine.h"
#include "dictionaries.h"

keywords_map index_map;

int main() {
    std::vector<std::string*> results;
    load_index_map( index_map );
    query( "Krzysztof Łuczka, wiersze ąźżćęłńó", index_map, results );
    show_results( results );
    unload_index_map( index_map );
    
    /*srand( time( NULL ) );
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    uint_fast64_t total = index( 2 );
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Elapsed time >> " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]\n";
    std::cout << "Total links indexed >> " << total << "\n";*/
}