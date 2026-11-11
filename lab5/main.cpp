#include <iostream>
#include <string>
#include <cstdint>
#include <cctype>
#include <cstdlib>
#include <dlfcn.h>


struct list_t;
struct frame_t;


void error(std::string str) __attribute__((noreturn));
void error(std::string str) {
    throw str;
}


struct node_t {
    virtual node_t* eval(frame_t* frame) {
        return this;
    }
    
    virtual node_t* apply(list_t* list, frame_t* frame) {
        error("This node type does not support apply()");
    }
    
    virtual std::string to_string() {
        return "???";
    }
    
    virtual list_t* as_list() {
        error("Cannot cast to list_t: Not a list");
        return nullptr; 
    }
    
    virtual uintptr_t int_value() {
        error("Cannot extract value: Not an integer");
        return 0; 
    }
    
    virtual bool is(std::string id) {
        return false;
    }
    
    virtual bool is_integer() {
        return false;
    }
};


struct integer_t : public node_t {
    uintptr_t value;
    
    integer_t(uintptr_t val) {
        this->value = val;
    }
    
    std::string to_string() override {
        return std::to_string(value);
    }
    
    uintptr_t int_value() override {
        return value;
    }
    
    bool is_integer() override {
        return true;
    }
    
    
    node_t* apply(list_t* list, frame_t* frame) override;
};

struct string_t : public node_t {
    std::string content;
    
    string_t(std::string text) {
        content = text;
    }
    
    std::string to_string() override {
        return content;
    }
};

struct symbol_t : public node_t {
    std::string id;
    
    symbol_t(std::string identifier) {
        id = identifier;
    }
    
    bool is(std::string other_id) override {
        return id == other_id;
    }
    
    std::string to_string() override {
        return id;
    }
    
    node_t* eval(frame_t* frame) override; 
};


struct list_t : public node_t {
    node_t** elements;
    size_t nb_elements;
    size_t capacity;

    list_t() {
        elements = nullptr;
        nb_elements = 0;
        capacity = 0;
    }

    list_t* as_list() override {
        return this;
    }
    
    size_t size() {
        return nb_elements;
    }

    node_t* at(size_t i) {
        if (i >= nb_elements) {
            error("List index out of bounds");
        }
        return elements[i];
    }

    void set_at(size_t i, node_t* value) {
        if (i >= nb_elements) {
            error("List index out of bounds");
        }
        elements[i] = value;
    }

    void append(node_t* value) {
        if (nb_elements == capacity) {
            size_t new_cap = (capacity == 0) ? 8 : capacity * 2;
            node_t** expanded_array = new node_t*[new_cap];
            
            for (size_t idx = 0; idx < nb_elements; ++idx) {
                expanded_array[idx] = elements[idx];
            }
            
            elements = expanded_array;
            capacity = new_cap;
        }
        elements[nb_elements] = value;
        nb_elements++;
    }

    std::string to_string() override {
        std::string out = "(";
        for (size_t idx = 0; idx < nb_elements; ++idx) {
            out += elements[idx]->to_string();
            if (idx + 1 < nb_elements) {
                out += " ";
            }
        }
        out += ")";
        return out;
    }

    node_t* eval(frame_t* frame) override;
    node_t* apply(list_t* list, frame_t* frame) override;
};

struct frame_t : public list_t {
    frame_t* parent;
    
    frame_t(frame_t* p_frame = nullptr) {
        parent = p_frame;
    }

    void set(node_t* sym, node_t* value) {
        
        for (size_t idx = 0; idx < nb_elements; ++idx) {
            list_t* binding = elements[idx]->as_list();
            if (sym->is(binding->at(0)->to_string())) {
                binding->set_at(1, value);
                return;
            }
        }
        
        list_t* new_binding = new list_t();
        new_binding->append(sym);
        new_binding->append(value);
        this->append(new_binding);
    }

    node_t* get(node_t* sym) {
        for (size_t idx = 0; idx < nb_elements; ++idx) {
            list_t* binding = elements[idx]->as_list();
            if (sym->is(binding->at(0)->to_string())) {
                return binding->at(1);
            }
        }
        
        if (parent != nullptr) {
            return parent->get(sym);
        }
        error("Lookup failed: symbol '" + sym->to_string() + "' is undefined");
        return nullptr;
    }
};

node_t* symbol_t::eval(frame_t* frame) {
    return frame->get(this);
}


struct keyword_t : public node_t {
    std::string id;
    
