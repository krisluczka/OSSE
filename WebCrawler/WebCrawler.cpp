#include <iostream>
#include <windows.h>
#include <wininet.h>
#include <fstream>
// idk why but it does not work without it
#pragma comment(lib, "wininet.lib")


/*
    Function that downloads webpage content
*/
std::string getWebPageContent( const std::string& url ) {
    // session opening
    HINTERNET hInternet = InternetOpenA( "Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0 );
    if ( !hInternet ) {
        std::cerr << "The session wasn't started correctly." << std::endl;
        return "";
    }

    // opening given URL in a session
    HINTERNET hUrl = InternetOpenUrlA( hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0 );
    if ( !hUrl ) {
        std::cerr << "Invalid URL." << std::endl;
        InternetCloseHandle( hInternet );
        return "";
    }

    // the content
    std::string content;
    // the buffer
    constexpr DWORD bufferSize = 1024;
    char buffer[bufferSize];
    DWORD bytesRead = 0;

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


int main() {
    std::string url;
    uint_fast64_t depth;
    std::cout << "The starting URL >> ";
    std::cin >> url;
    std::cout << "Depth >> ";
    std::cin >> depth;

    std::string content = getWebPageContent( url );
    if ( !content.empty() ) {
        std::ofstream outputFile( "webpage_content.html" );
        outputFile << content;
        outputFile.close();
        std::cout << "Website saved as webpage_content.html" << std::endl;
    } else {
        std::cout << "Error." << std::endl;
    }

    return 0;
}
