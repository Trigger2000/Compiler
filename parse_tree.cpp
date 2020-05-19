#include "parse_tree.h"

inline regnum newreg() 
{
    if (!++lastreg)
    {
        yyerror("Too many registers used.");
    }

    return lastreg;
}

template<typename L>
inline void append(L& list1, L& list2) 
{
    list1.splice(list1.end(), list2, list2.begin(), list2.end());
}

template<typename C, typename K>
inline bool contains(const C& contnr, const K& key) 
{
    return contnr.find(key)!=contnr.end();
}

oper_t::oper_t() {}
oper_t::~oper_t() {}


expr_t::expr_t() {}
expr_t::~expr_t() {}


block::block() {}

block::block(oper_t* op) 
{ 
    append(op); 
}

block::block(oper_t* op1, oper_t* op2) 
{ 
    append(op1); append(op2); 
}

void block::print(int indent)
{
    for (auto&& item: ops)
    {
        std::cout << std::string(indent, '\t');
        item->print(indent);
    }
}

cmdlist block::emit()
{
    cmdlist result, tmp;
    for (auto&& item: ops)
    {
        tmp = item->emit();
        ::append(result, tmp);
    }

    return result;
}

int block::size()
{
    return ops.size();
}

block::~block()
{
    for (auto&& item: ops)
    {
        delete item;
    }
}

void block::append(oper_t* op) 
{
    block* b = dynamic_cast<block*>(op);
    if(b) 
    {
        ops.splice(ops.end(), b->ops, b->ops.begin(), b->ops.end());
        delete b;
    }
    else ops.push_back(op);
}


exprop::exprop(expr_t* expr):
    expr(expr) 
{}

void exprop::print(int indent)
{
    expr->print();
    std::cout << ";" << std::endl;
}

cmdlist exprop::emit()
{
    return expr->emit();
}

exprop::~exprop()
{
    delete expr;
}


ifop::ifop(expr_t* cond, oper_t* thenops, oper_t* elseops):
    cond(cond), 
    thenops(thenops), 
    elseops(elseops) 
{}

void ifop::print(int indent)
{
    std::cout << "if "; 
    cond->print();  
    std::cout << " {" << std::endl;
    thenops.print(indent+1);

    if (elseops.size()) 
    {
        std::cout << std::string(indent, '\t') << "} else {" << std::endl;
        elseops.print(indent+1);
    }

    std::cout << std::string(indent, '\t') << "}" << std::endl;
}

cmdlist ifop::emit()
{
    cmdlist result = cond->emit(),
    thenops = this->thenops.emit(),
    elseops = this->elseops.emit();

    result.push_back(command(command::jz, cond->dest, thenops.size()+!!elseops.size()));

    append(result, thenops);
    if (elseops.size()) 
    {
        result.push_back(command(command::jz, 0, elseops.size()));
        append(result, elseops);
    }

    return result;
}

ifop::~ifop()
{
    delete cond;
}


whileop::whileop(expr_t* cond, oper_t* ops): 
    cond(cond), 
    ops(ops) 
{}

void whileop::print(int indent)
{
    std::cout << "while "; 
    cond->print();  
    std::cout << " {" << std::endl;
    ops.print(indent+1);
    std::cout << std::string(indent, '\t') << "}" << std::endl;
}

cmdlist whileop::emit()
{
    cmdlist result = cond->emit();
    cmdlist ops = this->ops.emit();
    int skip = 2 + result.size() + ops.size();
    result.push_back(command(command::jz, cond->dest, ops.size()+1));
    append(result, ops);
    result.push_back(command(command::jz, 0, -skip));
    return result;
}

whileop::~whileop()
{
    delete cond;
}


void exitop::print(int indent)
{
    std::cout << "exit;" << std::endl;
}

cmdlist exitop::emit()
{
    cmdlist result;
    result.push_back(command(command::hlt, 0,0));
    return result;
}


binary::binary(command::opcodes op, expr_t *arg1, expr_t *arg2):
    op(op), 
    arg1(arg1), 
    arg2(arg2) 
{}

void binary::print()
{
    std::cout<<"(";
    arg1->print();
    std::cout<<op;
    arg2->print();
    std::cout<<")";
}

cmdlist binary::emit()
{
    cmdlist result = arg1->emit(), tmp = arg2->emit();
    append(result, tmp);
    dest = newreg();
    result.push_back(command(op, dest, arg1->dest, arg2->dest));
    return result;
}

binary::~binary()
{
    delete arg1;
    delete arg2;
}


assign::assign(const std::string& name, expr_t* value):
    name(name), 
    value(value) 
{}

void assign:: print()
{
    std::cout << name << " = "; 
    value->print(); 
}

cmdlist assign:: emit()
{
    cmdlist result = value->emit();
    dest = value->dest;
    if(contains(vars, name))
    {
        result.push_back(command(command::add, vars[name], value->dest, 0)); // mov
    }
    else
    {
        vars[name] = dest;
    }

    return result;
}

assign::~assign()
{
    delete value;
}


unary::unary(command::opcodes op, expr_t* arg): 
    op(op), 
    arg(arg) 
{}

void unary:: print()
{
    std::cout << op;
    arg->print();
}

cmdlist unary:: emit()
{
    cmdlist result = arg->emit();
    dest = newreg();
    result.push_back(command(op, dest, 0, arg->dest));
    return result;
}

unary::~unary()
{
    delete arg;
}


value::value(const std::string& text): 
    text(text) 
{}

void value::print()
{
    std::cout << text;
}

cmdlist value::emit()
{
    cmdlist result; dest = newreg();
    result.push_back(command(command::mov, dest, atoi(text.c_str())));
    return result;
}


cmdlist varref::emit()
{
    if(contains(vars, name))
    {
        dest = vars[name];
    }
    else
    {
        yyerror("Undefined variable");
    }

    return cmdlist();
}

funcall::funcall(const std::string& name, const std::list<expr_t*>& args):
    name(name),
    args(args) 
{}

void funcall::print()
{
    std::cout<<name<<"(";
    for (auto&& item: args)
    {
        if (item != *args.begin())
        {
            std::cout << ", ";
        }

        item->print();
    }
    std::cout<<")";
}

cmdlist funcall::emit()
{
    cmdlist result;
    if(!name.compare("input")) 
    {
        if(args.size())
        {
            yyerror("Input: too many arguments");
        }

        dest = newreg();
        result.push_back(command(command::input, dest, 0));
    } 
    else if (!name.compare("echo")) 
    {
        if(!args.size())
        {
            yyerror("Input: missing arguments");
        }

        dest = 0;
        for (auto&& item: args)
        {
            string *s = dynamic_cast<string*>(item);
            if(s) 
            { // string
                if(contains(strings, s->text))
                {
                    result.push_back(command(command::echo, 0, strings[s->text]));
                }
                else 
                {
                    int newid = strings.size()+1;
                    strings[s->text] = newid;
                    result.push_back(command(command::echo, 0, newid));
                }
            } 
            else 
            {
                cmdlist tmp = item->emit();
                append(result, tmp);
                result.push_back(command(command::echo, item->dest, 0));
            }
        }
    } 
    else
    {
        yyerror("Undefined function");
    }

    return result;
}

funcall::~funcall()
{
    for (auto&& item: args)
    {
        delete item;
    }
}

/*std::string replaceAll(const std::string& where, const std::string& what, const std::string& withWhat) 
{
    std::string result = where;
    while(1) 
    {
        int pos = result.find(what);
        if (pos==-1) return result;
        result.replace(pos, what.size(), withWhat);
    }
} */