    keyword_t(std::string identifier) {
        id = identifier;
    }
    
    std::string to_string() override {
        return "<" + id + ">";
    }
};

struct add_t : public keyword_t {
    add_t() : keyword_t("+") {}
    
    node_t* apply(list_t* list, frame_t* frame) override {
        uintptr_t total = 0;
        for (size_t idx = 1; idx < list->size(); ++idx) {
            total += list->at(idx)->eval(frame)->int_value();
        }
        return new integer_t(total);
    }
};

struct sub_t : public keyword_t {
    sub_t() : keyword_t("-") {}
    
    node_t* apply(list_t* list, frame_t* frame) override {
        uintptr_t first_val = list->at(1)->eval(frame)->int_value();
        if (list->size() == 2) {
            return new integer_t(-first_val);
        }
        
        uintptr_t current_val = first_val;
        for (size_t idx = 2; idx < list->size(); ++idx) {
            current_val -= list->at(idx)->eval(frame)->int_value();
        }
        return new integer_t(current_val);
    }
};

struct set_t : public keyword_t {
    set_t() : keyword_t("set!") {}
    
    node_t* apply(list_t* list, frame_t* frame) override {
        node_t* target_sym = list->at(1);
        node_t* evaluated_val = list->at(2)->eval(frame);
        frame->set(target_sym, evaluated_val);
        return evaluated_val;
    }
};

struct eq_t : public keyword_t {
    eq_t() : keyword_t("=") {}
    node_t* apply(list_t* list, frame_t* frame) override {
        uintptr_t left = list->at(1)->eval(frame)->int_value();
        uintptr_t right = list->at(2)->eval(frame)->int_value();
        return new integer_t((left == right) ? 1 : 0);
    }
};

struct lt_t : public keyword_t {
    lt_t() : keyword_t("<") {}
    node_t* apply(list_t* list, frame_t* frame) override {
        uintptr_t left = list->at(1)->eval(frame)->int_value();
        uintptr_t right = list->at(2)->eval(frame)->int_value();
        return new integer_t((left < right) ? 1 : 0);
    }
};

struct if_t : public keyword_t {
    if_t() : keyword_t("if") {}
    node_t* apply(list_t* list, frame_t* frame) override {
        uintptr_t condition = list->at(1)->eval(frame)->int_value();
        if (condition != 0) {
            return list->at(2)->eval(frame);
        }
        if (list->size() > 3) {
            return list->at(3)->eval(frame);
        }
        return new integer_t(0);
    }
};

struct begin_t : public keyword_t {
    begin_t() : keyword_t("begin") {}
    node_t* apply(list_t* list, frame_t* frame) override {
        node_t* final_res = new integer_t(0);
        for (size_t idx = 1; idx < list->size(); ++idx) {
            final_res = list->at(idx)->eval(frame);
        }
        return final_res;
    }
};

struct while_t : public keyword_t {
    while_t() : keyword_t("while") {}
    node_t* apply(list_t* list, frame_t* frame) override {
        node_t* output = new integer_t(0);
        while (list->at(1)->eval(frame)->int_value() != 0) {
            for (size_t idx = 2; idx < list->size(); ++idx) {
                output = list->at(idx)->eval(frame);
            }
        }
        return output;
    }
};

struct lambda_t : public keyword_t {
    lambda_t() : keyword_t("lambda") {}
    node_t* apply(list_t* list, frame_t* frame) override {
        return list; // Lambda evaluates to itself[cite: 1]
    }
};


node_t* list_t::eval(frame_t* frame) {
    if (nb_elements == 0) return this;
    
    
    node_t* operation = elements[0]->eval(frame);
    return operation->apply(this, frame);
}

node_t* list_t::apply(list_t* call_args, frame_t* calling_frame) {
    if (nb_elements >= 3 && elements[0]->is("lambda")) {
        frame_t* exec_frame = new frame_t(calling_frame);
        list_t* param_names = elements[1]->as_list();
        
        
        for (size_t p = 0; p < param_names->size(); ++p) {
            node_t* arg_val = call_args->at(p + 1)->eval(calling_frame);
            exec_frame->set(param_names->at(p), arg_val);
        }
        
        
        node_t* block_result = new integer_t(0);
        for(size_t b = 2; b < nb_elements; b++) {
            block_result = elements[b]->eval(exec_frame);
        }
        return block_result;
    }
    error("Target is not a callable lambda");
    return nullptr;
}


