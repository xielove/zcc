/* author: dongchangzhang */
/* time: Fri 03 Mar 2017 12:39:51 PM CST */

#ifndef __LEXICAL_ANALYZER_H__
#define __LEXICAL_ANALYZER_H__

#include <fstream>
#include <string>
#include <regex>

#include "utils.h"

class Tokenizer {
    public:
        Tokenizer() : type(0), re_pattern(pattern), re_comment_end(comment_end), re_number(number) {  };
        Tokenizer(std::string f) : type(1), file(f),
        re_pattern(pattern), re_comment_end(comment_end), re_number(number) { in.open(f); };

        ~Tokenizer() { if(in) in.close(); }
        // get next touple
        Token next();
        // get line number
        int get_linenu() { return linenu; }
        // when meet EOF return true
        bool isend() { return finished; }
        // print log for debug
        void log_enable() { print_log = true; }
        // ignore log
        void log_disable() { print_log = false; }
    private:

        bool print_log = false;

        bool finished = false;

        int linenu = 0;

        int type = 0;

        Token token;

        std::string file;

        std::string line;

        std::ifstream in;

        std::string pattern = "(^[a-zA-Z_]+[a-zA-Z0-9_]*)|" // key word or id
            "(^0[xX]\\d+)|(^\\d+\\.?\\d*)|" // number
            "(^[*+-/=><&|!]+|^[()])|" // operator
            "(^[\\[\\]{};\"\'])"; // sepatator

        std::string comment_end = "\\*/";

        std::string number = "(^0[Xx]\\d+|(^\\d+\\.?\\d*))"; // number

        std::regex re_pattern;

        std::regex re_comment_end;

        std::regex re_number;

        void skip_ws();
        
        void ignore_comment();

        Token get_ch();

        Token get_string();

        Token deal_element(const std::string &element);

        Token do_next();

};

#endif

