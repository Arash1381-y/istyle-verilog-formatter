#include <iostream>
#include "astyle.h"
#include <string>

using namespace std;

#define STARTS_WITH(str, prefix) (str.compare(0, sizeof(prefix) - 1, prefix) == 0)

namespace astyle {
    ModuleDeclarationFormatter::ModuleDeclarationFormatter(ASStreamIterator *iter)
    {
        sourceIterator = iter;
        isInModulePortSection = false;
        isInModuleParamSection = false;
        isInMultiLineComment = false;
        currentIndex = 0;
        prevIndex = 0;
    }

    ModuleDeclarationFormatter::~ModuleDeclarationFormatter() = default;


    void ModuleDeclarationFormatter::format(ostream *out)
    {
        string writeBuffer;

        while (true) {
            // terminate condition
            if (!getNextToken()) {
                if (!getNextLine()) break;
            }

            if (isInModuleNameSection) {
                // current token must be the name of the module
                addSequence(out);
                isInModuleNameSection = false;
            } else if (isInModuleParamSection) {
                // check if end is reached
                if (currentTokenBuffer == AS_PAREN_CLOSE) {
                   // TODO: break line if needed
                }
                // do something here
            } else if (isInModulePortSection) {
                // check if end is reached
                if (currentTokenBuffer == AS_PAREN_CLOSE) {
                    // check if line break is needed or not
                }
            } else if (isInMultiLineComment) {
                // check if scanner is in multi line comment section
                addSequence(out);
                // check if end comment is reached
                if (currentTokenBuffer == AS_CLOSE_COMMENT) {
                    isInMultiLineComment = false;
                }
            } else if (currentTokenBuffer == AS_OPEN_LINE_COMMENT) {
                // check if one line comment is reached
                // manually set current index to end of the line
                currentIndex = currentLineBuffer.size();
                addSequence(out);
                prevIndex = currentIndex;

            } else if (currentTokenBuffer == AS_OPEN_COMMENT) {
                addSequence(out);
                isInMultiLineComment = true;

            } else if (currentTokenBuffer == AS_MODULE) {
                addSequence(out);
                isInModuleNameSection = true;
            } else if (currentTokenBuffer == AS_TAG) {
                // the param start parentheses is splited from tag
                // example : # ( parameter ....)
                isInModuleParamSection = true;

            } else if (currentTokenBuffer == AS_PARAM_DEC_START) {
                addSequence(out);
                isInModuleParamSection = true;
                // break line
                breakLine(out);
                addIndent(out, 2); //TODO: use standard space or config

            } else if (currentTokenBuffer == AS_PAREN) {
                addSequence(out);
                isInModulePortSection = true;
                // break line
                breakLine(out);
            }
        }
    }


    bool ModuleDeclarationFormatter::getNextToken()
    {
        // does find token (for escaping starting white spaces)
        bool find = false;

        // set prev index
        prevIndex = currentIndex;
        // remove previous token
        currentTokenBuffer.clear();

        // find next token and update currentIndex
        for (size_t i = prevIndex; i < currentLineBuffer.size(); i++) {
            char ch = currentLineBuffer[i];
            if (ch != ' ') {
                find = true;
                currentTokenBuffer += ch;
            } else if (find) {
                currentIndex = i;
                break;
            }
        }

        return find;
    }


    void ModuleDeclarationFormatter::addSequence(ostream *out)
    {
        // append from prev to current index into the out buffer
        *out << currentLineBuffer.substr(prevIndex, currentIndex);
    }

    void ModuleDeclarationFormatter::breakLine(ostream *out)
    {

        *out << endl;
    }

    void ModuleDeclarationFormatter::addIndent(ostream *out, int num)
    {
        *out << string(num, ' ');
    }

    bool ModuleDeclarationFormatter::getNextLine()
    {
        if (sourceIterator->hasMoreLines()) {
            currentLineBuffer = sourceIterator->nextLine();
            prevIndex = 0;
            currentIndex = 0;
            return true;
        }

        return false;
    }
}





