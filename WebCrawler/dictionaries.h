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
    latin equvialents.

    This should include both lowercase and uppercase variants.\


    DOES NOT WORK AT ALL
*/
const inline std::string remove_diactric( std::string word ) {
    for ( size_t i( 0 ); i < word.size(); ++i ) {
        switch ( word[i] ) {
            // Polish letters
            case '�':
                word[i] = 'A';
                break;
            case 'a':
                word[i] = 'a';
                break;
            case '�':
                word[i] = 'C';
                break;
            case '�':
                word[i] = 'c';
                break;
            case '�':
                word[i] = 'E';
                break;
            case '�':
                word[i] = 'e';
                break;
            case '�':
                word[i] = 'L';
                break;
            case '�':
                word[i] = 'l';
                break;
            case '�':
                word[i] = 'N';
                break;
            case '�':
                word[i] = 'n';
                break;
            case '�':
                word[i] = 'O';
                break;
            case '�':
                word[i] = 'o';
                break;
            case '�':
                word[i] = 'S';
                break;
            case '�':
                word[i] = 's';
                break;
            case '�':
                word[i] = 'Z';
                break;
            case '�':
                word[i] = 'z';
                break;
            case '�':
                word[i] = 'Z';
                break;
            case '�':
                word[i] = 'z';
                break;
        }
    }
    return word;
}