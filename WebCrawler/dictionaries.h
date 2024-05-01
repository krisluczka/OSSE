#pragma once

/*
    For all people who want to support the project by
    creating the set for a given language.

    The dictionary of ignored words must contain:
        - conjunctions
        - prepositions
        - most common particles
    Both with language's special characters and without.

    Keyword estimation will ignore these words in documents
    (but they will stay in the title)
*/
const std::map<std::string, std::vector<std::string>> dictionary = {
        {"pl", {
            "u", "za", "przed", "po", "w", "od", "nad", "pod", "z", "zza",
            "ad", "bez", "beze", "do", "pode", "gwoli", "jak", "jako", "na", "ode",
            "per", "podle", "temu", "we", "wedle", "znad", "ze", "o",

            "i", "oraz", "albo", "lub", "natomiast", "ale", "jednak", "lecz",
            "wiec", "wiêc", "zatem", "totez", "tote¿", "mianowicie", "to",
            "ani", "ni",

            "czy", "tylko", "oby", "sie", "siê",
        }},
        {"en", {"test"}},
};