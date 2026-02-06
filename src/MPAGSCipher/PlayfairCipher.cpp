#include "PlayfairCipher.hpp"

#include <algorithm>
#include <string>
#include <iostream>

/**
 * \file PlayfairCipher.cpp
 * \brief Contains the implementation of the PlayfairCipher class
 */

PlayfairCipher::PlayfairCipher(const std::string& key)
{
    this->setKey(key);
}

void PlayfairCipher::setKey(const std::string& key)
{
    // Store the original key
    key_ = key;

    // Append the alphabet to the key
    key_ += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Make sure the key is upper case
    std::transform(std::begin(key_), std::end(key_), std::begin(key_),
                   ::toupper);

    // Remove non-alphabet characters
    key_.erase(std::remove_if(std::begin(key_), std::end(key_),
                              [](char c) { return !std::isalpha(c); }),
               std::end(key_));

    // Change J -> I
    std::transform(std::begin(key_), std::end(key_), std::begin(key_),
                   [](char c) { return (c == 'J') ? 'I' : c; });

    // Remove duplicated letters
    std::string lettersFound{""};
    auto detectDuplicates = [&](char c) {
        if (lettersFound.find(c) == std::string::npos) {
            lettersFound += c;
            return false;
        } else {
            return true;
        }
    };
    key_.erase(
        std::remove_if(std::begin(key_), std::end(key_), detectDuplicates),
        std::end(key_));

    // Store the coordinates of each letter
    // (at this point the key length must be equal to the square of the grid dimension)
    for (std::size_t i{0}; i < keyLength_; ++i) {
        std::size_t row{i / gridSize_};
        std::size_t column{i % gridSize_};

        auto coords = std::make_pair(row, column);

        charLookup_[key_[i]] = coords;
        coordLookup_[coords] = key_[i];
    }
}

std::string PlayfairCipher::applyCipher(const std::string& inputText,
                                        const CipherMode cipherMode) const
{
    // Create the output string, initially a copy of the input text
    std::string outputText{inputText};
    if (cipherMode == CipherMode::Encrypt){
        std::transform(std::begin(outputText), std::end(outputText), std::begin(outputText),
                    [](char c) { return (c == 'J') ? 'I' : c; });

        // Find repeated characters and add an X (or a Q for repeated X's)
        for(std::size_t i{0}; i < outputText.size()-1; i+=2){
            if (outputText[i] == outputText[i+1]){
                outputText.insert(i+1, 1, (outputText[i] == 'X') ? 'Q' : 'X');
            }
        }
        if (outputText.size() % 2 != 0){
            if (outputText[outputText.size() - 1] == 'Z')
                outputText += 'X';
            outputText += 'Z';
        }
        // Loop over the input bigrams
        for (std::size_t i{0}; i < outputText.size()-1; i+=2){
            auto [row1,col1] = charLookup_.at(outputText[i]);
            auto [row2,col2] = charLookup_.at(outputText[i+1]);
            // - Find the coordinates in the grid for each bigram
            if (row1 == row2){
                col1 = (col1 + 1) % gridSize_;
                col2 = (col2 + 1) % gridSize_;
            }
            else if (col1 == col2){
                row1 = (row1 + 1) % gridSize_;
                row2 = (row2 + 1) % gridSize_;
            }
            else{
                std::swap(col1,col2);
            }
            
            outputText[i] = coordLookup_.at({row1, col1});
            outputText[i+1] = coordLookup_.at({row2, col2});
        }
    }
    else{
        // Loop over the input bigrams
        for (std::size_t i{0}; i < outputText.size()-1; i+=2){
            auto [row1,col1] = charLookup_.at(outputText[i]);
            auto [row2,col2] = charLookup_.at(outputText[i+1]);
            // - Find the coordinates in the grid for each bigram
            if (row1 == row2){
                col1 = (col1 - 1 + gridSize_) % gridSize_;
                col2 = (col2 - 1 + gridSize_) % gridSize_;
            }
            else if (col1 == col2){
                row1 = (row1 - 1 + gridSize_) % gridSize_;
                row2 = (row2 - 1 + gridSize_) % gridSize_;
            }
            else{
                std::swap(col1,col2);
            }
            
            outputText[i] = coordLookup_.at({row1, col1});
            outputText[i+1] = coordLookup_.at({row2, col2});
        }
    }
    return outputText;
}
