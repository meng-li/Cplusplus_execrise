/*
 * Simple calculator components:
 *    + syntactic analyzer
 *    + lexical analyzer
 *    + symbol table
 *    + driver program
 * based on stack to deal with priority of different operators
 */

#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <cctype>
#include <cmath>

using namespace std;

// tokens accepted by the parser / lexer
enum Token_value
{
    NAME,      NUMBER,     END,     FUNC='$',
    ADD='+',   SUB='-',    MUL='*', DIV='/',
    PRINT=';', ASSIGN='=', LP='(',  RP=')',
    EXPO='^',  LCB='{',    RCB='}', COMMA=','
};
Token_value cur_tok = PRINT;
istream* input;
map<string, double> table;
double number_value;
string string_value;
double expr(bool);
double term(bool);
double expo(bool);
double prim(bool); 

Token_value get_token()
{
    char ch = 0;
    do {
        if (!input->get(ch) )
            return cur_tok = END;
    } while (isspace(ch) && ch!='\n');

    switch (ch)
    {
        case 0:
            return cur_tok = END;
        case PRINT:
        case '\n':
            return cur_tok = PRINT;
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case ASSIGN:
        case LP:
        case RP:
        case EXPO:
        case RCB:
        case COMMA:
            return cur_tok = Token_value(ch);
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
            input->putback(ch);
            *input >> number_value;
            return cur_tok = NUMBER;
        default:
            if (isalpha(ch) )
            {
                string_value = ch;
                while (input->get(ch) && (isalnum(ch) || ch=='_') )
                    string_value.push_back(ch);
                input->putback(ch);
                return cur_tok = NAME;
            }
            return cur_tok = PRINT;
    }
}

double expr(bool is_get) {
    double left = term(is_get);
    while (true) {
        switch (cur_tok) {
            case ADD:
                left += term(true);
                break;
            case SUB:
                left -= term(true);
                break;
            default:
                return left;
        }
    }
}

double term(bool is_get) {
    double left = expo(is_get);
    while (true) {
        switch (cur_tok) {
            case MUL:
            {
                left *= expo(true);
                break;
            }
            case DIV:
            {
                if (double d = expo(true) ) {
                    left /= d;
                    break;
                } else {
                    std::cout << "Divided by zero, Error!" << std::endl;
                }
            }
            default:
                return left;
        }
    }
}

double expo(bool is_get) {
    double left = prim(is_get);
    while (true) {
        switch (cur_tok) {
            case EXPO:
            {
                left = pow(left, prim(true) );
                break;
            }
            default:
                return left;
        }
    }
}

double prim(bool is_get) {
    if (is_get) get_token();
    switch (cur_tok) {
        case NUMBER:
        {
            double v = number_value;
            get_token();
            return v;
        }
        case NAME:
        {
            double& value = (&table)->operator[](string_value);
            if (get_token() == ASSIGN)
                value = expr(true); 
            return value;
        }
        case SUB:
            return -prim(true);
        case LP:
        {
            double e = expr(true);
            get_token();
            if (cur_tok != RP) {
                std::cout << "Error! expect ')'" << std::endl;
            }
            return e;
        }
        default:
        {
            std::cout << "Error!" << std::endl;
            return -1;
        }
    }
}


int main(int argc, char* argv[])
{
    // input stream object
    switch (argc)
    {
        case 1:
            input = &cin;
            break;
        case 2:
        {
            istringstream ss(argv[1]);
            input = &ss;
            break;
        }
        default:
            std::cout << "too many parameters!";
            return 1;
    }

    // global table
    table["pi"] = 3.1415926535897932385;
    table["e"] = 2.7182818284590452354;

    // dealing with input
    while (*input) 
    {
        get_token();
        if (cur_tok == PRINT) continue;
        if (cur_tok == END) break;
        std::cout << expr(false) << std::endl;
    }
    return 0;
}
