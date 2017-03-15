/* author: dongchangzhang */
/* time: Fri 03 Mar 2017 12:40:02 PM CST */

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <cassert>

#include "tokenizer.h"
#include "symboltable.h"
#include "token.h"
#include "error.h"

void Tokenizer::skip_ws() {
    for (size_t i = 0; i < line.size(); ++i) {
        if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n') {
            line.erase(0, i);
            break;
        } 
    }
}

void Tokenizer::ignore_comment() {
    std::smatch outcome;
    do {
        if (print_log)
            std::cout << "# log: skip comment, line nu: " << linenu << std::endl;
        bool feedback = std::regex_search(line, outcome, re_comment_end);
        if (feedback) {
            line = outcome.suffix();
            return;
        }
        ++linenu;
    } while (getline(in, line));
    print_error(linenu, COMMENT_NOT_END);
    exit(2);

}

Token Tokenizer::get_ch() {
    std::string r = "'";
    if (line[1] == '\'') {
        r += line[0];
        r + "'";
        line.erase(0, 2);
    }
    Token z(VALUE, r);
    return z;
}

Token Tokenizer::get_string() {
    std::string r = "";
    bool okay = false;
    do {
        okay =false;
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == '"') {
                okay = true;
                r += line.substr(0, i);
                line.erase(0, i + 1);
                break;
            }
        }
        if (!okay) r += line;
        else break;
        ++linenu;
    } while(getline(in, line));

    Token z(VALUE, r);
    return z;
}
Token Tokenizer::next() {
    if (print_log) std::cout << "--------------------" << std::endl;
    if (line == "") {
        if (getline(in, line)) {
            ++linenu;
        }
        else {
            finished = true;
            Token z;
            return z;
        }
    }
    return do_next();
}

Token Tokenizer::deal_element(const std::string& element) {
    if (print_log)
        std::cout << "# log: input element -> " <<  element << " line nu: " << linenu << std::endl;
    if (std::regex_match(element, re_number)) {
        if (print_log) std::cout << "# log : element " << element << " is num, line nu: " << linenu << std::endl;
        Token z(VALUE, element);
        return z;
    } 
    else {
        int code = get_code(element);
        if (print_log) {
            if (element == get_token_info(code) || code == ID)
                std::cout << "# log: test pass " << std::endl;
            else
                std::cout << "# log: error at line nu: " << linenu << " element is: " << element << " but: " << get_token_info(code) << std::endl; 
        }
        Token z(code);
        if (code == ID) {
            std::string addr = install_id(element);
            z.put_attr(addr);
        }
        return z;
    }
}
Token Tokenizer::do_next() {
    // result
    Token result;
    // re match result
    std::smatch outcome;
    // skip white char
    skip_ws();
    if (line == "") return next();
    bool isfind = std::regex_search(line, outcome, re_pattern);
    if (isfind) {
        std::string element = outcome.str();
        if (element == "'") {
            if (print_log) std::cout << "# log: this element is char" << std::endl;
            line = outcome.suffix();
            // find the char or string in this function
            // after find ,return;
            return get_ch(); // should be func
        }
        else if (element == "\"") {
            if (print_log) std::cout << "# log: this element is string" << std::endl;
            line = outcome.suffix();
            return get_string();
        }
        else if (element == "//") {
            // skip comments and then find again;
            // func of ignore_comment
            if (print_log) std::cout << "# log: comment skiped at line nu: " << linenu << std::endl;
            line = "";
            return next();
        }
        else if (element == "/*"){
            ignore_comment();
            // ignore_comment(linenu, elements, re_comment_end, in, is);
            return do_next();
        }
        else {
            line = outcome.suffix();
            // may be an element, deal it
            return deal_element(element);
        }
    }
    else {
        // not find, there is an error 
        // deal error here
        print_error(-1, NONE_DEFINE_SYMBOL, line);
        exit(3);
        line.erase(0, 1);
        return result;
    }
}