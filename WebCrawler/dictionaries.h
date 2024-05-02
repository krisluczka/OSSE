#pragma once

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
            "albo", "lub", "natomiast", "ale", "jednak", "lecz", "wiec", "wiêc", 
            "zatem", "totez", "tote¿", "mianowicie", "to", "ani", "ni",
            "czy", "tylko", "oby", "sie", "siê", "oto", "co", "bo", "dla", "nie", "tak",
            "a", "no", "by", "niech"
        }},
        {"en", {
            "and", "to", "a", "an", "with", "by", "so"
        }},
};

/*
    This must contain all diactric converted to their
    latin equvialents.

    This should include both lowercase and uppercase variants.\


    DOES NOT WORK AT ALL
*/
const inline std::string remove_diactric( std::string word ) {
    for ( size_t i( 0 ); i < word.size(); ++i ) {
        switch ( word[i] ) {
            // Polish letters
            case '¥':
                word[i] = 'A';
                break;
            case 'a':
                word[i] = 'a';
                break;
            case 'Æ':
                word[i] = 'C';
                break;
            case 'æ':
                word[i] = 'c';
                break;
            case 'Ê':
                word[i] = 'E';
                break;
            case 'ê':
                word[i] = 'e';
                break;
            case '£':
                word[i] = 'L';
                break;
            case '³':
                word[i] = 'l';
                break;
            case 'Ñ':
                word[i] = 'N';
                break;
            case 'ñ':
                word[i] = 'n';
                break;
            case 'Ó':
                word[i] = 'O';
                break;
            case 'ó':
                word[i] = 'o';
                break;
            case 'Œ':
                word[i] = 'S';
                break;
            case 'œ':
                word[i] = 's';
                break;
            case '':
                word[i] = 'Z';
                break;
            case 'Ÿ':
                word[i] = 'z';
                break;
            case '¯':
                word[i] = 'Z';
                break;
            case '¿':
                word[i] = 'z';
                break;
        }
    }
    return word;
}