/*
 * Copyright (c) 2016 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <cstdlib.h>
#include <cstring.h>

#define DBG(x) cgc_fprintf(stderr, x "\n")

#include "parser.h"

static cgc_Node emptyNode(TokenType::Ignore);

static const char *cgc_typeToString(TokenType type)
{
#define T(t) case TokenType::t: return #t ; 
    switch (type)
    {
    T(Ignore)
    T(Optional)
    T(Repeat)
    T(Or)
    T(And)
    T(Indent)
    T(Identifier)
    T(Number)
    T(String)
    T(Discard)
    T(Var)
    T(Assign)
    T(Operator)
    T(Declare)
    T(DeclareAssign)
    T(Call)
    T(Stmt)
    T(Expr)
    T(Block)
    T(While)
    T(BinOp)
    T(Proc)
    T(Break)
    T(Return)
    T(If)
    T(Elif)
    T(Else)
    default:
        return "UNKNOWN";
    }
#undef T
}

cgc_Parser::cgc_Parser(const char *input_)
    : input(input_), root(nullptr)
{
    cgc_length = cgc_strlen(input);
}

cgc_Parser::~cgc_Parser()
{
    if (root)
        cgc_Node::cgc_deleteTree(root);
}

bool cgc_Parser::cgc_parse()
{
    auto indent = cgc_Token(cgc_Token(" "_T ++, TokenType::Optional), TokenType::Indent);

    auto ws = cgc_Token((" "_T | "\t"_T) ++);
    auto optws = cgc_Token(ws, TokenType::Optional);

    auto op = cgc_Token(("+"_T | "-"_T | "*"_T | "/"_T | "<"_T | ">"_T | "="_T | "@"_T | "$"_T | "~"_T | "&"_T | "%"_T | "!"_T | "^"_T | "|"_T) ++, TokenType::Operator);

    auto digit = "0"_T | "1"_T | "2"_T | "3"_T | "4"_T | "5"_T | "6"_T | "7"_T | "8"_T | "9"_T;
    auto hexdigit = (digit | "a"_T | "b"_T | "c"_T | "d"_T | "e"_T | "f"_T | "A"_T | "B"_T | "C"_T | "D"_T | "E"_T | "F"_T);
    auto number = cgc_Token(
            ("0x"_T + (hexdigit++)) |
            (digit++),
        TokenType::Number);

    auto lowercase = "a"_T | "b"_T | "c"_T | "d"_T | "e"_T | "f"_T | "g"_T | "h"_T | "i"_T | "j"_T | "k"_T | "l"_T | "m"_T | "n"_T | "o"_T | "p"_T | "q"_T | "r"_T | "s"_T | "t"_T | "u"_T | "v"_T | "w"_T | "x"_T | "y"_T | "z"_T;
    auto uppercase = "A"_T | "B"_T | "C"_T | "D"_T | "E"_T | "F"_T | "G"_T | "H"_T | "I"_T | "J"_T | "K"_T | "L"_T | "M"_T | "N"_T | "O"_T | "P"_T | "Q"_T | "R"_T | "S"_T | "T"_T | "U"_T | "V"_T | "W"_T | "X"_T | "Y"_T | "Z"_T;
    auto identifier = cgc_Token(
            (lowercase | uppercase) +
            cgc_Token((lowercase | uppercase | digit | "_") ++, TokenType::Optional),
        TokenType::Identifier);

    auto string = cgc_Token("\""_T + cgc_Token((lowercase | uppercase | "_"_T | ","_T | "."_T | ws), TokenType::Optional) + "\""_T, TokenType::String);

    auto assignment = cgc_Token(TokenType::Assign);
    auto call = cgc_Token(TokenType::Call);
    auto expression = cgc_Token(TokenType::Expr);
    expression.cgc_set(
        ("("_T + optws + &expression + optws + ")"_T) |
        (&assignment) |
        (op + optws + &expression) |
        (&call) |
        (number) |
        (string) |
        (identifier)
    );
    auto after_expression = cgc_Token(
        (optws + op + optws + &expression) |
        (ws + "or"_T + ws + &expression) |
        (ws + "and"_T + ws + &expression), TokenType::BinOp);
    expression.cgc_setAfter(after_expression);

    assignment.cgc_set(identifier + ws + "="_T + ws + &expression);

    auto callarguments_ = cgc_Token((optws + "," + optws + &expression)++, TokenType::Optional);
    auto callarguments = cgc_Token(&expression + callarguments_, TokenType::Optional);
    call.cgc_set(
        identifier + optws + "(" + optws + callarguments + optws + ")"
    );

    auto procarguments_ = cgc_Token((optws + "," + optws + identifier)++, TokenType::Optional);
    auto procarguments = cgc_Token(identifier + procarguments_, TokenType::Optional);
    auto proc =
        identifier + optws + "(" + optws + procarguments + optws + ")";

    auto block = cgc_Token(TokenType::Repeat);
    auto statement = cgc_Token(
        //cgc_Token("discard"_T + ws + &expression, TokenType::Discard) |
        cgc_Token("var"_T + ws + &assignment, TokenType::DeclareAssign) |
        cgc_Token("var"_T + ws + identifier, TokenType::Declare) |
        cgc_Token("block:"_T, TokenType::Block) |
        cgc_Token("while"_T + ws + &expression + optws + ":", TokenType::While) |
        cgc_Token("if"_T + ws + &expression + optws + ":", TokenType::If) |
        cgc_Token("elif"_T + ws + &expression + optws + ":", TokenType::Elif) |
        cgc_Token("else:"_T, TokenType::Else) |
        cgc_Token("proc"_T + ws + proc + optws + ":", TokenType::Proc) |
        cgc_Token("break", TokenType::Break) |
        cgc_Token("return"_T + optws + cgc_Token(&expression, TokenType::Optional), TokenType::Return) |
        &expression);

    auto line = cgc_Token(
        (cgc_Token(indent + statement, TokenType::Stmt) + optws + "\n"_T) |
        (optws + "\n"_T));

    block.cgc_set(line);

    parsedLength = 0;
    root = nullptr;
    bool result = block.cgc_parse(input, cgc_length, &root, &parsedLength);
    if (result)
    {
        if (parsedLength != cgc_length)
        {
            DBG("unable to cgc_parse entire file");
            result = false;
        }
    }
    else
    {
        DBG("unable to cgc_parse file");
    }
    return result;
}

cgc_Token::cgc_Token()
    : type(TokenType::Ignore), indirect(nullptr), after(nullptr), literal(nullptr), cgc_length(0)
{
}

cgc_Token::cgc_Token(TokenType type_)
    : cgc_Token()
{
    type = type_;
}

cgc_Token::cgc_Token(const char *str)
    : cgc_Token()
{
    literal = str;
    cgc_length = cgc_strlen(str);
}

cgc_Token::cgc_Token(const char *str, TokenType type_)
    : cgc_Token(type_)
{
    literal = str;
    cgc_length = cgc_strlen(str);
}

cgc_Token::cgc_Token(const char *str, cgc_size_t len, TokenType type_)
    : cgc_Token(type_)
{
    literal = str;
    cgc_length = len;
}

cgc_Token::cgc_Token(const cgc_Token &child_, TokenType type_)
    : cgc_Token(type_)
{
    children.cgc_resize(1);
    children[0] = child_;
}

cgc_Token cgc_Token::operator++(int ignored)
{
    return cgc_Token(*this, TokenType::Repeat);
}

cgc_Token cgc_Token::operator+(cgc_Token rhs)
{
    cgc_Token t(TokenType::And);
    t.children.cgc_resize(2);
    t.children[0] = *this;
    t.children[1] = rhs;
    return t;
}

cgc_Token cgc_Token::operator|(cgc_Token rhs)
{
    cgc_Token t(TokenType::Or);
    if (this->type == TokenType::Or)
    {
        t.children.cgc_resize(this->children.cgc_length() + 1);
        for (unsigned int i = 0; i < this->children.cgc_length(); i++)
            t.children[i] = this->children[i];
        t.children[this->children.cgc_length()] = rhs;
    }
    else
    {
        t.children.cgc_resize(2);
        t.children[0] = *this;
        t.children[1] = rhs;
    }
    return t;
}

cgc_Token cgc_Token::operator&()
{
    cgc_Token t;
    t.indirect = this;
    return t;
}

cgc_Token operator "" _T(const char *str, STD_SIZE_T len)
{
    return cgc_Token(str, len, TokenType::Ignore);
}

void cgc_Token::cgc_after_(const char *input, cgc_size_t cgc_length, cgc_Node **pNode, cgc_size_t *pLength) const
{
    cgc_Node *node;

    if (after == nullptr)
        return;

    cgc_size_t actual = 0;
    if (after->cgc_parse(input, cgc_length, &node, &actual))
    {
        if ((*pNode)->next != nullptr)
        {
            DBG("in cgc_after_, pNode already has next");
        }
        (*pNode)->next = node->child;
        node->child = *pNode;
        *pLength += actual;
        *pNode = node;
        node->cgc_setLiteral(input - node->child->cgc_length, node->child->cgc_length + actual);
    }
}

bool cgc_Token::cgc_parse_(const char *input, cgc_size_t cgc_length, cgc_Node **pNode, cgc_size_t *pLength) const
{
    if (indirect != nullptr)
    {
        return indirect->cgc_parse(input, cgc_length, pNode, pLength);
    }
    else if (literal != nullptr)
    {
        if (cgc_length < this->cgc_length)
            return false;
        if (cgc_memcmp(input, literal, this->cgc_length) != 0)
            return false;
        if (type != TokenType::Ignore && type != TokenType::Optional && type != TokenType::Repeat)
        {
            *pNode = new cgc_Node(type);
            (*pNode)->cgc_setLiteral(input, this->cgc_length);
        }
        else
        {
            *pNode = nullptr;
        }
        *pLength += this->cgc_length;
        return true;
    }
    else if (children.cgc_length())
    {
        if (type == TokenType::Or)
        {
            for (unsigned int i = 0; i < children.cgc_length(); i++)
            {
                if (children[i].cgc_parse(input, cgc_length, pNode, pLength))
                    return true;
            }
            return false;
        }
        else if (type == TokenType::And)
        {
            cgc_size_t old = *pLength;
            if (!children[0].cgc_parse(input, cgc_length, pNode, pLength))
                return false;
            if (*pNode)
            {
                cgc_Node *node = *pNode;
                while (node->next != nullptr) node = node->next;
                return children[1].cgc_parse(input + *pLength - old, cgc_length - (*pLength - old), &node->next, pLength);
            }
            else
                return children[1].cgc_parse(input + *pLength - old, cgc_length - (*pLength - old), pNode, pLength);
        }

        if (type == TokenType::Ignore || type == TokenType::Optional || type == TokenType::Repeat)
            return children[0].cgc_parse(input, cgc_length, pNode, pLength);

        cgc_Node *node;
        cgc_size_t actual = 0;
        if (!children[0].cgc_parse(input, cgc_length, &node, &actual))
            return false;

        *pNode = new cgc_Node(type);
        (*pNode)->child = node;
        (*pNode)->cgc_setLiteral(input, actual);
        *pLength += actual;
        return true;
    }
    else
    {
        DBG("missing child or literal");
        return false;
    }
}

bool cgc_Token::cgc_parse(const char *input, cgc_size_t cgc_length, cgc_Node **pNode, cgc_size_t *pLength) const
{
    bool result;
    cgc_size_t actual, old = *pLength;
    cgc_Node *node;

    if (type == TokenType::Optional)
    {
        actual = 0;
        result = cgc_parse_(input, cgc_length, &node, &actual);
        if (result)
        {
            *pNode = node;
            *pLength += actual;
            input += actual;
            cgc_length -= actual;
        }
        else
        {
            *pNode = nullptr;
        }
        cgc_after_(input, cgc_length, pNode, pLength);
        return true;
    }
    else if (type == TokenType::Repeat)
    {
        bool success = false;
        cgc_Node *head = nullptr, *tail = nullptr;
        do {
            actual = 0;
            result = cgc_parse_(input, cgc_length, &node, &actual);
            if (result)
            {
                if (node != nullptr)
                {
                    if (head == nullptr)
                        head = tail = node;
                    else
                        tail = tail->next = node;
                }
                input += actual;
                cgc_length -= actual;
                *pLength += actual;
                success = true;
            }
        } while (result);

        *pNode = head;
        if (success)
            cgc_after_(input, cgc_length, pNode, pLength);
        return success;
    }
    else
    {
        if (cgc_parse_(input, cgc_length, pNode, pLength))
        {
            cgc_after_(input + *pLength - old, cgc_length - (*pLength - old), pNode, pLength);
            return true;
        }
        else
            return false;
    }
}

void cgc_Token::cgc_set(const cgc_Token &child)
{
    children.cgc_resize(1);
    children[0] = child;
}

void cgc_Token::cgc_setAfter(cgc_Token &after)
{
    this->after = reinterpret_cast<cgc_Token *>(&reinterpret_cast<char&>(after));
}

cgc_Node::cgc_Node(TokenType type_)
    : literal(nullptr), type(type_), child(nullptr), next(nullptr)
{
}

cgc_Node::~cgc_Node()
{
    if (literal)
        delete[] literal;
}

void cgc_Node::cgc_deleteTree(cgc_Node *root)
{
    if (root->child)
        cgc_deleteTree(root->child);

    cgc_Node *next = root->next;
    delete root;
    if (next)
        cgc_deleteTree(next);
}

void cgc_Node::cgc_dump(unsigned int level)
{
    for (unsigned int i = 0; i < level * 2; i++)
        cgc_fprintf(stderr, " ");
    cgc_fprintf(stderr, "%s - (%d) %s\n", cgc_typeToString(type), cgc_length, literal);
    if (child != nullptr)
        child->cgc_dump(level + 1);
    if (next != nullptr)
        next->cgc_dump(level);
}

void cgc_Node::cgc_setLiteral(const char *start, cgc_size_t cgc_length)
{
    if (literal)
        delete[] literal;
    literal = new char [cgc_length + 1];
    cgc_memcpy(literal, start, cgc_length);
    literal[cgc_length] = 0;
    this->cgc_length = cgc_length;
}
