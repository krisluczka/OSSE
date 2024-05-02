#pragma once
#include <codecvt>
#include <locale>

/*
    The dictionary of ignored words must contain:
        - conjunctions
        - prepositions
        - most common particles
    Both with language's special characters and without.

    Keyword estimation will ignore these words in documents
    (but they will stay in the title)
*/
const std::map<std::string, std::vector<std::string>> dictionary = {
        // Polish language
        {"pl", {
            "u", "za", "przed", "po", "w", "od", "nad", "pod", "z", "zza",
            "ad", "bez", "beze", "do", "pode", "gwoli", "jak", "jako", "na", "ode",
            "per", "podle", "temu", "we", "wedle", "znad", "ze", "o", "i", "oraz",
            "albo", "lub", "natomiast", "ale", "jednak", "lecz", "wiec", "wi�c", 
            "zatem", "totez", "tote�", "mianowicie", "to", "ani", "ni",
            "czy", "tylko", "oby", "sie", "si�", "oto", "co", "bo", "dla", "nie", "tak",
            "a", "no", "by", "niech"
        }},
        {"en", {
            "and", "to", "a", "an", "with", "by", "so"
        }},
};

/*
    This must contain all diactric converted to their
    latin equvialents. Should include both lowercase
    and uppercase variants.

    Total hours spent to solve this - ~9 hours
*/
const inline std::string remove_diactric( const std::string &content ) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    std::u32string content32( conv.from_bytes( content ) );

    std::string result;

    for ( char32_t c : content32 ) {
        switch ( c ) {
            // Polish letters
            case U'�':
                result += U'A';
                break;
            case U'�':
                result += U'a';
                break;
            case U'�':
                result += U'C';
                break;
            case U'�':
                result += U'c';
                break;
            case U'�':
                result += U'E';
                break;
            case U'�':
                result += U'e';
                break;
            case U'�':
                result += U'L';
                break;
            case U'�':
                result += U'l';
                break;
            case U'�':
                result += U'N';
                break;
            case U'�':
                result += U'n';
                break;
            case U'�':
                result += U'O';
                break;
            case U'�':
                result += U'o';
                break;
            case U'�':
                result += U'S';
                break;
            case U'�':
                result += U's';
                break;
            case U'�':
                result += U'Z';
                break;
            case U'�':
                result += U'z';
                break;
            case U'�':
                result += U'Z';
                break;
            case U'�':
                result += U'z';
                break;
            default:
                result += c;
        }
    }

    return result;
}