node_t* integer_t::apply(frame_t* frame, list_t* list) {
    // Exact signature provided by the Readme instructions[cite: 1]
    uintptr_t (*f)(uintptr_t, uintptr_t, uintptr_t, uintptr_t) = (uintptr_t (*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t))int_value();
    uintptr_t (*g)(uintptr_t, ...) = (uintptr_t (*)(uintptr_t, ...))int_value();
    std::string strs[4];
    uintptr_t args[] = { 0, 0, 0, 0 };
    size_t n = list->size() < 5 ? list->size() : 5;

    for(size_t i=1; i<n; i++) {
        node_t* arg = list->at(i)->eval(frame);
        args[i-1] = arg->is_integer() ? arg->int_value() : (uintptr_t)(strs[i-1] = arg->to_string()).c_str();
    }

    if((uintptr_t)f == (uintptr_t)printf || (uintptr_t)f == (uintptr_t)vprintf || (uintptr_t)f == (uintptr_t)snprintf)
        return new integer_t { g(args[0], args[1], args[2], args[3]) };
    else
        return new integer_t { f(args[0], args[1], args[2], args[3]) };
}


struct lexer_t {
    std::istream* is;
    int next;
    
    lexer_t(std::istream* input_stream) {
        is = input_stream;
        next = ' ';
    }

    node_t* next_token() {
        while (std::isspace(next)) {
            next = is->get();
        }
        
        if (next == EOF) {
            throw std::string("EOF");
        }
        
        if (next == '(' || next == ')') {
            std::string paren(1, (char)next);
            next = is->get();
            return new symbol_t(paren);
        }
        
        if (next == '"') {
            std::string accum_str = "";
            next = is->get();
            while (next != '"' && next != EOF) {
                accum_str += next;
                next = is->get();
            }
            next = is->get(); // Consume closing quote
            return new string_t(accum_str);
        }
        
        
        uintptr_t number = 0;
        std::string str = "";
        bool is_number = true;
        
        for (;;) {
            if (next < '0' || next > '9') {
                is_number = false;
            }
            number = (number * 10) + (next - '0');
            str += (char)next;
            
            next = is->get();
            
            if (std::isspace(next) || next == '(' || next == ')' || next == '"' || next == EOF) {
                if (is_number) {
                    return new integer_t(number);
                } else {
                    return new symbol_t(str);
                }
            }
        }
    }
};

node_t* parser(lexer_t* lex) {
    node_t* tok = lex->next_token();
    
    if (!tok->is("(")) {
        return tok;
    }
    
    list_t* ast_node = new list_t();
    for (;;) {
        while (std::isspace(lex->next)) {
            lex->next = lex->is->get();
        }
        if (lex->next == ')') {
            lex->next = lex->is->get();
            break;
        }
        ast_node->append(parser(lex));
    }
    
    return ast_node;
}


int main() {
    frame_t* global_env = new frame_t(nullptr);
    
    // Register primitive keywords
    global_env->set(new symbol_t("+"), new add_t());
    global_env->set(new symbol_t("-"), new sub_t());
    global_env->set(new symbol_t("="), new eq_t());
    global_env->set(new symbol_t("<"), new lt_t());
    global_env->set(new symbol_t("if"), new if_t());
    global_env->set(new symbol_t("begin"), new begin_t());
    global_env->set(new symbol_t("while"), new while_t());
    global_env->set(new symbol_t("set!"), new set_t());
    global_env->set(new symbol_t("lambda"), new lambda_t());
    
    // Bind foreign symbols[cite: 1]
    global_env->set(new symbol_t("dlsym"), new integer_t((uintptr_t)dlsym));
    global_env->set(new symbol_t("dlself"), new integer_t((uintptr_t)RTLD_DEFAULT));

    lexer_t stream_lexer(&std::cin);
    std::cout << "lisp-repl> ";

    for (;;) {
        try {
            node_t* program_ast = parser(&stream_lexer);
            node_t* execution_result = program_ast->eval(global_env);
            std::cout << execution_result->to_string() << "\nlisp-repl> ";
        } 
        catch (std::string err_msg) {
            if (err_msg == "EOF") {
                break;
            }
            std::cout << "Runtime Error: " << err_msg << "\nlisp-repl> ";
            stream_lexer.next = ' '; 
        } 
        catch (...) {
            std::cout << "Fatal unknown error\nlisp-repl> ";
            stream_lexer.next = ' '; 
        }
    }
    
    return 0;
}