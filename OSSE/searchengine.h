#pragma once
#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

typedef std::map<std::string, std::vector<std::string*>> keywords_map;

/*
	Function that loads the index
*/
void load_index_map( keywords_map& index_map ) {
	std::string line, key, word;

	// deleting the index map contents
	for ( auto& pair : index_map ) {
		for ( const std::string* l : pair.second ) {
			delete l;
		}
		pair.second.clear();
	}
	index_map.clear();

	// opening file
	std::fstream index( "index.txt", std::ios::in );

	// loading the index
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

	// remember, just in case
	index.close();
}

void unload_index_map( keywords_map& index_map ) {
	// deleting the index map contents
	for ( auto& pair : index_map ) {
		for ( const std::string* l : pair.second ) {
			delete l;
		}
		pair.second.clear();
	}
	index_map.clear();
}

/*
	Function that executes a query
*/
void query( std::string query, const keywords_map& index_map, std::vector<std::string*>& results ) {
	// never, ever delete it's content 
	// it contains pointers to index_map's strings
	results.clear();

	std::istringstream iss( query );
	std::string keyword;

	while ( iss >> keyword ) {
		// if index_map contains a keyword
		if ( index_map.find( keyword ) != index_map.end() ) {
			// insert contains into the result
			results.insert( results.end(), index_map.at( keyword ).begin(), index_map.at( keyword ).end() );
		}
	}
}

void show_results( const std::vector<std::string*>& results ) {
	if ( results.size() )
		for ( uint_fast64_t i( 0 ); i < results.size(); ++i ) {
			std::cout << i + 1 << ". " << *results[i] << "\n";
		}
	else {
		std::cout << " No results found.\n";
	}
}

#endif