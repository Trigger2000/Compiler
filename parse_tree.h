#pragma once

#include <iostream>
#include <list>
#include <string>
#include <map>
#include <unistd.h>
#include "commands.h"
#include "vector/vector.hpp"

typedef std::list<command> cmdlist;
extern cmdlist pcode;
extern std::map <std::string, regnum> vars;
extern std::map <std::string, short> strings;
extern regnum lastreg;

extern void yyerror(char *s); 
inline regnum newreg();

template<typename L>
inline void append(L& list1, L& list2);

template<typename C, typename K>
inline bool contains(const C& contnr, const K& key);

class oper_t 
{
public: 
    virtual ~oper_t();
    virtual void print(int indent = 0) = 0;
    virtual cmdlist emit() = 0;

protected: 
    oper_t();
};


class expr_t 
{
public:
    virtual ~expr_t();
    virtual void print() = 0;
    virtual cmdlist emit() = 0;
    regnum dest;

protected:
    expr_t();
};


class block: public oper_t
{
public:
    block();
    block(oper_t* op);
    block(oper_t* op1, oper_t* op2);
    void print(int indent = 0);
    cmdlist emit();
    int size();
    ~block();

private:
    std::list<oper_t*> ops;
    void append(oper_t* op);
};


class exprop: public oper_t 
{
public: 
    exprop(expr_t* expr);
    void print(int indent = 0);
    cmdlist emit();
    ~exprop();

private:
    expr_t* expr;
};


class ifop: public oper_t 
{
public: 
    ifop(expr_t* cond, oper_t* thenops, oper_t* elseops);
    void print(int indent = 0);
    cmdlist emit();
    ~ifop();

private:
    expr_t* cond;
    block thenops, elseops;
};


class whileop: public oper_t 
{
public: 
    whileop(expr_t* cond, oper_t* ops);
    void print(int indent = 0);
    cmdlist emit();
    ~whileop();

private:
    expr_t* cond;
    block ops;
};


class exitop: public oper_t 
{
    void print(int indent = 0);
    cmdlist emit();
};


class binary: public expr_t 
{
public: 
    binary(command::opcodes op, expr_t *arg1, expr_t *arg2);
    void print();
    cmdlist emit();
    ~binary();

private:
    command::opcodes op;
    expr_t *arg1, *arg2;
};


class assign: public expr_t 
{
public: 
    assign(const std::string& name, expr_t* value);
    void print();
    cmdlist emit();
    ~assign();

private:
    std::string name;
    expr_t* value;
};


class unary: public expr_t 
{
public: 
    unary(command::opcodes op, expr_t* arg);
    void print();
    cmdlist emit();
    ~unary();

private:
    command::opcodes op;
    expr_t* arg;
};


class value: public expr_t 
{
public: 
    value(const std::string& text);
    void print();
    cmdlist emit();

private:
    std::string text;
};


class varref: public expr_t 
{
public: 
   varref(const std::string& name) : name(name) {}
   void print() {}
   cmdlist emit();

private:
    std::string name;
};


class string: public expr_t 
{ // not a real expr -- no need to emit()
public:
    std::string text;
    string(const std::string& text): text(text) {}
    void print() {}
    cmdlist emit() {}
};


class funcall: public expr_t 
{
public: 
    funcall(const std::string& name, const std::list<expr_t*>& args);
    void print();
    cmdlist emit();
    ~funcall();

private:
    std::string name;
    std::list<expr_t*> args;
};

//глобальная замена
//std::string replaceAll(const std::string& where, const std::string& what, const std::string& withWhat);